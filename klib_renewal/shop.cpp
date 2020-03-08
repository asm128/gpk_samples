//#define NOMINMAX

#include "Game.h"
#include "draw.h"

#include "Item.h"
#include "Enemy.h"

using namespace klib;

struct SBuyable {
	int16_t						Definition;
	int16_t						Grade;
	int64_t						Price;
	int64_t						MaintenanceCost;
	::gpk::array_pod<char_t>	Name;
} selectedChoice;

#define SHOP_EXIT_VALUE 0x7FFF
struct SShopMenus {
	::klib::SMenuItem<SBuyable>		MenuItemsAccessory	[256]	= {};
	::klib::SMenuItem<SBuyable>		MenuItemsStageProp	[256]	= {};
	::klib::SMenuItem<SBuyable>		MenuItemsFacility	[256]	= {};
	::klib::SMenuItem<SBuyable>		MenuItemsVehicle	[256]	= {};
	::klib::SMenuItem<SBuyable>		MenuItemsProfession	[256]	= {};
	::klib::SMenuItem<SBuyable>		MenuItemsWeapon		[256]	= {};
	::klib::SMenuItem<SBuyable>		MenuItemsArmor		[256]	= {};
	::klib::SMenuItem<SBuyable>		MenuItemsItem		[256]	= {};
	::klib::SMenuItem<SBuyable>		MenuItemsAgent		[256]	= {};

	::klib::SMenuHeader<SBuyable>	MenuAccessory				= {{SHOP_EXIT_VALUE},	::gpk::view_const_string{"Accessory"	" a la carte"}, 48};
	::klib::SMenuHeader<SBuyable>	MenuStageProp				= {{SHOP_EXIT_VALUE},	::gpk::view_const_string{"Stage Prop"	" a la carte"}, 48};
	::klib::SMenuHeader<SBuyable>	MenuFacility				= {{SHOP_EXIT_VALUE},	::gpk::view_const_string{"Facility"		" a la carte"}, 48};
	::klib::SMenuHeader<SBuyable>	MenuVehicle					= {{SHOP_EXIT_VALUE},	::gpk::view_const_string{"Vehicle"		" a la carte"}, 48};
	::klib::SMenuHeader<SBuyable>	MenuProfession				= {{SHOP_EXIT_VALUE},	::gpk::view_const_string{"Job License"	" a la carte"}, 48};
	::klib::SMenuHeader<SBuyable>	MenuWeapon					= {{SHOP_EXIT_VALUE},	::gpk::view_const_string{"Weapon"		" a la carte"}, 48};
	::klib::SMenuHeader<SBuyable>	MenuArmor					= {{SHOP_EXIT_VALUE},	::gpk::view_const_string{"Armor"		" a la carte"}, 48};
	::klib::SMenuHeader<SBuyable>	MenuItem					= {{SHOP_EXIT_VALUE},	::gpk::view_const_string{"Item"			" a la carte"}, 48};
	::klib::SMenuHeader<SBuyable>	MenuAgent					= {{SHOP_EXIT_VALUE},	::gpk::view_const_string{"Agent"		" a la carte"}, 48};

	::gpk::array_pod<char_t>		NamesAccessory		[256]	= {};
	::gpk::array_pod<char_t>		NamesStageProp		[256]	= {};
	::gpk::array_pod<char_t>		NamesFacility		[256]	= {};
	::gpk::array_pod<char_t>		NamesVehicle		[256]	= {};
	::gpk::array_pod<char_t>		NamesProfession		[256]	= {};
	::gpk::array_pod<char_t>		NamesWeapon			[256]	= {};
	::gpk::array_pod<char_t>		NamesArmor			[256]	= {};
	::gpk::array_pod<char_t>		NamesItem			[256]	= {};
	::gpk::array_pod<char_t>		NamesAgent			[256]	= {};
};

