//#define NOMINMAX

#include "Game.h"
#include "draw.h"

#include "Item.h"
#include "Enemy.h"

template <typename _tEntity>
static int32_t										reinitBuyMenuItem			(::klib::SMenuItem<::klib::SBuyable> & menuItem, const ::gpk::view_const_char & nameItem, const ::klib::SEntityContainer<_tEntity> & inventory) {
	char													preformatted	[256]		= {};
	int32_t													indexElement				= inventory.FindElement({menuItem.ReturnValue.Definition, 0, menuItem.ReturnValue.Grade, -1});
	char													strCount		[32];
	sprintf_s(strCount, "%i", (indexElement != -1) ? inventory[indexElement].Count : 0);
	::sprintf_s(preformatted, "x%3.3s: %s", strCount, nameItem.begin() ); menuItem.Text = preformatted;
	return 0;
}

template <typename _tEntity>
static int32_t										initBuyMenuItem				(const int32_t itemIndex, const ::klib::SEntityRecord<_tEntity> & definition, ::klib::SMenuItem<::klib::SBuyable> & menuItem, ::gpk::array_pod<char_t> & nameItem) {
	char													preformatted	[256]		= {};
	char													strPrice		[64];
	::sprintf_s(strPrice, "%lli", definition.Points.PriceBuy);
	::sprintf_s(preformatted, "%-28.28s $%8.8s", definition.Name.begin(), strPrice);
	menuItem											= { { (int16_t)itemIndex, 1, definition.Points.PriceBuy, definition.Points.CostMaintenance, definition.Name}, ""};
	nameItem											= preformatted;
	return 0;
}

int32_t												klib::initBuyMenus			(const ::klib::SEntityTables & entityTables, ::klib::SShopMenus& menus) {
	char													preformatted[256]			= {};
	for(uint32_t i = 0, itemCount = entityTables.Accessory	.Definitions.size(); i<itemCount; ++i){ ::initBuyMenuItem(i, entityTables.Accessory		.Definitions[i], menus.MenuItemsAccessory	[i], menus.NamesAccessory	[i]); };
	for(uint32_t i = 0, itemCount = entityTables.StageProp	.Definitions.size(); i<itemCount; ++i){ ::initBuyMenuItem(i, entityTables.StageProp		.Definitions[i], menus.MenuItemsStageProp	[i], menus.NamesStageProp	[i]); };
	for(uint32_t i = 0, itemCount = entityTables.Facility	.Definitions.size(); i<itemCount; ++i){ ::initBuyMenuItem(i, entityTables.Facility		.Definitions[i], menus.MenuItemsFacility	[i], menus.NamesFacility	[i]); };
	for(uint32_t i = 0, itemCount = entityTables.Vehicle	.Definitions.size(); i<itemCount; ++i){ ::initBuyMenuItem(i, entityTables.Vehicle		.Definitions[i], menus.MenuItemsVehicle		[i], menus.NamesVehicle		[i]); };
	for(uint32_t i = 0, itemCount = entityTables.Profession	.Definitions.size(); i<itemCount; ++i){ ::initBuyMenuItem(i, entityTables.Profession	.Definitions[i], menus.MenuItemsProfession	[i], menus.NamesProfession	[i]); };
	for(uint32_t i = 0, itemCount = entityTables.Weapon		.Definitions.size(); i<itemCount; ++i){ ::initBuyMenuItem(i, entityTables.Weapon		.Definitions[i], menus.MenuItemsWeapon		[i], menus.NamesWeapon		[i]); };
	for(uint32_t i = 0, itemCount = entityTables.Armor		.Definitions.size(); i<itemCount; ++i){ ::initBuyMenuItem(i, entityTables.Armor			.Definitions[i], menus.MenuItemsArmor		[i], menus.NamesArmor		[i]); };
	for(uint32_t i = 0, itemCount = (uint32_t)::gpk::size(::klib::itemDescriptions); i<itemCount; ++i){ char strPrice [64]; sprintf_s(strPrice, "%lli", ::klib::itemDescriptions[i].Price			); ::sprintf_s(preformatted, "%-28.28s $%8.8s", ::klib::itemDescriptions[i].Name.begin(), strPrice); menus.MenuItemsItem	[i] = { { (int16_t)i, 1, ::klib::itemDescriptions[i].Price			, 0													, ::klib::itemDescriptions	[i].Name}, ""};	menus.NamesItem	[i] = preformatted; };
	for(uint32_t i = 0, itemCount = (uint32_t)::gpk::size(::klib::enemyDefinitions); i<itemCount; ++i){ char strPrice [64]; sprintf_s(strPrice, "%lli", ::klib::enemyDefinitions[i].Points.PriceBuy	); ::sprintf_s(preformatted, "%-28.28s $%8.8s", ::klib::enemyDefinitions[i].Name.begin(), strPrice); menus.MenuItemsAgent	[i] = { { (int16_t)i, 1, ::klib::enemyDefinitions[i].Points.PriceBuy, ::klib::enemyDefinitions[i].Points.CostMaintenance, ::klib::enemyDefinitions	[i].Name}, ""};	menus.NamesAgent[i] = preformatted; };
	return 0;
}

