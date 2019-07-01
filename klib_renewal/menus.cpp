//#define NOMINMAX

#include "Game.h"
#include "draw.h"
#include "tactical_draw.h"

#include <algorithm>

SDrawMenuGlobals	drawMenu_globals = {{}, {0, 0.30}};

using namespace klib;

CONSOLE_FONT_INFOEX getFontParams()
{
	CONSOLE_FONT_INFOEX								infoFont				= {sizeof(CONSOLE_FONT_INFOEX)};
	infoFont.FontFamily							= 0;
	infoFont.FontWeight							= 0;
	infoFont.nFont								= 0;
	::wcscpy_s(infoFont.FaceName, L"Terminal");
	return infoFont;
}

SGameState processMenuReturn(SGame& instanceGame, const SGameState& returnValue_)
{
	SGameState returnValue;
	static bool bSmallFonts = false;

	static const HANDLE hConsoleOut = ::GetStdHandle( STD_OUTPUT_HANDLE );
	static CONSOLE_FONT_INFOEX infoFont = ::getFontParams();

	switch(returnValue_.State) { 
	case GAME_STATE_MENU_MAIN:
	default: return returnValue_;

	case GAME_STATE_MENU_OPTIONS:
		switch(returnValue_.Substate) {
		default: return returnValue_;
		case GAME_SUBSTATE_SCREEN:
			bSmallFonts = !bSmallFonts;
			if(bSmallFonts) {
				infoFont.dwFontSize.X	= 6;
				infoFont.dwFontSize.Y	= 8;
			}
			else {
				infoFont.dwFontSize.X	= 8;
				infoFont.dwFontSize.Y	= 12;
			}
			SetCurrentConsoleFontEx(hConsoleOut, FALSE, &infoFont);
			returnValue = returnValue_;
			returnValue.Substate = GAME_SUBSTATE_MAIN;
			break;
		}
	}
	
	return returnValue;
}

SGameState processMenuReturn(SGame& instanceGame, TURN_ACTION returnValue) {
	switch(returnValue) { 
	case TURN_ACTION_CONTINUE:
	default: return {GAME_STATE_TACTICAL_CONTROL, };
	}
}

void handleSubstateChange(SGame& instanceGame, const SGameState& newState, const SGameState& prevState) 
{
	::klib::clearASCIIBackBuffer(' ', COLOR_WHITE);
	instanceGame.GlobalDisplay		.Clear();
	//instanceGame.TacticalDisplay	.Clear();	
	//instanceGame.PostEffectDisplay	.Clear();	
	::klib::clearGrid(instanceGame.MenuDisplay);

	switch(newState.State) {
	case GAME_STATE_MENU_OPTIONS:
	case GAME_STATE_MENU_SELL:
	case GAME_STATE_MENU_UPGRADE:
	case GAME_STATE_MENU_FACTORY:
		instanceGame.UserError = "This function isn't available!";
		instanceGame.LogError();
	}

	//resetCursorString(instanceGame.SlowMessage);	we shuold leave this out unless it becomes a need. This is because it turns screen transitions into an annoyance.
}

void drawTacticalMap(SGame& instanceGame, SPostEffectDisplay& target)
{
	klib::drawTacticalBoard(instanceGame, instanceGame.TacticalInfo, target, PLAYER_INDEX_USER, TEAM_TYPE_CIVILIAN, instanceGame.Players[PLAYER_INDEX_USER].Selection, false);
}

void handleProductionStep(SGame& instanceGame);
void handleResearchStep(SGame& instanceGame);

void handleMissionEnd(SGame& instanceGame)
{
	SPlayer&			playerUser		= instanceGame.Players[PLAYER_INDEX_USER];
	SPlayerProjects&	playerProjects	= playerUser.Projects;

	if(playerProjects.QueuedProduction.size())
		handleProductionStep(instanceGame);

	if(playerProjects.QueuedResearch.size())
		handleResearchStep(instanceGame);
}

