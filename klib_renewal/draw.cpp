#include "draw.h"
#include "credits.h"
#include "tactical_draw.h"
#include "Agent_helper.h"

using namespace klib;

void drawIntro(SGame& instanceGame);

void										klib::printMultipageHelp										(char* targetASCII, uint32_t targetWidth, uint32_t targetHeight, uint32_t currentPage, uint32_t pageCount, uint32_t posXOffset)					{
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

template<typename _TCell>
void										drawDisplay						(::gpk::view_grid<_TCell> source, uint32_t offsetY, uint32_t offsetX, ::klib::SASCIITarget& asciiTarget)															{ blitGrid(source, offsetY, offsetX, (_TCell*)asciiTarget.Characters.begin(), asciiTarget.Characters.metrics().x, asciiTarget.Characters.metrics().y); }
void										drawStateBackground				( SGame& instanceGame )																																				{
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
		static const char format1[]		=
			"Damage Dealt        : %-8.8s - "
			"Damage Taken        : %-8.8s - "
			"Turns Played        : %-6.6s - "
			"Battles Won         : %-6.6s - "
			"Battles Lost        : %-6.6s - "
			"Escapes Succeeded   : %-6.6s"
			//"Escapes Failed      : %-6.6s - "
			;

		static const char format2[]		=
			"Enemies Killed      : %-8.8s - "
			"Attacks Hit         : %-8.8s - "
			"Attacks Missed      : %-6.6s - "
			"Attacks Received    : %-6.6s - "
			"Attacks Avoided     : %-6.6s - "
			"Potions Used        : %-6.6s - "
			"Grenades Used       : %-6.6s"
			;

		static const char format3[]		=
			"Money Earned        : %-8.8s - "
			"Money Spent         : %-8.8s"
			;
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
		sprintf_s(bufferPotionsUsed			, "%lli", (int64_t)deadCharacter.Score.PotionsUsed		);
		sprintf_s(bufferGrenadesUsed		, "%lli", (int64_t)deadCharacter.Score.GrenadesUsed		);

		int32_t messageColor = COLOR_GREEN;
		int32_t offsetX = 4;
		if((curLine+=2) >= 0 && (curLine < bbHeight))
			::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, 0, ::klib::SCREEN_CENTER, "-- %s --", deadCharacter.Name.begin());

		messageColor = COLOR_DARKGREY;
		if((curLine+=2) >= 0 && curLine < bbHeight)
			offsetX = printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, 3, ::klib::SCREEN_LEFT, format1
				, bufferDamageDealt
				, bufferDamageTaken
				, bufferTurnsPlayed
				, bufferBattlesWon
				, bufferBattlesLost
				, bufferEscapesSucceeded
				//, bufferEscapesFailed
				);
		if((curLine+=1) >= 0 && curLine < bbHeight)
			::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, offsetX, ::klib::SCREEN_LEFT, format2
				, bufferEnemiesKilled
				, bufferAttacksHit
				, bufferAttacksMissed
				, bufferAttacksReceived
				, bufferAttacksAvoided
				, bufferPotionsUsed
				, bufferGrenadesUsed
				);
		if((curLine+=1) >= 0 && curLine < bbHeight)
			::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, (uint16_t)messageColor, curLine, offsetX, ::klib::SCREEN_LEFT, format3
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

	::drawStateBackground	(instanceGame);
	::showMenu				(instanceGame);

	drawDisplay(instanceGame.GlobalDisplay.Screen, 0, 0, target);

	uint32_t										bbWidth							= target.Width		();
	uint32_t										bbHeight						= target.Height		();
	uint8_t											* bbChar						= target.Characters	.begin();
	uint16_t										* bbColor						= target.Colors		.begin();

	//drawDisplay(instanceGame.MenuDisplay, 0);
	switch(instanceGame.State.State) {
	//case GAME_STATE_MENU_ACTION:
	case GAME_STATE_MENU_LAN_MISSION	:
	case GAME_STATE_TACTICAL_CONTROL	:
	case GAME_STATE_START_MISSION		: drawDisplay(instanceGame.TacticalDisplay.Screen, TACTICAL_DISPLAY_POSY, (instanceGame.GlobalDisplay.Screen.Width>>1)-(instanceGame.TacticalDisplay.Width>>1), target);									break;
	case GAME_STATE_CREDITS				: drawCredits((char_t*)bbChar, bbWidth, bbHeight, instanceGame.FrameTimer.LastTimeSeconds, namesSpecialThanks, instanceGame.State);																			break;
	case GAME_STATE_MEMORIAL			: drawMemorial((char_t*)bbChar, bbWidth, bbHeight, &instanceGame.GlobalDisplay.TextAttributes.Cells[0][0], instanceGame.FrameTimer.LastTimeSeconds, instanceGame.Players[0].Memorial, instanceGame.State);	break;
	case GAME_STATE_WELCOME_COMMANDER	:
	case GAME_STATE_MENU_SQUAD_SETUP	:
	case GAME_STATE_MENU_EQUIPMENT		: break;
	default:
		drawDisplay(instanceGame.TacticalDisplay.Screen, TACTICAL_DISPLAY_POSY, (instanceGame.GlobalDisplay.Screen.Width >> 1) - (instanceGame.TacticalDisplay.Width >> 1), target);
	}

	memcpy(bbColor, &instanceGame.GlobalDisplay.TextAttributes.Cells[0][0], instanceGame.GlobalDisplay.TextAttributes.Width * instanceGame.GlobalDisplay.TextAttributes.Depth * sizeof(uint16_t));

	switch(instanceGame.State.State) {
	//case GAME_STATE_MENU_ACTION:
	case GAME_STATE_MENU_LAN_MISSION	:
	case GAME_STATE_TACTICAL_CONTROL	:
	case GAME_STATE_START_MISSION		:
		for(uint32_t y = 0; y<instanceGame.TacticalDisplay.TextAttributes.Depth; ++y)
			memcpy(&bbColor[(TACTICAL_DISPLAY_POSY + y) * bbWidth + ((bbWidth >> 1) - (instanceGame.TacticalDisplay.TextAttributes.Width >> 1))], &instanceGame.TacticalDisplay.TextAttributes.Cells[y][0], instanceGame.TacticalDisplay.TextAttributes.Width * sizeof(uint16_t));
		break;
	case GAME_STATE_CREDITS				:
	case GAME_STATE_MEMORIAL			:
	case GAME_STATE_WELCOME_COMMANDER	:
	case GAME_STATE_MENU_SQUAD_SETUP	:
	case GAME_STATE_MENU_EQUIPMENT		: break;
	default:
		for(uint32_t y = 0; y<instanceGame.TacticalDisplay.TextAttributes.Depth; ++y)
			memcpy(&bbColor[(TACTICAL_DISPLAY_POSY+y)*bbWidth+((bbWidth>>1)-(instanceGame.TacticalDisplay.TextAttributes.Width>>1))], &instanceGame.TacticalDisplay.TextAttributes.Cells[y][0], instanceGame.TacticalDisplay.TextAttributes.Width*sizeof(uint16_t));
	}

	// Frame timer
	frameMeasure.Frame();
	instanceGame.FrameTimer.Frame();
	if(instanceGame.FrameTimer.LastTimeSeconds >= 0.150001)
		instanceGame.FrameTimer.LastTimeSeconds		= 0.150001;

	int32_t											actualOffsetX;
	// Print log
	const int32_t									MAX_LOG_LINES					= (instanceGame.State.State == GAME_STATE_WELCOME_COMMANDER || ::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_TACTICAL)) ? 30 : 4;
	int32_t											logSize							= (int32_t)instanceGame.UserLog.size();
	for(uint32_t iLogLine=0, logLineCount = ::gpk::min(MAX_LOG_LINES, logSize); iLogLine< logLineCount; ++iLogLine)
		actualOffsetX								= ::klib::lineToRectColored(target, instanceGame.UserLog[logSize-1-iLogLine].Color, bbHeight-9-iLogLine, 1, ::klib::SCREEN_LEFT, instanceGame.UserLog[logSize-1-iLogLine].Message.c_str());

	// Print some debugging information
	if(::gpk::bit_true(instanceGame.Flags,GAME_FLAGS_HELPON)) {
		int32_t											offsetYHelp						= TACTICAL_DISPLAY_POSY+instanceGame.TacticalDisplay.Depth;
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_YELLOW	, offsetYHelp+18-5, 2, ::klib::SCREEN_RIGHT, "Help!!! Press F1 to hide.");
		//												::
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_ORANGE	, offsetYHelp+20-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Keys:");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_DARKGREY	, offsetYHelp+21-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Up    : Menu cursor up.");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_DARKGREY	, offsetYHelp+22-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Down  : Menu cursor down.");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_DARKGREY	, offsetYHelp+23-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Left  : Previous menu page.");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_DARKGREY	, offsetYHelp+24-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Right : Next menu page.");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_DARKGREY	, offsetYHelp+25-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Enter : Select menu option.");
		//
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_ORANGE	, offsetYHelp+27-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Mouse:");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_DARKGREY	, offsetYHelp+28-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Left Click  : Select menu option");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_DARKGREY	, offsetYHelp+29-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Left Click  : Select move destination");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_DARKGREY	, offsetYHelp+30-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Left Click  : Select attack target");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_DARKGREY	, offsetYHelp+31-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Right Click : Return to the previous screen.");
		//												::
		actualOffsetX								= ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5)? COLOR_CYAN	:	COLOR_DARKBLUE		, offsetYHelp+33-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "\x0C, \x0B: Blue characters are your own squad.");
		actualOffsetX								= ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5)? COLOR_MAGENTA	:	COLOR_DARKMAGENTA	, offsetYHelp+34-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "\x0C, \x0B: Magenta characters are your ally squads.");
		actualOffsetX								= ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5)? COLOR_RED		:	COLOR_DARKRED		, offsetYHelp+35-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "\x0C, \x0B: Red characters are your enemies.");
		actualOffsetX								= ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5)? COLOR_ORANGE	:	COLOR_DARKYELLOW	, offsetYHelp+36-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "\x04: Money is picked up when you walk over it.");
		actualOffsetX								= ::klib::lineToRectColored(target, (animTimerAccum.Value > 0.5)? COLOR_WHITE	:	COLOR_DARKGREY		, offsetYHelp+37-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "\xB9: Obstacles turn gray when destroyed.");
		//												::
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_YELLOW		, offsetYHelp+39-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "Melee attacks and grenades currently require ");
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_YELLOW		, offsetYHelp+40-5, bbWidth/4*3, ::klib::SCREEN_LEFT, "a valid target selected in order to work!");
	}
	else
		actualOffsetX								= ::klib::lineToRectColored(target, COLOR_YELLOW, TACTICAL_DISPLAY_POSY+instanceGame.TacticalDisplay.Depth+18-5, 2, ::klib::SCREEN_RIGHT, "Press F1 to display help.");

	actualOffsetX								= ::klib::printfToRectColored(target, COLOR_DARKGREEN	, bbHeight-4, 1, ::klib::SCREEN_LEFT, "Mouse: %i, %i."			, instanceGame.FrameInput.Mouse.Deltas.x, instanceGame.FrameInput.Mouse.Deltas.y);
	actualOffsetX								= ::klib::printfToRectColored(target, COLOR_GREEN		, bbHeight-3, 1, ::klib::SCREEN_LEFT, "Frame time: %.5f seconds."	, instanceGame.FrameTimer.LastTimeSeconds	);
	actualOffsetX								= ::klib::printfToRectColored(target, COLOR_CYAN		, bbHeight-2, 1, ::klib::SCREEN_LEFT, "Frames last second: %f."	, instanceGame.FrameTimer.FramesLastSecond	);
	time_t											curTimeWithUnreliableSize		= 0;
	{
		::gpk::mutex_guard								thelock							(instanceGame.ServerTimeMutex);
		curTimeWithUnreliableSize					= instanceGame.ServerTime;
	}
	char											send_buffer[64]					= {};
	ctime_s(send_buffer, sizeof(send_buffer), &curTimeWithUnreliableSize);

	::std::string									serverTime						= ::std::string("Server time: ") + send_buffer;
	serverTime									= serverTime.substr(0, serverTime .size()-2);
	actualOffsetX								= ::klib::printfToRectColored((char_t*)bbChar, bbWidth, bbHeight, bbColor, COLOR_CYAN		, bbHeight-2, 1, ::klib::SCREEN_RIGHT, "%s."	, serverTime.c_str());
	actualOffsetX								= ::klib::printfToRectColored((char_t*)bbChar, bbWidth, bbHeight, bbColor, COLOR_DARKGREY	, bbHeight-1, 1, ::klib::SCREEN_CENTER, "%s."	, instanceGame.StateMessage.c_str());

	// Print user error messages and draw cursor.
	if(instanceGame.State.State != GAME_STATE_CREDITS) {
		int32_t											finalMissionCost				= ::klib::missionCost(instanceGame.Players[PLAYER_INDEX_USER], instanceGame.Players[PLAYER_INDEX_USER].Squad, instanceGame.Players[PLAYER_INDEX_USER].Squad.Size);
		int32_t											playerFunds						= instanceGame.Players[PLAYER_INDEX_USER].Money;
		::klib::printfToRectColored(target, (finalMissionCost > playerFunds) ? COLOR_ORANGE : COLOR_CYAN	, bbHeight-5, 1, ::klib::SCREEN_RIGHT, "Squad size: %i."	, instanceGame.Players[PLAYER_INDEX_USER].Squad.Size);
		::klib::printfToRectColored(target, (finalMissionCost > playerFunds) ? COLOR_RED : COLOR_ORANGE		, bbHeight-4, 1, ::klib::SCREEN_RIGHT, "Mission cost: %i"	, finalMissionCost);
		::klib::printfToRectColored(target, (playerFunds < 0) ? COLOR_RED : COLOR_YELLOW					, bbHeight-3, 1, ::klib::SCREEN_RIGHT, "Funds: %i"			, playerFunds);

		::klib::printfToRectColored(target, COLOR_YELLOW	, bbHeight-5, 0, ::klib::SCREEN_CENTER, "%s", instanceGame.UserMessage.c_str());
		::klib::printfToRectColored(target, COLOR_CYAN		, bbHeight-4, 0, ::klib::SCREEN_CENTER, "%s", instanceGame.UserSuccess.c_str());
		::klib::printfToRectColored(target, COLOR_RED		, bbHeight-3, 0, ::klib::SCREEN_CENTER, "%s", instanceGame.UserMiss.c_str());
		::klib::printfToRectColored(target, COLOR_RED		, bbHeight-2, 0, ::klib::SCREEN_CENTER, "%s", instanceGame.UserError.c_str());

		// Draw cursor
		//int32_t mouseX = instanceGame.FrameInput.MouseX, mouseY = instanceGame.FrameInput.MouseY;
		//bbColor[mouseY*bbWidth+mouseX] =
		//	( ((bbColor[mouseY*bbWidth+mouseX] & 0xF0) >> 4)
		//	| ((bbColor[mouseY*bbWidth+mouseX] & 0x0F) << 4)
		//	);
	}

	// increase coins
	static ::klib::STimer							profitTimer;
	static ::klib::SAccumulator<double>				profitTimerAccum				= {0.0, 3.0};

	profitTimer.Frame();
	if( profitTimerAccum.Accumulate(profitTimer.LastTimeSeconds) ) {
		instanceGame.Players[PLAYER_INDEX_USER].Money++;
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

void										drawIntro						( SGame& instanceGame ) {
	drawFireBackground(instanceGame.TacticalDisplay, instanceGame.FrameTimer.LastTimeSeconds);
	int32_t											displayDepth					= (int32_t)instanceGame.TacticalDisplay.Depth;

	static const	::std::string					words			[]				= {"Vulgar", "Display", "of", "Power"};
	for( uint32_t i=0; i < ::gpk::size(words); ++i) {
		uint32_t										offsetY							= (uint32_t)((displayDepth >> 1)-(::gpk::size(words) >> 1) + i * 2);
		//uint32_t										offsetX							=
			printfToGridColored(instanceGame.TacticalDisplay.Screen, instanceGame.TacticalDisplay.TextAttributes, COLOR_ORANGE, offsetY, 0, ::klib::SCREEN_CENTER, "%s", words[i].c_str());
	}
}


char										klib::getASCIIWall				(const ::gpk::view_grid<const STileProp>& propGrid, int32_t x, int32_t z) {
	::klib::SASCIIWallConnection					connection						= {false};

	char											result							= '-';
	static const ::gpk::label						labelWall						= "Wall";

	bool											bIsReinforced					= propGrid[z][x].Modifier > 0;

	if(x - 1 >= 0								&& propGrid[z    ][x - 1].Definition != -1 && definitionsStageProp[propGrid[z    ][x - 1].Definition].Name == labelWall) { connection.Left		= true;	if(propGrid[z    ][x - 1].Modifier > 0) connection.ReinforcedLeft	= true; }
	if(x + 1 < (int32_t)propGrid.metrics().x	&& propGrid[z    ][x + 1].Definition != -1 && definitionsStageProp[propGrid[z    ][x + 1].Definition].Name == labelWall) { connection.Right		= true;	if(propGrid[z    ][x + 1].Modifier > 0) connection.ReinforcedRight	= true; }
	if(z - 1 >= 0								&& propGrid[z - 1][x    ].Definition != -1 && definitionsStageProp[propGrid[z - 1][x    ].Definition].Name == labelWall) { connection.Top		= true;	if(propGrid[z - 1][x    ].Modifier > 0) connection.ReinforcedTop	= true; }
	if(z + 1 < (int32_t)propGrid.metrics().y	&& propGrid[z + 1][x    ].Definition != -1 && definitionsStageProp[propGrid[z + 1][x    ].Definition].Name == labelWall) { connection.Bottom	= true;	if(propGrid[z + 1][x    ].Modifier > 0) connection.ReinforcedBottom	= true; }

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

uint16_t									klib::getPlayerColor			( const STacticalInfo& tacticalInfo, const SPlayer& boardPlayer, int8_t indexBoardPlayer, int8_t indexPlayerViewer, bool bIsSelected )	{
	uint16_t										color							= COLOR_BLACK;
	if(tacticalInfo.Setup.TeamPerPlayer[indexBoardPlayer] == tacticalInfo.Setup.TeamPerPlayer[indexPlayerViewer]) {
		if(indexBoardPlayer == indexPlayerViewer)
			color								= (bIsSelected) ? COLOR_CYAN :COLOR_DARKBLUE;
		else
			color								= (bIsSelected) ? COLOR_MAGENTA :COLOR_DARKMAGENTA;
	}
	else {
		switch(boardPlayer.Control.Type) {
		case PLAYER_CONTROL_REMOTE	:
		case PLAYER_CONTROL_LOCAL	: color		= (bIsSelected) ? COLOR_RED : COLOR_DARKRED;
			break;

		case PLAYER_CONTROL_AI:
			switch(boardPlayer.Control.AIMode) {
			case PLAYER_AI_NEUTRAL		: color		= bIsSelected ? COLOR_DARKGREY	: COLOR_DARKGREY	; break;
			case PLAYER_AI_FEARFUL		: color		= bIsSelected ? COLOR_DARKGREY	: COLOR_DARKGREY	; break;
			case PLAYER_AI_CURIOUS		: color		= bIsSelected ? COLOR_DARKGREY	: COLOR_DARKGREY	; break;
			case PLAYER_AI_ASSISTS		: color		= bIsSelected ? COLOR_WHITE		: COLOR_GREEN		; break;
			case PLAYER_AI_RIOTERS		: color		= bIsSelected ? COLOR_YELLOW	: COLOR_ORANGE		; break;
			case PLAYER_AI_VIOLENT		: color		= bIsSelected ? COLOR_YELLOW	: COLOR_ORANGE		; break;
			case PLAYER_AI_TEAMERS		: color		= bIsSelected ? COLOR_RED		: COLOR_DARKRED		; break;
			}
		}
	}
	return color;
}

uint16_t									klib::getStatusColor			( COMBAT_STATUS status, bool bSwap, uint16_t defaultColor )																																	{
	static SStatusColor								statusColors	[32];
	static const int32_t							initedColors			= initStatusColors(statusColors);

	uint32_t										bitIndex				= (uint32_t)-1;

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
	if(entityStatus.Inflict		)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityStatus.Inflict	, MAX_COMBAT_STATUS_COUNT, COLOR_YELLOW, COLOR_RED	, "- Inflicts: %-14.14s");
	if(entityStatus.Immunity	)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityStatus.Immunity	, MAX_COMBAT_STATUS_COUNT, COLOR_YELLOW, COLOR_CYAN	, "- Immunity: %-14.14s");

	return iLine;
}