static int32_t initBuyMenus(const ::klib::SEntityTables & entityTables, SShopMenus& menus) {
	char preformatted[256] = {};
	for(uint32_t i = 0, itemCount = entityTables.Accessory	.Definitions.size(); i<itemCount; ++i){ const ::klib::SEntityRecord<::klib::SAccessory	> &	definition = entityTables.Accessory		.Definitions[i]; char strPrice [64]; sprintf_s(strPrice, "%lli", definition.Points.PriceBuy); ::sprintf_s(preformatted, "%-28.28s $%8.8s", definition.Name.begin(), strPrice); menus.MenuItemsAccessory	[i] = { { (int16_t)i, 1, definition.Points.PriceBuy	, definition.Points.CostMaintenance	, definition.Name}, ""}; menus.NamesAccessory	[i] = preformatted; };
	for(uint32_t i = 0, itemCount = entityTables.StageProp	.Definitions.size(); i<itemCount; ++i){ const ::klib::SEntityRecord<::klib::SStageProp	> &	definition = entityTables.StageProp		.Definitions[i]; char strPrice [64]; sprintf_s(strPrice, "%lli", definition.Points.PriceBuy); ::sprintf_s(preformatted, "%-28.28s $%8.8s", definition.Name.begin(), strPrice); menus.MenuItemsStageProp	[i] = { { (int16_t)i, 1, definition.Points.PriceBuy	, definition.Points.CostMaintenance	, definition.Name}, ""}; menus.NamesStageProp	[i] = preformatted; };
	for(uint32_t i = 0, itemCount = entityTables.Facility	.Definitions.size(); i<itemCount; ++i){ const ::klib::SEntityRecord<::klib::SFacility	> &	definition = entityTables.Facility		.Definitions[i]; char strPrice [64]; sprintf_s(strPrice, "%lli", definition.Points.PriceBuy); ::sprintf_s(preformatted, "%-28.28s $%8.8s", definition.Name.begin(), strPrice); menus.MenuItemsFacility	[i] = { { (int16_t)i, 1, definition.Points.PriceBuy	, definition.Points.CostMaintenance	, definition.Name}, ""}; menus.NamesFacility	[i] = preformatted; };
	for(uint32_t i = 0, itemCount = entityTables.Vehicle	.Definitions.size(); i<itemCount; ++i){ const ::klib::SEntityRecord<::klib::SVehicle	> &	definition = entityTables.Vehicle		.Definitions[i]; char strPrice [64]; sprintf_s(strPrice, "%lli", definition.Points.PriceBuy); ::sprintf_s(preformatted, "%-28.28s $%8.8s", definition.Name.begin(), strPrice); menus.MenuItemsVehicle	[i] = { { (int16_t)i, 1, definition.Points.PriceBuy	, definition.Points.CostMaintenance	, definition.Name}, ""}; menus.NamesVehicle		[i] = preformatted; };
	for(uint32_t i = 0, itemCount = entityTables.Profession	.Definitions.size(); i<itemCount; ++i){ const ::klib::SEntityRecord<::klib::SProfession	> &	definition = entityTables.Profession	.Definitions[i]; char strPrice [64]; sprintf_s(strPrice, "%lli", definition.Points.PriceBuy); ::sprintf_s(preformatted, "%-28.28s $%8.8s", definition.Name.begin(), strPrice); menus.MenuItemsProfession[i] = { { (int16_t)i, 1, definition.Points.PriceBuy	, definition.Points.CostMaintenance	, definition.Name}, ""}; menus.NamesProfession	[i] = preformatted; };
	for(uint32_t i = 0, itemCount = entityTables.Weapon		.Definitions.size(); i<itemCount; ++i){ const ::klib::SEntityRecord<::klib::SWeapon		> &	definition = entityTables.Weapon		.Definitions[i]; char strPrice [64]; sprintf_s(strPrice, "%lli", definition.Points.PriceBuy); ::sprintf_s(preformatted, "%-28.28s $%8.8s", definition.Name.begin(), strPrice); menus.MenuItemsWeapon	[i] = { { (int16_t)i, 1, definition.Points.PriceBuy	, definition.Points.CostMaintenance	, definition.Name}, ""}; menus.NamesWeapon		[i] = preformatted; };
	for(uint32_t i = 0, itemCount = entityTables.Armor		.Definitions.size(); i<itemCount; ++i){ const ::klib::SEntityRecord<::klib::SArmor		> &	definition = entityTables.Armor			.Definitions[i]; char strPrice [64]; sprintf_s(strPrice, "%lli", definition.Points.PriceBuy); ::sprintf_s(preformatted, "%-28.28s $%8.8s", definition.Name.begin(), strPrice); menus.MenuItemsArmor		[i] = { { (int16_t)i, 1, definition.Points.PriceBuy	, definition.Points.CostMaintenance	, definition.Name}, ""}; menus.NamesArmor		[i] = preformatted; };
	for(uint32_t i = 0, itemCount = (uint32_t)::gpk::size(itemDescriptions); i<itemCount; ++i){ char strPrice [64]; sprintf_s(strPrice, "%lli", itemDescriptions[i].Price			); ::sprintf_s(preformatted, "%-28.28s $%8.8s", itemDescriptions[i].Name.begin(), strPrice); menus.MenuItemsItem	[i] = { { (int16_t)i, 1, itemDescriptions[i].Price			, 0											, itemDescriptions	[i].Name}, ""};	menus.NamesItem	[i] = preformatted; };
	for(uint32_t i = 0, itemCount = (uint32_t)::gpk::size(enemyDefinitions); i<itemCount; ++i){ char strPrice [64]; sprintf_s(strPrice, "%lli", enemyDefinitions[i].Points.PriceBuy	); ::sprintf_s(preformatted, "%-28.28s $%8.8s", enemyDefinitions[i].Name.begin(), strPrice); menus.MenuItemsAgent	[i] = { { (int16_t)i, 1, enemyDefinitions[i].Points.PriceBuy, enemyDefinitions[i].Points.CostMaintenance, enemyDefinitions	[i].Name}, ""};	menus.NamesAgent[i] = preformatted; };
	return 0;
}

