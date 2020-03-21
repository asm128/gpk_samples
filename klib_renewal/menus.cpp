//#define NOMINMAX

#include "Game.h"
#include "draw.h"
#include "tactical_draw.h"

#include <algorithm>

CONSOLE_FONT_INFOEX							getFontParams			() {
	CONSOLE_FONT_INFOEX								infoFont				= {sizeof(CONSOLE_FONT_INFOEX)};
	infoFont.FontFamily							= 0;
	infoFont.FontWeight							= 0;
	infoFont.nFont								= 0;
	::wcscpy_s(infoFont.FaceName, L"Terminal");
	return infoFont;
}

::klib::SGameState							processMenuReturn		(const ::klib::SGameState& returnValue_) {
	::klib::SGameState								returnValue;
	static bool										bSmallFonts				= false;

	static const HANDLE								hConsoleOut				= ::GetStdHandle( STD_OUTPUT_HANDLE );
	static CONSOLE_FONT_INFOEX						infoFont				= ::getFontParams();

	switch(returnValue_.State) {
	case ::klib::GAME_STATE_MENU_MAIN:
	default:
		return returnValue_;
	case ::klib::GAME_STATE_MENU_OPTIONS:
		switch(returnValue_.Substate) {
		default:
			return returnValue_;
		case ::klib::GAME_SUBSTATE_SCREEN:
			bSmallFonts									= !bSmallFonts;
			if(bSmallFonts) {
				infoFont.dwFontSize.X						= 6;
				infoFont.dwFontSize.Y						= 8;
			}
			else {
				infoFont.dwFontSize.X						= 8;
				infoFont.dwFontSize.Y						= 12;
			}
			SetCurrentConsoleFontEx(hConsoleOut, FALSE, &infoFont);
			returnValue									= returnValue_;
			returnValue.Substate						= ::klib::GAME_SUBSTATE_MAIN;
			break;
		}
	}
	return returnValue;
}

static	void						handleSubstateChange				(::klib::SGame& instanceGame, const ::klib::SGameState& newState) {
	::klib::clearASCIIBackBuffer(' ', ::klib::ASCII_COLOR_INDEX_WHITE);
	instanceGame.GlobalDisplay.Clear();
	switch(newState.State) {
	case ::klib::GAME_STATE_MENU_OPTIONS:
	case ::klib::GAME_STATE_MENU_SELL:
	case ::klib::GAME_STATE_MENU_UPGRADE:
	case ::klib::GAME_STATE_MENU_FACTORY:
		instanceGame.Messages.UserError		= ::gpk::view_const_string{"This function isn't available!"};
		instanceGame.LogError();
	}
	//resetCursorString(instanceGame.SlowMessage);	we shuold leave this out unless it becomes a need. This is because it turns screen transitions into an annoyance.
}

static	void						drawTacticalMap						(::klib::SGame& instanceGame, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes) {
	klib::drawTacticalBoard(instanceGame, instanceGame.TacticalInfo, display, textAttributes, ::klib::PLAYER_INDEX_USER, ::klib::TEAM_TYPE_CIVILIAN, instanceGame.Players[::klib::PLAYER_INDEX_USER].Tactical.Selection, false);
}

void								handleMissionEnd					(::klib::SGame& instanceGame) {
	::klib::SGamePlayer							& player							= instanceGame.Players[::klib::PLAYER_INDEX_USER];
	::klib::SPlayerProjects					& playerProjects					= player.Projects;

	if(playerProjects.QueuedProduction.size())
		::klib::handleProductionStep(instanceGame.EntityTables, player.Inventory, playerProjects, player.Tactical.Money, player.Tactical.Score, instanceGame.Messages);

	if(playerProjects.QueuedResearch.size())
		::klib::handleResearchStep(player.Tactical.Research, playerProjects, player.Tactical.Money, player.Tactical.Score, instanceGame.Messages);

	::klib::playerUpdateResearchLists(instanceGame.EntityTables, player);
}

