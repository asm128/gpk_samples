#include "Agent_helper.h"
#include "EntityDetail.h"

#include "Game.h"
#include "draw.h"

#include "Item.h"
#include "Enemy.h"

static const ::gpk::view_const_string		labelAccessory		= "Accessory"		;
static const ::gpk::view_const_string		labelStageProp		= "Stage Prop"		;
static const ::gpk::view_const_string		labelFacility		= "Facility"		;
static const ::gpk::view_const_string		labelVehicle		= "Vehicle"			;
static const ::gpk::view_const_string		labelJob			= "Job License"		;
static const ::gpk::view_const_string		labelWeapon			= "Weapon"			;
static const ::gpk::view_const_string		labelArmor			= "Armor"			;
static const ::gpk::view_const_string		labelItem			= "Item"			;

bool	klib::equipIfResearchedProfession	(::klib::SGameMessages& messages, ::klib::SGamePlayer& player, const ::klib::SEntityTables & entityTables, int32_t indexAgent, int16_t selectedChoice) { static const ::gpk::view_const_string modifierTypeName = "Rank"		; ::klib::CCharacter& playerAgent = *player.Tactical.Army[indexAgent];	bool bResult = equipIfResearched(entityTables, selectedChoice, playerAgent, player.Inventory.Profession	, playerAgent.CurrentEquip.Profession	, player.Tactical.Research.Profession	, playerAgent.Goods.CompletedResearch.Profession, entityTables.Profession	, modifierTypeName, messages); return bResult;};
bool	klib::equipIfResearchedWeapon		(::klib::SGameMessages& messages, ::klib::SGamePlayer& player, const ::klib::SEntityTables & entityTables, int32_t indexAgent, int16_t selectedChoice) { static const ::gpk::view_const_string modifierTypeName = "Science"		; ::klib::CCharacter& playerAgent = *player.Tactical.Army[indexAgent];	bool bResult = equipIfResearched(entityTables, selectedChoice, playerAgent, player.Inventory.Weapon		, playerAgent.CurrentEquip.Weapon		, player.Tactical.Research.Weapon		, playerAgent.Goods.CompletedResearch.Weapon	, entityTables.Weapon		, modifierTypeName, messages); return bResult;};
bool	klib::equipIfResearchedArmor		(::klib::SGameMessages& messages, ::klib::SGamePlayer& player, const ::klib::SEntityTables & entityTables, int32_t indexAgent, int16_t selectedChoice) { static const ::gpk::view_const_string modifierTypeName = "Technology"	; ::klib::CCharacter& playerAgent = *player.Tactical.Army[indexAgent];	bool bResult = equipIfResearched(entityTables, selectedChoice, playerAgent, player.Inventory.Armor		, playerAgent.CurrentEquip.Armor		, player.Tactical.Research.Armor		, playerAgent.Goods.CompletedResearch.Armor		, entityTables.Armor		, modifierTypeName, messages); return bResult;};
bool	klib::equipIfResearchedAccessory	(::klib::SGameMessages& messages, ::klib::SGamePlayer& player, const ::klib::SEntityTables & entityTables, int32_t indexAgent, int16_t selectedChoice) { static const ::gpk::view_const_string modifierTypeName = "Crafting"	; ::klib::CCharacter& playerAgent = *player.Tactical.Army[indexAgent];	bool bResult = equipIfResearched(entityTables, selectedChoice, playerAgent, player.Inventory.Accessory	, playerAgent.CurrentEquip.Accessory	, player.Tactical.Research.Accessory	, playerAgent.Goods.CompletedResearch.Accessory	, entityTables.Accessory	, modifierTypeName, messages); return bResult;};

