//#define NOMINMAX

#include "Agent_helper.h"
#include "draw.h"


using namespace klib;

SGameState														drawSquadSetupMenu									(SGame& instanceGame, const SGameState& returnValue)												{
	drawSquadSlots(instanceGame);
	SPlayer																& player											= instanceGame.Players[PLAYER_INDEX_USER];

	static SMenuItem<int32_t>											menuItems[MAX_AGENT_SQUAD_SLOTS]					= {};
	static int32_t														maxNameLen											= 0;
	for(uint32_t i=0, count=player.Squad.Size/*(uint32_t)::gpk::size(menuItems)*/; i<count; i++) {
		menuItems[i].ReturnValue										= i;

		char																buffer[128];
		if(player.Squad.Agents[i] != -1)  {
			const CCharacter													& playerAgent										= *player.Army[player.Squad.Agents[i]];
			maxNameLen														= std::max(maxNameLen, sprintf_s(buffer, "Agent #%u: %s", i+1, playerAgent.Name.c_str()));
			menuItems[i].Text												= buffer;
		}
		else {
			maxNameLen														= std::max(maxNameLen, sprintf_s(buffer, "Agent #%u: Empty slot", i+1));
			menuItems[i].Text												= buffer;
		}
	}

	int32_t																result												= drawMenu(instanceGame.GlobalDisplay.Screen, &instanceGame.GlobalDisplay.TextAttributes.Cells[0][0], player.Squad.Size, "Squad setup", menuItems, instanceGame.FrameInput, (int32_t)::gpk::size(player.Squad.Agents), -1, ::gpk::max(24, maxNameLen+4));
	if(::gpk::size(player.Squad.Agents) == result)
		return {GAME_STATE_WELCOME_COMMANDER};

	if( result < 0 || result >= (int32_t)::gpk::size(player.Squad.Agents) )
		return {GAME_STATE_MENU_SQUAD_SETUP};

	player.Selection.PlayerUnit										= result;

	if( player.Squad.Agents[result] != -1 && 0 == instanceGame.FrameInput.Keys[VK_LSHIFT] )
		return {GAME_STATE_MENU_EQUIPMENT};

	return {GAME_STATE_MENU_EQUIPMENT, GAME_SUBSTATE_CHARACTER};
}