#include "draw.h"
#include "credits.h"
#include "tactical_draw.h"
#include "Agent_helper.h"

using namespace klib;

static	void								drawIntro											(SGame& instanceGame);

void										klib::printMultipageHelp							(char* targetASCII, uint32_t targetWidth, uint32_t targetHeight, uint32_t currentPage, uint32_t pageCount, uint32_t posXOffset)					{
	static const ::gpk::view_const_string			textToShow[3]										=
		{	"Page down: Next page."
		,	"Page up: Previous page."
		,	"Page up: Previous page. Page down: Next page"
		};
	::gpk::view_const_string						selectedText;
		 if(currentPage == 0)							selectedText										= textToShow[0];
	else if(currentPage == (pageCount-1))				selectedText										= textToShow[1];
	else												selectedText										= textToShow[2];
	::klib::lineToRect(targetASCII, targetWidth, targetHeight, (int32_t)targetHeight-MENU_ROFFSET+1, posXOffset, ::klib::SCREEN_CENTER, selectedText.begin());
}

// Currently what this function is lacking is the ability to receive negative offsets.
template<typename _TCell>
void										blitGrid						(::gpk::view_grid<_TCell> source, int32_t offsetY, uint32_t offsetX, _TCell* target, size_t targetWidth, size_t targetHeight, int32_t rowPitch=-1)	{
	size_t											actualWidth						= ::gpk::min(source.metrics().x, ::gpk::max(0U, (uint32_t)targetWidth - offsetX));
	if(rowPitch < 0)
		rowPitch									= (int32_t)targetWidth;
	for(int32_t z = 0, maxZ = (int32_t)::gpk::min(source.metrics().y, (uint32_t)targetHeight - offsetY); z < maxZ; ++z) {
		if( (offsetY+z) < 0 )
			continue;
		memcpy(&target[(offsetY+z)*rowPitch+offsetX], &source[z][0], actualWidth);
	}
}

static	void											drawSnowBackground							(::klib::SWeightedDisplay & display, double lastTimeSeconds, uint32_t disturbance = 1 )																							{
	int32_t														displayWidth								= (int32_t)display.Screen.metrics().x;
	//int32_t														displayDepth								= (int32_t)display.Depth;

	for(int32_t x=0; x<displayWidth; ++x)
		if(display.DisplayWeights[0][x] == 0)  {
			if( 0 == (rand()%200) && x % 2) {
				display.Screen.Color		[0][x]						= (::gpk::noise1D((uint32_t)(lastTimeSeconds*10000+x), disturbance) > 0.0) ? '.' : (::gpk::noise1D((uint32_t)(lastTimeSeconds*10000-x*x), disturbance) > 0.0) ? 15 : ',';
				display.DisplayWeights		[0][x]						= .00001f;
				display.Speed				[0][x]						= rand()*.001f;
				display.SpeedTarget			[0][x]						= rand()*.001f;
				display.Screen.DepthStencil	[0][x]						= (::gpk::noise1D((uint32_t)(lastTimeSeconds*10000-x), disturbance) > 0.0) ? ::klib::ASCII_COLOR_INDEX_CYAN : ::klib::ASCII_COLOR_INDEX_WHITE;
			}
		}

	for(uint32_t z=0; z<display.Screen.metrics().y - 2; ++z)
	for(uint32_t x=0; x<display.Screen.metrics().x; ++x) {
		if(display.DisplayWeights[z][x] == 0)
			continue;

		display.DisplayWeights[z][x]						+= (float)(lastTimeSeconds * display.Speed[z][x]);

		if(display.Speed[z][x] < display.SpeedTarget[z][x])
			display.Speed[z][x]								+= (float)((display.Speed[z][x]*lastTimeSeconds*lastTimeSeconds));//*.1f;
		else
			display.Speed[z][x]								-= (float)((display.Speed[z][x]*lastTimeSeconds*lastTimeSeconds));//*.1f;
	}

	for(uint32_t z=0; z<display.Screen.metrics().y - 2; ++z)
	for(uint32_t x=0; x<display.Screen.metrics().x; ++x) {
		if(display.DisplayWeights[z][x] == 0)
			continue;

		if(display.DisplayWeights[z][x] > 1.0) {
			int															randX										= (rand()%2) ? rand()%(1+disturbance*2)-disturbance : 0;
			int32_t														xpos										= ::gpk::max(::gpk::min((int)x+randX, displayWidth-1), 0);
			display.Screen.Color		[z + 1][xpos]				= display.Screen.Color	[z][x];
			display.Speed				[z + 1][xpos]				= display.Speed			[z][x];
			display.Screen.DepthStencil	[z + 1][xpos]				= (::gpk::noise1D((uint32_t)(lastTimeSeconds*10000+x), disturbance) > 0.0) ? ::klib::ASCII_COLOR_INDEX_CYAN : ::klib::ASCII_COLOR_INDEX_WHITE;
			//display.Screen.DepthStencil	[z + 1][xpos]				= display.Screen.DepthStencil[z][x];
			display.DisplayWeights		[z + 1][xpos]				= 0.0001f;
			display.SpeedTarget			[z + 1][xpos]				= (float)((rand()%5000))*0.001f+0.001f;
			display.Screen.Color		[z][x]						= ' ';
			display.DisplayWeights		[z][x]						= 0;
			display.Speed				[z][x]						= 0;
			display.SpeedTarget			[z][x]						= 0;
			display.Screen.DepthStencil	[z][x]						= ::klib::ASCII_COLOR_INDEX_WHITE;
		}
	}

}


void													klib::drawFireBackground					( ::klib::SWeightedDisplay& display, double lastTimeSeconds, uint32_t disturbance, uint32_t disappearChanceDivisor, bool bReverse, bool bDontSlowdown)			{
	uint32_t													displayWidth								= (int32_t)display.Screen.metrics().x;
	uint32_t													displayDepth								= (int32_t)display.Screen.metrics().y;

	uint32_t													firstRow									= bReverse ? 0 : displayDepth - 1;
	uint32_t													lastRow										= bReverse ? displayDepth - 1 : 0;
	uint64_t													seed										= (uint64_t)(disturbance+lastTimeSeconds*100000*(1+(rand()%100)));
	uint32_t													randBase									= (uint32_t)(lastTimeSeconds*(disturbance+654)*100000			);
	for(uint32_t x = 0; x<displayWidth; ++x)
		if(display.DisplayWeights[firstRow][x] == 0) {
			if( 0 == (rand()%4) ) {
				display.Screen.Color		[firstRow][x]			=  (::gpk::noise1D(randBase+x, seed+1203) > 0.0) ? '.' :  (::gpk::noise1D(randBase + 1 + x * x, seed+1235) > 0.0) ? '|' : ',';
				display.DisplayWeights		[firstRow][x]			= .00001f;
				display.Speed				[firstRow][x]			= rand()*.001f+0.001f;
				display.SpeedTarget			[firstRow][x]			= rand()*.0009f+0.001f;
				display.Screen.DepthStencil	[firstRow][x]			= bReverse ? ((::gpk::noise1D(randBase+321+x, seed+91423) > 0.0)? ::klib::ASCII_COLOR_INDEX_CYAN : ::klib::ASCII_COLOR_INDEX_BLUE) :  (::gpk::noise1D(randBase+32+x, seed<<1) > 0.0) ? ::klib::ASCII_COLOR_INDEX_RED : (::gpk::noise1D(randBase+987429654+x, seed+98234) > 0.0) ? ::klib::ASCII_COLOR_INDEX_ORANGE : ::klib::ASCII_COLOR_INDEX_DARKYELLOW;
			}
		}

	for(uint32_t z = 0; z < displayDepth; ++z)
	for(uint32_t x = 0; x < displayWidth; ++x) {
		if(lastRow == z) {
			display.Screen.Color	[lastRow][x]				= ' ';
			display.DisplayWeights	[lastRow][x]				= 0;
			display.Speed			[lastRow][x]				= 0;
			display.SpeedTarget		[lastRow][x]				= 0;
		}
		if(display.Screen.Color[z][x] == ' ')
			continue;

		display.DisplayWeights[z][x]						+= (float)(lastTimeSeconds*display.Speed[z][x]);

		if(display.Speed[z][x] < display.SpeedTarget[z][x])
			display.Speed[z][x] += (float)(display.Speed[z][x]*lastTimeSeconds);
		else
			display.Speed[z][x] -= (float)(display.Speed[z][x]*lastTimeSeconds);
	}

	for(uint32_t z = 0, maxZ = displayDepth; z < maxZ; z ++)
		for(uint32_t x=0; x<displayWidth; ++x) {
			if(display.Screen.Color[z][x] == ' ')
				continue;

			if(display.DisplayWeights[z][x] > 1.0) {
				int randX = ((::gpk::noise1D(randBase + x + z * displayWidth), seed+544) > 0.0) ? rand()%(1+disturbance*2)-disturbance : 0;
				int32_t xpos = ::gpk::min(x + randX, displayWidth - 1);
				int32_t zpos = bReverse ? z+1 : z-1;

				if((rand()%disappearChanceDivisor) == 0) {
					display.Screen			[zpos][xpos] = ' ';
					display.DisplayWeights	[zpos][xpos] = 0;
				}
				else {
					if(('|' == display.Screen		[z][x]) && z < (displayDepth / 5 * 4)) {
						display.Screen				[zpos][xpos] = '.';
						display.Screen.DepthStencil	[zpos][xpos] = ((bReverse) || (::gpk::noiseNormal1D(x, seed<<2) < 0.0)) ? ::klib::ASCII_COLOR_INDEX_DARKGREY : ::klib::ASCII_COLOR_INDEX_YELLOW;
					}
					else if( bReverse && z > (displayDepth / 5)) {
						display.Screen				[zpos][xpos] = '|';
						display.Screen.DepthStencil	[zpos][xpos] = ::klib::ASCII_COLOR_INDEX_CYAN;
					}
					else {
						display.Screen				[zpos][xpos]	= display.Screen[z][x];
						display.Screen.DepthStencil	[zpos][xpos]	= display.Screen.DepthStencil[z][x];
					}

					display.DisplayWeights	[zpos][xpos]	= 0.00001f;
					display.Speed			[zpos][xpos]	= display.Speed[z][x];
					display.SpeedTarget		[zpos][xpos]	= (float)::gpk::noiseNormal1D(x, seed)*50.0f;
					if(bDontSlowdown)
						display.SpeedTarget	[zpos][xpos]	*= ((bReverse ? displayDepth - z : z ) * 2 / (float)displayDepth);
					display.SpeedTarget		[zpos][xpos]	+= 0.001f;
				}

				display.Screen				[z][x]	= ' ';
				display.DisplayWeights		[z][x]	= 0;
				display.Speed				[z][x]	= 0;
				display.SpeedTarget			[z][x]	= 0;
				display.Screen.DepthStencil	[z][x]	= ::klib::ASCII_COLOR_INDEX_WHITE;
			}
		}
}

