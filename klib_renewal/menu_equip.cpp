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

static const ::gpk::label labelAccessory	= "Accessory"		;
static const ::gpk::label labelStageProp	= "Stage Prop"		;
static const ::gpk::label labelFacility		= "Facility"		;
static const ::gpk::label labelVehicle		= "Vehicle"			;
static const ::gpk::label labelJob			= "Job License"		;
static const ::gpk::label labelWeapon		= "Weapon"			;
static const ::gpk::label labelArmor		= "Armor"			;
static const ::gpk::label labelItem			= "Item"			;

bool	equipIfResearchedProfession	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::label modifierTypeName = "Rank"				, entityTypePlural = "Licenses"		; CCharacter& playerAgent = *player.Army[indexAgent];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Profession	, playerAgent.CurrentEquip.Profession	, player.Goods.CompletedResearch.Profession	, playerAgent.Goods.CompletedResearch.Profession	, definitionsProfession		, modifiersProfession	, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
bool	equipIfResearchedWeapon		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::label modifierTypeName = "Science"			, entityTypePlural = "Weapons"		; CCharacter& playerAgent = *player.Army[indexAgent];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Weapon		, playerAgent.CurrentEquip.Weapon		, player.Goods.CompletedResearch.Weapon		, playerAgent.Goods.CompletedResearch.Weapon		, definitionsWeapon			, modifiersWeapon		, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
bool	equipIfResearchedArmor		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::label modifierTypeName = "Technology"		, entityTypePlural = "Armors"		; CCharacter& playerAgent = *player.Army[indexAgent];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Armor		, playerAgent.CurrentEquip.Armor		, player.Goods.CompletedResearch.Armor		, playerAgent.Goods.CompletedResearch.Armor			, definitionsArmor			, modifiersArmor		, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
bool	equipIfResearchedAccessory	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::label modifierTypeName = "Crafting"			, entityTypePlural = "Accessories"	; CCharacter& playerAgent = *player.Army[indexAgent];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Accessory	, playerAgent.CurrentEquip.Accessory	, player.Goods.CompletedResearch.Accessory	, playerAgent.Goods.CompletedResearch.Accessory		, definitionsAccessory		, modifiersAccessory	, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
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
	SGlobalDisplay&	display	= instanceGame.GlobalDisplay;

	int16_t selectedChoice=0;
	std::string menuTitle = "Equip ";

	if( player.Selection.PlayerUnit != -1 && player.Squad.Agents[player.Selection.PlayerUnit] != -1 && GAME_SUBSTATE_CHARACTER != instanceGame.State.Substate)
		menuTitle = player.Army[player.Squad.Agents[player.Selection.PlayerUnit]]->Name + ": ";
	else if(player.Selection.PlayerUnit != -1)
		menuTitle = "Agent #" + std::to_string(player.Selection.PlayerUnit+1);

	char formatted[128] = {};
	switch(instanceGame.State.Substate) {
	case GAME_SUBSTATE_ACCESSORY	:	menuTitle += labelAccessory	.begin();	for(iSlot=0, itemCount=playerInventory.Accessory	.Count; iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Accessory	[iSlot].Count).c_str(), std::to_string(playerInventory.	Accessory	[iSlot].Entity.Level).c_str(), getAccessoryName		(playerInventory.Accessory	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen, &display.TextAttributes.Cells[0][0], itemCount, menuTitle, menuItems, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_STAGEPROP	:	menuTitle += labelStageProp	.begin();	for(iSlot=0, itemCount=playerInventory.StageProp	.Count; iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.StageProp	[iSlot].Count).c_str(), std::to_string(playerInventory.	StageProp	[iSlot].Entity.Level).c_str(), getStagePropName		(playerInventory.StageProp	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen, &display.TextAttributes.Cells[0][0], itemCount, menuTitle, menuItems, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_FACILITY		:	menuTitle += labelFacility	.begin();	for(iSlot=0, itemCount=playerInventory.Facility		.Count; iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Facility	[iSlot].Count).c_str(), std::to_string(playerInventory.	Facility	[iSlot].Entity.Level).c_str(), getFacilityName		(playerInventory.Facility	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen, &display.TextAttributes.Cells[0][0], itemCount, menuTitle, menuItems, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_VEHICLE		:	menuTitle += labelVehicle	.begin();	for(iSlot=0, itemCount=playerInventory.Vehicle		.Count; iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Vehicle		[iSlot].Count).c_str(), std::to_string(playerInventory.	Vehicle		[iSlot].Entity.Level).c_str(), getVehicleName		(playerInventory.Vehicle	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen, &display.TextAttributes.Cells[0][0], itemCount, menuTitle, menuItems, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_PROFESSION	:	menuTitle += labelJob		.begin();	for(iSlot=0, itemCount=playerInventory.Profession	.Count; iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Profession	[iSlot].Count).c_str(), std::to_string(playerInventory.	Profession	[iSlot].Entity.Level).c_str(), getProfessionName	(playerInventory.Profession	[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen, &display.TextAttributes.Cells[0][0], itemCount, menuTitle, menuItems, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_WEAPON		:	menuTitle += labelWeapon	.begin();	for(iSlot=0, itemCount=playerInventory.Weapon		.Count; iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Weapon		[iSlot].Count).c_str(), std::to_string(playerInventory.	Weapon		[iSlot].Entity.Level).c_str(), getWeaponName		(playerInventory.Weapon		[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen, &display.TextAttributes.Cells[0][0], itemCount, menuTitle, menuItems, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_ARMOR		:	menuTitle += labelArmor		.begin();	for(iSlot=0, itemCount=playerInventory.Armor		.Count; iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Armor		[iSlot].Count).c_str(), std::to_string(playerInventory.	Armor		[iSlot].Entity.Level).c_str(), getArmorName			(playerInventory.Armor		[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen, &display.TextAttributes.Cells[0][0], itemCount, menuTitle, menuItems, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_ITEM			:	menuTitle += labelItem		.begin();	for(iSlot=0, itemCount=playerInventory.Items		.Count; iSlot<itemCount; ++iSlot) { sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", std::to_string(playerInventory.Items		[iSlot].Count).c_str(), std::to_string(playerInventory.	Items		[iSlot].Entity.Level).c_str(), getItemName			(playerInventory.Items		[iSlot].Entity).c_str()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(display.Screen, &display.TextAttributes.Cells[0][0], itemCount, menuTitle, menuItems, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false, "Exit this menu");	break;
	case GAME_SUBSTATE_CHARACTER	:	
		for(iCharacter=0, count=player.Army.size(); iCharacter<count; ++iCharacter)	
		{	
			if(0 == player.Army[iCharacter])
				continue;

			if(player.Squad.IsAgentAssigned((int32_t)iCharacter))
				continue;

			menuItems[itemCount++]	= { (int32_t)iCharacter, {player.Army[iCharacter]->Name.data(), (uint32_t)player.Army[iCharacter]->Name.size()}}; 
		}
		selectedChoice	= (int16_t)drawMenu(display.Screen, &display.TextAttributes.Cells[0][0], (size_t)itemCount, menuTitle, menuItems, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 40U, false, "Exit this menu");
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


SGameState drawEquip(SGame& instanceGame, const SGameState& returnState)
{
	SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER];
	SGlobalDisplay& display = instanceGame.GlobalDisplay;
	//uint16_t color = COLOR_GREEN;

	SGameState actualReturnState = returnState;
	
	std::string menuTitle = "Agent Setup";
	if( player.Selection.PlayerUnit != -1 && player.Squad.Agents[player.Selection.PlayerUnit] != -1) 
	{
		static const int32_t slotWidth		= display.Width / MAX_ENTITY_COLUMNS;
		static const int32_t slotRowSpace	= 30;// display.Depth / (MAX_AGENT_ROWS);
		CCharacter& playerAgent = *player.Army[player.Squad.Agents[player.Selection.PlayerUnit]];
		menuTitle = "Agent #" + std::to_string(player.Selection.PlayerUnit+1) + ": "+ playerAgent.Name + ".";

		int32_t offsetY = TACTICAL_DISPLAY_POSY-4, offsetX;
		drawEntityDetail(display, offsetY					, offsetX = 3, playerAgent.CurrentEquip.Profession	, definitionsProfession	, modifiersProfession	, labelJob			);
		drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Weapon		, definitionsWeapon		, modifiersWeapon		, labelWeapon		);
		drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Armor		, definitionsArmor		, modifiersArmor		, labelArmor		);
		drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Accessory	, definitionsAccessory	, modifiersAccessory	, labelAccessory	);
		//drawEntityDetail(display, offsetY+=slotRowSpace	, offsetX = 3, playerAgent.CurrentEquip.Vehicle		, definitionsVehicle	, modifiersVehicle		, labelVehicle		);
		//drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Facility	, definitionsFacility	, modifiersFacility		, labelFacility		);
		//drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.StageProp	, definitionsStageProp	, modifiersStageProp	, labelStageProp	);
		//displayAgentSlot(display, offsetY					, offsetX+=48, player.Selection.PlayerUnit+1		, playerAgent, false);
		displayAgentSlot(display, offsetY+=slotRowSpace		, offsetX = 3, player.Selection.PlayerUnit+1		, playerAgent, false);
		displayStatusEffectsAndTechs(display, offsetY		, offsetX+=50, playerAgent);
		drawScore					(display, offsetY		, offsetX = display.Width-40, playerAgent.Score);
	}
	else if(player.Selection.PlayerUnit != -1)
	{
		drawSquadSlots(instanceGame);
		menuTitle = "Agent #" + std::to_string(player.Selection.PlayerUnit+1) + ": Open position.";
	}

	if(GAME_SUBSTATE_MAIN == instanceGame.State.Substate) 
	{
		actualReturnState = drawMenu(instanceGame.GlobalDisplay.Screen, &instanceGame.GlobalDisplay.TextAttributes.Cells[0][0], menuTitle, optionsEquip, instanceGame.FrameInput, {GAME_STATE_MENU_SQUAD_SETUP}, instanceGame.State, 30);
	}
	else 
	{
		if( player.Selection.PlayerUnit >= (int16_t)::gpk::size(player.Squad.Agents))
			player.Selection.PlayerUnit = -1;

		actualReturnState = drawEquipMenu(instanceGame, returnState);
	}

	return actualReturnState;
};