static int32_t reinitBuyMenus(SGame& instanceGame, SShopMenus& menus) {
	char preformatted[256] = {};
	SCharacterInventory										& playerInventory			= instanceGame.Players[PLAYER_INDEX_USER].Inventory;
	for(size_t i = 0, itemCount = instanceGame.EntityTables.Accessory	.Definitions.size(); i<itemCount; ++i){	int32_t indexElement = playerInventory.Accessory	.FindElement({	menus.MenuItemsAccessory	[i].ReturnValue.Definition, 0, menus.MenuItemsAccessory		[i].ReturnValue.Grade, -1}); char strCount [32]; sprintf_s(strCount, "%i", (indexElement != -1) ? playerInventory.Accessory	[indexElement].Count : 0); ::sprintf_s(preformatted, "x%3.3s: %s", strCount, menus.NamesAccessory	[i].begin() );	menus.MenuItemsAccessory	[i].Text = preformatted; }
	for(size_t i = 0, itemCount = instanceGame.EntityTables.StageProp	.Definitions.size(); i<itemCount; ++i){	int32_t indexElement = playerInventory.StageProp	.FindElement({	menus.MenuItemsStageProp	[i].ReturnValue.Definition, 0, menus.MenuItemsStageProp		[i].ReturnValue.Grade, -1}); char strCount [32]; sprintf_s(strCount, "%i", (indexElement != -1) ? playerInventory.StageProp	[indexElement].Count : 0); ::sprintf_s(preformatted, "x%3.3s: %s", strCount, menus.NamesStageProp	[i].begin() );	menus.MenuItemsStageProp	[i].Text = preformatted; }
	for(size_t i = 0, itemCount = instanceGame.EntityTables.Facility	.Definitions.size(); i<itemCount; ++i){	int32_t indexElement = playerInventory.Facility		.FindElement({	menus.MenuItemsFacility		[i].ReturnValue.Definition, 0, menus.MenuItemsFacility		[i].ReturnValue.Grade, -1}); char strCount [32]; sprintf_s(strCount, "%i", (indexElement != -1) ? playerInventory.Facility	[indexElement].Count : 0); ::sprintf_s(preformatted, "x%3.3s: %s", strCount, menus.NamesFacility	[i].begin() );	menus.MenuItemsFacility		[i].Text = preformatted; }
	for(size_t i = 0, itemCount = instanceGame.EntityTables.Vehicle		.Definitions.size(); i<itemCount; ++i){	int32_t indexElement = playerInventory.Vehicle		.FindElement({	menus.MenuItemsVehicle		[i].ReturnValue.Definition, 0, menus.MenuItemsVehicle		[i].ReturnValue.Grade, -1}); char strCount [32]; sprintf_s(strCount, "%i", (indexElement != -1) ? playerInventory.Vehicle	[indexElement].Count : 0); ::sprintf_s(preformatted, "x%3.3s: %s", strCount, menus.NamesVehicle		[i].begin() );	menus.MenuItemsVehicle		[i].Text = preformatted; }
	for(size_t i = 0, itemCount = instanceGame.EntityTables.Profession	.Definitions.size(); i<itemCount; ++i){	int32_t indexElement = playerInventory.Profession	.FindElement({	menus.MenuItemsProfession	[i].ReturnValue.Definition, 0, menus.MenuItemsProfession	[i].ReturnValue.Grade, -1}); char strCount [32]; sprintf_s(strCount, "%i", (indexElement != -1) ? playerInventory.Profession[indexElement].Count : 0); ::sprintf_s(preformatted, "x%3.3s: %s", strCount, menus.NamesProfession	[i].begin() );	menus.MenuItemsProfession	[i].Text = preformatted; }
	for(size_t i = 0, itemCount = instanceGame.EntityTables.Weapon		.Definitions.size(); i<itemCount; ++i){	int32_t indexElement = playerInventory.Weapon		.FindElement({	menus.MenuItemsWeapon		[i].ReturnValue.Definition, 0, menus.MenuItemsWeapon		[i].ReturnValue.Grade, -1}); char strCount [32]; sprintf_s(strCount, "%i", (indexElement != -1) ? playerInventory.Weapon	[indexElement].Count : 0); ::sprintf_s(preformatted, "x%3.3s: %s", strCount, menus.NamesWeapon		[i].begin() );	menus.MenuItemsWeapon		[i].Text = preformatted; }
	for(size_t i = 0, itemCount = instanceGame.EntityTables.Armor		.Definitions.size(); i<itemCount; ++i){	int32_t indexElement = playerInventory.Armor		.FindElement({	menus.MenuItemsArmor		[i].ReturnValue.Definition, 0, menus.MenuItemsArmor			[i].ReturnValue.Grade, -1}); char strCount [32]; sprintf_s(strCount, "%i", (indexElement != -1) ? playerInventory.Armor		[indexElement].Count : 0); ::sprintf_s(preformatted, "x%3.3s: %s", strCount, menus.NamesArmor		[i].begin() );	menus.MenuItemsArmor		[i].Text = preformatted; }
	for(size_t i = 0, itemCount = ::gpk::size(itemDescriptions); i<itemCount; ++i){	int32_t indexElement = playerInventory.Items.FindElement({ menus.MenuItemsItem[i].ReturnValue.Definition, 0, menus.MenuItemsItem[i].ReturnValue.Grade, -1}); char strCount [32]; sprintf_s(strCount, "%i", (indexElement != -1) ? playerInventory.Items[indexElement].Count : 0); ::sprintf_s(preformatted, "x%3.3s: %s", strCount, menus.NamesItem[i].begin() ); menus.MenuItemsItem[i].Text = preformatted; }
	for(size_t i = 0, itemCount = ::gpk::size(enemyDefinitions); i<itemCount; ++i){	menus.MenuItemsAgent[i].Text = menus.NamesAgent[i]; }
	return 0;
}