void drawRainBackground( ::klib::SWeightedDisplay& display, double lastTimeSeconds ) {
	for(uint32_t i=0; i<display.Screen.metrics().x; ++i)
		if(rand()%2)
			display.Screen[display.Screen.metrics().y - 1][i]	= (rand()%2) ? '.' : '|';
	return drawFireBackground( display, lastTimeSeconds*1.5, 0, 20, true, false );
}

template<typename _TCell>
void										drawDisplay						(::gpk::view_grid<_TCell> source, uint32_t offsetY, uint32_t offsetX, ::klib::SASCIITarget& asciiTarget)	{ ::blitGrid(source, offsetY, offsetX, (_TCell*)asciiTarget.Characters.begin(), asciiTarget.Characters.metrics().x, asciiTarget.Characters.metrics().y); }
static void									drawStateBackground				( SGame& instanceGame )																						{
	switch(instanceGame.State.State) {
	case	GAME_STATE_MENU_MAIN		:	drawIntro(instanceGame);																	; break;
	case	GAME_STATE_CREDITS			:	//drawSnowBackground(instanceGame.GlobalDisplay, instanceGame.FrameTimer.LastTimeSeconds);	; break;
	case	GAME_STATE_MEMORIAL			:	drawSnowBackground(instanceGame.GlobalDisplay, instanceGame.FrameTimer.LastTimeSeconds);	; break;
	case	GAME_STATE_MENU_SQUAD_SETUP	:	//drawRainBackground(instanceGame.GlobalDisplay, instanceGame.FrameTimer.LastTimeSeconds);	; break;
	case	GAME_STATE_MENU_EQUIPMENT	:	drawRainBackground(instanceGame.GlobalDisplay, instanceGame.FrameTimer.LastTimeSeconds);	; break;
	}
}

static	SGameState							drawMemorial			(char* display, uint32_t width, uint32_t depth, uint16_t* textAttributes, double lastFrameTime, const ::gpk::array_obj<CDeadCharacter>& namesMemorial, const SGameState& returnValue) {
	static double									offset					= (double)depth;
	int32_t											curLine					= (int32_t)offset;
	static int32_t									maxDifference			= curLine;
	const int32_t									curDifference			= curLine;
	const double									bbHeight				= (double)depth;

	for(uint32_t i=0, memorialLines = namesMemorial.size(); i < memorialLines && curLine < bbHeight; ++i) {
		char bufferMoneyEarned		[32]	= {};
		char bufferMoneySpent		[32]	= {};
		char bufferDamageDealt		[32]	= {};
		char bufferDamageTaken		[32]	= {};
		char bufferTurnsPlayed		[32]	= {};
		char bufferBattlesWon		[32]	= {};
		char bufferBattlesLost		[32]	= {};
		char bufferEscapesSucceeded	[32]	= {};
		//char bufferEscapesFailed	[32]	= {};
		char bufferEnemiesKilled	[32]	= {};
		char bufferAttacksHit		[32]	= {};
		char bufferAttacksMissed	[32]	= {};
		char bufferAttacksReceived	[32]	= {};
		char bufferAttacksAvoided	[32]	= {};
		char bufferPotionsUsed		[32]	= {};
		char bufferGrenadesUsed		[32]	= {};

		const CDeadCharacter					& deadCharacter									= namesMemorial[i];
		sprintf_s(bufferMoneyEarned			, "%lli", (int64_t)deadCharacter.Score.MoneyEarned		);
		sprintf_s(bufferMoneySpent			, "%lli", (int64_t)deadCharacter.Score.MoneySpent		);
		sprintf_s(bufferDamageDealt			, "%lli", (int64_t)deadCharacter.Score.DamageDealt		);
		sprintf_s(bufferDamageTaken			, "%lli", (int64_t)deadCharacter.Score.DamageTaken		);
		sprintf_s(bufferTurnsPlayed			, "%lli", (int64_t)deadCharacter.Score.TurnsPlayed		);
		sprintf_s(bufferBattlesWon			, "%lli", (int64_t)deadCharacter.Score.BattlesWon		);
		sprintf_s(bufferBattlesLost			, "%lli", (int64_t)deadCharacter.Score.BattlesLost		);
		sprintf_s(bufferEscapesSucceeded	, "%lli", (int64_t)deadCharacter.Score.EscapesSucceeded	);
		//sprintf_s(bufferEscapesFailed		, "%lli", (int64_t)deadCharacter.Score.EscapesFailed	);
		sprintf_s(bufferEnemiesKilled		, "%lli", (int64_t)deadCharacter.Score.EnemiesKilled	);
		sprintf_s(bufferAttacksHit			, "%lli", (int64_t)deadCharacter.Score.AttacksHit		);
		sprintf_s(bufferAttacksMissed		, "%lli", (int64_t)deadCharacter.Score.AttacksMissed	);
		sprintf_s(bufferAttacksReceived		, "%lli", (int64_t)deadCharacter.Score.AttacksReceived	);
		sprintf_s(bufferAttacksAvoided		, "%lli", (int64_t)deadCharacter.Score.AttacksAvoided	);
		sprintf_s(bufferPotionsUsed			, "%lli", (int64_t)deadCharacter.Score.UsedPotions		);
		sprintf_s(bufferGrenadesUsed		, "%lli", (int64_t)deadCharacter.Score.UsedGrenades		);

		static const char format0[]		=
			"Turns Played        : %-8.8s - "
			"Battles Won         : %-8.8s - "
			"Battles Lost        : %-8.8s - "
			"Escapes Succeeded   : %-8.8s"
			//"Escapes Failed      : %-6.6s - "
			;
		static const char format1[]		=
			"Damage Dealt        : %-8.8s - "
			"Damage Taken        : %-8.8s - "
			"Enemies Killed      : %-8.8s - "
			;
		static const char format2[]		=
			"Attacks Hit         : %-8.8s - "
			"Attacks Missed      : %-8.8s - "
			"Attacks Received    : %-8.8s - "
			"Attacks Avoided     : %-8.8s - "
			;
		static const char format3[]		=
			"Potions Used        : %-8.8s - "
			"Grenades Used       : %-8.8s"
			;
		static const char format4[]		=
			"Money Earned        : %-8.8s - "
			"Money Spent         : %-8.8s"
			;
		int32_t					messageColor			= ::klib::ASCII_COLOR_INDEX_GREEN;
		int32_t					offsetX					= 4;
		if((curLine+=2) >= 0 && (curLine < bbHeight))
			::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, 0, ::klib::SCREEN_CENTER, "-- %s --", deadCharacter.Name.begin());

		messageColor = ::klib::ASCII_COLOR_INDEX_DARKGREY;
		if((curLine+=2) >= 0 && curLine < bbHeight)
			offsetX = printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, 3, ::klib::SCREEN_LEFT, format0
				, bufferTurnsPlayed
				, bufferBattlesWon
				, bufferBattlesLost
				, bufferEscapesSucceeded
				//, bufferEscapesFailed
				);
		if((curLine+=1) >= 0 && curLine < bbHeight)
			::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, offsetX, ::klib::SCREEN_LEFT, format1
				, bufferDamageDealt
				, bufferDamageTaken
				, bufferEnemiesKilled
				);
		if((curLine+=1) >= 0 && curLine < bbHeight)
			::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, offsetX, ::klib::SCREEN_LEFT, format2
				, bufferAttacksHit
				, bufferAttacksMissed
				, bufferAttacksReceived
				, bufferAttacksAvoided
				);
		if((curLine+=1) >= 0 && curLine < bbHeight)
			::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, offsetX, ::klib::SCREEN_LEFT, format3
				, bufferPotionsUsed
				, bufferGrenadesUsed
				);
		if((curLine+=1) >= 0 && curLine < bbHeight)
			::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, offsetX, ::klib::SCREEN_LEFT, format4
				, bufferMoneyEarned
				, bufferMoneySpent
				);
	}

	maxDifference	= ::gpk::max(curLine - curDifference, maxDifference);
	offset			-= lastFrameTime * 6.0;

	if( offset <= -maxDifference )
		offset			+= depth + maxDifference;

	return returnValue;
}