static int32_t							displayEntityEffect				(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, const SEntityEffect& entityEffect)										{
	int32_t										iLine							= 0;
	if(entityEffect.Attack	)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityEffect.Attack	, MAX_ATTACK_EFFECT_COUNT, COLOR_YELLOW, COLOR_RED	, "- Attack effect: %-14.14s"	);
	if(entityEffect.Defend	)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityEffect.Defend	, MAX_DEFEND_EFFECT_COUNT, COLOR_YELLOW, COLOR_CYAN	, "- Defend effect: %-14.14s"	);
	if(entityEffect.Passive	)	iLine += displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityEffect.Passive	, MAX_PASSIVE_EFFECT_COUNT, COLOR_YELLOW, COLOR_GREEN	, "- Passive effect: %-14.14s"	);

	return iLine;
}

static int32_t							displayEntityTechnology			(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, const SEntityGrade& entityTech)											{
	int32_t										iLine							= 0;
	if(entityTech.Tech				) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.Tech				, MAX_ENTITY_TECHNOLOGY_COUNT, COLOR_YELLOW, COLOR_GREEN	, "- Technology: %-14.14s"	);
	if(entityTech.Grade				) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.Grade				, 3, COLOR_YELLOW, COLOR_GREEN	, "- Grade: %-14.14s"		);
	if(entityTech.AttackType		) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.AttackType			, MAX_ATTACK_TYPE_COUNT, COLOR_YELLOW, COLOR_RED		, "- Attack type: %-14.14s"	);
	if(entityTech.ProjectileClass	) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.ProjectileClass	, MAX_PROJECTILE_CLASS_COUNT, COLOR_YELLOW, COLOR_RED	, "- Projectile Class: %-14.14s" );
	if(entityTech.AmmoEffect		) iLine	+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, entityTech.AmmoEffect			, MAX_AMMO_EFFECT_COUNT, COLOR_YELLOW, COLOR_RED		, "- Ammo Effect: %-14.14s"	);

	return iLine;
}