bool	klib::restrictedProfession			(::klib::SGameMessages& messages, ::klib::SGamePlayer& player, const ::klib::SEntityTables & tables, int32_t indexAgent, int16_t selectedChoice) { static const ::gpk::view_const_string modifierTypeName = "Rank"		; ::klib::CCharacter& playerAgent = *player.Tactical.Army[indexAgent]; return ::klib::equipRestriction(messages, player.Inventory.Profession	, selectedChoice, tables.Profession	, player.Tactical.Research.Profession	, playerAgent.Goods.CompletedResearch.Profession, modifierTypeName); };
bool	klib::restrictedWeapon				(::klib::SGameMessages& messages, ::klib::SGamePlayer& player, const ::klib::SEntityTables & tables, int32_t indexAgent, int16_t selectedChoice) { static const ::gpk::view_const_string modifierTypeName = "Science"	; ::klib::CCharacter& playerAgent = *player.Tactical.Army[indexAgent]; return ::klib::equipRestriction(messages, player.Inventory.Weapon		, selectedChoice, tables.Weapon		, player.Tactical.Research.Weapon		, playerAgent.Goods.CompletedResearch.Weapon	, modifierTypeName); };
bool	klib::restrictedArmor				(::klib::SGameMessages& messages, ::klib::SGamePlayer& player, const ::klib::SEntityTables & tables, int32_t indexAgent, int16_t selectedChoice) { static const ::gpk::view_const_string modifierTypeName = "Technology"; ::klib::CCharacter& playerAgent = *player.Tactical.Army[indexAgent]; return ::klib::equipRestriction(messages, player.Inventory.Armor			, selectedChoice, tables.Armor		, player.Tactical.Research.Armor		, playerAgent.Goods.CompletedResearch.Armor		, modifierTypeName); };
bool	klib::restrictedAccessory			(::klib::SGameMessages& messages, ::klib::SGamePlayer& player, const ::klib::SEntityTables & tables, int32_t indexAgent, int16_t selectedChoice) { static const ::gpk::view_const_string modifierTypeName = "Crafting"	; ::klib::CCharacter& playerAgent = *player.Tactical.Army[indexAgent]; return ::klib::equipRestriction(messages, player.Inventory.Accessory		, selectedChoice, tables.Accessory	, player.Tactical.Research.Accessory	, playerAgent.Goods.CompletedResearch.Accessory	, modifierTypeName); };