void										klib::drawAndPresentGame		(SGame& instanceGame, ::klib::SASCIITarget& target)																													{
	static ::klib::STimer							frameMeasure;
	static ::klib::STimer							animTimer;
	static ::klib::SAccumulator<double>				animTimerAccum					= {0.0, 1.00};
	// increase time
	animTimer.Frame();
	// Check for help display
	if(animTimerAccum.Accumulate(animTimer.LastTimeSeconds))
		animTimerAccum.Value						= 0;

	::drawStateBackground(instanceGame);


	uint32_t										bbWidth							= target.Width		();
	uint32_t										bbHeight						= target.Height		();
	uint8_t											* bbChar						= target.Characters	.begin();
	uint16_t										* bbColor						= target.Colors		.begin();

	// Print log
	::showMenu(instanceGame);

	::gpk::SRenderTarget<char_t, uint16_t>			& displayGlobal					= instanceGame.GlobalDisplay	.Screen;
	::gpk::SRenderTarget<char_t, uint16_t>			& displayTactical				= instanceGame.TacticalDisplay	.Screen;
	const ::gpk::SCoord2<uint32_t>					& displayMetricsGlobal			= displayGlobal		.metrics();
	const ::gpk::SCoord2<uint32_t>					& displayMetricsTactical		= displayTactical	.metrics();

	const int32_t									MAX_LOG_LINES					= (instanceGame.State.State == GAME_STATE_WELCOME_COMMANDER || ::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_TACTICAL)) ? 30 : 4;
	int32_t											logSize							= (int32_t)instanceGame.Messages.UserLog.size();
	for(uint32_t iLogLine=0, logLineCount = ::gpk::min(MAX_LOG_LINES, logSize); iLogLine< logLineCount; ++iLogLine)
		::klib::lineToRectColored(displayGlobal.Color.View, displayGlobal.DepthStencil.View, instanceGame.Messages.UserLog[logSize-1-iLogLine].Color, bbHeight-9-iLogLine, 1, ::klib::SCREEN_LEFT, instanceGame.Messages.UserLog[logSize-1-iLogLine].Message.begin());

	drawDisplay(displayGlobal.Color.View, 0, 0, target);
	memcpy(bbColor, displayGlobal.DepthStencil.begin(), displayGlobal.size() * sizeof(uint16_t));

	//drawDisplay(instanceGame.MenuDisplay, 0);
	switch(instanceGame.State.State) {
	//case GAME_STATE_MENU_ACTION:
	case GAME_STATE_MENU_LAN_MISSION	:
	case GAME_STATE_TACTICAL_CONTROL	:
	case GAME_STATE_START_MISSION		: drawDisplay(displayTactical.Color.View, TACTICAL_DISPLAY_POSY, (displayMetricsGlobal.x >> 1) - (displayMetricsTactical.x >> 1), target);	break;
	case GAME_STATE_CREDITS				: drawCredits((char_t*)bbChar, bbWidth, bbHeight, instanceGame.FrameTimer.LastTimeSeconds, namesSpecialThanks, instanceGame.State);																		break;
	case GAME_STATE_MEMORIAL			: drawMemorial((char_t*)bbChar, bbWidth, bbHeight, displayGlobal.DepthStencil.begin(), instanceGame.FrameTimer.LastTimeSeconds, instanceGame.Players[0].Memorial, instanceGame.State);	break;
	case GAME_STATE_WELCOME_COMMANDER	:
	case GAME_STATE_MENU_SQUAD_SETUP	:
	case GAME_STATE_MENU_EQUIPMENT		: break;
	default:
		drawDisplay(displayTactical.Color.View, TACTICAL_DISPLAY_POSY, (displayMetricsGlobal.x >> 1) - (displayMetricsTactical.x >> 1), target);
	}


	switch(instanceGame.State.State) {
	//case GAME_STATE_MENU_ACTION:
	case GAME_STATE_MENU_LAN_MISSION	:
	case GAME_STATE_TACTICAL_CONTROL	:
	case GAME_STATE_START_MISSION		:
		for(uint32_t y = 0; y < displayMetricsTactical.y; ++y)
			memcpy(&bbColor[(TACTICAL_DISPLAY_POSY + y) * bbWidth + ((bbWidth >> 1) - (displayMetricsTactical.x >> 1))], &displayTactical.DepthStencil[y][0], displayMetricsTactical.x * sizeof(uint16_t));
		break;
	case GAME_STATE_CREDITS				:
	case GAME_STATE_MEMORIAL			:
	case GAME_STATE_WELCOME_COMMANDER	:
	case GAME_STATE_MENU_SQUAD_SETUP	:
	case GAME_STATE_MENU_EQUIPMENT		: break;
	default:
		for(uint32_t y = 0; y<displayMetricsTactical.y; ++y)
			memcpy(&bbColor[(TACTICAL_DISPLAY_POSY + y) * bbWidth + ((bbWidth >> 1) - (displayMetricsTactical.x >> 1))], &displayTactical.DepthStencil[y][0], displayMetricsTactical.x * sizeof(uint16_t));
	}

	// Frame timer
	frameMeasure.Frame();
	instanceGame.FrameTimer.Frame();
	if(instanceGame.FrameTimer.LastTimeSeconds >= 0.150001)
		instanceGame.FrameTimer.LastTimeSeconds		= 0.150001;

	//int32_t											actualOffsetX;

	// Print some debugging information
	if(::gpk::bit_true(instanceGame.Flags,GAME_FLAGS_HELPON)) {
		int32_t											offsetYHelp						= 0;
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_YELLOW	, offsetYHelp, 2, ::klib::SCREEN_RIGHT, "Help!!! Press F1 to hide.");
		++offsetYHelp;
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_ORANGE	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Keys:");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Up    : Menu cursor up.");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Down  : Menu cursor down.");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Left  : Previous menu page.");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Right : Next menu page.");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Enter : Select menu option.");
		++offsetYHelp;
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_ORANGE	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Mouse:");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Left Click  : Select menu option");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Left Click  : Select move destination");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Left Click  : Select attack target");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Right Click : Return to the previous screen.");
		++offsetYHelp;
		++offsetYHelp; ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5) ? ::klib::ASCII_COLOR_INDEX_CYAN		:	::klib::ASCII_COLOR_INDEX_DARKBLUE		, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "\x0C, \x0B: Blue characters are your own squad.");
		++offsetYHelp; ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5) ? ::klib::ASCII_COLOR_INDEX_MAGENTA	:	::klib::ASCII_COLOR_INDEX_DARKMAGENTA	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "\x0C, \x0B: Magenta characters are your ally squads.");
		++offsetYHelp; ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5) ? ::klib::ASCII_COLOR_INDEX_RED		:	::klib::ASCII_COLOR_INDEX_DARKRED		, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "\x0C, \x0B: Red characters are your enemies.");
		++offsetYHelp; ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5) ? ::klib::ASCII_COLOR_INDEX_ORANGE	:	::klib::ASCII_COLOR_INDEX_DARKYELLOW	, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "\x04: Money is picked up when you walk over it.");
		++offsetYHelp; ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5) ? ::klib::ASCII_COLOR_INDEX_WHITE		:	::klib::ASCII_COLOR_INDEX_DARKGREY		, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "\xB9: Obstacles turn gray when destroyed.");
		++offsetYHelp;
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_YELLOW		, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "Melee attacks and grenades currently require ");
		++offsetYHelp; ::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_YELLOW		, offsetYHelp, bbWidth/4*3, ::klib::SCREEN_LEFT, "a valid target selected in order to work!");
	}
	else
		::klib::lineToRectColored(target, ::klib::ASCII_COLOR_INDEX_YELLOW, 1, 2, ::klib::SCREEN_RIGHT, "Press F1 to display help.");

	::klib::printfToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREEN	, bbHeight-4, 1, ::klib::SCREEN_LEFT, "Mouse: %i, %i."				, instanceGame.FrameInput.Mouse.Deltas.x, instanceGame.FrameInput.Mouse.Deltas.y);
	::klib::printfToRectColored(target, ::klib::ASCII_COLOR_INDEX_GREEN		, bbHeight-3, 1, ::klib::SCREEN_LEFT, "Frame time: %.5f seconds."	, instanceGame.FrameTimer.LastTimeSeconds	);
	::klib::printfToRectColored(target, ::klib::ASCII_COLOR_INDEX_CYAN		, bbHeight-2, 1, ::klib::SCREEN_LEFT, "Frames last second: %f."		, instanceGame.FrameTimer.FramesLastSecond	);
	time_t											curTimeWithUnreliableSize		= 0;
	{
		::gpk::mutex_guard								thelock							(instanceGame.ServerTimeMutex);
		curTimeWithUnreliableSize					= instanceGame.ServerTime;
	}
	char											send_buffer[64]					= {};
	ctime_s(send_buffer, sizeof(send_buffer), &curTimeWithUnreliableSize);

	::gpk::array_pod<char_t>						serverTime						= ::gpk::view_const_string{"Server time: "};
	serverTime.append_string({send_buffer, (uint32_t)strlen(send_buffer) - 1});
	serverTime									= ::gpk::view_const_char{serverTime.begin(), serverTime.size() - 2};
	::klib::printfToRectColored(target, ::klib::ASCII_COLOR_INDEX_CYAN		, bbHeight-2, 0, ::klib::SCREEN_CENTER, "%s.", serverTime.begin());
	::klib::printfToRectColored(target, ::klib::ASCII_COLOR_INDEX_DARKGREY	, bbHeight-1, 0, ::klib::SCREEN_CENTER, "%s.", instanceGame.Messages.StateMessage.begin());

	// Print user error messages and draw cursor.
	if(instanceGame.State.State != GAME_STATE_CREDITS) {
		::klib::SGamePlayer									& player						= instanceGame.Players[PLAYER_INDEX_USER];
		int64_t											finalMissionCost				= ::klib::missionCost(player, player.Tactical.Squad, player.Tactical.Squad.Size);
		int64_t											playerFunds						= instanceGame.Players[PLAYER_INDEX_USER].Tactical.Money;
		::klib::printfToRectColored(target, (finalMissionCost > playerFunds) ? ::klib::ASCII_COLOR_INDEX_ORANGE : ::klib::ASCII_COLOR_INDEX_CYAN	, bbHeight-17, 1, ::klib::SCREEN_RIGHT, "Squad size: %i."	, instanceGame.Players[PLAYER_INDEX_USER].Tactical.Squad.Size);
		::klib::printfToRectColored(target, ::klib::ASCII_COLOR_INDEX_YELLOW	, bbHeight-5, 0, ::klib::SCREEN_CENTER, "%s", instanceGame.Messages.UserMessage	.begin());
		::klib::printfToRectColored(target, ::klib::ASCII_COLOR_INDEX_CYAN		, bbHeight-4, 0, ::klib::SCREEN_CENTER, "%s", instanceGame.Messages.UserSuccess	.begin());
		::klib::printfToRectColored(target, ::klib::ASCII_COLOR_INDEX_RED		, bbHeight-3, 0, ::klib::SCREEN_CENTER, "%s", instanceGame.Messages.UserMiss	.begin());
		::klib::printfToRectColored(target, ::klib::ASCII_COLOR_INDEX_RED		, bbHeight-2, 0, ::klib::SCREEN_CENTER, "%s", instanceGame.Messages.UserError	.begin());

		// Draw cursor
		int32_t mouseX = instanceGame.FrameInput.Mouse.Deltas.x, mouseY = instanceGame.FrameInput.Mouse.Deltas.y;
		bbColor[mouseY*bbWidth+mouseX] =
			( ((bbColor[mouseY*bbWidth+mouseX] & 0xF0) >> 4)
			| ((bbColor[mouseY*bbWidth+mouseX] & 0x0F) << 4)
			);
	}

	// increase coins
	static ::klib::STimer							profitTimer;
	static ::klib::SAccumulator<double>				profitTimerAccum				= {0.0, 3.0};

	profitTimer.Frame();
	if( profitTimerAccum.Accumulate(profitTimer.LastTimeSeconds) ) {
		instanceGame.Players[PLAYER_INDEX_USER].Tactical.Money++;
		profitTimerAccum.Value						= 0;
	}

	static ::klib::STimer							keyTimer;
	static ::klib::SAccumulator<double>				keyTimerAccum					= {0.0, .50};
	// increase time
	keyTimer.Frame();
	// Check for help display
	if(keyTimerAccum.Accumulate(keyTimer.LastTimeSeconds) && instanceGame.FrameInput.Keys[VK_F1]) {
		if(::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_HELPON))
			::gpk::bit_clear(instanceGame.Flags, GAME_FLAGS_HELPON);
		else
			::gpk::bit_set(instanceGame.Flags, GAME_FLAGS_HELPON);
		keyTimerAccum.Value								= 0;
	}
}