void									klib::displayStatusEffectsAndTechs	(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, CCharacter& character)													{
	int32_t										iLine							= 0;
	if(character.Flags.Tech.Gender)
		iLine									+= displayFlag(display, textAttributes, {offsetX, offsetY+iLine}, character.Flags.Tech.Gender, 2	, COLOR_YELLOW, COLOR_GREEN	, "- Gender: %-13.13s");
	iLine									+= displayEntityTechnology	(display, textAttributes, offsetY+iLine, offsetX, character.FinalFlags.Tech		);
	iLine									+= displayEntityStatus		(display, textAttributes, offsetY+iLine, offsetX, character.FinalFlags.Status	);
	iLine									+= displayEntityEffect		(display, textAttributes, offsetY+iLine, offsetX, character.FinalFlags.Effect	);
}

static void								displayEmptySlot				(::gpk::view_grid<char>& display, ::gpk::view_grid<uint16_t>& textAttributes, int32_t offsetY, int32_t offsetX, int32_t agentIndex)																			{
	static const size_t							LINE_SIZE						= 30;
	uint16_t									color							= COLOR_GREEN;
	printfToGridColored(display, textAttributes, color, offsetY, offsetX, ::klib::SCREEN_LEFT, "-- Agent #%i: %-14.14s --", agentIndex, "Open position");
	valueToGrid(textAttributes, offsetY, offsetX+13, ::klib::SCREEN_LEFT, &(color = COLOR_DARKCYAN), 1, LINE_SIZE-14);
}

