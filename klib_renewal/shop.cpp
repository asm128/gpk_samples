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
static int32_t										initBuyMenuItem				(const int32_t itemIndex, const ::klib::SEntityRecord<_tEntity> & definition, ::klib::SMenuItem<::klib::SBuyable> & menuItem, ::gpk::apod<char> & nameItem) {
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
	::klib::SShopMenus										& menus						= instanceGame.ShopMenus;
	::gpk::view_const_char									menuTitle;
	static ::klib::SDrawMenuState							menuState;
	::gpk::array_obj<::gpk::view_const_char>				menuItemsText;
	switch(instanceGame.State.Substate) {
	case ::klib::GAME_SUBSTATE_ACCESSORY	: { menuTitle = menus.MenuAccessory	.Title; for(uint32_t iItem = 0; iItem < ::gpk::size(menus.NamesAccessory	); ++iItem) { if(0 == menus.MenuItemsAccessory	[iItem].Text.size()) break; menuItemsText.push_back(menus.MenuItemsAccessory	[iItem].Text); } break; }
	case ::klib::GAME_SUBSTATE_STAGEPROP	: { menuTitle = menus.MenuStageProp	.Title; for(uint32_t iItem = 0; iItem < ::gpk::size(menus.NamesStageProp	); ++iItem) { if(0 == menus.MenuItemsStageProp	[iItem].Text.size()) break; menuItemsText.push_back(menus.MenuItemsStageProp	[iItem].Text); } break; }
	case ::klib::GAME_SUBSTATE_FACILITY		: { menuTitle = menus.MenuFacility	.Title; for(uint32_t iItem = 0; iItem < ::gpk::size(menus.NamesFacility		); ++iItem) { if(0 == menus.MenuItemsFacility	[iItem].Text.size()) break; menuItemsText.push_back(menus.MenuItemsFacility		[iItem].Text); } break; }
	case ::klib::GAME_SUBSTATE_VEHICLE		: { menuTitle = menus.MenuVehicle	.Title; for(uint32_t iItem = 0; iItem < ::gpk::size(menus.NamesVehicle		); ++iItem) { if(0 == menus.MenuItemsVehicle	[iItem].Text.size()) break; menuItemsText.push_back(menus.MenuItemsVehicle		[iItem].Text); } break; }
	case ::klib::GAME_SUBSTATE_PROFESSION	: { menuTitle = menus.MenuProfession.Title; for(uint32_t iItem = 0; iItem < ::gpk::size(menus.NamesProfession	); ++iItem) { if(0 == menus.MenuItemsProfession	[iItem].Text.size()) break; menuItemsText.push_back(menus.MenuItemsProfession	[iItem].Text); } break; }
	case ::klib::GAME_SUBSTATE_WEAPON		: { menuTitle = menus.MenuWeapon	.Title; for(uint32_t iItem = 0; iItem < ::gpk::size(menus.NamesWeapon		); ++iItem) { if(0 == menus.MenuItemsWeapon		[iItem].Text.size()) break; menuItemsText.push_back(menus.MenuItemsWeapon		[iItem].Text); } break; }
	case ::klib::GAME_SUBSTATE_ARMOR		: { menuTitle = menus.MenuArmor		.Title; for(uint32_t iItem = 0; iItem < ::gpk::size(menus.NamesArmor		); ++iItem) { if(0 == menus.MenuItemsArmor		[iItem].Text.size()) break; menuItemsText.push_back(menus.MenuItemsArmor		[iItem].Text); } break; }
	case ::klib::GAME_SUBSTATE_ITEM			: { menuTitle = menus.MenuItem		.Title; for(uint32_t iItem = 0; iItem < ::gpk::size(menus.NamesItem			); ++iItem) { if(0 == menus.MenuItemsItem		[iItem].Text.size()) break; menuItemsText.push_back(menus.MenuItemsItem			[iItem].Text); } break; }
	case ::klib::GAME_SUBSTATE_CHARACTER	: { menuTitle = menus.MenuAgent		.Title; for(uint32_t iItem = 0; iItem < 4/*::gpk::size(menus.NamesAgent	  )*/; ++iItem) { if(0 == menus.MenuItemsAgent		[iItem].Text.size()) break; menuItemsText.push_back(menus.MenuItemsAgent		[iItem].Text); } break; }
	default:
		break;
	}
	if(0 == menuTitle.size())
		return returnState;
	int32_t													selectedIndex				= ::klib::drawMenu
		( menuState
		, instanceGame.GlobalDisplay.Screen.Color.View
		, instanceGame.GlobalDisplay.Screen.DepthStencil.begin()
		, menuTitle
		, menuItemsText
		, instanceGame.FrameInput
		, {-1}
		, 50
		);

	if(selectedIndex == (int32_t)menuItemsText.size())
		return {::klib::GAME_STATE_MENU_BUY};
	else if( selectedIndex == -1 )
		return returnState;

	::gpk::ptr_obj<::klib::CCharacter> newCharacter;
	switch(instanceGame.State.Substate) {
	case ::klib::GAME_SUBSTATE_ACCESSORY	: instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, selectedIndex}); break;
	case ::klib::GAME_SUBSTATE_STAGEPROP	: instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, selectedIndex}); break;
	case ::klib::GAME_SUBSTATE_FACILITY		: instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, selectedIndex}); break;
	case ::klib::GAME_SUBSTATE_VEHICLE		: instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, selectedIndex}); break;
	case ::klib::GAME_SUBSTATE_PROFESSION	: instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, selectedIndex}); break;
	case ::klib::GAME_SUBSTATE_WEAPON		: instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, selectedIndex}); break;
	case ::klib::GAME_SUBSTATE_ARMOR		: instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, selectedIndex}); break;
	case ::klib::GAME_SUBSTATE_ITEM			: instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, selectedIndex}); break;
	case ::klib::GAME_SUBSTATE_CHARACTER	: instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, selectedIndex}); break;
	default:
		break;
	}
	return returnState;
}

::klib::SGameState							drawBuy								(::klib::SGame& instanceGame, const ::klib::SGameState& returnState) {
	static const ::gpk::view_const_string			textToPrint							= ::gpk::view_const_string{"Tell me how much money you have and I will tell you what you'll become."};

	static ::klib::SMessageSlow						slowMessage;
	bool											bDonePrinting						= ::klib::getMessageSlow(slowMessage, textToPrint.begin(), textToPrint.size(), instanceGame.FrameTimer.LastTimeSeconds*3);
	::gpk::n2<uint32_t>						position							= instanceGame.TacticalDisplay.Screen.metrics() / 2;
	const uint32_t									messageLen							= (uint32_t)strlen(slowMessage.Message);
	memcpy(&instanceGame.TacticalDisplay.Screen[position.y][position.x - (messageLen + 1) / 2], slowMessage.Message, messageLen);
	if ( !bDonePrinting )
		return returnState;

	if(::klib::GAME_SUBSTATE_MAIN == instanceGame.State.Substate) {
		static ::klib::SMenuHeader<::klib::SGameState>		menuBuy								({::klib::GAME_STATE_WELCOME_COMMANDER}, ::gpk::view_const_string{"Order Menu"}, 26);
		return ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menuBuy, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{::klib::optionsBuy}, instanceGame.FrameInput, instanceGame.State);
	}
	else
		return ::drawBuyMenu(instanceGame, returnState);
};