static void									drawIntro						( SGame& instanceGame ) {
	drawFireBackground(instanceGame.TacticalDisplay, instanceGame.FrameTimer.LastTimeSeconds);
	int32_t											displayDepth					= (int32_t)instanceGame.TacticalDisplay.Screen.metrics().y;

	static const	::gpk::view_const_string		words			[]				= {"Vulgar", "Display", "of", "Power"};
	for( uint32_t i=0; i < ::gpk::size(words); ++i) {
		uint32_t										offsetY							= (uint32_t)((displayDepth >> 1)-(::gpk::size(words) >> 1) + i * 2);
		//uint32_t										offsetX							=
			printfToGridColored(instanceGame.TacticalDisplay.Screen.Color.View, instanceGame.TacticalDisplay.Screen.DepthStencil, ::klib::ASCII_COLOR_INDEX_ORANGE, offsetY, 0, ::klib::SCREEN_CENTER, "%s", words[i].begin());
	}
}


char										klib::getASCIIWall				(const ::gpk::view_array<const ::klib::SEntityRecord<::klib::SStageProp>> definitions, const ::gpk::view_grid<const STileProp>& propGrid, int32_t x, int32_t z) {
	::klib::SASCIIWallConnection					connection						= {false};

	char											result							= '-';
	static	const ::gpk::view_const_string			labelWall						= "Wall";

	bool											bIsReinforced					= propGrid[z][x].Modifier > 0;

	if(x - 1 >= 0								&& propGrid[z    ][x - 1].Definition != -1 && definitions[propGrid[z    ][x - 1].Definition].Name == labelWall) { connection.Left	= true;	if(propGrid[z    ][x - 1].Modifier > 0) connection.ReinforcedLeft	= true; }
	if(x + 1 < (int32_t)propGrid.metrics().x	&& propGrid[z    ][x + 1].Definition != -1 && definitions[propGrid[z    ][x + 1].Definition].Name == labelWall) { connection.Right	= true;	if(propGrid[z    ][x + 1].Modifier > 0) connection.ReinforcedRight	= true; }
	if(z - 1 >= 0								&& propGrid[z - 1][x    ].Definition != -1 && definitions[propGrid[z - 1][x    ].Definition].Name == labelWall) { connection.Top	= true;	if(propGrid[z - 1][x    ].Modifier > 0) connection.ReinforcedTop	= true; }
	if(z + 1 < (int32_t)propGrid.metrics().y	&& propGrid[z + 1][x    ].Definition != -1 && definitions[propGrid[z + 1][x    ].Definition].Name == labelWall) { connection.Bottom	= true;	if(propGrid[z + 1][x    ].Modifier > 0) connection.ReinforcedBottom	= true; }

		 if(connection.Bottom	&& connection.Top		&& connection.Left && connection.Right	) { result = ::klib::resolveASCIIConnectionCross			(bIsReinforced, connection); }
	else if(connection.Left		&& connection.Right		&& connection.Top						) { result = ::klib::resolveASCIIConnectionHorizontalUp		(bIsReinforced, connection); }
	else if(connection.Left		&& connection.Right		&& connection.Bottom					) { result = ::klib::resolveASCIIConnectionHorizontalDown	(bIsReinforced, connection); }
	else if(connection.Top		&& connection.Bottom	&& connection.Right						) { result = ::klib::resolveASCIIConnectionVerticalRight	(bIsReinforced, connection); }
	else if(connection.Top		&& connection.Bottom	&& connection.Left						) { result = ::klib::resolveASCIIConnectionVerticalLeft		(bIsReinforced, connection); }
	else if(connection.Top		&& connection.Right												) { result = ::klib::resolveASCIICornerUpRight				(bIsReinforced, connection); }
	else if(connection.Top		&& connection.Left												) { result = ::klib::resolveASCIICornerUpLeft				(bIsReinforced, connection); }
	else if(connection.Bottom	&& connection.Right												) { result = ::klib::resolveASCIICornerDownRight			(bIsReinforced, connection); }
	else if(connection.Bottom	&& connection.Left												) { result = ::klib::resolveASCIICornerDownLeft				(bIsReinforced, connection); }
	//else iconnection.(bTop	&& connection.Bottom											) { result = ::klib::resolveASCIIVertical					(bIsReinforced, connection); }
	//else iconnection.(bLeft	&& connection.Right												) { result = ::klib::resolveASCIIHorizontal					(bIsReinforced, connection); }
	else if(connection.Top		|| connection.Bottom											) { result = ::klib::resolveASCIIVertical					(bIsReinforced, connection); }
	else if(connection.Left		|| connection.Right												) { result = ::klib::resolveASCIIHorizontal					(bIsReinforced, connection); }

	return result;
}