static void								displayResumedAgentSlot			(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, int32_t agentIndex, CCharacter& character)													{

	static const char							formatAgentTitle	[]			= "-- Agent #%i:" " %-34.34s --"	;
	static const char							formatAgentCoins	[]			= "%-21.21s: %-11.11s"				;
	static const char							formatAgentPoints	[]			= "%-21.21s: %-10.10s"				;
	static const char							formatAgentEquip	[]			= "%-10.10s: %-36.36s Lv. %i"		;

	static const size_t							LINE_SIZE						= 56;
	uint16_t									color							= COLOR_GREEN;
	::klib::printfToGridColored(display, textAttributes, color, offsetY, offsetX, ::klib::SCREEN_LEFT,  formatAgentTitle, agentIndex, character.Name.c_str());
	::klib::valueToGrid(textAttributes, offsetY, offsetX+13, ::klib::SCREEN_LEFT, &color, 1, LINE_SIZE-14);
	offsetY									+= 2;

	std::string									equipName;
	equipName	 = getProfessionName	(character.CurrentEquip.Profession	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Class"		, equipName.c_str(), character.CurrentEquip.Profession	.Level);
	equipName	 = getWeaponName		(character.CurrentEquip.Weapon		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Weapon"	, equipName.c_str(), character.CurrentEquip.Weapon		.Level);
	equipName	 = getArmorName			(character.CurrentEquip.Armor		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Armor"		, equipName.c_str(), character.CurrentEquip.Armor		.Level);
	equipName	 = getAccessoryName		(character.CurrentEquip.Accessory	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Accessory"	, equipName.c_str(), character.CurrentEquip.Accessory	.Level);

	::klib::lineToGridColored(display, textAttributes, COLOR_RED, ++offsetY, offsetX, ::klib::SCREEN_LEFT, "- Final Points:");
	offsetY									+=2;

	const SEntityPoints							& agentFinalPoints				= character.FinalPoints;
	char										formattedGauge[32];

	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Health	), agentFinalPoints.LifeMax.Health	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Health"	, formattedGauge);
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Shield	), agentFinalPoints.LifeMax.Shield	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Shield"	, formattedGauge);
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Mana	), agentFinalPoints.LifeMax.Mana	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Mana"	, formattedGauge);

	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Hit						); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Hit Chance"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Damage					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Damage"					, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Health		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Health"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Shield		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Shield"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Mana			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Mana"		, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Absorption				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Absorption"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Range						); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Range"					, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Attack					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Attack Speed"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Movement					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Movement Speed"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Reflexes					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Reflexes"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Sight					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Sight"					, formattedGauge);

	const SEntityPoints							& agentBasePoints				= character.Points;
	sprintf_s(formattedGauge, "%i", agentBasePoints.Coins				); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins in wallet"		, formattedGauge);
	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
	sprintf_s(formattedGauge, "%i", agentFinalPoints.Coins				); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins per turn"		, formattedGauge);
	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (agentFinalPoints.Coins >= 0) ? COLOR_ORANGE : COLOR_RED), 1, 11);
	sprintf_s(formattedGauge, "%i", agentBasePoints.CostMaintenance		); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Salary"				, formattedGauge);
	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
	sprintf_s(formattedGauge, "%i", agentFinalPoints.CostMaintenance	); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Total Cost"			, formattedGauge);
	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
}

