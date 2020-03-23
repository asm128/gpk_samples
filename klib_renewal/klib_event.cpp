#include "Game.h"
#include "helper_projects.h"
#include "Enemy.h"

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

template<typename _tEntity>
static	::gpk::error_t					buyEntity					(const ::klib::SBuyable & buyable, ::klib::SGamePlayer & player, ::klib::SEntityContainer<_tEntity> & inventory, ::klib::SGameMessages & messages) {
	if(buyable.Price > player.Tactical.Money) {
		messages.UserError						= ::gpk::view_const_string{"You don't have enough money for "};
		messages.UserError.append(buyable.Name);
		messages.UserError.append_string("!!");
		messages.LogError();
		return 1;
	}
	if(inventory.AddElement({buyable.Definition, 0, buyable.Grade, -1})) {
		messages.UserSuccess					= ::gpk::view_const_string{"You have successfully bought "};
		messages.UserSuccess.append(buyable.Name);
		messages.UserSuccess.append_string(" for ");
		char										strPrice [64];
		sprintf_s(strPrice, "%lli", buyable.Price);
		messages.UserSuccess.append_string(strPrice);
		messages.UserSuccess.append_string(" Coins.");
		return 0;
	}
	return 2;
}

static	::gpk::error_t					eventProcessBuy				(const ::klib::SGameEvent & event, ::klib::SShopMenus & shopMenus, ::klib::SGamePlayer & player, ::klib::SGameMessages & messages, const ::klib::SEntityTables & entityTables) {
	const int32_t								item						= (int32_t) (((uint64_t)event.Value) & 0x7FFFFFFF);
	bool										sold						= false;
	::klib::SBuyable							buyable						= {-1, -1, LLONG_MAX, LLONG_MAX, "Invalid item"};
	::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>	menuItems					= {};
	::gpk::view_array<const ::klib::CCharacter>	menuCharacters				= {};
	switch(event.GameState.Substate) {
	case ::klib::GAME_SUBSTATE_ACCESSORY	: { menuItems = shopMenus.MenuItemsAccessory	; buyable = menuItems[item].ReturnValue; sold = 0 == buyEntity(buyable, player, player.Inventory.Accessory	, messages); break; }
	case ::klib::GAME_SUBSTATE_STAGEPROP	: { menuItems = shopMenus.MenuItemsStageProp	; buyable = menuItems[item].ReturnValue; sold = 0 == buyEntity(buyable, player, player.Inventory.StageProp	, messages); break; }
	case ::klib::GAME_SUBSTATE_FACILITY		: { menuItems = shopMenus.MenuItemsFacility		; buyable = menuItems[item].ReturnValue; sold = 0 == buyEntity(buyable, player, player.Inventory.Facility	, messages); break; }
	case ::klib::GAME_SUBSTATE_VEHICLE		: { menuItems = shopMenus.MenuItemsVehicle		; buyable = menuItems[item].ReturnValue; sold = 0 == buyEntity(buyable, player, player.Inventory.Vehicle	, messages); break; }
	case ::klib::GAME_SUBSTATE_PROFESSION	: { menuItems = shopMenus.MenuItemsProfession	; buyable = menuItems[item].ReturnValue; sold = 0 == buyEntity(buyable, player, player.Inventory.Profession	, messages); break; }
	case ::klib::GAME_SUBSTATE_WEAPON		: { menuItems = shopMenus.MenuItemsWeapon		; buyable = menuItems[item].ReturnValue; sold = 0 == buyEntity(buyable, player, player.Inventory.Weapon		, messages); break; }
	case ::klib::GAME_SUBSTATE_ARMOR		: { menuItems = shopMenus.MenuItemsArmor		; buyable = menuItems[item].ReturnValue; sold = 0 == buyEntity(buyable, player, player.Inventory.Armor		, messages); break; }
	case ::klib::GAME_SUBSTATE_ITEM			: { menuItems = shopMenus.MenuItemsItem			; buyable = menuItems[item].ReturnValue; sold = 0 == buyEntity(buyable, player, player.Inventory.Items		, messages); break; }
	case ::klib::GAME_SUBSTATE_CHARACTER	: {
		menuItems								= shopMenus.MenuItemsAgent;
		buyable									= menuItems[item].ReturnValue;
		::gpk::ptr_obj<::klib::CCharacter>			newCharacter;
		menuCharacters							= {::klib::enemyDefinitions, 4};
		newCharacter.create(menuCharacters[item]);
		bool										bFoundFreeCharacterSlot		= false;
		for(uint32_t iAgent = 0, countAgents = player.Tactical.Army.size(); iAgent < countAgents; ++iAgent) {
			if(0 == player.Tactical.Army[iAgent]) {
				player.Tactical.Army[iAgent]		= newCharacter;
				bFoundFreeCharacterSlot				= true;
				break;
			}
		}
		if(!bFoundFreeCharacterSlot)
			player.Tactical.Army.push_back(newCharacter);
		::klib::setupAgent(entityTables, *newCharacter, *newCharacter);
		messages.UserSuccess = ::gpk::view_const_string{"You have successfully hired "};
		messages.UserSuccess.append(buyable.Name);
		messages.UserSuccess.append_string(" for ");
		char										maintCost [64];
		sprintf_s(maintCost, "%lli", buyable.MaintenanceCost);
		messages.UserSuccess.append_string(maintCost);
		messages.UserSuccess.append_string(" Coins/Mission.");
		sold									= true;
		}
		break;
	}
	if(sold) {
		messages.LogSuccess();
		player.Tactical.Money					-= buyable.Price;
		player.Tactical.Score.MoneySpent		+= buyable.Price;
		::klib::reinitBuyMenus(entityTables, player.Inventory, shopMenus);
	}
	else {
		messages.UserError						= "There is not enough space in your inventory!";
		messages.LogError();
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
		case ::klib::GAME_STATE_MENU_BUY		: ::eventProcessBuy		(event, instanceGame.ShopMenus, player, instanceGame.Messages, instanceGame.EntityTables);	break;
		}
	}
	instanceGame.Events.clear();
	return 0;
}