int32_t												klib::reinitBuyMenus		(const ::klib::SEntityTables & entityTables, ::klib::SCharacterInventory & playerInventory, ::klib::SShopMenus& menus) {
	char													preformatted[256]			= {};
	for(uint32_t i = 0, itemCount = entityTables.Accessory	.Definitions.size(); i<itemCount; ++i){	::reinitBuyMenuItem(menus.MenuItemsAccessory	[i], menus.NamesAccessory	[i], playerInventory.Accessory	);  }
	for(uint32_t i = 0, itemCount = entityTables.StageProp	.Definitions.size(); i<itemCount; ++i){	::reinitBuyMenuItem(menus.MenuItemsStageProp	[i], menus.NamesStageProp	[i], playerInventory.StageProp	);  }
	for(uint32_t i = 0, itemCount = entityTables.Facility	.Definitions.size(); i<itemCount; ++i){	::reinitBuyMenuItem(menus.MenuItemsFacility		[i], menus.NamesFacility	[i], playerInventory.Facility	);  }
	for(uint32_t i = 0, itemCount = entityTables.Vehicle	.Definitions.size(); i<itemCount; ++i){	::reinitBuyMenuItem(menus.MenuItemsVehicle		[i], menus.NamesVehicle		[i], playerInventory.Vehicle	);  }
	for(uint32_t i = 0, itemCount = entityTables.Profession	.Definitions.size(); i<itemCount; ++i){	::reinitBuyMenuItem(menus.MenuItemsProfession	[i], menus.NamesProfession	[i], playerInventory.Profession	);  }
	for(uint32_t i = 0, itemCount = entityTables.Weapon		.Definitions.size(); i<itemCount; ++i){	::reinitBuyMenuItem(menus.MenuItemsWeapon		[i], menus.NamesWeapon		[i], playerInventory.Weapon		);  }
	for(uint32_t i = 0, itemCount = entityTables.Armor		.Definitions.size(); i<itemCount; ++i){	::reinitBuyMenuItem(menus.MenuItemsArmor		[i], menus.NamesArmor		[i], playerInventory.Armor		);  }
	for(uint32_t i = 0, itemCount = ::gpk::size(::klib::itemDescriptions); i<itemCount; ++i){	int32_t indexElement = playerInventory.Items.FindElement({ menus.MenuItemsItem[i].ReturnValue.Definition, 0, menus.MenuItemsItem[i].ReturnValue.Grade, -1}); char strCount [32]; sprintf_s(strCount, "%i", (indexElement != -1) ? playerInventory.Items[indexElement].Count : 0); ::sprintf_s(preformatted, "x%3.3s: %s", strCount, menus.NamesItem[i].begin() ); menus.MenuItemsItem[i].Text = preformatted; }
	for(uint32_t i = 0, itemCount = ::gpk::size(::klib::enemyDefinitions); i<itemCount; ++i){	menus.MenuItemsAgent[i].Text = menus.NamesAgent[i]; }
	return 0;
}

