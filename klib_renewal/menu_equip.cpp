//#define NOMINMAX

#include "Agent_helper.h"
#include "EntityDetail.h"

#include "Game.h"
#include "draw.h"

#include "Accessory.h"
#include "Armor.h"
#include "Weapon.h"
#include "Profession.h"
#include "Vehicle.h"
#include "Facility.h"
#include "StageProp.h"
#include "Item.h"
#include "Enemy.h"

using namespace klib;

static const ::gpk::view_const_string labelAccessory	= "Accessory"		;
static const ::gpk::view_const_string labelStageProp	= "Stage Prop"		;
static const ::gpk::view_const_string labelFacility		= "Facility"		;
static const ::gpk::view_const_string labelVehicle		= "Vehicle"			;
static const ::gpk::view_const_string labelJob			= "Job License"		;
static const ::gpk::view_const_string labelWeapon		= "Weapon"			;
static const ::gpk::view_const_string labelArmor		= "Armor"			;
static const ::gpk::view_const_string labelItem			= "Item"			;

bool	equipIfResearchedProfession	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Rank"				, entityTypePlural = "Licenses"		; CCharacter& playerAgent = *player.Army[indexAgent];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Profession	, playerAgent.CurrentEquip.Profession	, player.Goods.CompletedResearch.Profession	, playerAgent.Goods.CompletedResearch.Profession	, definitionsProfession		, modifiersProfession	, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
bool	equipIfResearchedWeapon		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Science"			, entityTypePlural = "Weapons"		; CCharacter& playerAgent = *player.Army[indexAgent];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Weapon		, playerAgent.CurrentEquip.Weapon		, player.Goods.CompletedResearch.Weapon		, playerAgent.Goods.CompletedResearch.Weapon		, definitionsWeapon			, modifiersWeapon		, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
bool	equipIfResearchedArmor		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Technology"		, entityTypePlural = "Armors"		; CCharacter& playerAgent = *player.Army[indexAgent];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Armor		, playerAgent.CurrentEquip.Armor		, player.Goods.CompletedResearch.Armor		, playerAgent.Goods.CompletedResearch.Armor			, definitionsArmor			, modifiersArmor		, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
bool	equipIfResearchedAccessory	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Crafting"			, entityTypePlural = "Accessories"	; CCharacter& playerAgent = *player.Army[indexAgent];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Accessory	, playerAgent.CurrentEquip.Accessory	, player.Goods.CompletedResearch.Accessory	, playerAgent.Goods.CompletedResearch.Accessory		, definitionsAccessory		, modifiersAccessory	, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
//static	bool equipIfResearchedStageProp	(SGame& instanceGame, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::label modifierTypeName = ""				, entityTypePlural = "Stage Props"	; CCharacter& playerAgent = *player.Army[player.Squad.Agents[player.Selection.PlayerUnit]];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.StageProp	, playerAgent.CurrentEquip.StageProp	, player.Goods.CompletedResearch.StageProp	, playerAgent.Goods.CompletedResearch.StageProp		, definitionsStageProp		, modifiersStageProp	, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
//static	bool equipIfResearchedFacility	(SGame& instanceGame, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::label modifierTypeName = "Architectonics"	, entityTypePlural = "Facilities"	; CCharacter& playerAgent = *player.Army[player.Squad.Agents[player.Selection.PlayerUnit]];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Facility	, playerAgent.CurrentEquip.Facility		, player.Goods.CompletedResearch.Facility	, playerAgent.Goods.CompletedResearch.Facility		, definitionsFacility		, modifiersFacility		, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
//static	bool equipIfResearchedVehicle	(SGame& instanceGame, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::label modifierTypeName = "Transportation"	, entityTypePlural = "Vehicles"		; CCharacter& playerAgent = *player.Army[player.Squad.Agents[player.Selection.PlayerUnit]];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Vehicle	, playerAgent.CurrentEquip.Vehicle		, player.Goods.CompletedResearch.Vehicle	, playerAgent.Goods.CompletedResearch.Vehicle		, definitionsVehicle		, modifiersVehicle		, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
//			bool equipIfResearchedItems		(SGame& instanceGame, int16_t selectedChoice) { return false; } //SPlayer& player = instanceGame.Player; return equipIfResearched(selectedChoice, player.Army, player.Inventory.Items		, player.CurrentEquip.Items			, player.Squad, player.Selection, player.CompletedResearch.Items			, definitionsItems			, modifiersItems		, ""		, entityTypePlural, messageSuccess, messageError);};