uint16_t									klib::getPlayerColor			( const ::klib::STacticalInfo & tacticalInfo, const ::klib::STacticalPlayer & boardPlayer, int8_t indexBoardPlayer, int8_t indexPlayerViewer, bool bIsSelected )	{
	uint16_t										color							= ::klib::ASCII_COLOR_INDEX_BLACK;
	if(tacticalInfo.Setup.TeamPerPlayer[indexBoardPlayer] == tacticalInfo.Setup.TeamPerPlayer[indexPlayerViewer]) {
		if(indexBoardPlayer == indexPlayerViewer)
			color								= (bIsSelected) ? ::klib::ASCII_COLOR_INDEX_CYAN : ::klib::ASCII_COLOR_INDEX_DARKBLUE;
		else
			color								= (bIsSelected) ? ::klib::ASCII_COLOR_INDEX_MAGENTA : ::klib::ASCII_COLOR_INDEX_DARKMAGENTA;
	}
	else {
		switch(boardPlayer.Control.Type) {
		case PLAYER_CONTROL_REMOTE	:
		case PLAYER_CONTROL_LOCAL	: color		= (bIsSelected) ? ::klib::ASCII_COLOR_INDEX_RED : ::klib::ASCII_COLOR_INDEX_DARKRED;
			break;

		case PLAYER_CONTROL_AI:
			switch(boardPlayer.Control.AIMode) {
			case PLAYER_AI_NEUTRAL		: color		= bIsSelected ? ::klib::ASCII_COLOR_INDEX_DARKGREY	: ::klib::ASCII_COLOR_INDEX_DARKGREY	; break;
			case PLAYER_AI_FEARFUL		: color		= bIsSelected ? ::klib::ASCII_COLOR_INDEX_DARKGREY	: ::klib::ASCII_COLOR_INDEX_DARKGREY	; break;
			case PLAYER_AI_CURIOUS		: color		= bIsSelected ? ::klib::ASCII_COLOR_INDEX_DARKGREY	: ::klib::ASCII_COLOR_INDEX_DARKGREY	; break;
			case PLAYER_AI_ASSISTS		: color		= bIsSelected ? ::klib::ASCII_COLOR_INDEX_WHITE		: ::klib::ASCII_COLOR_INDEX_GREEN		; break;
			case PLAYER_AI_RIOTERS		: color		= bIsSelected ? ::klib::ASCII_COLOR_INDEX_YELLOW	: ::klib::ASCII_COLOR_INDEX_ORANGE		; break;
			case PLAYER_AI_VIOLENT		: color		= bIsSelected ? ::klib::ASCII_COLOR_INDEX_YELLOW	: ::klib::ASCII_COLOR_INDEX_ORANGE		; break;
			case PLAYER_AI_TEAMERS		: color		= bIsSelected ? ::klib::ASCII_COLOR_INDEX_RED		: ::klib::ASCII_COLOR_INDEX_DARKRED		; break;
			}
		}
	}
	return color;
}

uint16_t									klib::getStatusColor		( COMBAT_STATUS status, bool bSwap, uint16_t defaultColor )																																	{
	static ::klib::SStatusColor						statusColors	[32];
	static const int32_t							initedColors				= ::klib::initStatusColors(statusColors);

	uint32_t										bitIndex					= (uint32_t)-1;

		 if(::gpk::bit_true(status, COMBAT_STATUS_FROZEN	)) { bitIndex	= getBitIndex(COMBAT_STATUS_FROZEN		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_PANIC		)) { bitIndex	= getBitIndex(COMBAT_STATUS_PANIC		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_SLEEP		)) { bitIndex	= getBitIndex(COMBAT_STATUS_SLEEP		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_STUN		)) { bitIndex	= getBitIndex(COMBAT_STATUS_STUN		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_SHOCK		)) { bitIndex	= getBitIndex(COMBAT_STATUS_SHOCK		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_PETRIFY	)) { bitIndex	= getBitIndex(COMBAT_STATUS_PETRIFY		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_CHARMED	)) { bitIndex	= getBitIndex(COMBAT_STATUS_CHARMED		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_BERSERK	)) { bitIndex	= getBitIndex(COMBAT_STATUS_BERSERK		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_BLEEDING	)) { bitIndex	= getBitIndex(COMBAT_STATUS_BLEEDING	, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_POISON	)) { bitIndex	= getBitIndex(COMBAT_STATUS_POISON		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_BURN		)) { bitIndex	= getBitIndex(COMBAT_STATUS_BURN		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_FREEZING	)) { bitIndex	= getBitIndex(COMBAT_STATUS_FREEZING	, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_WEAKNESS	)) { bitIndex	= getBitIndex(COMBAT_STATUS_WEAKNESS	, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_SLOW		)) { bitIndex	= getBitIndex(COMBAT_STATUS_SLOW		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_BULLIED	)) { bitIndex	= getBitIndex(COMBAT_STATUS_BULLIED		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_DRUNK		)) { bitIndex	= getBitIndex(COMBAT_STATUS_DRUNK		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_BLIND		)) { bitIndex	= getBitIndex(COMBAT_STATUS_BLIND		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_RAGE		)) { bitIndex	= getBitIndex(COMBAT_STATUS_RAGE		, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_INVISIBLE	)) { bitIndex	= getBitIndex(COMBAT_STATUS_INVISIBLE	, MAX_COMBAT_STATUS_COUNT); }
	else if(::gpk::bit_true(status, COMBAT_STATUS_BLACKOUT	)) { bitIndex	= getBitIndex(COMBAT_STATUS_BLACKOUT	, MAX_COMBAT_STATUS_COUNT); }
	if(bitIndex != -1)
		defaultColor														= (bSwap ?	statusColors[bitIndex].Bright : statusColors[bitIndex].Dark);

	return defaultColor;
}

static int32_t							displayEntityStatus				(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, const SEntityStatus& entityStatus)										{
	int32_t										iLine							= 0;
	if(entityStatus.Inflict		)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityStatus.Inflict	, MAX_COMBAT_STATUS_COUNT, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_RED	, ::gpk::view_const_string{"- Inflicts: %-14.14s"});
	if(entityStatus.Immunity	)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityStatus.Immunity	, MAX_COMBAT_STATUS_COUNT, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_CYAN	, ::gpk::view_const_string{"- Immunity: %-14.14s"});

	return iLine;
}