void handleStateChange(SGame& instanceGame, const SGameState& newState, const SGameState& prevState)
{
	::klib::clearASCIIBackBuffer(' ', COLOR_WHITE);
	instanceGame.ClearDisplays();
	::klib::resetCursorString(instanceGame.SlowMessage);

	SPlayer& playerUser = instanceGame.Players[PLAYER_INDEX_USER];
	int64_t playCost = 0;
	switch(newState.State)
	{
	case GAME_STATE_MENU_MAIN:
		drawTacticalMap(instanceGame, instanceGame.PostEffectDisplay);
		instanceGame.StateMessage = "Main Menu";
		break;

	case GAME_STATE_WELCOME_COMMANDER:
		if(newState.Substate == GAME_SUBSTATE_RESET) {
			instanceGame.StateMessage = "Welcome commander";
			resetGame(instanceGame);
			handleSubstateChange(instanceGame, newState, prevState);
		}
		else {
			instanceGame.StateMessage = "Welcome back commander";
			if(::gpk::bit_false(instanceGame.Flags, GAME_FLAGS_TACTICAL) && (prevState.State == GAME_STATE_TACTICAL_CONTROL || prevState.State == GAME_STATE_START_MISSION))
				handleMissionEnd(instanceGame);
		}
		break;

	case GAME_STATE_TACTICAL_CONTROL	: instanceGame.StateMessage = "Tactical control"	; break;
	case GAME_STATE_START_MISSION		: 
		playCost			= ::klib::missionCost(playerUser, playerUser.Squad, playerUser.Squad.Size);
		playerUser.Money	-= (int32_t)playCost;
		instanceGame.StateMessage = "Start mission"		; 
		break;
	case GAME_STATE_MENU_LAN_MISSION	: instanceGame.StateMessage = "LAN mission setup"	; break;
	case GAME_STATE_MENU_OPTIONS		: instanceGame.StateMessage = "Options"				; break;
	case GAME_STATE_MENU_BUY			: instanceGame.StateMessage = "Buy"					; break;
	case GAME_STATE_MENU_SELL			: instanceGame.StateMessage = "Sell"				; break;
	case GAME_STATE_MENU_UPGRADE		: instanceGame.StateMessage = "Upgrade"				; break;
	case GAME_STATE_MENU_FACTORY		: instanceGame.StateMessage = "Factory"				; break;
	case GAME_STATE_MENU_SQUAD_SETUP	: instanceGame.StateMessage = "Squad Setup"			; break;
	case GAME_STATE_CREDITS				: instanceGame.StateMessage = "Credits"				; break;
	case GAME_STATE_MENU_EQUIPMENT		: instanceGame.StateMessage	= "Equipment Setup"		; break;
	case GAME_STATE_MENU_RESEARCH		: instanceGame.StateMessage = "Research Center"		; break;
	default:
		break;
	}

	if(prevState.State != GAME_STATE_START_MISSION && prevState.State != GAME_STATE_TACTICAL_CONTROL)
		instanceGame.ClearMessages();
}

void updateState(SGame& instanceGame, const SGameState& newState) 
{
	if(newState.State == GAME_STATE_START_MISSION && newState.State != instanceGame.State.State)
	{
		SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER];
		int64_t playCost = ::klib::missionCost(player, player.Squad, player.Squad.Size);
		
		if( player.Money < playCost )
		{
			instanceGame.UserError = "You don't have enough money to start a mission with the current squad setup.";
			instanceGame.LogError();
			//return;
		}
	}
	else if(newState.State == GAME_STATE_WELCOME_COMMANDER || newState.State == GAME_STATE_MENU_CONTROL_CENTER)
	{
	}

	instanceGame.PreviousState = instanceGame.State;

	//
	if(newState.State != instanceGame.State.State) {
		instanceGame.State			= newState;
		handleStateChange(instanceGame, instanceGame.State, instanceGame.PreviousState);
	}
	else if(newState.Substate != instanceGame.State.Substate) {
		instanceGame.State.Substate	= newState.Substate;
		handleSubstateChange(instanceGame, instanceGame.State, instanceGame.PreviousState);
	}
}

SGameState drawSquadSetupMenu	(SGame& instanceGame, const SGameState& returnState);
SGameState drawResearch			(SGame& instanceGame, const SGameState& returnState);
SGameState drawWelcome			(SGame& instanceGame, const SGameState& returnState);
SGameState drawBuy				(SGame& instanceGame, const SGameState& returnState);
SGameState drawEquip			(SGame& instanceGame, const SGameState& returnState);
SGameState drawTacticalScreen	(SGame& instanceGame, const SGameState& returnState);
SGameState drawUpgrade			(SGame& instanceGame, const SGameState& returnState);
SGameState drawFactory			(SGame& instanceGame, const SGameState& returnState);

