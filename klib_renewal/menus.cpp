//#define NOMINMAX

#include "Game.h"
#include "draw.h"
#include "tactical_draw.h"

#include <algorithm>

using namespace klib;

CONSOLE_FONT_INFOEX getFontParams() {
	CONSOLE_FONT_INFOEX								infoFont				= {sizeof(CONSOLE_FONT_INFOEX)};
	infoFont.FontFamily							= 0;
	infoFont.FontWeight							= 0;
	infoFont.nFont								= 0;
	::wcscpy_s(infoFont.FaceName, L"Terminal");
	return infoFont;
}

::klib::SGameState			processMenuReturn		(const SGameState& returnValue_) {
	::klib::SGameState				returnValue;
	static bool						bSmallFonts				= false;

	static const HANDLE hConsoleOut = ::GetStdHandle( STD_OUTPUT_HANDLE );
	static CONSOLE_FONT_INFOEX infoFont = ::getFontParams();

	switch(returnValue_.State) {
	case GAME_STATE_MENU_MAIN:
	default:
		return returnValue_;
	case GAME_STATE_MENU_OPTIONS:
		switch(returnValue_.Substate) {
		default:
			return returnValue_;
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
			returnValue				= returnValue_;
			returnValue.Substate	= GAME_SUBSTATE_MAIN;
			break;
		}
	}

	return returnValue;
}

SGameState processMenuReturn(TURN_ACTION returnValue) {
	switch(returnValue) {
	case TURN_ACTION_CONTINUE:
	default: return {GAME_STATE_TACTICAL_CONTROL, };
	}
}

void handleSubstateChange(SGame& instanceGame, const SGameState& newState) {
	::klib::clearASCIIBackBuffer(' ', ::klib::ASCII_COLOR_INDEX_WHITE);
	instanceGame.GlobalDisplay		.Clear();
	//instanceGame.TacticalDisplay	.Clear();
	//instanceGame.PostEffectDisplay	.Clear();
	//::klib::clearGrid(instanceGame.MenuDisplay);

	switch(newState.State) {
	case GAME_STATE_MENU_OPTIONS:
	case GAME_STATE_MENU_SELL:
	case GAME_STATE_MENU_UPGRADE:
	case GAME_STATE_MENU_FACTORY:
		instanceGame.Messages.UserError = "This function isn't available!";
		instanceGame.LogError();
	}

	//resetCursorString(instanceGame.SlowMessage);	we shuold leave this out unless it becomes a need. This is because it turns screen transitions into an annoyance.
}

void drawTacticalMap(SGame& instanceGame, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes) {
	klib::drawTacticalBoard(instanceGame, instanceGame.TacticalInfo, display, textAttributes, PLAYER_INDEX_USER, TEAM_TYPE_CIVILIAN, instanceGame.Players[PLAYER_INDEX_USER].Tactical.Selection, false);
}

void											handleMissionEnd					(SGame& instanceGame) {
	::klib::SPlayer										& player							= instanceGame.Players[PLAYER_INDEX_USER];
	SPlayerProjects										& playerProjects					= player.Projects;

	if(playerProjects.QueuedProduction.size())
		::klib::handleProductionStep(instanceGame.EntityTables, player.Inventory, playerProjects, player.Tactical.Money, player.Tactical.Score, instanceGame.Messages);

	if(playerProjects.QueuedResearch.size())
		::klib::handleResearchStep(player.Tactical.Research, playerProjects, player.Tactical.Money, player.Tactical.Score, instanceGame.Messages);

	::klib::playerUpdateResearchLists(instanceGame.EntityTables, player);
}