static int32_t							displayEntityEffect				(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, const SEntityEffect& entityEffect)										{
	int32_t										iLine							= 0;
	if(entityEffect.Attack	)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityEffect.Attack		, MAX_ATTACK_EFFECT_COUNT	, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_RED	, ::gpk::view_const_string{"- Attack effect: %-14.14s"	});
	if(entityEffect.Defend	)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityEffect.Defend		, MAX_DEFEND_EFFECT_COUNT	, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_CYAN	, ::gpk::view_const_string{"- Defend effect: %-14.14s"	});
	if(entityEffect.Passive	)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityEffect.Passive	, MAX_PASSIVE_EFFECT_COUNT	, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_GREEN	, ::gpk::view_const_string{"- Passive effect: %-14.14s"	});

	return iLine;
}

static int32_t							displayEntityTechnology			(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, const SEntityGrade& entityTech)											{
	int32_t										iLine							= 0;
	if(entityTech.Tech				) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.Tech				, MAX_ENTITY_TECHNOLOGY_COUNT	, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_GREEN	, ::gpk::view_const_string{"- Technology: %-14.14s"			});
	if(entityTech.Grade				) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.Grade				, 3								, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_GREEN	, ::gpk::view_const_string{"- Grade: %-14.14s"				});
	if(entityTech.AttackType		) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.AttackType			, MAX_ATTACK_TYPE_COUNT			, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_RED	, ::gpk::view_const_string{"- Attack type: %-14.14s"		});
	if(entityTech.ProjectileClass	) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.ProjectileClass	, MAX_PROJECTILE_CLASS_COUNT	, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_RED	, ::gpk::view_const_string{"- Projectile Class: %-14.14s"	});
	if(entityTech.AmmoEffect		) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.AmmoEffect			, MAX_AMMO_EFFECT_COUNT			, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_RED	, ::gpk::view_const_string{"- Ammo Effect: %-14.14s"		});

	return iLine;
}

void									klib::displayStatusEffectsAndTechs	(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, CCharacter& character)													{
	int32_t										iLine							= 0;
	if(character.Flags.Tech.Gender)
		iLine									+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, character.Flags.Tech.Gender, 2	, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_GREEN	, ::gpk::view_const_string{"- Gender: %-13.13s"});
	iLine									+= displayEntityTechnology	(display, textAttributes, offsetY+iLine, offsetX, character.FinalFlags.Tech		);
	iLine									+= displayEntityStatus		(display, textAttributes, offsetY+iLine, offsetX, character.FinalFlags.Status	);
	iLine									+= displayEntityEffect		(display, textAttributes, offsetY+iLine, offsetX, character.FinalFlags.Effect	);

	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, character.ActiveBonus.Status.Status, MAX_COMBAT_STATUS_COUNT, ::klib::ASCII_COLOR_INDEX_YELLOW, ::klib::ASCII_COLOR_INDEX_RED, ::gpk::view_const_string{"- Inflicted: %-14.14s"});
}

static void								displayEmptySlot				(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, int32_t agentIndex)																			{
	static const size_t							LINE_SIZE						= 30;
	uint16_t									color							= ::klib::ASCII_COLOR_INDEX_GREEN;
	printfToGridColored(display, textAttributes, color, offsetY, offsetX, ::klib::SCREEN_LEFT, "-- Agent #%i: %-14.14s --", agentIndex, "Open position");
	valueToGrid(textAttributes, offsetY, offsetX+13, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_DARKCYAN), 1, LINE_SIZE-14);
}

static void								displayResumedAgentSlot			(const ::klib::SEntityTables & tables, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, int32_t agentIndex, CCharacter& character)													{
	static const char							formatAgentTitle	[]			= "-- Agent #%i:" " %-34.34s --"	;
	static const char							formatAgentCoins	[]			= "%-21.21s: %-11.11s"				;
	static const char							formatAgentPoints	[]			= "%-21.21s: %-10.10s"				;
	static const char							formatAgentEquip	[]			= "%-10.10s: %-36.36s Lv. %i"		;

	static const size_t							LINE_SIZE						= 56;
	uint16_t									color							= ::klib::ASCII_COLOR_INDEX_GREEN;
	::klib::printfToGridColored(display, textAttributes, color, offsetY, offsetX, ::klib::SCREEN_LEFT,  formatAgentTitle, agentIndex, character.Name.begin());
	::klib::valueToGrid(textAttributes, offsetY, offsetX+13, ::klib::SCREEN_LEFT, &color, 1, LINE_SIZE-14);
	offsetY									+= 2;

	::gpk::array_pod<char_t>					equipName;
	equipName	 = getEntityName	(tables.Profession	, character.CurrentEquip.Profession	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Class"		, equipName.begin(), character.CurrentEquip.Profession	.Level);
	equipName	 = getEntityName	(tables.Weapon		, character.CurrentEquip.Weapon		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Weapon"	, equipName.begin(), character.CurrentEquip.Weapon		.Level);
	equipName	 = getEntityName	(tables.Armor		, character.CurrentEquip.Armor		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Armor"		, equipName.begin(), character.CurrentEquip.Armor		.Level);
	equipName	 = getEntityName	(tables.Accessory	, character.CurrentEquip.Accessory	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Accessory"	, equipName.begin(), character.CurrentEquip.Accessory	.Level);

	::klib::lineToGridColored(display, textAttributes, ::klib::ASCII_COLOR_INDEX_RED, ++offsetY, offsetX, ::klib::SCREEN_LEFT, "- Final Points:");
	offsetY									+=2;

	const SEntityPoints							& agentFinalPoints				= character.FinalPoints;
	char										formattedGauge[32];

	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Health	), agentFinalPoints.LifeMax.Health	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Health"	, formattedGauge);
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Shield	), agentFinalPoints.LifeMax.Shield	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Shield"	, formattedGauge);
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Mana	), agentFinalPoints.LifeMax.Mana	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Mana"		, formattedGauge);

	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Hit						); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Hit Chance"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Damage					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Damage"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Health		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Health"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Shield		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Shield"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Mana			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Mana"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Absorption				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Absorption"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Range						); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Range"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Attack					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Attack Speed"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Movement					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Movement Speed"		, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Reflexes					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Reflexes"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Sight					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Sight"				, formattedGauge);

	const SEntityPoints							& agentBasePoints				= character.Points;
	sprintf_s(formattedGauge, "%lli", agentBasePoints.Coins				); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins in wallet"		, formattedGauge);
	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);
	sprintf_s(formattedGauge, "%lli", agentFinalPoints.Coins			); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins per turn"		, formattedGauge);
	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (agentFinalPoints.Coins >= 0) ? ::klib::ASCII_COLOR_INDEX_ORANGE : ::klib::ASCII_COLOR_INDEX_RED), 1, 11);
	sprintf_s(formattedGauge, "%lli", agentBasePoints.CostMaintenance	); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Salary"				, formattedGauge);
	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);
	sprintf_s(formattedGauge, "%lli", agentFinalPoints.CostMaintenance	); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Total Cost"			, formattedGauge);
	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);
}