SGameState drawBuyMenu(SGame& instanceGame, const SGameState& returnState) {
#define MAX_BUY_ITEMS 64
	static ::SShopMenus						menus			= {};
	static const int32_t					initedMenus		= ::initBuyMenus(instanceGame.EntityTables, menus);
	::reinitBuyMenus(instanceGame, menus);
	static ::klib::SMenuItem<SBuyable>		menuItems[MAX_BUY_ITEMS+1] = {};
	switch(instanceGame.State.Substate) {
	case GAME_SUBSTATE_ACCESSORY	: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuAccessory	,	::gpk::view_array<const ::klib::SMenuItem<::SBuyable>>{menus.MenuItemsAccessory		},  instanceGame.FrameInput, {-1});		} break;
	case GAME_SUBSTATE_STAGEPROP	: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuStageProp	,	::gpk::view_array<const ::klib::SMenuItem<::SBuyable>>{menus.MenuItemsStageProp		},  instanceGame.FrameInput, {-1});		} break;
	case GAME_SUBSTATE_FACILITY		: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuFacility	,	::gpk::view_array<const ::klib::SMenuItem<::SBuyable>>{menus.MenuItemsFacility		},  instanceGame.FrameInput, {-1});		} break;
	case GAME_SUBSTATE_VEHICLE		: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuVehicle	,	::gpk::view_array<const ::klib::SMenuItem<::SBuyable>>{menus.MenuItemsVehicle		},  instanceGame.FrameInput, {-1});		} break;
	case GAME_SUBSTATE_PROFESSION	: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuProfession	,	::gpk::view_array<const ::klib::SMenuItem<::SBuyable>>{menus.MenuItemsProfession	},  instanceGame.FrameInput, {-1});		} break;
	case GAME_SUBSTATE_WEAPON		: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuWeapon		,	::gpk::view_array<const ::klib::SMenuItem<::SBuyable>>{menus.MenuItemsWeapon		},  instanceGame.FrameInput, {-1});		} break;
	case GAME_SUBSTATE_ARMOR		: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuArmor		,	::gpk::view_array<const ::klib::SMenuItem<::SBuyable>>{menus.MenuItemsArmor			},  instanceGame.FrameInput, {-1});		} break;
	case GAME_SUBSTATE_ITEM			: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuItem		,	::gpk::view_array<const ::klib::SMenuItem<::SBuyable>>{menus.MenuItemsItem			},  instanceGame.FrameInput, {-1});		} break;
	case GAME_SUBSTATE_CHARACTER	: { static ::klib::SDrawMenuState menuState; selectedChoice = ::klib::drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menus.MenuAgent		,	::gpk::view_array<const ::klib::SMenuItem<::SBuyable>>{menus.MenuItemsAgent	,  4	}, instanceGame.FrameInput, {-1});	} break;
	default:
		break;
	}

	if(selectedChoice.Definition == SHOP_EXIT_VALUE)
		return {GAME_STATE_MENU_BUY};
	else if( selectedChoice.Definition == -1 )
		return returnState;

	SGameState retVal = returnState;
	SPlayer		& player = instanceGame.Players[PLAYER_INDEX_USER];
	instanceGame.ClearMessages();
	if(selectedChoice.Price > player.Tactical.Money) {
		instanceGame.Messages.UserError		= ::gpk::view_const_string{"You don't have enough money for "};
		instanceGame.Messages.UserError.append(selectedChoice.Name);
		instanceGame.Messages.UserError.append_string("!!");
		instanceGame.LogError();
		return retVal;
	}

	SCharacterInventory& playerInventory = player.Inventory;
	int32_t iCharacterInArmy = 0, armySize = player.Tactical.Army.size();
	bool bFoundFreeCharacterSlot = false;
	bool bSold = false;
	::gpk::ptr_obj<::klib::CCharacter> newCharacter;
	switch(instanceGame.State.Substate) {
	case GAME_SUBSTATE_ACCESSORY	:	if(playerInventory.Accessory	.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case GAME_SUBSTATE_STAGEPROP	:	if(playerInventory.StageProp	.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case GAME_SUBSTATE_FACILITY		:	if(playerInventory.Facility		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case GAME_SUBSTATE_VEHICLE		:	if(playerInventory.Vehicle		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case GAME_SUBSTATE_PROFESSION	:	if(playerInventory.Profession	.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" Job License for "	); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case GAME_SUBSTATE_WEAPON		:	if(playerInventory.Weapon		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case GAME_SUBSTATE_ARMOR		:	if(playerInventory.Armor		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case GAME_SUBSTATE_ITEM			:	if(playerInventory.Items		.AddElement({selectedChoice.Definition, 0, selectedChoice.Grade, -1})) 	{ instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully bought "}; instanceGame.Messages.UserSuccess.append(selectedChoice.Name); instanceGame.Messages.UserSuccess.append_string(" for "				); char price [64]; sprintf_s(price, "%lli", selectedChoice.Price); instanceGame.Messages.UserSuccess.append_string(price); instanceGame.Messages.UserSuccess.append_string(" Coins."); bSold = true; } break;
	case GAME_SUBSTATE_CHARACTER	:
		newCharacter.create(enemyDefinitions[selectedChoice.Definition]);
		for(iCharacterInArmy ; iCharacterInArmy < armySize; ++iCharacterInArmy) {
			if(0 == player.Tactical.Army[iCharacterInArmy]) {
				player.Tactical.Army[iCharacterInArmy].create(*newCharacter);
				bFoundFreeCharacterSlot = true;
				break;
			}
		}
		if(!bFoundFreeCharacterSlot)
			player.Tactical.Army.push_back(newCharacter);
		::setupAgent(instanceGame.EntityTables, *newCharacter, *newCharacter);
		instanceGame.Messages.UserSuccess = ::gpk::view_const_string{"You have successfully hired "};
		instanceGame.Messages.UserSuccess.append(selectedChoice.Name);
		instanceGame.Messages.UserSuccess.append_string(" for ");
		char maintCost [64];
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
		player.Tactical.Money -= selectedChoice.Price;
		player.Tactical.Score.MoneySpent += selectedChoice.Price;
	}
	else {
		instanceGame.Messages.UserError = "There is not enough space in your inventory!";
		instanceGame.LogError();
	}

	return retVal;
}

SGameState drawBuy(SGame& instanceGame, const SGameState& returnState) {
	static const ::gpk::view_const_string	textToPrint			= "Tell me how much money you have and I will tell you what you'll become.";

	static ::klib::SMessageSlow						slowMessage;
	bool											bDonePrinting						= ::klib::getMessageSlow(slowMessage, textToPrint.begin(), textToPrint.size(), instanceGame.FrameTimer.LastTimeSeconds*3);
	::gpk::SCoord2<uint32_t>						position							= instanceGame.TacticalDisplay.Screen.metrics() / 2;
	const uint32_t									messageLen							= (uint32_t)strlen(slowMessage.Message);
	memcpy(&instanceGame.TacticalDisplay.Screen[position.y][position.x - (messageLen + 1) / 2], slowMessage.Message, messageLen);
	if ( !bDonePrinting )
		return returnState;

	if(GAME_SUBSTATE_MAIN == instanceGame.State.Substate) {
		static SMenuHeader<SGameState>		menuBuy								({GAME_STATE_WELCOME_COMMANDER}, "Order Menu", 26);
		return drawMenu(instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menuBuy, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsBuy}, instanceGame.FrameInput, instanceGame.State);
	}
	else
		return drawBuyMenu(instanceGame, returnState);
};