::gpk::error_t						handleStateChange				(SGame& instanceGame, const SGameState& newState, const SGameState& prevState)	{
	::klib::clearASCIIBackBuffer(' ', ::klib::ASCII_COLOR_INDEX_WHITE);
	instanceGame.ClearDisplays();
	//::klib::resetCursorString(instanceGame.SlowMessage);

	SPlayer									& playerUser					= instanceGame.Players[PLAYER_INDEX_USER];
	int64_t									playCost						= 0;
	switch(newState.State) {
	case GAME_STATE_TACTICAL_CONTROL	: instanceGame.Messages.StateMessage = "Tactical control"		; break;
	case GAME_STATE_MENU_LAN_MISSION	: instanceGame.Messages.StateMessage = "LAN mission setup"		; break;
	case GAME_STATE_MENU_OPTIONS		: instanceGame.Messages.StateMessage = "Options"				; break;
	case GAME_STATE_MENU_BUY			: instanceGame.Messages.StateMessage = "Buy"					; break;
	case GAME_STATE_MENU_SELL			: instanceGame.Messages.StateMessage = "Sell"					; break;
	case GAME_STATE_MENU_UPGRADE		: instanceGame.Messages.StateMessage = "Upgrade"				; break;
	case GAME_STATE_MENU_FACTORY		: instanceGame.Messages.StateMessage = "Factory"				; break;
	case GAME_STATE_MENU_SQUAD_SETUP	: instanceGame.Messages.StateMessage = "Squad Setup"			; break;
	case GAME_STATE_CREDITS				: instanceGame.Messages.StateMessage = "Credits"				; break;
	case GAME_STATE_MENU_EQUIPMENT		: instanceGame.Messages.StateMessage = "Equipment Setup"		; break;
	case GAME_STATE_MENU_RESEARCH		: instanceGame.Messages.StateMessage = "Research Center"		; break;
	case GAME_STATE_MENU_MAIN:
		drawTacticalMap(instanceGame, instanceGame.TacticalDisplay.Screen.Color, instanceGame.TacticalDisplay.Screen.DepthStencil);
		instanceGame.Messages.StateMessage			= "Main Menu";
		break;
	case GAME_STATE_START_MISSION		:
		playCost							= ::klib::missionCost(playerUser, playerUser.Tactical.Squad, playerUser.Tactical.Squad.Size);
		playerUser.Tactical.Money			-= playCost;
		instanceGame.Messages.StateMessage			= "Start mission"		;
		break;
	case GAME_STATE_WELCOME_COMMANDER:
		if(newState.Substate == GAME_SUBSTATE_RESET) {
			instanceGame.Messages.StateMessage		= "Welcome commander";
			resetGame(instanceGame);
			handleSubstateChange(instanceGame, newState);
		}
		else {
			instanceGame.Messages.StateMessage		= "Welcome back commander";
			if(::gpk::bit_false(instanceGame.Flags, GAME_FLAGS_TACTICAL) && (prevState.State == GAME_STATE_TACTICAL_CONTROL || prevState.State == GAME_STATE_START_MISSION))
				handleMissionEnd(instanceGame);
		}
		break;
	default:
		break;
	}
	//if(prevState.State != GAME_STATE_START_MISSION && prevState.State != GAME_STATE_TACTICAL_CONTROL)
	//	instanceGame.Messages.UserLog.clear();
	return 0;
}

void									updateState					(SGame& instanceGame, const SGameState& newState) {
	if(newState.State == GAME_STATE_START_MISSION && newState.State != instanceGame.State.State) {
		SPlayer										& player					= instanceGame.Players[PLAYER_INDEX_USER];
		int64_t										playCost					= ::klib::missionCost(player, player.Tactical.Squad, player.Tactical.Squad.Size);

		if( player.Tactical.Money < playCost ){
			instanceGame.Messages.UserError			= "You don't have enough money to start a mission with the current squad setup.";
			instanceGame.LogError();
			//return;
		}
	}
	else if(newState.State == GAME_STATE_WELCOME_COMMANDER || newState.State == GAME_STATE_MENU_CONTROL_CENTER){
	}

	instanceGame.PreviousState = instanceGame.State;
	//
	if(newState.State != instanceGame.State.State) {
		instanceGame.State							= newState;
		handleStateChange(instanceGame, instanceGame.State, instanceGame.PreviousState);
	}
	else if(newState.Substate != instanceGame.State.Substate) {
		instanceGame.State.Substate					= newState.Substate;
		handleSubstateChange(instanceGame, instanceGame.State);
	}
}

SGameState drawSquadSetupMenu	(SGame& instanceGame);
SGameState drawResearch			(SGame& instanceGame, const SGameState& returnState);
SGameState drawWelcome			(SGame& instanceGame, const SGameState& returnState);
SGameState drawBuy				(SGame& instanceGame, const SGameState& returnState);
SGameState drawEquip			(SGame& instanceGame, const SGameState& returnState);
SGameState drawTacticalScreen	(SGame& instanceGame, const SGameState& returnState);
SGameState drawUpgrade			(SGame& instanceGame, const SGameState& returnState);
SGameState drawFactory			(SGame& instanceGame, const SGameState& returnState);