void klib::showMenu(SGame& instanceGame) {

	SGameState newAction=instanceGame.State;
	
	static const SMenu<SGameState> menuMain			(  {GAME_STATE_EXIT					},	"Main Menu"		, 20, true, "Exit game");
	static const SMenu<SGameState> menuMainInGame	(  {GAME_STATE_EXIT					},	"Main Menu"		, 20, true, "Exit game");
	static const SMenu<SGameState> menuConfig		(  {GAME_STATE_MENU_MAIN			},	"Options"		, 26);
	static const SMenu<SGameState> menuSell			(  {GAME_STATE_WELCOME_COMMANDER	},	"Sell"			);

	klib::SGlobalDisplay& globalDisplay = instanceGame.GlobalDisplay;

	switch(instanceGame.State.State) {
	case GAME_STATE_MENU_MAIN			:	
		if( ::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_STARTED) )
			newAction = processMenuReturn(instanceGame, drawMenu(globalDisplay.Screen, &globalDisplay.TextAttributes[0][0], menuMainInGame, optionsMainInGame, instanceGame.FrameInput, instanceGame.State));	
		else													
			newAction = processMenuReturn(instanceGame, drawMenu(globalDisplay.Screen, &globalDisplay.TextAttributes[0][0], menuMain, optionsMain, instanceGame.FrameInput, instanceGame.State));	

		break;

	case GAME_STATE_MENU_OPTIONS		:	newAction = processMenuReturn(instanceGame, drawMenu(globalDisplay.Screen, &globalDisplay.TextAttributes.Cells[0][0], menuConfig, optionsConfig, instanceGame.FrameInput, instanceGame.State ));	break;
	case GAME_STATE_MENU_EQUIPMENT		:	newAction = processMenuReturn(instanceGame, drawEquip(instanceGame, instanceGame.State));	break;
	case GAME_STATE_MENU_SELL			:	newAction = processMenuReturn(instanceGame, drawMenu(globalDisplay.Screen, &globalDisplay.TextAttributes[0][0], menuSell, optionsSell, instanceGame.FrameInput, instanceGame.State ));	break;
	case GAME_STATE_MENU_LAN_MISSION	:	//newAction = processMenuReturn(instanceGame, drawLANSetup		(instanceGame, instanceGame.State));	break;
	case GAME_STATE_START_MISSION		:	newAction = processMenuReturn(instanceGame, drawTacticalScreen	(instanceGame, instanceGame.State));	break;
	case GAME_STATE_TACTICAL_CONTROL	:	newAction = processMenuReturn(instanceGame, drawTacticalScreen	(instanceGame, instanceGame.State));	break;
	case GAME_STATE_MENU_SQUAD_SETUP	:	newAction = processMenuReturn(instanceGame, drawSquadSetupMenu	(instanceGame, instanceGame.State));	break;
	case GAME_STATE_WELCOME_COMMANDER	:	newAction = processMenuReturn(instanceGame, drawWelcome			(instanceGame, instanceGame.State));	break;
	case GAME_STATE_MENU_RESEARCH		:	newAction = processMenuReturn(instanceGame, drawResearch		(instanceGame, instanceGame.State));	break;
	case GAME_STATE_MENU_BUY			:	newAction = processMenuReturn(instanceGame, drawBuy				(instanceGame, instanceGame.State));	break;
	case GAME_STATE_MENU_UPGRADE		:	newAction = processMenuReturn(instanceGame, drawFactory			(instanceGame, instanceGame.State));	break;
	case GAME_STATE_MENU_FACTORY		:	newAction = processMenuReturn(instanceGame, drawFactory			(instanceGame, instanceGame.State));	break;
	
	case GAME_STATE_MEMORIAL			:	
		if(instanceGame.FrameInput.Keys[VK_ESCAPE]) 
			newAction={GAME_STATE_WELCOME_COMMANDER}; 

		break;

	case GAME_STATE_CREDITS				:	
		if(instanceGame.FrameInput.Keys[VK_ESCAPE]) 
			newAction={GAME_STATE_MENU_MAIN}; 

		break;
	case GAME_STATE_EXIT				:	
		instanceGame.StateMessage = "Exiting game...";	
		::gpk::bit_clear(instanceGame.Flags, GAME_FLAGS_RUNNING);
		newAction = instanceGame.State; 
		break;

	default:
		newAction.State = (GAME_STATE)-1;
		instanceGame.StateMessage = "Unrecognized game state!!";
	}			

	updateState(instanceGame, newAction);
};