void								klib::displayDetailedAgentSlot		(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, const CCharacter& character, uint16_t color)										{
	static const char							formatAgentTitle	[]			= " - %-34.34s"			;
	static const char							formatAgentEquip	[]			= "%-36.36s Lv. %i"		;
	static const char							formatAgentPoints	[]			= "%-21.21s: %-10.10s"	;
	static const char							formatAgentCoins	[]			= "%-21.21s: %-11.11s"	;

	printfToGridColored(display, textAttributes, color, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentTitle, character.Name.c_str());
	offsetY									+=1;

	std::string									equipName;
	equipName	= getProfessionName	(character.CurrentEquip.Profession		); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.c_str(), character.CurrentEquip.Profession	.Level);
	equipName	= getWeaponName		(character.CurrentEquip.Weapon			); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.c_str(), character.CurrentEquip.Weapon		.Level);
	equipName	= getArmorName		(character.CurrentEquip.Armor			); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.c_str(), character.CurrentEquip.Armor		.Level);
	equipName	= getAccessoryName	(character.CurrentEquip.Accessory		); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.c_str(), character.CurrentEquip.Accessory	.Level);

	lineToGridColored(display, textAttributes, COLOR_RED, ++offsetY, offsetX, ::klib::SCREEN_LEFT, "- Final Points:");
	offsetY									+=2;

	const SEntityPoints							& agentFinalPoints				= character.FinalPoints;
	char										formattedGauge[32];
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Health	), agentFinalPoints.LifeMax.Health	); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Health"	, formattedGauge);
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Shield	), agentFinalPoints.LifeMax.Shield	); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Shield"	, formattedGauge);
	sprintf_s(formattedGauge, "%i/%i"	, ::gpk::max(0, character.Points.LifeCurrent.Mana	), agentFinalPoints.LifeMax.Mana	); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Mana"		, formattedGauge);
	++offsetY;	//

	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.LifeCurrent.Health			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Health per turn"		, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.LifeCurrent.Shield			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Shield per turn"		, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.LifeCurrent.Mana				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Mana per turn"			, formattedGauge);
	++offsetY;	//

	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Hit					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Hit Chance"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Damage				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Damage"					, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Health	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Health"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Shield	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Shield"	, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Mana		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Mana"		, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Absorption			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Absorption"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Range					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Range"					, formattedGauge);
	++offsetY;	//

	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Attack				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Attack Speed"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Movement				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Movement Speed"			, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Reflexes				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Reflexes"				, formattedGauge);
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Sight				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Sight"					, formattedGauge);

	const SEntityPoints							& agentBasePoints				= character.Points;
	sprintf_s(formattedGauge, "%i"		, agentBasePoints.Coins							); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins in wallet"	, formattedGauge); valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);	//
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Coins						); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins per turn"	, formattedGauge); valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (agentFinalPoints.Coins >= 0) ? COLOR_ORANGE : COLOR_RED), 1, 11);	//
	sprintf_s(formattedGauge, "%i"		, agentBasePoints.CostMaintenance				); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Salary"			, formattedGauge); valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);	//
	sprintf_s(formattedGauge, "%i"		, agentFinalPoints.CostMaintenance				); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Total Cost"		, formattedGauge); valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
}

