//#define NOMINMAX

#include "Agent_helper.h"
#include "EntityDetail.h"

#include "Game.h"
#include "draw.h"

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

bool	equipIfResearchedProfession	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Rank"		, entityTypePlural = "Licenses"		; CCharacter& playerAgent = *player.Tactical.Army[indexAgent];	bool bResult = equipIfResearched(instanceGame.EntityTables, selectedChoice, playerAgent, player.Inventory.Profession	, playerAgent.CurrentEquip.Profession	, player.Tactical.Research.Profession	, playerAgent.Goods.CompletedResearch.Profession		, instanceGame.EntityTables.Profession	, modifierTypeName, instanceGame.Messages.UserSuccess, instanceGame.Messages.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
bool	equipIfResearchedWeapon		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Science"	, entityTypePlural = "Weapons"		; CCharacter& playerAgent = *player.Tactical.Army[indexAgent];	bool bResult = equipIfResearched(instanceGame.EntityTables, selectedChoice, playerAgent, player.Inventory.Weapon		, playerAgent.CurrentEquip.Weapon		, player.Tactical.Research.Weapon		, playerAgent.Goods.CompletedResearch.Weapon			, instanceGame.EntityTables.Weapon		, modifierTypeName, instanceGame.Messages.UserSuccess, instanceGame.Messages.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
bool	equipIfResearchedArmor		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Technology", entityTypePlural = "Armors"		; CCharacter& playerAgent = *player.Tactical.Army[indexAgent];	bool bResult = equipIfResearched(instanceGame.EntityTables, selectedChoice, playerAgent, player.Inventory.Armor			, playerAgent.CurrentEquip.Armor		, player.Tactical.Research.Armor		, playerAgent.Goods.CompletedResearch.Armor				, instanceGame.EntityTables.Armor		, modifierTypeName, instanceGame.Messages.UserSuccess, instanceGame.Messages.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
bool	equipIfResearchedAccessory	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Crafting"	, entityTypePlural = "Accessories"	; CCharacter& playerAgent = *player.Tactical.Army[indexAgent];	bool bResult = equipIfResearched(instanceGame.EntityTables, selectedChoice, playerAgent, player.Inventory.Accessory		, playerAgent.CurrentEquip.Accessory	, player.Tactical.Research.Accessory	, playerAgent.Goods.CompletedResearch.Accessory			, instanceGame.EntityTables.Accessory	, modifierTypeName, instanceGame.Messages.UserSuccess, instanceGame.Messages.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
//static	bool equipIfResearchedStageProp	(SGame& instanceGame, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = ""				, entityTypePlural = "Stage Props"	; CCharacter& playerAgent = *player.Army[player.Squad.Agents[player.Selection.PlayerUnit]];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.StageProp	, playerAgent.CurrentEquip.StageProp	, player.Goods.CompletedResearch.StageProp	, playerAgent.Goods.CompletedResearch.StageProp	, definitionsStageProp		, modifiersStageProp	, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
//static	bool equipIfResearchedFacility	(SGame& instanceGame, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Architectonics"	, entityTypePlural = "Facilities"	; CCharacter& playerAgent = *player.Army[player.Squad.Agents[player.Selection.PlayerUnit]];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Facility	, playerAgent.CurrentEquip.Facility		, player.Goods.CompletedResearch.Facility	, playerAgent.Goods.CompletedResearch.Facility		, definitionsFacility		, modifiersFacility		, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
//static	bool equipIfResearchedVehicle	(SGame& instanceGame, int16_t selectedChoice) { SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER]; static const ::gpk::view_const_string modifierTypeName = "Transportation"	, entityTypePlural = "Vehicles"		; CCharacter& playerAgent = *player.Army[player.Squad.Agents[player.Selection.PlayerUnit]];	bool bResult = equipIfResearched(selectedChoice, playerAgent, player.Goods.Inventory.Vehicle	, playerAgent.CurrentEquip.Vehicle		, player.Goods.CompletedResearch.Vehicle	, playerAgent.Goods.CompletedResearch.Vehicle		, definitionsVehicle		, modifiersVehicle		, modifierTypeName, instanceGame.UserSuccess, instanceGame.UserError); if(bResult)instanceGame.LogSuccess(); else instanceGame.LogError(); return bResult;};
//			bool equipIfResearchedItems		(SGame& instanceGame, int16_t selectedChoice) { return false; } //SPlayer& player = instanceGame.Player; return equipIfResearched(selectedChoice, player.Army, player.Inventory.Items		, player.CurrentEquip.Items			, player.Squad, player.Selection, player.CompletedResearch.Items			, definitionsItems			, modifiersItems		, ""		, entityTypePlural, messageSuccess, messageError);};