::gpk::error_t						handleStateChange					(::klib::SGame& instanceGame, const ::klib::SGameState& newState, const ::klib::SGameState& prevState)	{
	::klib::clearASCIIBackBuffer(' ', ::klib::ASCII_COLOR_INDEX_WHITE);
	instanceGame.ClearDisplays();
	//::klib::resetCursorString(instanceGame.SlowMessage);

	::klib::SGamePlayer							& playerUser						= instanceGame.Players[::klib::PLAYER_INDEX_USER];
	int64_t									playCost							= 0;
	switch(newState.State) {
	case ::klib::GAME_STATE_TACTICAL_CONTROL	: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Tactical control"	}; break;
	case ::klib::GAME_STATE_MENU_LAN_MISSION	: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"LAN mission setup"	}; break;
	case ::klib::GAME_STATE_MENU_OPTIONS		: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Options"			}; break;
	case ::klib::GAME_STATE_MENU_BUY			: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Buy"				}; break;
	case ::klib::GAME_STATE_MENU_SELL			: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Sell"				}; break;
	case ::klib::GAME_STATE_MENU_UPGRADE		: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Upgrade"			}; break;
	case ::klib::GAME_STATE_MENU_FACTORY		: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Factory"			}; break;
	case ::klib::GAME_STATE_MENU_SQUAD_SETUP	: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Squad Setup"		}; break;
	case ::klib::GAME_STATE_CREDITS				: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Credits"			}; break;
	case ::klib::GAME_STATE_MENU_EQUIPMENT		: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Equipment Setup"	}; break;
	case ::klib::GAME_STATE_MENU_RESEARCH		: instanceGame.Messages.StateMessage = ::gpk::view_const_string{"Research Center"	}; break;
	case ::klib::GAME_STATE_MENU_MAIN:
		drawTacticalMap(instanceGame, instanceGame.TacticalDisplay.Screen.Color, instanceGame.TacticalDisplay.Screen.DepthStencil);
		instanceGame.Messages.StateMessage	= "Main Menu";
		break;
	case ::klib::GAME_STATE_START_MISSION		:
		playCost							= ::klib::missionCost(playerUser, playerUser.Tactical.Squad, playerUser.Tactical.Squad.Size);
		playerUser.Tactical.Money			-= playCost;
		instanceGame.Messages.StateMessage	= ::gpk::view_const_string{"Start mission"};
		break;
	case ::klib::GAME_STATE_WELCOME_COMMANDER:
		if(newState.Substate == ::klib::GAME_SUBSTATE_RESET) {
			instanceGame.Messages.StateMessage	= ::gpk::view_const_string{"Welcome commander"};
			::klib::resetGame(instanceGame);
			::handleSubstateChange(instanceGame, newState);
		}
		else {
			instanceGame.Messages.StateMessage	= ::gpk::view_const_string{"Welcome back commander"};
			if(::gpk::bit_false(instanceGame.Flags, ::klib::GAME_FLAGS_TACTICAL) && (prevState.State == ::klib::GAME_STATE_TACTICAL_CONTROL || prevState.State == ::klib::GAME_STATE_START_MISSION))
				::handleMissionEnd(instanceGame);
		}
		break;
	default:
		break;
	}
	//if(prevState.State != GAME_STATE_START_MISSION && prevState.State != GAME_STATE_TACTICAL_CONTROL)
	//	instanceGame.Messages.UserLog.clear();
	return 0;
}

static	void							updateState					(::klib::SGame& instanceGame, const ::klib::SGameState& newState) {
	if(newState.State == ::klib::GAME_STATE_START_MISSION && newState.State != instanceGame.State.State) {
		::klib::SGamePlayer								& player					= instanceGame.Players[::klib::PLAYER_INDEX_USER];
		int64_t										playCost					= ::klib::missionCost(player, player.Tactical.Squad, player.Tactical.Squad.Size);

		if( player.Tactical.Money < playCost ){
			instanceGame.Messages.UserError			= ::gpk::view_const_string{"You don't have enough money to start a mission with the current squad setup."};
			instanceGame.LogError();
			//return;
		}
	}
	else if(newState.State == ::klib::GAME_STATE_WELCOME_COMMANDER || newState.State == ::klib::GAME_STATE_MENU_CONTROL_CENTER){
	}

	instanceGame.PreviousState				= instanceGame.State;
	//
	if(newState.State != instanceGame.State.State) {
		instanceGame.State						= newState;
		::handleStateChange(instanceGame, instanceGame.State, instanceGame.PreviousState);
	}
	else if(newState.Substate != instanceGame.State.Substate) {
		instanceGame.State.Substate				= newState.Substate;
		::handleSubstateChange(instanceGame, instanceGame.State);
	}
}

::klib::SGameState					drawSquadSetupMenu	(::klib::SGame& instanceGame);
::klib::SGameState					drawResearch		(::klib::SGame& instanceGame, const ::klib::SGameState& returnState);
::klib::SGameState					drawWelcome			(::klib::SGame& instanceGame, const ::klib::SGameState& returnState);
::klib::SGameState					drawBuy				(::klib::SGame& instanceGame, const ::klib::SGameState& returnState);
::klib::SGameState					drawEquip			(::klib::SGame& instanceGame, const ::klib::SGameState& returnState);
::klib::SGameState					drawTacticalScreen	(::klib::SGame& instanceGame, const ::klib::SGameState& returnState);
::klib::SGameState					drawUpgrade			(::klib::SGame& instanceGame, const ::klib::SGameState& returnState);
::klib::SGameState					drawFactory			(::klib::SGame& instanceGame, const ::klib::SGameState& returnState);

