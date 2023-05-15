#include "Game.h"
#include "draw.h"

#include "projects.h"
#include "helper_projects.h"

static	::klib::SGameState					drawUpgradeMenu				(::klib::SGame& instanceGame, const ::klib::SGameState& returnState)				{
	::klib::SGamePlayer								& player					= instanceGame.Players[::klib::PLAYER_INDEX_USER];
	::klib::playerUpdateResearchLists(instanceGame.EntityTables, player);

	::gpk::array_obj<::klib::SEntityResearch>	& menuItemsValue			= player.ResearchablesValue;
	::gpk::array_obj<::gpk::apod<char>>	& menuItemsText				= player.ResearchablesText;

	::gpk::array_obj<::gpk::view_const_char>	menuItemsView				= {};
	menuItemsView.resize(menuItemsText.size());
	for(uint32_t i = 0, count = menuItemsText.size(); i < count; ++i)
		menuItemsView[i]						= menuItemsText[i];

	static ::klib::SDrawMenuState				menuState;
	int32_t										selectedChoice								= ::klib::drawMenu
		( menuState
		, instanceGame.GlobalDisplay.Screen.Color.View
		, instanceGame.GlobalDisplay.Screen.DepthStencil.begin()
		, ::gpk::view_const_string{"Available Production"}
		, menuItemsView
		, instanceGame.FrameInput
		, -1
		, 50U
		);
	if(selectedChoice == (int32_t)menuItemsValue.size())
		return {::klib::GAME_STATE_WELCOME_COMMANDER};

	if(selectedChoice == -1)
		return returnState;

	::klib::acknowledgeProduction(menuItemsValue[selectedChoice], player.Projects, instanceGame.Messages.UserSuccess);
	instanceGame.LogSuccess(); //break;
	return returnState;
}

::klib::SGameState						drawUpgrade					(::klib::SGame& instanceGame, const ::klib::SGameState& returnState)																						{
	const ::gpk::view_const_string				textToPrint					= "Factory.";
	static ::klib::SMessageSlow					slowMessage;
	bool										bDonePrinting				= ::klib::getMessageSlow(slowMessage, textToPrint, instanceGame.FrameTimer.LastTimeSeconds);
	::gpk::n2<uint32_t>					position					= instanceGame.TacticalDisplay.Screen.metrics() / 2;
	const uint32_t								messageLen					= (uint32_t)strlen(slowMessage.Message);
	memcpy(&instanceGame.TacticalDisplay.Screen[position.y][position.x - (messageLen + 1) / 2], slowMessage.Message, messageLen);
	if ( !bDonePrinting )
		return returnState;

	::klib::drawFireBackground(instanceGame.TacticalDisplay, instanceGame.FrameTimer.LastTimeSeconds);
	return ::drawUpgradeMenu(instanceGame, returnState);
};