::gpk::error_t						klib::showMenu		(SGame& instanceGame)			{
	SGameState								newAction			= instanceGame.State;

	static SMenuHeader<SGameState>			menuMain			( {GAME_STATE_EXIT				},	"Main Menu"		, 20, true, "Exit game");
	static SMenuHeader<SGameState>			menuMainInGame		( {GAME_STATE_EXIT				},	"Main Menu"		, 20, true, "Exit game");
	static SMenuHeader<SGameState>			menuConfig			( {GAME_STATE_MENU_MAIN			},	"Options"		, 26);
	static SMenuHeader<SGameState>			menuSell			( {GAME_STATE_WELCOME_COMMANDER	},	"Sell"			);

	klib::SWeightedDisplay					& globalDisplay		= instanceGame.GlobalDisplay;


	switch(instanceGame.State.State) {
	case GAME_STATE_MENU_MAIN			:
		if( ::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_STARTED) ) {
			newAction = processMenuReturn(drawMenu(globalDisplay.Screen.Color.View, globalDisplay.Screen.DepthStencil.begin(), menuMainInGame, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsMainInGame}, instanceGame.FrameInput, instanceGame.State));
		}
		else {
			newAction = processMenuReturn(drawMenu(globalDisplay.Screen.Color.View, globalDisplay.Screen.DepthStencil.begin(), menuMain, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsMain}, instanceGame.FrameInput, instanceGame.State));
		}

		break;

	case GAME_STATE_MENU_OPTIONS		: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawMenu	(globalDisplay.Screen.Color.View, globalDisplay.Screen.DepthStencil.begin(), menuConfig, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsConfig}, instanceGame.FrameInput, instanceGame.State ));	}break;
	case GAME_STATE_MENU_EQUIPMENT		: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawEquip	(instanceGame, instanceGame.State));	}break;
	case GAME_STATE_MENU_SELL			: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawMenu	(globalDisplay.Screen.Color.View, globalDisplay.Screen.DepthStencil.begin(), menuSell, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsSell}, instanceGame.FrameInput, instanceGame.State ));	}break;
	case GAME_STATE_MENU_LAN_MISSION	: { static ::klib::SDrawMenuState	menuState; }//newAction = processMenuReturn(drawLANSetup			(instanceGame, instanceGame.State));	}break;
	case GAME_STATE_START_MISSION		: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawTacticalScreen		(instanceGame, instanceGame.State));	}break;
	case GAME_STATE_TACTICAL_CONTROL	: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawTacticalScreen		(instanceGame, instanceGame.State));	}break;
	case GAME_STATE_MENU_SQUAD_SETUP	: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawSquadSetupMenu		(instanceGame));	}break;
	case GAME_STATE_WELCOME_COMMANDER	: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawWelcome			(instanceGame, instanceGame.State));	}break;
	case GAME_STATE_MENU_RESEARCH		: {
		newAction = processMenuReturn(drawResearch(instanceGame, instanceGame.State));	}break;
	case GAME_STATE_MENU_BUY			: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawBuy				(instanceGame, instanceGame.State));	}break;
	case GAME_STATE_MENU_UPGRADE		: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawFactory			(instanceGame, instanceGame.State));	}break;
	case GAME_STATE_MENU_FACTORY		: { static ::klib::SDrawMenuState	menuState; newAction = processMenuReturn(drawFactory			(instanceGame, instanceGame.State));	}break;

	case GAME_STATE_MEMORIAL			:
		if(instanceGame.FrameInput.Keys[VK_ESCAPE])
			newAction			= {GAME_STATE_WELCOME_COMMANDER};

		break;

	case GAME_STATE_CREDITS				:
		if(instanceGame.FrameInput.Keys[VK_ESCAPE])
			newAction			= {GAME_STATE_MENU_MAIN};

		break;
	case GAME_STATE_EXIT				:
		instanceGame.Messages.StateMessage = "Exiting game...";
		::gpk::bit_clear(instanceGame.Flags, GAME_FLAGS_RUNNING);
		newAction = instanceGame.State;
		break;

	default:
		newAction.State = (GAME_STATE)-1;
		instanceGame.Messages.StateMessage = "Unrecognized game state!!";
	}

	updateState(instanceGame, newAction);
	return 0;
}