static SGameState drawEquipMenu(SGame& instanceGame, const SGameState& returnState) {
#define MAX_BUY_ITEMS 1024
#define SHOP_EXIT_VALUE ((MAX_BUY_ITEMS)*2)

	static klib::SMenuItem<int32_t> menuItems[MAX_BUY_ITEMS+1] = {};

	uint32_t itemCount=0, iCharacter, iSlot, count;

	SPlayer&				player			= instanceGame.Players[PLAYER_INDEX_USER];
	SCharacterGoods&		playerCompany	= instanceGame.Players[PLAYER_INDEX_USER].Goods;
	SCharacterInventory&	playerInventory	= playerCompany.Inventory;
	SWeightedDisplay&	display	= instanceGame.GlobalDisplay;

	int16_t selectedChoice=0;
	std::string menuTitle = "Equip ";

	if( player.Selection.PlayerUnit != -1 && player.Squad.Agents[player.Selection.PlayerUnit] != -1 && GAME_SUBSTATE_CHARACTER != instanceGame.State.Substate)
		menuTitle = player.Army[player.Squad.Agents[player.Selection.PlayerUnit]]->Name + ": ";
	else if(player.Selection.PlayerUnit != -1)
		menuTitle = "Agent #" + std::to_string(player.Selection.PlayerUnit+1);

	char formatted[128] = {};
	switch(instanceGame.State.Substate) {
	case GAME_SUBSTATE_ACCESSORY	:	menuTitle += labelAccessory	.begin();	for(iSlot=0, itemCount=playerInventory.Accessory	.Slots.size(); iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Accessory	[iSlot].Count).c_str(), std::to_string(playerInventory.	Accessory	[iSlot].Entity.Level).c_str(), getAccessoryName		(playerInventory.Accessory	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen.View, display.TextAttributes.begin(), itemCount, menuTitle, ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{menuItems}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_STAGEPROP	:	menuTitle += labelStageProp	.begin();	for(iSlot=0, itemCount=playerInventory.StageProp	.Slots.size(); iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.StageProp	[iSlot].Count).c_str(), std::to_string(playerInventory.	StageProp	[iSlot].Entity.Level).c_str(), getStagePropName		(playerInventory.StageProp	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen.View, display.TextAttributes.begin(), itemCount, menuTitle, ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{menuItems}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_FACILITY		:	menuTitle += labelFacility	.begin();	for(iSlot=0, itemCount=playerInventory.Facility		.Slots.size(); iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Facility		[iSlot].Count).c_str(), std::to_string(playerInventory.	Facility	[iSlot].Entity.Level).c_str(), getFacilityName		(playerInventory.Facility	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen.View, display.TextAttributes.begin(), itemCount, menuTitle, ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{menuItems}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_VEHICLE		:	menuTitle += labelVehicle	.begin();	for(iSlot=0, itemCount=playerInventory.Vehicle		.Slots.size(); iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Vehicle		[iSlot].Count).c_str(), std::to_string(playerInventory.	Vehicle		[iSlot].Entity.Level).c_str(), getVehicleName		(playerInventory.Vehicle	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen.View, display.TextAttributes.begin(), itemCount, menuTitle, ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{menuItems}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_PROFESSION	:	menuTitle += labelJob		.begin();	for(iSlot=0, itemCount=playerInventory.Profession	.Slots.size(); iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Profession	[iSlot].Count).c_str(), std::to_string(playerInventory.	Profession	[iSlot].Entity.Level).c_str(), getProfessionName	(playerInventory.Profession	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen.View, display.TextAttributes.begin(), itemCount, menuTitle, ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{menuItems}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_WEAPON		:	menuTitle += labelWeapon	.begin();	for(iSlot=0, itemCount=playerInventory.Weapon		.Slots.size(); iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Weapon		[iSlot].Count).c_str(), std::to_string(playerInventory.	Weapon		[iSlot].Entity.Level).c_str(), getWeaponName		(playerInventory.Weapon		[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen.View, display.TextAttributes.begin(), itemCount, menuTitle, ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{menuItems}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_ARMOR		:	menuTitle += labelArmor		.begin();	for(iSlot=0, itemCount=playerInventory.Armor		.Slots.size(); iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Armor		[iSlot].Count).c_str(), std::to_string(playerInventory.	Armor		[iSlot].Entity.Level).c_str(), getArmorName			(playerInventory.Armor		[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen.View, display.TextAttributes.begin(), itemCount, menuTitle, ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{menuItems}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_ITEM			:	menuTitle += labelItem		.begin();	for(iSlot=0, itemCount=playerInventory.Items		.Slots.size(); iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Items		[iSlot].Count).c_str(), std::to_string(playerInventory.	Items		[iSlot].Entity.Level).c_str(), getItemName			(playerInventory.Items		[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen.View, display.TextAttributes.begin(), itemCount, menuTitle, ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{menuItems}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_CHARACTER	:
		for(iCharacter=0, count=player.Army.size(); iCharacter<count; ++iCharacter)
		{
			if(0 == player.Army[iCharacter])
				continue;

			if(player.Squad.IsAgentAssigned((int32_t)iCharacter))
				continue;

			menuItems[itemCount++]	= { (int32_t)iCharacter, {player.Army[iCharacter]->Name.data(), (uint32_t)player.Army[iCharacter]->Name.size()}};
		}
		selectedChoice	= (int16_t)drawMenu(display.Screen.View, display.TextAttributes.begin(), (size_t)itemCount, menuTitle, ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{menuItems}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 40U, false, "Exit this menu");
		break;
	default:
		break;
	}

	if( selectedChoice == -1 ) {
		return returnState;
	}
	else if(selectedChoice == SHOP_EXIT_VALUE) {
		if( player.Selection.PlayerUnit == -1 || player.Squad.Agents[player.Selection.PlayerUnit] == -1)
			return { GAME_STATE_MENU_SQUAD_SETUP };
		else
			return { GAME_STATE_MENU_EQUIPMENT };
	}

	SGameState retVal = returnState;

	SEntity selectedItem = {0,0,0};
	instanceGame.ClearMessages();
	std::string itemName = "Invalid item?";
	CCharacter* playerAgent = 0;
	switch(instanceGame.State.Substate)
	{
	case GAME_SUBSTATE_PROFESSION	:	if(equipIfResearchedProfession	(instanceGame, player.Squad.Agents[player.Selection.PlayerUnit], selectedChoice)) { retVal = { GAME_STATE_MENU_EQUIPMENT }; }break;
	case GAME_SUBSTATE_WEAPON		:	if(equipIfResearchedWeapon		(instanceGame, player.Squad.Agents[player.Selection.PlayerUnit], selectedChoice)) { retVal = { GAME_STATE_MENU_EQUIPMENT }; }break;
	case GAME_SUBSTATE_ARMOR		:	if(equipIfResearchedArmor		(instanceGame, player.Squad.Agents[player.Selection.PlayerUnit], selectedChoice)) { retVal = { GAME_STATE_MENU_EQUIPMENT }; }break;
	case GAME_SUBSTATE_ACCESSORY	:	if(equipIfResearchedAccessory	(instanceGame, player.Squad.Agents[player.Selection.PlayerUnit], selectedChoice)) { retVal = { GAME_STATE_MENU_EQUIPMENT }; }break;
	case GAME_SUBSTATE_ITEM			:
		playerAgent = player.Army[player.Squad.Agents[player.Selection.PlayerUnit]];
		itemName = getItemName(playerInventory.Items[selectedChoice].Entity);
		if( playerAgent->Goods.Inventory.Items.AddElement(playerInventory.Items[selectedChoice].Entity) )
		{
			instanceGame.UserSuccess = "You equipped " + itemName + " to " + playerAgent->Name + ".";
			playerInventory.Items.DecreaseEntity(selectedChoice);
			instanceGame.LogSuccess();
		}
		else
		{
			instanceGame.UserError = "You can't equip " + itemName + " to " + playerAgent->Name + " because the inventory is full!";
			instanceGame.LogError();
		}
		playerAgent->Recalculate();
		break;
	case GAME_SUBSTATE_CHARACTER	:
		player.Squad.Agents[player.Selection.PlayerUnit] = selectedChoice;
		::klib::resetCursorString(instanceGame.SlowMessage);
		player.Army[selectedChoice]->Recalculate();
		instanceGame.UserSuccess = "You assigned " + player.Army[selectedChoice]->Name + " as Agent #" + std::to_string(player.Selection.PlayerUnit+1) + "";
		instanceGame.LogSuccess();
		retVal = { GAME_STATE_MENU_EQUIPMENT };
		break;
	default:
		break;
	}

	return retVal;
}

static void								drawScore						(::gpk::view_grid<char> display, int32_t offsetY, int32_t offsetX, const SCharacterScore& score)																{
	//char										buffer[128]						= {};
	char										bufferValues[64]				= {};
	const char									format[]						= "- %-21.21s: %-12.12s";

	sprintf_s(bufferValues, "%lli", (int64_t)score.MoneyEarned			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Money Earned"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.MoneySpent			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Money Spent"				, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.DamageDealt			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Damage Dealt"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.DamageTaken			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Damage Taken"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.TurnsPlayed			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Turns Played"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.BattlesWon			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Battles Won"				, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.BattlesLost			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Battles Lost"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.EscapesSucceeded		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Escapes Succeeded"		, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.EscapesFailed		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Escapes Failed"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.EnemiesKilled		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Enemies Killed"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksHit			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Hit"				, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksMissed		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Missed"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksReceived		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Received"		, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksAvoided		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Avoided"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.PotionsUsed			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Potions Used"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.GrenadesUsed			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Grenades Used"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.CompletedResearch	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Completed Research"		, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.CompletedProduction	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Completed Productions"	, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.CompletedUpgrade		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Completed Upgrades"		, bufferValues);
}

SGameState							drawEquip				(SGame& instanceGame, const SGameState& returnState) {
	static constexpr	const uint32_t		MAX_ENTITY_COLUMNS		= 4;
	SPlayer									& player				= instanceGame.Players[PLAYER_INDEX_USER];
	::gpk::view_grid<char>					display					= instanceGame.GlobalDisplay.Screen;
	::gpk::view_grid<uint16_t>				textAttributes			= instanceGame.GlobalDisplay.TextAttributes;
	//uint16_t color = COLOR_GREEN;

	SGameState								actualReturnState		= returnState;
	::std::string							menuTitle				= "Agent Setup";
	if( player.Selection.PlayerUnit != -1 && player.Squad.Agents[player.Selection.PlayerUnit] != -1)
	{
		const int32_t							slotWidth				= display.metrics().x / MAX_ENTITY_COLUMNS;
		const int32_t							slotRowSpace			= 30;// display.Depth / (MAX_AGENT_ROWS);
		CCharacter& playerAgent = *player.Army[player.Squad.Agents[player.Selection.PlayerUnit]];
		menuTitle = "Agent #" + std::to_string(player.Selection.PlayerUnit+1) + ": "+ playerAgent.Name + ".";

		int32_t offsetY = TACTICAL_DISPLAY_POSY-4, offsetX;
		drawEntityDetail(display, textAttributes, offsetY					, offsetX = 3, playerAgent.CurrentEquip.Profession	, definitionsProfession	, modifiersProfession	, labelJob			);
		drawEntityDetail(display, textAttributes, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Weapon		, definitionsWeapon		, modifiersWeapon		, labelWeapon		);
		drawEntityDetail(display, textAttributes, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Armor		, definitionsArmor		, modifiersArmor		, labelArmor		);
		drawEntityDetail(display, textAttributes, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Accessory	, definitionsAccessory	, modifiersAccessory	, labelAccessory	);
		//drawEntityDetail(display, offsetY+=slotRowSpace	, offsetX = 3, playerAgent.CurrentEquip.Vehicle		, definitionsVehicle	, modifiersVehicle		, labelVehicle		);
		//drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Facility	, definitionsFacility	, modifiersFacility		, labelFacility		);
		//drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.StageProp	, definitionsStageProp	, modifiersStageProp	, labelStageProp	);
		//displayAgentSlot(display, offsetY					, offsetX+=48, player.Selection.PlayerUnit+1		, playerAgent, false);
		::klib::displayAgentSlot(display, textAttributes, offsetY+=slotRowSpace		, offsetX = 3, player.Selection.PlayerUnit+1		, playerAgent, false);
		displayStatusEffectsAndTechs(display, textAttributes, offsetY		, offsetX+=50, playerAgent);
		drawScore					(display, offsetY		, offsetX = display.metrics().x - 40, playerAgent.Score);
	}
	else if(player.Selection.PlayerUnit != -1) {
		drawSquadSlots(instanceGame);
		menuTitle = "Agent #" + std::to_string(player.Selection.PlayerUnit+1) + ": Open position.";
	}

	if(GAME_SUBSTATE_MAIN == instanceGame.State.Substate)
		actualReturnState = drawMenu(instanceGame.GlobalDisplay.Screen.View, instanceGame.GlobalDisplay.TextAttributes.begin(), menuTitle, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsEquip}, instanceGame.FrameInput, {GAME_STATE_MENU_SQUAD_SETUP}, instanceGame.State, 30);
	else {
		if( player.Selection.PlayerUnit >= (int16_t)::gpk::size(player.Squad.Agents))
			player.Selection.PlayerUnit = -1;

		actualReturnState = drawEquipMenu(instanceGame, returnState);
	}

	return actualReturnState;
};