void									klib::displayDetailedAgentSlot		(const ::klib::SEntityTables & tables, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, const CCharacter& character, uint16_t color)										{
	static const char							formatAgentTitle	[]			= " - %-34.34s"			;
	static const char							formatAgentEquip	[]			= "%-36.36s Lv. %i"		;
	static const char							formatAgentPoints	[]			= "%-21.21s: %-10.10s"	;
	static const char							formatAgentCoins	[]			= "%-21.21s: %-11.11s"	;

	printfToGridColored(display, textAttributes, color, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentTitle, character.Name.begin());
	offsetY									+=1;

	::gpk::array_pod<char_t>					equipName;
	equipName	= ::klib::getEntityName(tables.Profession	, character.CurrentEquip.Profession	); ::klib::printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.begin(), character.CurrentEquip.Profession	.Level);
	equipName	= ::klib::getEntityName(tables.Weapon		, character.CurrentEquip.Weapon		); ::klib::printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.begin(), character.CurrentEquip.Weapon		.Level);
	equipName	= ::klib::getEntityName(tables.Armor		, character.CurrentEquip.Armor		); ::klib::printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.begin(), character.CurrentEquip.Armor		.Level);
	equipName	= ::klib::getEntityName(tables.Accessory	, character.CurrentEquip.Accessory	); ::klib::printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.begin(), character.CurrentEquip.Accessory	.Level);

	::klib::lineToGridColored(display, textAttributes, ::klib::ASCII_COLOR_INDEX_RED, ++offsetY, offsetX, ::klib::SCREEN_LEFT, "- Final Points:");
	offsetY									+=2;

	const SEntityPoints							& agentFinalPoints				= character.FinalPoints;
	char										formattedGauge[32];
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Health	), agentFinalPoints.LifeMax.Health	); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Health"	, formattedGauge);
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Shield	), agentFinalPoints.LifeMax.Shield	); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Shield"	, formattedGauge);
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Mana	), agentFinalPoints.LifeMax.Mana	); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Mana"	, formattedGauge);
	++offsetY;	//

	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.LifeCurrent.Health			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Health per turn"		, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.LifeCurrent.Shield			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Shield per turn"		, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.LifeCurrent.Mana				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Mana per turn"		, formattedGauge);
	++offsetY;	//

	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Hit					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Hit Chance"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Damage				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Damage"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Health	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Health"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Shield	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Shield"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Mana		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Mana"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Absorption			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Absorption"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Range					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Range"				, formattedGauge);
	++offsetY;	//

	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Attack				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Attack Speed"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Movement				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Movement Speed"		, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Reflexes				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Reflexes"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Sight				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Sight"				, formattedGauge);

	const SEntityPoints							& agentBasePoints				= character.Points;
	sprintf_s(formattedGauge, "%lli"	, agentBasePoints.Coins							); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins in wallet"	, formattedGauge); valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);	//
	sprintf_s(formattedGauge, "%lli"	, agentFinalPoints.Coins						); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins per turn"	, formattedGauge); valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (agentFinalPoints.Coins >= 0) ? ::klib::ASCII_COLOR_INDEX_ORANGE : ::klib::ASCII_COLOR_INDEX_RED), 1, 11);	//
	sprintf_s(formattedGauge, "%lli"	, agentBasePoints.CostMaintenance				); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Salary"			, formattedGauge); valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);	//
	sprintf_s(formattedGauge, "%lli"	, agentFinalPoints.CostMaintenance				); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Total Cost"		, formattedGauge); valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);
}

void									klib::displayAgentSlot					(const ::klib::SEntityTables & tables, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, int32_t agentIndex, CCharacter& character, bool bShort, uint16_t color)	{
	if( bShort )
		::displayResumedAgentSlot		(tables, display, textAttributes, offsetY, offsetX, agentIndex, character);
	else
		::klib::displayDetailedAgentSlot	(tables, display, textAttributes, offsetY, offsetX, character, color);
}

void									klib::drawSquadSlots					(SGame& instanceGame)																																						{
	::gpk::SRenderTarget<char, uint16_t>		& display								= instanceGame.GlobalDisplay.Screen;
	static const int32_t						slotWidth								= display.metrics().x / ::klib::MAX_AGENT_COLUMNS;
	static const int32_t						slotRowSpace							= 30;// display.Depth / (MAX_AGENT_ROWS);

	static const int32_t						offsetYBase								= 1;
	static const int32_t						offsetXBase								= 1;

	SGamePlayer									& player								= instanceGame.Players[PLAYER_INDEX_USER];
	int32_t										playerOffset							= (player.Tactical.Selection.PlayerUnit != -1) ? ::gpk::min(::gpk::max(0, (int32_t)player.Tactical.Selection.PlayerUnit - 2), (int32_t)player.Tactical.Squad.Agents.size() - 3) : 0;

	bool										bStop									= false;
	for(uint32_t y = 0, countY = ::klib::MAX_AGENT_ROWS; y < countY; ++y) {
		for(uint32_t x = 0, countX = ::klib::MAX_AGENT_COLUMNS; x < countX; ++x)  {
			uint32_t									linearIndex								= y*countX+x;
			if(linearIndex >= player.Tactical.Squad.Size) {
				bStop									= true;
				break;
			}
			int32_t										agentIndexOffset						= linearIndex + playerOffset;
			if(agentIndexOffset < (int32_t)player.Tactical.Squad.Agents.size())  {
				if( player.Tactical.Squad.Agents[agentIndexOffset] != -1 )
					::displayAgentSlot(instanceGame.EntityTables, display.Color, display.DepthStencil, offsetYBase + slotRowSpace * y, offsetXBase + slotWidth * x, agentIndexOffset + 1, *player.Tactical.Army[player.Tactical.Squad.Agents[agentIndexOffset]], true);
				else
					::displayEmptySlot(display.Color.View, display.DepthStencil.View, offsetYBase + slotRowSpace * y, offsetXBase + slotWidth * x, agentIndexOffset + 1);
			}
		}
		if(bStop)
			break;
	}
}

static int32_t							processInput						(const ::klib::SInput& frameInput, int32_t actualOffsetX, uint32_t targetHeight, ::klib::SDrawMenuState& localPersistentState, bool& bResetMenuStuff, bool& bResetTitle, int32_t lineOffset, uint32_t actualOptionCount, uint32_t pageCount, int32_t numberCharsAvailable, const int32_t noActionValue, bool disableEscKeyClose, const ::gpk::view_const_char& exitText)	{
	bool										bMouseOverExit						= mouseOver({(int32_t)frameInput.Mouse.Deltas.x, (int32_t)frameInput.Mouse.Deltas.y}, {actualOffsetX - 4, (int32_t)targetHeight-MENU_ROFFSET-1}, (int32_t)exitText.size()+4);
	int32_t										resultVal							= noActionValue;
	if(localPersistentState.CurrentPage < (pageCount-1) && (frameInput.Keys[VK_NEXT] || frameInput.Keys[VK_RIGHT]))
		(bResetMenuStuff = true) && ++localPersistentState.CurrentPage;
	else if(localPersistentState.CurrentPage > 0 && (frameInput.Keys[VK_PRIOR] || frameInput.Keys[VK_LEFT]))
		(bResetMenuStuff = true) && --localPersistentState.CurrentPage;
	else if(frameInput.Keys[VK_DOWN])  {		// Test down and up keys.
		++localPersistentState.CurrentOption;
		bResetMenuStuff							= true;
		if( localPersistentState.CurrentOption == (int32_t)actualOptionCount ) {
			if(localPersistentState.CurrentPage < (pageCount-1) )
				(localPersistentState.CurrentOption = 0) && ++localPersistentState.CurrentPage;
			else
				localPersistentState.CurrentOption		= actualOptionCount-1;
		}
	}
	else if(frameInput.Keys[VK_UP]) {
		--localPersistentState.CurrentOption;
		bResetMenuStuff							= true;
		if( localPersistentState.CurrentOption < 0 ) {
			if(localPersistentState.CurrentPage > 0 )
				(localPersistentState.CurrentOption = 8) && --localPersistentState.CurrentPage;
			else
				localPersistentState.CurrentOption		= 0;
		}
	}
	else if(frameInput.Keys['0'] || frameInput.Keys[VK_NUMPAD0] || ((frameInput.Mouse.Buttons[4] || frameInput.Keys[VK_ESCAPE]) && !disableEscKeyClose) 	// Test exit request keys.
		|| (frameInput.Mouse.Buttons[0] && bMouseOverExit)
		)
	{
		bResetMenuStuff							= true;
		bResetTitle								= true;
		localPersistentState.CurrentPage		= 0;
		localPersistentState.CurrentOption		= 0;
		resultVal								= actualOptionCount;
	}
	else if(frameInput.Keys[VK_RETURN] && localPersistentState.CurrentOption != -1) {	// Test execute keys.
		bResetMenuStuff							= true;
		resultVal								= localPersistentState.CurrentOption;
	}
	else { // look if any of the options was chose from the possible inputs
		for(uint32_t i=0, count = (uint32_t)actualOptionCount; i < count; i++) {
			bool											bMouseOver					= ::klib::mouseOver({(int32_t)frameInput.Mouse.Deltas.x, (int32_t)frameInput.Mouse.Deltas.y}, {(int32_t)(actualOffsetX - 2), (int32_t)(lineOffset + i)}, numberCharsAvailable + 2);
			if(bMouseOver && frameInput.Mouse.Buttons[0]) {
				bResetMenuStuff								= true;
				resultVal									= i;
				break;
			}
			if(frameInput.Keys['1'+i] || frameInput.Keys[VK_NUMPAD1+i]) {
				bResetMenuStuff								= true;
				resultVal									= i;
				break;
			}
		}
	}

	return resultVal;
}


struct SDrawMenuGlobals {
							::klib::STimer					Timer;
							::klib::SAccumulator<double>	Accumulator;
};

static SDrawMenuGlobals	drawMenu_globals = {{}, {0, 0.30}};