static	::klib::SGameState					drawEquipMenu						(::klib::SGame& instanceGame, const ::klib::SGameState& returnState) {
	//static constexpr const uint32_t					MAX_BUY_ITEMS						= 1024;
	//static constexpr const uint32_t					SHOP_EXIT_VALUE						= ((MAX_BUY_ITEMS)*2);
#define MAX_BUY_ITEMS							1024
	static klib::SMenuItem<int32_t>					menuItems[MAX_BUY_ITEMS+1]			= {};

	uint32_t										itemCount							= 0
		,											iCharacter							= 0
		,											iSlot								= 0
		,											count								= 0
		;
	::klib::SGamePlayer								& player							= instanceGame.Players[::klib::PLAYER_INDEX_USER];
	::klib::SCharacterInventory						& playerInventory					= instanceGame.Players[::klib::PLAYER_INDEX_USER].Inventory;
	::gpk::SRenderTarget<char, uint16_t>			& display							= instanceGame.GlobalDisplay.Screen;
	int16_t											selectedChoice						= 0;
	::gpk::array_pod<char_t>						menuTitle							= ::gpk::view_const_string{"Equip "};

	char 											playerUnitPlusOne [32]	;
	if( player.Tactical.Selection.PlayerUnit != -1 && player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit] != -1 && ::klib::GAME_SUBSTATE_CHARACTER != instanceGame.State.Substate) {
		menuTitle.append(player.Tactical.Army[player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit]]->Name);
		menuTitle.append_string(": ");
	}
	else if(player.Tactical.Selection.PlayerUnit != -1) {
		menuTitle.append_string("Agent #");
		sprintf_s(playerUnitPlusOne, "%i", player.Tactical.Selection.PlayerUnit + 1);
		menuTitle.append_string(playerUnitPlusOne);
	}

	char											strCount [32];
	char											formatted[128]						= {};
	switch(instanceGame.State.Substate) {
	case ::klib::GAME_SUBSTATE_ACCESSORY	: { menuTitle.append(labelAccessory	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Accessory	.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Accessory	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Accessory	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Accessory	, playerInventory.Accessory		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Color.View, display.DepthStencil.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, (int32_t)itemCount, -1, 50U, false);} break;
	case ::klib::GAME_SUBSTATE_STAGEPROP	: { menuTitle.append(labelStageProp	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.StageProp	.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.StageProp	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.StageProp	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.StageProp	, playerInventory.StageProp		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Color.View, display.DepthStencil.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, (int32_t)itemCount, -1, 50U, false);} break;
	case ::klib::GAME_SUBSTATE_FACILITY		: { menuTitle.append(labelFacility	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Facility		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Facility	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Facility	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Facility	, playerInventory.Facility		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Color.View, display.DepthStencil.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, (int32_t)itemCount, -1, 50U, false);} break;
	case ::klib::GAME_SUBSTATE_VEHICLE		: { menuTitle.append(labelVehicle	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Vehicle		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Vehicle	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Vehicle	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Vehicle		, playerInventory.Vehicle		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Color.View, display.DepthStencil.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, (int32_t)itemCount, -1, 50U, false);} break;
	case ::klib::GAME_SUBSTATE_PROFESSION	: { menuTitle.append(labelJob		); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Profession	.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Profession	[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Profession[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Profession	, playerInventory.Profession	[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Color.View, display.DepthStencil.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, (int32_t)itemCount, -1, 50U, false);} break;
	case ::klib::GAME_SUBSTATE_WEAPON		: { menuTitle.append(labelWeapon	); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Weapon		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Weapon		[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Weapon	[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Weapon		, playerInventory.Weapon		[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Color.View, display.DepthStencil.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, (int32_t)itemCount, -1, 50U, false);} break;
	case ::klib::GAME_SUBSTATE_ARMOR		: { menuTitle.append(labelArmor		); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Armor		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Armor		[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Armor		[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getEntityName(instanceGame.EntityTables.Armor		, playerInventory.Armor			[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Color.View, display.DepthStencil.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, (int32_t)itemCount, -1, 50U, false);} break;
	case ::klib::GAME_SUBSTATE_ITEM			: { menuTitle.append(labelItem		); static ::klib::SDrawMenuState menuState; for(iSlot=0, itemCount=playerInventory.Items		.Slots.size(); iSlot<itemCount; ++iSlot) { char strLevel [32]; sprintf_s(strCount, "%i", playerInventory.Items		[iSlot].Count); sprintf_s(strLevel, "%i", playerInventory.Items		[iSlot].Entity.Level); sprintf_s(formatted, "x%2.2s - Lv.%3.3s %s", strCount, strLevel, getItemName(playerInventory.Items[iSlot].Entity).begin()); menuItems[iSlot] = { (int32_t)iSlot, formatted }; } selectedChoice = (int16_t)drawMenu(menuState, display.Color.View, display.DepthStencil.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, (int32_t)itemCount, -1, 50U, false); } break;
	case ::klib::GAME_SUBSTATE_CHARACTER	: {
		static ::klib::SDrawMenuState					menuState;
		for(iCharacter = 0, count = player.Tactical.Army.size(); iCharacter < count; ++iCharacter) {
			if(0 == player.Tactical.Army[iCharacter].get_ref())
				continue;

			if(player.Tactical.Squad.IsAgentAssigned((int32_t)iCharacter))
				continue;

			menuItems[itemCount++]		= {(int32_t)iCharacter, player.Tactical.Army[iCharacter]->Name};
		}
		selectedChoice				= (int16_t)::klib::drawMenu(menuState, display.Color.View, display.DepthStencil.begin(), menuTitle, {menuItems, itemCount}, instanceGame.FrameInput, (int32_t)itemCount, -1, 40U, false);
	}
		break;
	default:
		break;
	}

	if( selectedChoice == -1 )
		return returnState;

	else if(selectedChoice == (int32_t)itemCount) {
		if( player.Tactical.Selection.PlayerUnit == -1 || player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit] == -1)
			return { ::klib::GAME_STATE_MENU_SQUAD_SETUP };
		else
			return { ::klib::GAME_STATE_MENU_EQUIPMENT };
	}
	::klib::SGameState							retVal								= returnState;
	::klib::SEntity								selectedItem						= {0,0,0};
	instanceGame.Messages.ClearMessages();
	::gpk::array_pod<char_t>					itemName							= "Invalid item?";
	const int32_t								iAgent								= (-1 == player.Tactical.Selection.PlayerUnit) ? -1 : player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit];
	switch(instanceGame.State.Substate) {
	case ::klib::GAME_SUBSTATE_PROFESSION	:	if(0 == ::klib::restrictedProfession	(instanceGame.Messages, player, instanceGame.EntityTables, iAgent, selectedChoice)) { instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, (uint64_t)iAgent | (((uint64_t)selectedChoice) << 32)}); retVal = { ::klib::GAME_STATE_MENU_EQUIPMENT }; } else { } break;
 	case ::klib::GAME_SUBSTATE_WEAPON		:	if(0 == ::klib::restrictedWeapon		(instanceGame.Messages, player, instanceGame.EntityTables, iAgent, selectedChoice)) { instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, (uint64_t)iAgent | (((uint64_t)selectedChoice) << 32)}); retVal = { ::klib::GAME_STATE_MENU_EQUIPMENT }; } else { } break;
	case ::klib::GAME_SUBSTATE_ARMOR		:	if(0 == ::klib::restrictedArmor			(instanceGame.Messages, player, instanceGame.EntityTables, iAgent, selectedChoice)) { instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, (uint64_t)iAgent | (((uint64_t)selectedChoice) << 32)}); retVal = { ::klib::GAME_STATE_MENU_EQUIPMENT }; } else { } break;
	case ::klib::GAME_SUBSTATE_ACCESSORY	:	if(0 == ::klib::restrictedAccessory		(instanceGame.Messages, player, instanceGame.EntityTables, iAgent, selectedChoice)) { instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, (uint64_t)iAgent | (((uint64_t)selectedChoice) << 32)}); retVal = { ::klib::GAME_STATE_MENU_EQUIPMENT }; } else { } break;
	case ::klib::GAME_SUBSTATE_ITEM			: {
		::klib::CCharacter							* playerAgent						= player.Tactical.Army[iAgent];
		itemName								= ::klib::getItemName(playerInventory.Items[selectedChoice].Entity);
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
	}
		break;
	case ::klib::GAME_SUBSTATE_CHARACTER	:
		instanceGame.Events.push_back({::klib::GAME_EVENT_CONFIRM, instanceGame.State, (uint64_t)player.Tactical.Selection.PlayerUnit | (((uint64_t)selectedChoice) << 32)});
		retVal											= { ::klib::GAME_STATE_MENU_EQUIPMENT };
		break;
	default:
		break;
	}

	return retVal;
}

