#include "draw.h"
#include "credits.h"
#include "tactical_draw.h"
#include "Agent_helper.h"

using namespace klib;

void drawIntro(SGame& instanceGame);

void										klib::printMultipageHelp										(char* targetASCII, uint32_t targetWidth, uint32_t targetHeight, uint32_t currentPage, uint32_t pageCount, uint32_t posXOffset)					{
	static const ::gpk::label						textToShow[3]										= 
		{	"Page down: Next page."						
		,	"Page up: Previous page."						
		,	"Page up: Previous page. Page down: Next page"	
		};
	::gpk::label									selectedText;
		 if(currentPage == 0)				selectedText	= textToShow[0];	
	else if(currentPage == (pageCount-1))	selectedText	= textToShow[1];	
	else									selectedText	= textToShow[2];	
	::klib::lineToRect(targetASCII, targetWidth, targetHeight, (int32_t)targetHeight-MENU_ROFFSET+1, posXOffset, ::klib::SCREEN_CENTER, selectedText.begin());
}

// Currently what this function is lacking is the ability to receive negative offsets.
template<typename _TCell, size_t _Width, size_t _Depth>
void										blitGrid						(::klib::SGrid<_TCell, _Width, _Depth>& source, int32_t offsetY, uint32_t offsetX, _TCell* target, size_t targetWidth, size_t targetHeight, int32_t rowPitch=-1)	{
	size_t											actualWidth						= ::gpk::min(_Width, ::gpk::max((size_t)0, targetWidth-offsetX));
	if(rowPitch < 0)
		rowPitch									= (int32_t)targetWidth;

	for(int32_t z = 0, maxZ = (int32_t)::gpk::min(_Depth, targetHeight-offsetY); z < maxZ; ++z) {
		if( (offsetY+z) < 0 )
			continue;

		memcpy(&target[(offsetY+z)*rowPitch+offsetX], &source.Cells[z][0], actualWidth);
	}
}

template<typename _TCell, size_t _Width, size_t _Depth>
void										drawDisplay						(::klib::SGrid<_TCell, _Width, _Depth>& source, uint32_t offsetY, uint32_t offsetX, ::klib::SASCIITarget& asciiTarget)												{ blitGrid(source, offsetY, offsetX, (_TCell*)asciiTarget.Characters.begin(), asciiTarget.Characters.metrics().x, asciiTarget.Characters.metrics().y); }
void										drawStateBackground				( SGame& instanceGame )																																				{
	switch(instanceGame.State.State) {
	case	GAME_STATE_MENU_MAIN		:	drawIntro(instanceGame);																	; break;
	case	GAME_STATE_CREDITS			:	//drawSnowBackground(instanceGame.GlobalDisplay, instanceGame.FrameTimer.LastTimeSeconds);	; break;
	case	GAME_STATE_MEMORIAL			:	drawSnowBackground(instanceGame.GlobalDisplay, instanceGame.FrameTimer.LastTimeSeconds);	; break;
	case	GAME_STATE_MENU_SQUAD_SETUP	:	//drawRainBackground(instanceGame.GlobalDisplay, instanceGame.FrameTimer.LastTimeSeconds);	; break;
	case	GAME_STATE_MENU_EQUIPMENT	:	drawRainBackground(instanceGame.GlobalDisplay, instanceGame.FrameTimer.LastTimeSeconds);	; break;
	}
}

SGameState									drawMemorial					(SGame& instanceGame, const SGameState& returnState)																												{ return returnState; }
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
		drawDisplay(instanceGame.PostEffectDisplay.Screen, TACTICAL_DISPLAY_POSY, (instanceGame.GlobalDisplay.Screen.Width >> 1) - (instanceGame.PostEffectDisplay.Width >> 1), target);
	}

	memcpy(bbColor, &instanceGame.GlobalDisplay.TextAttributes.Cells[0][0], instanceGame.GlobalDisplay.TextAttributes.Width * instanceGame.GlobalDisplay.TextAttributes.Depth * sizeof(uint16_t));

	switch(instanceGame.State.State) { 
	//case GAME_STATE_MENU_ACTION:
	case GAME_STATE_MENU_LAN_MISSION	:
	case GAME_STATE_TACTICAL_CONTROL	:
	case GAME_STATE_START_MISSION		:
		for(uint32_t y = 0; y<instanceGame.PostEffectDisplay.TextAttributes.Depth; ++y)
			memcpy(&bbColor[(TACTICAL_DISPLAY_POSY + y) * bbWidth + ((bbWidth >> 1) - (instanceGame.TacticalDisplay.TextAttributes.Width >> 1))], &instanceGame.TacticalDisplay.TextAttributes.Cells[y][0], instanceGame.TacticalDisplay.TextAttributes.Width * sizeof(uint16_t));
		break;
	case GAME_STATE_CREDITS				:
	case GAME_STATE_MEMORIAL			:
	case GAME_STATE_WELCOME_COMMANDER	:
	case GAME_STATE_MENU_SQUAD_SETUP	:
	case GAME_STATE_MENU_EQUIPMENT		: break;
	default:
		for(uint32_t y = 0; y<instanceGame.PostEffectDisplay.TextAttributes.Depth; ++y)
			memcpy(&bbColor[(TACTICAL_DISPLAY_POSY+y)*bbWidth+((bbWidth>>1)-(instanceGame.PostEffectDisplay.TextAttributes.Width>>1))], &instanceGame.PostEffectDisplay.TextAttributes.Cells[y][0], instanceGame.PostEffectDisplay.TextAttributes.Width*sizeof(uint16_t));
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
	for(uint32_t iLogLine=0, logLineCount = std::min(MAX_LOG_LINES, logSize); iLogLine< logLineCount; ++iLogLine)
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
	drawFireBackground(instanceGame.PostEffectDisplay, instanceGame.FrameTimer.LastTimeSeconds);

	int32_t											displayWidth					= (int32_t)instanceGame.PostEffectDisplay.Width;
	int32_t											displayDepth					= (int32_t)instanceGame.PostEffectDisplay.Depth;

	static ::std::string							words			[]				= {"Vulgar", "Display", "of", "Power"};
	for( uint32_t i=0; i < ::gpk::size(words); ++i) {
		uint32_t										offsetY							= (uint32_t)((displayDepth >> 1)-(::gpk::size(words) >> 1) + i * 2);
		uint32_t										offsetX							= printfToGridColored(instanceGame.PostEffectDisplay.Screen, instanceGame.PostEffectDisplay.TextAttributes, COLOR_ORANGE, offsetY, 0, ::klib::SCREEN_CENTER, "%s", words[i].c_str());
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


void									klib::drawSquadSlots					(SGame& instanceGame)																																						{
	SGlobalDisplay								& display								= instanceGame.GlobalDisplay;
	static const int32_t						slotWidth								= display.Width / MAX_AGENT_COLUMNS;
	static const int32_t						slotRowSpace							= 30;// display.Depth / (MAX_AGENT_ROWS);

	static const int32_t						offsetYBase								= 2;
	static const int32_t						offsetXBase								= 5;

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
					::klib::displayAgentSlot(display, offsetYBase+slotRowSpace*y, offsetXBase+slotWidth*x, agentIndexOffset+1, *player.Army[player.Squad.Agents[agentIndexOffset]], true);
				else											 
					::klib::displayEmptySlot(display, offsetYBase+slotRowSpace*y, offsetXBase+slotWidth*x, agentIndexOffset+1);
			}
		}
		if(bStop)
			break;
	}
}