int32_t													drawMenu
	( ::klib::SDrawMenuState									& localPersistentState
	, ::gpk::view2d_char										targetASCII
	, uint16_t													* targetAttributes
	, int32_t													& lineOffset
	, const ::gpk::SCoord2<int32_t>								mousePos
	, const ::gpk::view_const_char								& title
	, const ::gpk::view_array<const ::gpk::view_const_char>		& menuItems
	, const uint32_t											actualOptionCount
	, const uint32_t											itemOffset
	, const bool												multipage
	, const uint32_t											pageCount
	, const uint32_t											numberCharsAvailable
	, uint32_t													rowWidth
	, const ::gpk::view_const_char								& exitText
	) {
	const uint32_t												targetWidth											= targetASCII.metrics().x;
	const uint32_t												targetHeight										= targetASCII.metrics().y;
	const int32_t												clearOffset											= (int32_t)(targetHeight - MENU_ROFFSET - 2 - 9);

	::gpk::array_pod<char_t>									clearString											;
	clearString.resize(::gpk::max(rowWidth, 64U), ' ');
	for(int32_t i = -2, count = (int32_t)targetHeight-clearOffset; i<count; ++i)
		::klib::printfToRectColored(targetASCII.begin(), targetWidth, targetHeight, targetAttributes, (::klib::ASCII_COLOR_INDEX_BLACK << 4) | ::klib::ASCII_COLOR_INDEX_YELLOW, clearOffset+i, 0, ::klib::SCREEN_CENTER, "%s", clearString.begin()); // clear all lines where we're going to draw


	// Build formatted menu title
	char														titleFormat		[]									= "-- %s --";
	char														titleFormatted	[128]								= {};
	int32_t														titleLength											= (int32_t)sprintf_s(titleFormatted, titleFormat, title.begin());

	rowWidth												= ::gpk::max((uint32_t)titleLength, ::gpk::max((uint32_t)exitText.size()+3, rowWidth));
	const bool													bDonePrinting										= ::klib::getMessageSlow(localPersistentState.SlowTitle, titleFormatted, titleLength, drawMenu_globals.Timer.LastTimeSeconds * 4);
	int32_t														actualOffsetX										= ::klib::lineToRect(targetASCII.begin(), targetWidth, targetHeight, lineOffset, 0, ::klib::SCREEN_CENTER, localPersistentState.SlowTitle.Message);		//"-- %s --", title.c_str() );	// Print menu title
	for(uint32_t i=0; i<rowWidth+1; i++)
		targetAttributes[lineOffset*targetWidth+actualOffsetX+i] = ::klib::ASCII_COLOR_INDEX_GREEN;

	if( !bDonePrinting )
		return 0;

	lineOffset												+= 2;

	// Print menu options
	char														formatString	[24]								= {};
	char														numberKey		[4]									= {};
	sprintf_s(formatString, "%%2.2s: %%-%u.%us", numberCharsAvailable, numberCharsAvailable);

	// Draw options
	if(drawMenu_globals.Accumulator.Value < 0.575 && localPersistentState.MenuItemAccum < actualOptionCount)
		drawMenu_globals.Accumulator.Value						= 0.575;

	actualOffsetX											= (int32_t)(targetWidth - targetWidth / 2 - numberCharsAvailable / 2);
	for(uint32_t i = 0, count = (uint32_t)actualOptionCount; i < count; ++i) {
		if(::klib::mouseOver(mousePos, {actualOffsetX - 2, (int32_t)(lineOffset + i)}, numberCharsAvailable + 2)) {
			localPersistentState.CurrentOption						= i;
			break;
		}
	}

	if( localPersistentState.CurrentOption >= (int32_t)actualOptionCount )
		localPersistentState.CurrentOption						= actualOptionCount-1;

	for(uint32_t i = 0, count = (localPersistentState.MenuItemAccum < actualOptionCount) ? localPersistentState.MenuItemAccum : actualOptionCount; i < count; ++i) {
		::sprintf_s(numberKey, "%u", (uint32_t)(i+1));
		actualOffsetX											= ::klib::printfToRect(targetASCII.begin(), targetWidth, targetHeight, lineOffset, 0, ::klib::SCREEN_CENTER, formatString, numberKey, menuItems[itemOffset+i].begin());
		if(localPersistentState.CurrentOption == (int32_t)i)
			for(uint32_t j = 0; j < rowWidth + 1; ++j)
				targetAttributes[lineOffset * targetWidth + actualOffsetX + j]	= ::klib::ASCII_COLOR_INDEX_YELLOW << 4;
		else
			for(uint32_t j = 0; j < rowWidth + 1; ++j)
				targetAttributes[lineOffset * targetWidth + actualOffsetX + j]	= ::klib::ASCII_COLOR_INDEX_YELLOW;

		++lineOffset;
	}

	// Print Exit option at the end.
	if(localPersistentState.MenuItemAccum > actualOptionCount)
		::klib::drawExitOption(targetASCII.begin(), targetAttributes, targetWidth, targetHeight, 0, ::klib::SCREEN_CENTER, formatString, exitText, localPersistentState.CurrentOption == 10);

	if(multipage)
		::klib::printMultipageHelp(targetASCII.begin(), targetWidth, targetHeight, localPersistentState.CurrentPage, pageCount, 0);
	return 0;
}

int32_t												klib::drawMenu											(::klib::SDrawMenuState	& localPersistentState, ::gpk::view2d_char display, uint16_t* targetAttributes, const ::gpk::view_const_char& title, const ::gpk::view_array<const ::gpk::view_const_char> & menuItems, const ::klib::SInput& frameInput, const int32_t noActionValue, uint32_t rowWidth, bool disableEscKeyClose, const ::gpk::view_const_char& exitText) {
	drawMenu_globals.Timer.Frame();
	const uint32_t												targetWidth											= display.metrics().x;
	const uint32_t												targetHeight										= display.metrics().y;
	const uint32_t												optionCount											= menuItems.size();
	const bool													multipage											= optionCount > 9;
	const uint32_t												pageCount											= (uint32_t)((multipage == false) ? 1 : optionCount/9 + ((optionCount%9)?1:0));
	if( localPersistentState.CurrentPage >= pageCount )
		localPersistentState.CurrentPage = 0;
	const uint32_t												itemOffset											= localPersistentState.CurrentPage * 9;
	const uint32_t												actualOptionCount									= ::gpk::min(9U, (uint32_t)(optionCount - (localPersistentState.CurrentPage * 9)));
	int32_t														lineOffset											= (int32_t)(targetHeight - MENU_ROFFSET - 4 - ::gpk::min((int32_t)optionCount, 9));
	const int32_t												optionsOffset										= lineOffset;
	const uint32_t												numberCharsAvailable								= rowWidth - 4;	// 4 is for "%2.2s: "
	int32_t														menuOffsetX											= (int32_t)(targetWidth / 2 - numberCharsAvailable / 2);

	int32_t														resultVal											= noActionValue;
	if(::drawMenu(localPersistentState, display, targetAttributes, lineOffset, {(int32_t)frameInput.Mouse.Deltas.x, (int32_t)frameInput.Mouse.Deltas.y}, title, menuItems, actualOptionCount, itemOffset, multipage, pageCount, numberCharsAvailable, rowWidth, exitText))
		return resultVal;
	// Print page control help if multipage.

	bool														bResetMenuStuff										= false;
	bool														bResetTitle											= false;
	if( drawMenu_globals.Accumulator.Accumulate(drawMenu_globals.Timer.LastTimeSeconds) ) {
		if(localPersistentState.MenuItemAccum <= actualOptionCount) // Don't process keys until the menu has finished displaying
			(drawMenu_globals.Accumulator.Value = 0.575) && ++localPersistentState.MenuItemAccum;
		else {	// Process page change keys first.
			resultVal												= ::processInput(frameInput, menuOffsetX, targetHeight, localPersistentState, bResetMenuStuff, bResetTitle, lineOffset - (int32_t)actualOptionCount, actualOptionCount, pageCount, numberCharsAvailable, -1, disableEscKeyClose, exitText);
			if(resultVal != -1) {
				if(resultVal >= (int32_t)actualOptionCount)
					resultVal												= menuItems.size();
				else
					resultVal												+= itemOffset;
			}
		}

	}

	if(bResetMenuStuff)
		::drawMenu_globals.Accumulator.Value						= 0;

	if(bResetTitle) {
		::klib::resetCursorString(localPersistentState.SlowTitle);
		localPersistentState.MenuItemAccum						= actualOptionCount >> 1;
	}

	return resultVal;
}