static	SGameState						drawEquipMenu						(SGame& instanceGame, const SGameState& returnState) {
#define MAX_BUY_ITEMS 1024
#define SHOP_EXIT_VALUE ((MAX_BUY_ITEMS)*2)

	static klib::SMenuItem<int32_t>				menuItems[MAX_BUY_ITEMS+1]			= {};

	uint32_t									itemCount							= 0
		,										iCharacter							= 0
		,										iSlot								= 0
		,										count								= 0
		;
	SPlayer										& player							= instanceGame.Players[PLAYER_INDEX_USER];
	SCharacterInventory							& playerInventory					= instanceGame.Players[PLAYER_INDEX_USER].Inventory;
	SWeightedDisplay							& display							= instanceGame.GlobalDisplay;
	int16_t										selectedChoice						= 0;
	::gpk::array_pod<char_t>					menuTitle							= ::gpk::view_const_string{"Equip "};

	char 										playerUnitPlusOne [32]	;
	if( player.Tactical.Selection.PlayerUnit != -1 && player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit] != -1 && GAME_SUBSTATE_CHARACTER != instanceGame.State.Substate) {
		menuTitle.append(player.Tactical.Army[player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit]]->Name);
		menuTitle.append_string(": ");
	}
	else if(player.Tactical.Selection.PlayerUnit != -1) {
		menuTitle.append_string("Agent #");
		sprintf_s(playerUnitPlusOne, "%i", player.Tactical.Selection.PlayerUnit + 1);
		menuTitle.append_string(playerUnitPlusOne);
	}

	char							strCount [32];
	char formatted[128] = {};
	switch(instanceGame.State.Substate) {
	case GAME_SUBSTATE_ACCESSORY	: { menuTitle.append(labelAccessory	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Accessory	.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Accessory	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Accessory	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Accessory	, playerInventory.Accessory		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Screen.View, display.TextAttributes.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false);} break;
	case GAME_SUBSTATE_STAGEPROP	: { menuTitle.append(labelStageProp	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.StageProp	.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.StageProp	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.StageProp	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.StageProp	, playerInventory.StageProp		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Screen.View, display.TextAttributes.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false);} break;
	case GAME_SUBSTATE_FACILITY		: { menuTitle.append(labelFacility	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Facility		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Facility	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Facility	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Facility	, playerInventory.Facility		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Screen.View, display.TextAttributes.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false);} break;
	case GAME_SUBSTATE_VEHICLE		: { menuTitle.append(labelVehicle	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Vehicle		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Vehicle	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Vehicle	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Vehicle		, playerInventory.Vehicle		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Screen.View, display.TextAttributes.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false);} break;
	case GAME_SUBSTATE_PROFESSION	: { menuTitle.append(labelJob		); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Profession	.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Profession	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Profession[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Profession	, playerInventory.Profession	[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Screen.View, display.TextAttributes.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false);} break;
	case GAME_SUBSTATE_WEAPON		: { menuTitle.append(labelWeapon	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Weapon		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Weapon		[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Weapon	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Weapon		, playerInventory.Weapon		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Screen.View, display.TextAttributes.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false);} break;
	case GAME_SUBSTATE_ARMOR		: { menuTitle.append(labelArmor		); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Armor		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Armor		[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Armor		[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Armor		, playerInventory.Armor			[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Screen.View, display.TextAttributes.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false);} break;
	case GAME_SUBSTATE_ITEM			: { menuTitle.append(labelItem		); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Items		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Items		[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Items		[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getItemName(playerInventory.Items[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Screen.View, display.TextAttributes.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 50U, false); } break;
	case GAME_SUBSTATE_CHARACTER	: {
		static ::klib::SDrawMenuState	menuState;
		for(iCharacter = 0, count = player.Tactical.Army.size(); iCharacter < count; ++iCharacter) {
			if(0 == player.Tactical.Army[iCharacter])
				continue;

			if(player.Tactical.Squad.IsAgentAssigned((int32_t)iCharacter))
				continue;

			menuItems[itemCount++]		= {(int32_t)iCharacter, player.Tactical.Army[iCharacter]->Name};
		}
		selectedChoice				= (int16_t)drawMenu(menuState, display.Screen.View, display.TextAttributes.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, SHOP_EXIT_VALUE, -1, 40U, false);
	}
		break;
	default:
		break;
	}

	if( selectedChoice == -1 )
		return returnState;

	else if(selectedChoice == SHOP_EXIT_VALUE) {
		if( player.Tactical.Selection.PlayerUnit == -1 || player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit] == -1)
			return { GAME_STATE_MENU_SQUAD_SETUP };
		else
			return { GAME_STATE_MENU_EQUIPMENT };
	}

	SGameState								retVal								= returnState;
	SEntity									selectedItem						= {0,0,0};
	instanceGame.Messages.ClearMessages();
	::gpk::array_pod<char_t>				itemName							= "Invalid item?";
	CCharacter								* playerAgent						= 0;
	switch(instanceGame.State.Substate)
	{
	case GAME_SUBSTATE_PROFESSION	:	if(equipIfResearchedProfession	(instanceGame, player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit], selectedChoice)) { retVal = { GAME_STATE_MENU_EQUIPMENT }; }break;
	case GAME_SUBSTATE_WEAPON		:	if(equipIfResearchedWeapon		(instanceGame, player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit], selectedChoice)) { retVal = { GAME_STATE_MENU_EQUIPMENT }; }break;
	case GAME_SUBSTATE_ARMOR		:	if(equipIfResearchedArmor		(instanceGame, player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit], selectedChoice)) { retVal = { GAME_STATE_MENU_EQUIPMENT }; }break;
	case GAME_SUBSTATE_ACCESSORY	:	if(equipIfResearchedAccessory	(instanceGame, player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit], selectedChoice)) { retVal = { GAME_STATE_MENU_EQUIPMENT }; }break;
	case GAME_SUBSTATE_ITEM			:
		playerAgent							= player.Tactical.Army[player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit]];
		itemName							= getItemName(playerInventory.Items[selectedChoice].Entity);
		if( playerAgent->Goods.Inventory.Items.AddElement(playerInventory.Items[selectedChoice].Entity) ) {
			instanceGame.Messages.UserSuccess			= ::gpk::view_const_string{"You equipped "};
			instanceGame.Messages.UserSuccess.append(itemName);
			instanceGame.Messages.UserSuccess.append_string(" to ");
			instanceGame.Messages.UserSuccess.append(playerAgent->Name);
			instanceGame.Messages.UserSuccess.append_string(".");
			playerInventory.Items.DecreaseEntity(selectedChoice);
			instanceGame.LogSuccess();
		}
		else {
			instanceGame.Messages.UserError				= ::gpk::view_const_string{"You can't equip "};
			instanceGame.Messages.UserError.append(itemName);
			instanceGame.Messages.UserError.append_string(" to ");
			instanceGame.Messages.UserError.append(playerAgent->Name);
			instanceGame.Messages.UserError.append_string(" because the inventory is full!");
			instanceGame.LogError();
		}
		playerAgent->Recalculate(instanceGame.EntityTables);
		break;
	case GAME_SUBSTATE_CHARACTER	:
		player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit] = selectedChoice;
		//::klib::resetCursorString(instanceGame.SlowMessage);
		player.Tactical.Army[selectedChoice]->Recalculate(instanceGame.EntityTables);
		instanceGame.Messages.UserSuccess				= ::gpk::view_const_string{"You assigned "};
		instanceGame.Messages.UserSuccess.append(player.Tactical.Army[selectedChoice]->Name);
		instanceGame.Messages.UserSuccess.append_string(" as Agent #");
		sprintf_s(playerUnitPlusOne, "%i", player.Tactical.Selection.PlayerUnit + 1);
		instanceGame.Messages.UserSuccess.append_string(playerUnitPlusOne);
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
	::gpk::array_pod<char_t>				menuTitle				= ::gpk::view_const_string{"Agent Setup"};
	char 									playerUnitPlusOne[32];
	sprintf_s(playerUnitPlusOne, "%i", player.Tactical.Selection.PlayerUnit+1);
	if( player.Tactical.Selection.PlayerUnit != -1 && player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit] != -1) {
		const int32_t							slotWidth				= display.metrics().x / MAX_ENTITY_COLUMNS;
		const int32_t							slotRowSpace			= 30;// display.Depth / (MAX_AGENT_ROWS);
		CCharacter								& playerAgent			= *player.Tactical.Army[player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit]];
		menuTitle							= ::gpk::view_const_string{"Agent #"};
		menuTitle.append_string(playerUnitPlusOne);
		menuTitle.append_string(": ");
		menuTitle.append(playerAgent.Name);
		menuTitle.append_string(".");

		int32_t									offsetY					= TACTICAL_DISPLAY_POSY-4, offsetX;
		drawEntityDetail(display, textAttributes, offsetY, offsetX = 3, playerAgent.CurrentEquip.Profession	, instanceGame.EntityTables.Profession	, labelJob			);
		drawEntityDetail(display, textAttributes, offsetY, offsetX+=48, playerAgent.CurrentEquip.Weapon		, instanceGame.EntityTables.Weapon		, labelWeapon		);
		drawEntityDetail(display, textAttributes, offsetY, offsetX+=48, playerAgent.CurrentEquip.Armor		, instanceGame.EntityTables.Armor		, labelArmor		);
		drawEntityDetail(display, textAttributes, offsetY, offsetX+=48, playerAgent.CurrentEquip.Accessory	, instanceGame.EntityTables.Accessory	, labelAccessory	);
		//drawEntityDetail(display, offsetY+=slotRowSpace	, offsetX = 3, playerAgent.CurrentEquip.Vehicle		, instanceGame.EntityTables.Vehicle		, labelVehicle		);
		//drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Facility	, instanceGame.EntityTables.Facility	, labelFacility		);
		//drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.StageProp	, instanceGame.EntityTables.StageProp	, labelStageProp	);
		//displayAgentSlot(display, offsetY					, offsetX+=48, player.Selection.PlayerUnit+1		, playerAgent, false);
		::klib::displayAgentSlot(instanceGame.EntityTables, display, textAttributes, offsetY += slotRowSpace, offsetX = 3, player.Tactical.Selection.PlayerUnit + 1, playerAgent, false);
		displayStatusEffectsAndTechs(display, textAttributes, offsetY		, offsetX+=50, playerAgent);
		drawScore					(display, offsetY		, offsetX = display.metrics().x - 40, playerAgent.Score);
	}
	else if(player.Tactical.Selection.PlayerUnit != -1) {
		drawSquadSlots(instanceGame);
		menuTitle							= ::gpk::view_const_string{"Agent #"};
		menuTitle.append_string(playerUnitPlusOne);
		menuTitle.append_string(": Open position.");
	}

	static ::klib::SDrawMenuState				menuState;
	if(GAME_SUBSTATE_MAIN == instanceGame.State.Substate)
		actualReturnState = drawMenu(menuState, instanceGame.GlobalDisplay.Screen.View, instanceGame.GlobalDisplay.TextAttributes.begin(), menuTitle, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{optionsEquip}, instanceGame.FrameInput, {GAME_STATE_MENU_SQUAD_SETUP}, instanceGame.State, 30);
	else {
		if( player.Tactical.Selection.PlayerUnit >= (int16_t)::gpk::size(player.Tactical.Squad.Agents))
			player.Tactical.Selection.PlayerUnit = -1;

		actualReturnState = drawEquipMenu(instanceGame, returnState);
	}

	return actualReturnState;
};