static void								drawScore						(::gpk::view_grid<char> display, int32_t offsetY, int32_t offsetX, const ::klib::SCharacterScore& score)																{
	//char										buffer[128]						= {};
	char										bufferValues	[64]			= {};
	const char									format			[]				= "- %-21.21s: %-12.12s";
	sprintf_s(bufferValues, "%lli", (int64_t)score.MoneyEarned			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Money Earned"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.MoneySpent			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Money Spent"				, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.DamageDealt			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Damage Dealt"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.DamageTaken			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Damage Taken"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.TurnsPlayed			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Turns Played"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.BattlesWon			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Battles Won"				, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.BattlesLost			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Battles Lost"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.EscapesSucceeded		); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Escapes Succeeded"		, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.EscapesFailed		); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Escapes Failed"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.EnemiesKilled		); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Enemies Killed"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksHit			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Hit"				, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksMissed		); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Missed"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksReceived		); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Received"		, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksAvoided		); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Avoided"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.UsedPotions			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Potions Used"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.UsedGrenades			); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Grenades Used"			, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.CompletedResearch	); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Completed Research"		, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.CompletedProduction	); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Completed Productions"	, bufferValues);
	sprintf_s(bufferValues, "%lli", (int64_t)score.CompletedUpgrade		); ::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Completed Upgrades"		, bufferValues);
}