::gpk::error_t						klib::showMenu		(::klib::SGame& instanceGame)			{
	SGameState								newAction			= instanceGame.State;

	static ::klib::SMenuHeader<::klib::SGameState>	menuMain			( {::klib::GAME_STATE_EXIT				},	"Main Menu"		, 20, true, "Exit game");
	static ::klib::SMenuHeader<::klib::SGameState>	menuMainInGame		( {::klib::GAME_STATE_EXIT				},	"Main Menu"		, 20, true, "Exit game");
	static ::klib::SMenuHeader<::klib::SGameState>	menuConfig			( {::klib::GAME_STATE_MENU_MAIN			},	"Options"		, 26);
	static ::klib::SMenuHeader<::klib::SGameState>	menuSell			( {::klib::GAME_STATE_WELCOME_COMMANDER	},	"Sell"			);

	::gpk::SRenderTarget<char, uint16_t>			& globalDisplay		= instanceGame.GlobalDisplay.Screen;


	switch(instanceGame.State.State) {
	case ::klib::GAME_STATE_MENU_MAIN			:
		if( ::gpk::bit_true(instanceGame.Flags, ::klib::GAME_FLAGS_STARTED) )
			newAction = ::processMenuReturn(::klib::drawMenu(globalDisplay.Color.View, globalDisplay.DepthStencil.begin(), menuMainInGame, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsMainInGame}, instanceGame.FrameInput, instanceGame.State));
		else
			newAction = ::processMenuReturn(::klib::drawMenu(globalDisplay.Color.View, globalDisplay.DepthStencil.begin(), menuMain, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsMain}, instanceGame.FrameInput, instanceGame.State));
		break;

	case ::klib::GAME_STATE_MENU_OPTIONS		: { newAction = ::processMenuReturn(::klib::drawMenu	(globalDisplay.Color.View, globalDisplay.DepthStencil.begin(), menuConfig	, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsConfig}, instanceGame.FrameInput, instanceGame.State ));	}break;
	case ::klib::GAME_STATE_MENU_SELL			: { newAction = ::processMenuReturn(::klib::drawMenu	(globalDisplay.Color.View, globalDisplay.DepthStencil.begin(), menuSell		, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsSell}, instanceGame.FrameInput, instanceGame.State ));	}break;
	case ::klib::GAME_STATE_MENU_EQUIPMENT		: { newAction = ::processMenuReturn(::drawEquip				(instanceGame, instanceGame.State));	} break;
	case ::klib::GAME_STATE_TACTICAL_CONTROL	: { newAction = ::processMenuReturn(::drawTacticalScreen	(instanceGame, instanceGame.State));	} break;
	case ::klib::GAME_STATE_MENU_SQUAD_SETUP	: { newAction = ::processMenuReturn(::drawSquadSetupMenu	(instanceGame));						} break;
	case ::klib::GAME_STATE_WELCOME_COMMANDER	: { newAction = ::processMenuReturn(::drawWelcome			(instanceGame, instanceGame.State));	} break;
	case ::klib::GAME_STATE_MENU_RESEARCH		: { newAction = ::processMenuReturn(::drawResearch			(instanceGame, instanceGame.State));	} break;
	case ::klib::GAME_STATE_MENU_BUY			: { newAction = ::processMenuReturn(::drawBuy				(instanceGame, instanceGame.State));	} break;
	case ::klib::GAME_STATE_MENU_UPGRADE		: { newAction = ::processMenuReturn(::drawFactory			(instanceGame, instanceGame.State));	} break;
	case ::klib::GAME_STATE_MENU_FACTORY		: { newAction = ::processMenuReturn(::drawFactory			(instanceGame, instanceGame.State));	} break;
	case ::klib::GAME_STATE_MENU_LAN_MISSION	:
	case ::klib::GAME_STATE_START_MISSION		: { newAction = ::processMenuReturn(::drawTacticalScreen	(instanceGame, instanceGame.State));	} break;
	case ::klib::GAME_STATE_MEMORIAL			:
		if(instanceGame.FrameInput.Keys[VK_ESCAPE])
			newAction						= {::klib::GAME_STATE_WELCOME_COMMANDER};

		break;

	case ::klib::GAME_STATE_CREDITS		:
		if(instanceGame.FrameInput.Keys[VK_ESCAPE])
			newAction						= {::klib::GAME_STATE_MENU_MAIN};

		break;
	case ::klib::GAME_STATE_EXIT		:
		instanceGame.Messages.StateMessage	= ::gpk::view_const_string{"Exiting game..."};
		::gpk::bit_clear(instanceGame.Flags, ::klib::GAME_FLAGS_RUNNING);
		newAction							= instanceGame.State;
		break;

	default:
		newAction.State						= (::klib::GAME_STATE)-1;
		instanceGame.Messages.StateMessage	= ::gpk::view_const_string{"Unrecognized game state!!"};
	}

	::updateState(instanceGame, newAction);
	return 0;
}