::klib::SGameState									drawBuyMenu					(::klib::SGame& instanceGame, const ::klib::SGameState& returnState) {
#define MAX_BUY_ITEMS 64
	::klib::SShopMenus										& menus						= instanceGame.ShopMenus;
	::klib::SGamePlayer										& player					= instanceGame.Players[::klib::PLAYER_INDEX_USER];
	static ::klib::SMenuItem<::klib::SBuyable>				menuItems[MAX_BUY_ITEMS+1]	= {};
	::klib::SBuyable										selectedChoice				= {-1, -1, LLONG_MAX, LLONG_MAX, "Invalid item"};
	switch(instanceGame.State.Substate) {
	case ::klib::GAME_SUBSTATE_ACCESSORY	: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuAccessory	, ::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>{menus.MenuItemsAccessory		}, instanceGame.FrameInput, {-1});	} break;
	case ::klib::GAME_SUBSTATE_STAGEPROP	: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuStageProp	, ::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>{menus.MenuItemsStageProp		}, instanceGame.FrameInput, {-1});	} break;
	case ::klib::GAME_SUBSTATE_FACILITY		: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuFacility		, ::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>{menus.MenuItemsFacility		}, instanceGame.FrameInput, {-1});	} break;
	case ::klib::GAME_SUBSTATE_VEHICLE		: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuVehicle		, ::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>{menus.MenuItemsVehicle		}, instanceGame.FrameInput, {-1});	} break;
	case ::klib::GAME_SUBSTATE_PROFESSION	: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuProfession	, ::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>{menus.MenuItemsProfession	}, instanceGame.FrameInput, {-1});	} break;
	case ::klib::GAME_SUBSTATE_WEAPON		: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuWeapon		, ::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>{menus.MenuItemsWeapon		}, instanceGame.FrameInput, {-1});	} break;
	case ::klib::GAME_SUBSTATE_ARMOR		: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuArmor		, ::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>{menus.MenuItemsArmor			}, instanceGame.FrameInput, {-1});	} break;
	case ::klib::GAME_SUBSTATE_ITEM			: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuItem			, ::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>{menus.MenuItemsItem			}, instanceGame.FrameInput, {-1});	} break;
	case ::klib::GAME_SUBSTATE_CHARACTER	: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuAgent		, ::gpk::view_array<const ::klib::SMenuItem<::klib::SBuyable>>{menus.MenuItemsAgent	,  4	}, instanceGame.FrameInput, {-1});	} break;
	default:
		break;
	}

	if(selectedChoice.Definition == ::klib::SHOP_EXIT_VALUE)
		return {::klib::GAME_STATE_MENU_BUY};
	else if( selectedChoice.Definition == -1 )
		return returnState;

	::klib::SGameState										retVal						= returnState;
	instanceGame.ClearMessages();
	if(selectedChoice.Price > player.Tactical.Money) {
		instanceGame.Messages.UserError		= ::gpk::view_const_string{"You don't have enough money for "};
		instanceGame.Messages.UserError.append(selectedChoice.Name);
		instanceGame.Messages.UserError.append_string("!!");
		instanceGame.LogError();
		return retVal;
	}

	::klib::SCharacterInventory		& playerInventory		= player.Inventory;
	int32_t							iCharacterInArmy		= 0
		,							armySize				= player.Tactical.Army.size()
		;
	bool							bFoundFreeCharacterSlot	= false;
	bool							bSold					= false;
	::gpk::ptr_obj<::klib::CCharacter> newCharacter;
	switch(instanceGame.State.Substate) {
	case ::klib::GAME_SUBSTATE_ACCESSORY	:	if(playerInventory.Accessory	.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case ::klib::GAME_SUBSTATE_STAGEPROP	:	if(playerInventory.StageProp	.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case ::klib::GAME_SUBSTATE_FACILITY		:	if(playerInventory.Facility		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case ::klib::GAME_SUBSTATE_VEHICLE		:	if(playerInventory.Vehicle		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case ::klib::GAME_SUBSTATE_PROFESSION	:	if(playerInventory.Profession	.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" Job License for "	); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case ::klib::GAME_SUBSTATE_WEAPON		:	if(playerInventory.Weapon		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case ::klib::GAME_SUBSTATE_ARMOR		:	if(playerInventory.Armor		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case ::klib::GAME_SUBSTATE_ITEM			:	if(playerInventory.Items		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case ::klib::GAME_SUBSTATE_CHARACTER	:
		newCharacter.create(::klib::enemyDefinitions[selectedChoice.Definition]);
		for(iCharacterInArmy ; iCharacterInArmy < armySize; ++iCharacterInArmy) {
			if(0 == player.Tactical.Army[iCharacterInArmy]) {
				player.Tactical.Army[iCharacterInArmy]		= newCharacter;
				bFoundFreeCharacterSlot						= true;
				break;
			}
		}
		if(!bFoundFreeCharacterSlot)
			player.Tactical.Army.push_back(newCharacter);
		::klib::setupAgent(instanceGame.EntityTables, *newCharacter, *newCharacter);
		instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully hired "};
		instanceGame.Messages.UserSuccess.append(selectedChoice.Name);
		instanceGame.Messages.UserSuccess.append_string(" for ");
		char						maintCost [64];
		sprintf_s(maintCost, "%lli", selectedChoice.MaintenanceCost);
		instanceGame.Messages.UserSuccess.append_string(maintCost);
		instanceGame.Messages.UserSuccess.append_string(" Coins/Mission.");
		bSold = true;
		break;
	default:
		break;
	}

	if(bSold) {
		instanceGame.LogSuccess();
		player.Tactical.Money				-= selectedChoice.Price;
		player.Tactical.Score.MoneySpent	+= selectedChoice.Price;
		::klib::reinitBuyMenus(instanceGame.EntityTables, player.Inventory, menus);
	}
	else {
		instanceGame.Messages.UserError		= "There is not enough space in your inventory!";
		instanceGame.LogError();
	}

	return retVal;
}

::klib::SGameState							drawBuy								(::klib::SGame& instanceGame, const ::klib::SGameState& returnState) {
	static const ::gpk::view_const_string			textToPrint							= ::gpk::view_const_string{"Tell me how much money you have and I will tell you what you'll become."};

	static ::klib::SMessageSlow						slowMessage;
	bool											bDonePrinting						= ::klib::getMessageSlow(slowMessage, textToPrint.begin(), textToPrint.size(), instanceGame.FrameTimer.LastTimeSeconds*3);
	::gpk::SCoord2<uint32_t>						position							= instanceGame.TacticalDisplay.Screen.metrics() / 2;
	const uint32_t									messageLen							= (uint32_t)strlen(slowMessage.Message);
	memcpy(&instanceGame.TacticalDisplay.Screen[position.y][position.x - (messageLen + 1) / 2], slowMessage.Message, messageLen);
	if ( !bDonePrinting )
		return returnState;

	if(::klib::GAME_SUBSTATE_MAIN == instanceGame.State.Substate) {
		static ::klib::SMenuHeader<::klib::SGameState>		menuBuy								({::klib::GAME_STATE_WELCOME_COMMANDER}, ::gpk::view_const_string{"Order Menu"}, 26);
		return ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menuBuy, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{::klib::optionsBuy}, instanceGame.FrameInput, instanceGame.State);
	}
	else
		return drawBuyMenu(instanceGame, returnState);
};