::klib::SGameState						drawEquip				(::klib::SGame& instanceGame, const ::klib::SGameState& returnState) {
	static constexpr	const uint32_t		MAX_ENTITY_COLUMNS		= 4;
	::klib::SGamePlayer						& player				= instanceGame.Players[::klib::PLAYER_INDEX_USER];
	::gpk::view_grid<char>					display					= instanceGame.GlobalDisplay.Screen.Color;
	::gpk::view_grid<uint16_t>				textAttributes			= instanceGame.GlobalDisplay.Screen.DepthStencil;

	::klib::SGameState						actualReturnState		= returnState;
	::gpk::array_pod<char_t>				menuTitle				= ::gpk::view_const_string{"Agent Setup"};
	char 									playerUnitPlusOne[32];
	sprintf_s(playerUnitPlusOne, "%i", player.Tactical.Selection.PlayerUnit+1);
	if( player.Tactical.Selection.PlayerUnit != -1 && player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit] != -1) {
		const int32_t							slotWidth				= display.metrics().x / MAX_ENTITY_COLUMNS;
		const int32_t							slotRowSpace			= 24;// display.Depth / (MAX_AGENT_ROWS);
		::klib::CCharacter						& playerAgent			= *player.Tactical.Army[player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit]];
		menuTitle							= ::gpk::view_const_string{"Agent #"};
		menuTitle.append_string(playerUnitPlusOne);
		menuTitle.append_string(": ");
		menuTitle.append(playerAgent.Name);
		menuTitle.append_string(".");

		int32_t									offsetY					= TACTICAL_DISPLAY_POSY-4, offsetX;
		::klib::drawEntityDetail(display, textAttributes, offsetY, offsetX = 1, playerAgent.CurrentEquip.Profession	, instanceGame.EntityTables.Profession	, labelJob			);
		::klib::drawEntityDetail(display, textAttributes, offsetY, offsetX+=slotWidth, playerAgent.CurrentEquip.Weapon		, instanceGame.EntityTables.Weapon		, labelWeapon		);
		::klib::drawEntityDetail(display, textAttributes, offsetY, offsetX+=slotWidth, playerAgent.CurrentEquip.Armor		, instanceGame.EntityTables.Armor		, labelArmor		);
		::klib::drawEntityDetail(display, textAttributes, offsetY, offsetX+=slotWidth, playerAgent.CurrentEquip.Accessory	, instanceGame.EntityTables.Accessory	, labelAccessory	);
		//drawEntityDetail(display, offsetY+=slotRowSpace	, offsetX = 3, playerAgent.CurrentEquip.Vehicle		, instanceGame.EntityTables.Vehicle		, labelVehicle		);
		//drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.Facility	, instanceGame.EntityTables.Facility	, labelFacility		);
		//drawEntityDetail(display, offsetY					, offsetX+=48, playerAgent.CurrentEquip.StageProp	, instanceGame.EntityTables.StageProp	, labelStageProp	);
		//displayAgentSlot(display, offsetY					, offsetX+=48, player.Selection.PlayerUnit+1		, playerAgent, false);
		::klib::displayAgentSlot(instanceGame.EntityTables, display, textAttributes, offsetY += slotRowSpace, offsetX = 3, player.Tactical.Selection.PlayerUnit + 1, playerAgent, false);
		::klib::displayStatusEffectsAndTechs(display, textAttributes, offsetY		, offsetX+=50, playerAgent);
		::drawScore					(display, offsetY		, offsetX = display.metrics().x - 40, playerAgent.Score);
	}
	else if(player.Tactical.Selection.PlayerUnit != -1) {
		::klib::drawSquadSlots(instanceGame);
		menuTitle							= ::gpk::view_const_string{"Agent #"};
		menuTitle.append_string(playerUnitPlusOne);
		menuTitle.append_string(": Open position.");
	}

	static ::klib::SDrawMenuState				menuState;
	if(::klib::GAME_SUBSTATE_MAIN == instanceGame.State.Substate)
		actualReturnState = ::klib::drawMenu(menuState, instanceGame.GlobalDisplay.Screen.Color.View, instanceGame.GlobalDisplay.Screen.DepthStencil.begin(), menuTitle, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{::klib::optionsEquip}, instanceGame.FrameInput, {::klib::GAME_STATE_MENU_SQUAD_SETUP}, instanceGame.State, 30);
	else {
		if( player.Tactical.Selection.PlayerUnit >= (int16_t)player.Tactical.Squad.Agents.size())
			player.Tactical.Selection.PlayerUnit = -1;

		actualReturnState = ::drawEquipMenu(instanceGame, returnState);
	}

	return actualReturnState;
};


