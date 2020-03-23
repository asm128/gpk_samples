#include "Game.h"
#include "helper_projects.h"

static	::gpk::error_t					eventProcessResearch		(const ::klib::SGameEvent & event, ::klib::SGamePlayer & player, ::klib::SGameMessages & messages, const ::klib::SEntityTables & entityTables) {
	const ::klib::SEntityResearch				& acknowladgedResearch		= player.ResearchablesValue[(int32_t)event.Value];
	switch(event.Event) {
	default: break;
	case ::klib::GAME_EVENT_CONFIRM:
		::klib::acknowledgeResearch(acknowladgedResearch, player.Projects, messages.UserSuccess);
		messages.LogSuccess();
		::klib::playerUpdateResearchLists(entityTables, player);
		break;
	}
	return 0;
}

static	::gpk::error_t					eventProcessFactory			(const ::klib::SGameEvent & event, ::klib::SGamePlayer & player, ::klib::SGameMessages & messages) {
	const ::klib::SEntityResearch				& acknowladgedResearch		= player.ResearchedValue[(int32_t)event.Value];
	switch(event.Event) {
	default: break;
	case ::klib::GAME_EVENT_CONFIRM:
		::klib::acknowledgeProduction(acknowladgedResearch, player.Projects, messages.UserSuccess);
		messages.LogSuccess();
		break;
	}
	return 0;
}

static int32_t							agentAssign					(const ::klib::SEntityTables & entityTables, ::klib::SGameMessages & messages, ::klib::STacticalPlayer& player, int32_t agent, int32_t squadSlot) {
	ree_if((uint32_t)squadSlot	>= player.Squad.Size, "Invalid squad slot: %i. Squad size: %u.", squadSlot, player.Squad.Size);
	ree_if((uint32_t)agent		>= player.Army.size(), "Invalid agent: %i. Army size: %i.", agent, player.Army.size());
	player.Squad.Agents[squadSlot]			= (int16_t)agent;
	player.Army[agent]->Recalculate(entityTables);
	messages.UserSuccess					= ::gpk::view_const_string{"You assigned "};
	messages.UserSuccess.append(player.Army[agent]->Name);
	messages.UserSuccess.append_string(" as Agent #");
	char										playerUnitPlusOne [32];
	sprintf_s(playerUnitPlusOne, "%i", squadSlot + 1);
	messages.UserSuccess.append_string(playerUnitPlusOne);
	messages.LogSuccess();
	return 0;
}

static	::gpk::error_t					eventProcessEquip			(const ::klib::SGameEvent & event, ::klib::SGamePlayer & player, ::klib::SGameMessages & messages, const ::klib::SEntityTables & entityTables) {
	const int32_t								iAgent						= (int32_t) (((uint64_t)event.Value) & 0xFFFFFFFF);
	const int32_t								indexRow	 				= (int32_t)((((uint64_t)event.Value) & (0xFFFFFFFFULL << 32)) >> 32);
	switch(event.GameState.Substate) {
	case ::klib::GAME_SUBSTATE_CHARACTER	: ::agentAssign(entityTables, messages, player.Tactical, indexRow, iAgent); break;
	case ::klib::GAME_SUBSTATE_PROFESSION	: if(event.Event == ::klib::GAME_EVENT_CONFIRM) ::klib::equipIfResearchedProfession	(messages, player, entityTables, iAgent, (int16_t)indexRow); break;
	case ::klib::GAME_SUBSTATE_WEAPON		: if(event.Event == ::klib::GAME_EVENT_CONFIRM) ::klib::equipIfResearchedWeapon		(messages, player, entityTables, iAgent, (int16_t)indexRow); break;
	case ::klib::GAME_SUBSTATE_ARMOR		: if(event.Event == ::klib::GAME_EVENT_CONFIRM) ::klib::equipIfResearchedArmor		(messages, player, entityTables, iAgent, (int16_t)indexRow); break;
	case ::klib::GAME_SUBSTATE_ACCESSORY	: if(event.Event == ::klib::GAME_EVENT_CONFIRM) ::klib::equipIfResearchedAccessory	(messages, player, entityTables, iAgent, (int16_t)indexRow); break;
	default:
		break;
	}
	return 0;
}

::gpk::error_t							klib::eventProcess			(::klib::SGame& instanceGame)	{
	::klib::SGamePlayer							& player					= instanceGame.Players[::klib::PLAYER_INDEX_USER];
	instanceGame.Messages.UserSuccess.clear();
	for(uint32_t iEvent = 0; iEvent < instanceGame.Events.size(); ++iEvent) {
		const ::klib::SGameEvent					& event						= instanceGame.Events[iEvent];
		switch(event.GameState.State) {
		default: break;
		case ::klib::GAME_STATE_MENU_RESEARCH	: ::eventProcessResearch(event, player, instanceGame.Messages, instanceGame.EntityTables);	break;
		case ::klib::GAME_STATE_MENU_FACTORY	: ::eventProcessFactory	(event, player, instanceGame.Messages);								break;
		case ::klib::GAME_STATE_MENU_EQUIPMENT	: ::eventProcessEquip	(event, player, instanceGame.Messages, instanceGame.EntityTables);	break;
		}
	}
	instanceGame.Events.clear();
	return 0;
}