void									klib::displayAgentSlot					(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, int32_t agentIndex, CCharacter& character, bool bShort, uint16_t color)	{
	if( bShort )
		displayResumedAgentSlot		(display, textAttributes, offsetY, offsetX, agentIndex, character);
	else
		displayDetailedAgentSlot	(display, textAttributes, offsetY, offsetX, character, color);
}

void									klib::drawSquadSlots					(SGame& instanceGame)																																						{
	SGlobalDisplay								& display								= instanceGame.GlobalDisplay;
	static const int32_t						slotWidth								= display.Width / MAX_AGENT_COLUMNS;
	static const int32_t						slotRowSpace							= 30;// display.Depth / (MAX_AGENT_ROWS);

	static const int32_t						offsetYBase								= 2;
	static const int32_t						offsetXBase								= 3;

	SPlayer										& player								= instanceGame.Players[PLAYER_INDEX_USER];
	int32_t										playerOffset							= (player.Selection.PlayerUnit != -1) ? ::gpk::min(::gpk::max(0, player.Selection.PlayerUnit-5), (int16_t)::gpk::size(player.Squad.Agents)-6) : 0;

	bool										bStop									= false;
	for(int32_t y = 0, countY=MAX_AGENT_ROWS; y < countY; ++y) {
		for(int32_t x = 0, countX=MAX_AGENT_COLUMNS; x < countX; ++x)  {
			int32_t										linearIndex								= y*countX+x;
			if(linearIndex >= player.Squad.Size) {
				bStop									= true;
				break;
			}
			int32_t										agentIndexOffset						= linearIndex+playerOffset;
			if(agentIndexOffset < (int32_t)::gpk::size(player.Squad.Agents))  {
				if( player.Squad.Agents[agentIndexOffset] != -1 )
					::displayAgentSlot(display.Screen, display.TextAttributes, offsetYBase+slotRowSpace*y, offsetXBase+slotWidth*x, agentIndexOffset+1, *player.Army[player.Squad.Agents[agentIndexOffset]], true);
				else
					::displayEmptySlot(display.Screen, display.TextAttributes, offsetYBase+slotRowSpace*y, offsetXBase+slotWidth*x, agentIndexOffset+1);
			}
		}
		if(bStop)
			break;
	}
}

