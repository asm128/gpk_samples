//#define NOMINMAX

#include "Game.h"
#include "draw.h"

#include "projects.h"
#include "helper_projects.h"

using namespace klib;

static	SGameState						drawResearchMenu				(SGame& instanceGame, const SGameState& returnState) {
	::klib::SPlayer								& player						= instanceGame.Players[PLAYER_INDEX_USER];
	::klib::playerUpdateResearchLists(instanceGame.EntityTables, player);

	::gpk::array_obj<::klib::SEntityResearch>	& menuItemsValue			= player.ResearchablesValue;
	::gpk::array_obj<::gpk::array_pod<char_t>>	& menuItemsText				= player.ResearchablesText;

	::gpk::array_obj<::gpk::view_const_char>	menuItemsView					= {};
	menuItemsView.resize(menuItemsText.size());
	for(uint32_t i = 0, count = menuItemsText.size(); i < count; ++i)
		menuItemsView[i]						= menuItemsText[i];

	static ::klib::SDrawMenuState				menuState;
	int32_t										selectedChoice					= ::klib::drawMenu
		( menuState
		, instanceGame.GlobalDisplay.Screen.Color.View
		, instanceGame.GlobalDisplay.Screen.DepthStencil.begin()
		, ::gpk::view_const_string{"Available Research"}
		, menuItemsView
		, instanceGame.FrameInput
		, -1
		, 55U
		);
	if(selectedChoice == (int32_t)menuItemsView.size())
		return {GAME_STATE_WELCOME_COMMANDER};

	if(selectedChoice == -1)
		return returnState;

	instanceGame.ClearMessages();
	const ::klib::SEntityResearch				& selectedValue					= menuItemsValue[selectedChoice];
	switch(menuItemsValue[selectedChoice].Type) {
	case ENTITY_TYPE_ACCESSORY	: acknowledgeResearch(selectedValue, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_STAGE_PROP	: acknowledgeResearch(selectedValue, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_FACILITY	: acknowledgeResearch(selectedValue, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_VEHICLE	: acknowledgeResearch(selectedValue, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_PROFESSION	: acknowledgeResearch(selectedValue, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_WEAPON		: acknowledgeResearch(selectedValue, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_ARMOR		: acknowledgeResearch(selectedValue, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	default:
		break;
	}

	return returnState;
}

static void				drawBubblesBackground		( SWeightedDisplay & display, double lastTimeSeconds, uint32_t disturbance=2 ) {
	uint32_t				displayWidth				= (int32_t)display.Screen.metrics().x;
	uint32_t				displayDepth				= (int32_t)display.Screen.metrics().y;

	uint64_t				seed						= (uint64_t)(disturbance+lastTimeSeconds*100000*(1+(rand()%100)));
	uint32_t				randBase					= (uint32_t)(lastTimeSeconds*(disturbance+654)*100000			);
	for(uint32_t x=0; x < displayWidth; ++x)
		//if(display.DisplayWeights[displayDepth-1][x] == 0)
		if(	display.Screen[displayDepth-1][x] != '0' &&
			display.Screen[displayDepth-1][x] != 'o' &&
			display.Screen[displayDepth-1][x] != '.' &&
			display.Screen[displayDepth-1][x] != 'O'
		)
		{
			if( rand()%2 ) {
				display.Screen			[displayDepth-1][x] = (::gpk::noise1D(randBase + x, seed + 1203) > 0.0) ? 'o' : (::gpk::noise1D(randBase+561+x, seed+2135) > 0.0) ? '0' : (::gpk::noise1D(randBase+x+6, seed+103) > 0.0) ? '.' : 'O';
				display.DisplayWeights	[displayDepth-1][x] = .000001f;
				display.Speed			[displayDepth-1][x] = rand()*.001f+0.001f;
				display.SpeedTarget		[displayDepth-1][x] = rand()*.0025f+0.001f;
				display.Screen.DepthStencil	[displayDepth-1][x] = (rand() % 2) ? ::klib::ASCII_COLOR_INDEX_GREEN : ::klib::ASCII_COLOR_INDEX_DARKGREEN;
			}
		}

	for(uint32_t z=1; z<displayDepth; ++z)
	for(uint32_t x=0; x<displayWidth; ++x) {
		if(display.Screen[z][x] == ' ')
			continue;

		display.DisplayWeights[z][x] += (float)(lastTimeSeconds * display.Speed[z][x]);

		if(display.Speed[z][x] < display.SpeedTarget[z][x])
			display.Speed[z][x] += (float)((display.Speed[z][x] * lastTimeSeconds));
		else
			display.Speed[z][x] -= (float)((display.Speed[z][x] * lastTimeSeconds));

		display.Speed[z][x] *= .999f;
	}

	for(uint32_t z=1; z<displayDepth; ++z)
	for(uint32_t x=0; x<displayWidth; ++x) {
		if(display.Screen[z][x] == ' ')
			continue;

		if(display.DisplayWeights[z][x] > 1.0) {
			int randX = (rand()%2) ? rand()%(1+disturbance*2)-disturbance : 0;
			if(1 == z) {
				display.Screen			[0][x]	= ' ';
				display.DisplayWeights	[0][x]	= 0;
				display.Speed			[0][x]	= 0;
				display.SpeedTarget		[0][x]	= 0;
				display.Screen.DepthStencil	[0][x]	= ::klib::ASCII_COLOR_INDEX_WHITE;
			}
			else {
				int32_t									xpos				= ::gpk::min(x + randX, displayWidth - 1);
				if((rand()%10) == 0)  {
					display.Screen[z-1][xpos]			= ' ';
					display.DisplayWeights[z-1][xpos]	= 0;
				}
				else {
							if( '0' == display.Screen[z][x] && z < (displayDepth/5*4))	display.Screen[z-1][xpos] = 'O';
					else if( 'O' == display.Screen[z][x] && z < (displayDepth/3*2))	display.Screen[z-1][xpos] = (::gpk::noise1D(randBase+x, seed+12345) > 0.0) ? 'o' : '\'';
					else if( 'o' == display.Screen[z][x] && z < (displayDepth>>1))	display.Screen[z-1][xpos] = '.';
					else
						display.Screen[z-1][xpos]	= display.Screen[z][x];

					display.Screen.DepthStencil	[z-1][xpos]	= (::gpk::noise1D(randBase+x+x, seed+41203) > 0.0) ? ::klib::ASCII_COLOR_INDEX_DARKGREEN : ::klib::ASCII_COLOR_INDEX_GREEN;
					display.DisplayWeights	[z-1][xpos]	= 0.00001f;
					display.Speed			[z-1][xpos]	= display.Speed[z][x];
					display.SpeedTarget		[z-1][xpos]	= (float)::gpk::noiseNormal1D(x, seed) * 20.0f * (z*1.0f/displayDepth)+0.001f;
				}
			}

			display.Screen			[z][x]		= ' ';
			display.DisplayWeights	[z][x]		= 0;
			display.Speed			[z][x]		= 0;
			display.SpeedTarget		[z][x]		= 0;
			display.Screen.DepthStencil	[z][x]		= ::klib::ASCII_COLOR_INDEX_WHITE;
		}
	}
}

SGameState									drawResearch				(SGame& instanceGame, const SGameState& returnState) {
	const ::gpk::view_const_string					textToPrint					= "Research center.";

	static ::klib::SMessageSlow						slowMessage;
	bool											bDonePrinting				= ::klib::getMessageSlow(slowMessage, textToPrint, instanceGame.FrameTimer.LastTimeSeconds);
	::gpk::SCoord2<uint32_t>						position					= instanceGame.TacticalDisplay.Screen.metrics() / 2;
	const uint32_t									messageLen					= (uint32_t)strlen(slowMessage.Message);
	memcpy(&instanceGame.TacticalDisplay.Screen[position.y][position.x - (messageLen + 1) / 2], slowMessage.Message, messageLen);
	if ( !bDonePrinting )
		return returnState;

	drawBubblesBackground(instanceGame.TacticalDisplay, instanceGame.FrameTimer.LastTimeSeconds);
	return drawResearchMenu(instanceGame, returnState);
};
