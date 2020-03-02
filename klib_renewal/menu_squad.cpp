//#define NOMINMAX

#include "Agent_helper.h"
#include "draw.h"


using namespace klib;

SGameState														drawSquadSetupMenu									(SGame& instanceGame)												{
	drawSquadSlots(instanceGame);
	SPlayer																& player											= instanceGame.Players[PLAYER_INDEX_USER];

	::gpk::array_obj<::gpk::array_pod<char_t>>							menuItems											= {};
	::gpk::array_obj<::gpk::view_const_char>							menuItemsView										= {};
	menuItems		.resize(player.Tactical.Squad.Size);
	menuItemsView	.resize(menuItems.size());
	static int32_t														maxNameLen											= 0;
	for(uint32_t i = 0, count = player.Tactical.Squad.Size; i < count; ++i) {
		char																buffer[128];
		if(player.Tactical.Squad.Agents[i] != -1)  {
			const CCharacter													& playerAgent										= *player.Tactical.Army[player.Tactical.Squad.Agents[i]];
			maxNameLen														= ::gpk::max(maxNameLen, sprintf_s(buffer, "Agent #%u: %s", i + 1, playerAgent.Name.begin()));
			menuItems[i]													= ::gpk::view_const_string{buffer};
		}
		else {
			maxNameLen														= ::gpk::max(maxNameLen, sprintf_s(buffer, "Agent #%u: Empty slot", i + 1));
			menuItems[i]													= ::gpk::view_const_string{buffer};
		}
		menuItemsView[i]												= menuItems[i];
	}

	static ::klib::SDrawMenuState										menuState;
	int32_t																result												= ::klib::drawMenu
		( menuState
		, instanceGame.GlobalDisplay.Screen.View
		, instanceGame.GlobalDisplay.TextAttributes.begin()
		, ::gpk::view_const_string{"Squad setup"}
		, ::gpk::view_array<const ::gpk::view_const_char>{menuItemsView.begin(), menuItemsView.size()}
		, instanceGame.FrameInput
		, -1
		, ::gpk::max(24, maxNameLen+4)
		);
	if(menuItems.size() == (uint32_t)result)
		return {GAME_STATE_WELCOME_COMMANDER};

	if( result < 0 || result >= (int32_t)::gpk::size(player.Tactical.Squad.Agents) )
		return {GAME_STATE_MENU_SQUAD_SETUP};

	player.Tactical.Selection.PlayerUnit										= (int16_t)result;

	if( player.Tactical.Squad.Agents[result] != -1 && 0 == instanceGame.FrameInput.Keys[VK_LSHIFT] )
		return {GAME_STATE_MENU_EQUIPMENT};

	return {GAME_STATE_MENU_EQUIPMENT, GAME_SUBSTATE_CHARACTER};
}
