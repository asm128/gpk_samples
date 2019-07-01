#include "draw.h"
#include "Agent_helper.h"
#include "EntityDetail.h"

#include "klib_ascii_reference.h"

using namespace klib;

#define AGENT_ROW_WIDTH 43

template <typename _TReturn>
struct SListItem {
	_TReturn	ReturnValue;
	uint32_t	Color;
	std::string	Text;
};

int32_t mouseOverList(const ::klib::SInput& frameInput, uint32_t rowCount, int32_t offsetX, int32_t offsetY, int32_t width) {
	int32_t indexSelected = -1;
	for(uint32_t iRow = 0; iRow < rowCount; ++iRow)
		if(mouseOver(frameInput.Mouse.Deltas.x, frameInput.Mouse.Deltas.y, offsetX, offsetY+iRow, width)) {
			indexSelected = iRow | (frameInput.Mouse.Buttons[0] ? 0x80000000 : 0);
			break;
		}

	return indexSelected;
}

template <typename _TReturn, size_t _Size>
_TReturn processListInput(const ::klib::SInput& frameInput, const SListItem<_TReturn> (&listItems)[_Size], uint32_t rowCount, int32_t offsetX, int32_t offsetY, int32_t width, const _TReturn& noActionValue) {
	int32_t indexSelected = mouseOverList(frameInput, rowCount, offsetX, offsetY, width);
	if(indexSelected != -1 && (indexSelected & 0x80000000))
		return listItems[indexSelected & ~0x80000000].ReturnValue;

	return noActionValue;

}

template <typename _TReturn, size_t _Width, size_t _Height, size_t _SizeList>
int32_t drawList(SWeightedDisplay<_Width, _Height>& display, const SListItem<_TReturn> (&listItems)[_SizeList], uint32_t rowCount, int32_t offsetX, int32_t offsetY, int32_t width) {
	char formatRow[128] = {};
	sprintf_s(formatRow, "%%-%i.%is", width, width);
	rowCount = ::gpk::min((uint32_t)_SizeList, rowCount);
	for(uint32_t iRow = 0; iRow < rowCount; ++iRow)
		::klib::printfToGridColored(display.Screen, display.TextAttributes, (uint16_t)listItems[iRow].Color, offsetY+iRow, offsetX, ::klib::SCREEN_LEFT, formatRow, listItems[iRow].Text.c_str());

	return 0;
}

template <typename _TReturn>
int32_t drawList(SGlobalDisplay& display, const ::gpk::array_obj<SListItem<_TReturn>>& listItems, uint32_t rowCount, int32_t offsetX, int32_t offsetY, int32_t width) {
	char formatRow[128] = {};
	sprintf_s(formatRow, "%%-%i.%is", width, width);
	for(uint32_t iRow = 0; iRow < rowCount; ++iRow) {
		const std::string& rowText = listItems[iRow].Text;
		::klib::printfToGridColored(display.Screen, display.TextAttributes, (uint16_t)listItems[iRow].Color, offsetY, offsetX, ::klib::SCREEN_LEFT, formatRow, rowText.c_str());
	}
	return 0;
}

int32_t processEquipAgentInput(SGlobalDisplay& display, const ::klib::SInput& frameInput, const SPlayer& player, uint32_t rowCount, int32_t offsetY, int32_t offsetX) {
	const int32_t mouseX = frameInput.Mouse.Deltas.x; 
	const int32_t mouseY = frameInput.Mouse.Deltas.y; 
	int32_t indexEquip = -1;

	offsetY += 1; 
	int32_t actualAgentsProcessed = 0;
	for(uint32_t iRow = 0; iRow < rowCount; ++iRow) {
		if(0 == player.Army[iRow] || player.Squad.IsAgentAssigned((int32_t)iRow))
			continue;

		if(mouseOver(mouseX, mouseY, offsetX, offsetY+actualAgentsProcessed, AGENT_ROW_WIDTH)) { indexEquip = iRow | (frameInput.Mouse.Buttons[0] ? 0x80000000 : 0); break; }
		
		++actualAgentsProcessed;
	}

	return indexEquip;
}

int32_t drawAgentResume(SGlobalDisplay& display, const CCharacter& agent, int32_t offsetY, int32_t offsetX, int32_t selectedAgentField) {
	char preformatted0[12]	= {};
	char preformatted1[12]	= {};
	char formatted[64]		= {};

	int32_t columnOffset;
	int32_t finalColor, genderColor = agent.Flags.Tech.Gender == GENDER_FEMALE ? COLOR_DARKMAGENTA : agent.Flags.Tech.Gender == GENDER_MALE ? COLOR_BLUE : COLOR_GREEN;
	std::string equipName;

	finalColor = (selectedAgentField == ENTITY_TYPE_CHARACTER) ? (genderColor<<4)|COLOR_DARKGREY : (COLOR_DARKGREY<<4)|genderColor;											
	columnOffset = printfToGridColored(display.Screen, display.TextAttributes, finalColor, offsetY++, offsetX, ::klib::SCREEN_LEFT, " %c - %-38.38s", ::klib::ascii_gender[agent.Flags.Tech.Gender], agent.Name.c_str());

	SListItem<int32_t> equipSlots[4];
	for(uint32_t i=0; i<::gpk::size(equipSlots); ++i)
		equipSlots[i].Color = (selectedAgentField == i+1) ? COLOR_GREEN : COLOR_GREEN << 4; 

	equipName = getProfessionName	(agent.CurrentEquip.Profession	);	sprintf_s(formatted, " %-10.10s: %-30.30s", "Job"		, equipName.c_str());	equipSlots[ENTITY_TYPE_PROFESSION	-1].Text = std::string(formatted);
	equipName = getWeaponName		(agent.CurrentEquip.Weapon		);	sprintf_s(formatted, " %-10.10s: %-30.30s", "Weapon"	, equipName.c_str());	equipSlots[ENTITY_TYPE_WEAPON		-1].Text = std::string(formatted);
	equipName = getArmorName		(agent.CurrentEquip.Armor		);	sprintf_s(formatted, " %-10.10s: %-30.30s", "Armor"		, equipName.c_str());	equipSlots[ENTITY_TYPE_ARMOR		-1].Text = std::string(formatted);
	equipName = getAccessoryName	(agent.CurrentEquip.Accessory	);	sprintf_s(formatted, " %-10.10s: %-30.30s", "Accessory"	, equipName.c_str());	equipSlots[ENTITY_TYPE_ACCESSORY	-1].Text = std::string(formatted);
	drawList(display, equipSlots, (uint32_t)::gpk::size(equipSlots), offsetX, offsetY, 43);

	offsetY += (uint32_t)::gpk::size(equipSlots);

	sprintf_s(preformatted0, "%i", agent.Points.LifeCurrent.Health	);	sprintf_s(preformatted1, "%i", agent.FinalPoints.LifeMax.Health	);	sprintf_s(formatted, " Health    : %7.7s/%7.7s"	, preformatted0, preformatted1); columnOffset = printfToGridColored(display.Screen, display.TextAttributes, COLOR_GREEN<<0, offsetY++, offsetX, ::klib::SCREEN_LEFT, "%-43.43s", formatted);
	sprintf_s(preformatted0, "%i", agent.Points.LifeCurrent.Mana	);	sprintf_s(preformatted1, "%i", agent.FinalPoints.LifeMax.Mana	);	sprintf_s(formatted, " Mana      : %7.7s/%7.7s"	, preformatted0, preformatted1); columnOffset = printfToGridColored(display.Screen, display.TextAttributes, COLOR_GREEN<<0, offsetY++, offsetX, ::klib::SCREEN_LEFT, "%-43.43s", formatted);
	sprintf_s(preformatted0, "%i", agent.Points.LifeCurrent.Shield	);	sprintf_s(preformatted1, "%i", agent.FinalPoints.LifeMax.Shield	);	sprintf_s(formatted, " Shield    : %7.7s/%7.7s"	, preformatted0, preformatted1); columnOffset = printfToGridColored(display.Screen, display.TextAttributes, COLOR_GREEN<<0, offsetY++, offsetX, ::klib::SCREEN_LEFT, "%-43.43s", formatted);
	return 0;
}

int32_t processSliderInput(const ::klib::SInput& frameInput, int32_t offsetY, int32_t offsetX, int32_t& value, int32_t minValue, int32_t maxValue, int32_t labelMaxLen) {
	int32_t mouseX = frameInput.Mouse.Deltas.x; 
	int32_t mouseY = frameInput.Mouse.Deltas.y; 
		 if( mouseOver(mouseX, mouseY, offsetX+11, offsetY, 3) && frameInput.Mouse.Buttons[0]) {if( value > minValue ) --value; }
	else if( mouseOver(mouseX, mouseY, offsetX+20, offsetY, 3) && frameInput.Mouse.Buttons[0]) {if( value < maxValue ) ++value; }

	return 0;
}

int32_t drawValueSlider(SGlobalDisplay& display, int32_t offsetY, int32_t offsetX, int32_t value, int32_t labelMaxLen, const ::gpk::label& controlLabel ) {
	char preformatted[16] = {};
	
	sprintf_s(preformatted, "%%-%i.%is:", labelMaxLen, labelMaxLen);	; printfToGridColored(display.Screen, display.TextAttributes, COLOR_GREEN<<0	, offsetY, offsetX+00				, ::klib::SCREEN_LEFT, preformatted	, controlLabel.begin());
	preformatted[0] = ::klib::ascii_arrow[1]; preformatted[1] = 0;		; printfToGridColored(display.Screen, display.TextAttributes, COLOR_GREEN<<4	, offsetY, offsetX+labelMaxLen+ 1	, ::klib::SCREEN_LEFT, " %s "			, preformatted);
	sprintf_s(preformatted, "%i", value);								; printfToGridColored(display.Screen, display.TextAttributes, COLOR_GREEN<<0	, offsetY, offsetX+labelMaxLen+ 4	, ::klib::SCREEN_LEFT, " %3.3s%% "	, preformatted);
	preformatted[0] = ::klib::ascii_arrow[3]; preformatted[1] = 0;		; printfToGridColored(display.Screen, display.TextAttributes, COLOR_GREEN<<4	, offsetY, offsetX+labelMaxLen+10	, ::klib::SCREEN_LEFT, " %s"			, preformatted);
	return 0;
};

void drawBalance(SGlobalDisplay& display, const ::klib::SInput& frameInput, SPlayer& player, int32_t offsetX, int32_t offsetY) {
	// Budgets
	printfToGridColored(display.Screen, display.TextAttributes, COLOR_YELLOW << 4, offsetY, offsetX+14, ::klib::SCREEN_LEFT, "%s", "    Project budgets   ");
	drawValueSlider		(display	, offsetY+2, offsetX+14, player.Projects.BudgetProduction	.Money	, 10, "Production");
	drawValueSlider		(display	, offsetY+4, offsetX+14, player.Projects.BudgetResearch		.Money	, 10, "Research");
	processSliderInput	(frameInput	, offsetY+2, offsetX+14, player.Projects.BudgetProduction	.Money	, 0, 100, 10);
	processSliderInput	(frameInput	, offsetY+4, offsetX+14, player.Projects.BudgetResearch		.Money	, 0, 100, 10);

	// Funds/Costs
	int32_t funds			= player.Money;
	int32_t cost			= ::klib::missionCost(player, player.Squad, player.Squad.Size);
	int32_t fundsAfterCost	= funds - cost;

	int32_t productionCost	= (int32_t)(fundsAfterCost* (player.Projects.BudgetProduction	.Money/100.0));
	productionCost			= ::gpk::max(0, ::gpk::min(productionCost, player.Projects.CostProduction	));

	int32_t researchCost	= (int32_t)((fundsAfterCost-productionCost)*(player.Projects.BudgetResearch.Money/100.0));
	researchCost			= ::gpk::max(0, ::gpk::min(researchCost, player.Projects.CostResearch		));

	char formatted[64];
	sprintf_s(formatted, "%15.15s%s%14.14s", "", "Balance", "");
	printfToGridColored(display.Screen, display.TextAttributes, COLOR_YELLOW << 4, offsetY+7, offsetX, ::klib::SCREEN_LEFT, "%s", formatted);

	// draw fields
	int32_t selectedIndex = mouseOverList(frameInput, 5, offsetX, offsetY+8, 37) & ~0x80000000;
	SListItem<int32_t> balanceOptions[5] = {};

	for(uint32_t i=0; i<::gpk::size(balanceOptions); ++i)
		balanceOptions[i].Color = (selectedIndex == i) ? COLOR_GREEN : COLOR_GREEN << 4; 
	
	char preformatted[16];
	sprintf_s(preformatted, "%i", funds										);	sprintf_s(formatted, " Funds               : %13.13s", preformatted); balanceOptions[0].Text = std::string(formatted); 
	sprintf_s(preformatted, "%i", cost										);	sprintf_s(formatted, " Mission Cost        : %13.13s", preformatted); balanceOptions[1].Text = std::string(formatted); 
	sprintf_s(preformatted, "%i", productionCost							);	sprintf_s(formatted, " Production Cost     : %13.13s", preformatted); balanceOptions[2].Text = std::string(formatted); 
	sprintf_s(preformatted, "%i", researchCost								);	sprintf_s(formatted, " Research Cost       : %13.13s", preformatted); balanceOptions[3].Text = std::string(formatted); 
	sprintf_s(preformatted, "%i", fundsAfterCost-productionCost-researchCost);	sprintf_s(formatted, " Funds after mission : %13.13s", preformatted); balanceOptions[4].Text = std::string(formatted); 
																			
	drawList(display, balanceOptions, (uint32_t)::gpk::size(balanceOptions), offsetX, offsetY+8, 36);

}

template <typename _TEntity, size_t _DefinitionCount, size_t _ModifierCount>
int32_t drawEquipDetail
(	SGlobalDisplay& display
,	const SPlayer& player
,	const CCharacter& agent
,	int32_t offsetY
,	int32_t offsetX
,	const ::gpk::label& entityTypeName
,	const _TEntity& entity
,	const SEntityRecord<_TEntity>(&tableDefinitions	)[_DefinitionCount]
,	const SEntityRecord<_TEntity>(&tableModifiers	)[_ModifierCount]
)
{
	const std::string& entityName = ::getEntityName(entity, tableDefinitions, tableModifiers); 
	::drawEntityDetail(display, offsetY, offsetX, entity, tableDefinitions, tableModifiers, entityTypeName);
	
	char formattedTitle[32] = {};
	sprintf_s(formattedTitle, " - %s:", entityTypeName.begin());
	for(uint32_t i=4, count= (uint32_t)::gpk::size(formattedTitle); i<count; ++i)
		formattedTitle[i] = tolower(formattedTitle[i]);

	printfToGridColored(display.Screen, display.TextAttributes, COLOR_YELLOW << 4 | COLOR_BLUE, offsetY, offsetX, ::klib::SCREEN_LEFT, "%-37.37s", formattedTitle);

	return 0;
}

template <typename _TEntity, size_t _Size, size_t _DefinitionCount, size_t _ModifierCount>
int32_t drawEquipList
(	SGlobalDisplay& display
,	const ::klib::SInput& frameInput
,	const SPlayer& player
,	const CCharacter& agent
,	int32_t offsetY
,	int32_t offsetX
,	int32_t selectedRow
,	const ::gpk::label& entityTypeName
,	const SEntityContainer<_TEntity, _Size>& entityContainer
,	const SEntityRecord<_TEntity>(&tableDefinitions	)[_DefinitionCount]
,	const SEntityRecord<_TEntity>(&tableModifiers	)[_ModifierCount]
)
{
	char formattedTitle[32] = {};
	sprintf_s(formattedTitle, " - %s:", entityTypeName.begin());
	for(uint32_t i=4; i<::gpk::size(formattedTitle); ++i)
		formattedTitle[i] = tolower(formattedTitle[i]);

	selectedRow &= ~0x80000000;
	std::string entityName;
	for(uint32_t iEntity = 0, entityCount = entityContainer.Count; iEntity < entityCount; ++iEntity) 
	{ 
		entityName = getEntityName(entityContainer[iEntity].Entity, tableDefinitions, tableModifiers); 
		uint16_t colorRow = (iEntity == selectedRow) ? COLOR_YELLOW : COLOR_YELLOW << 4;

		printfToGridColored(display.Screen, display.TextAttributes, colorRow, offsetY+1+iEntity, offsetX, ::klib::SCREEN_LEFT, "%12.12s %-30.30s", "", entityName.c_str());
	} 

	return 0;
}

int32_t drawAgentList
(	SGlobalDisplay& display
,	const ::klib::SInput& frameInput
,	const SPlayer& player
,	int32_t offsetY
,	int32_t offsetX
,	int32_t selectedRow
,	const ::gpk::label& entityTypeName
,	const ::gpk::array_obj<::gpk::ptr_obj<::klib::CCharacter>>& army
)
{
	int32_t actualRowsDisplayed = 0;
	selectedRow &= ~0x80000000;
	for(uint32_t iEntity = 0, entityCount = army.size(); iEntity < entityCount; ++iEntity) { 
		if(0 == army[iEntity] || player.Squad.IsAgentAssigned(iEntity))
			continue;

		const CCharacter& agent = *army[iEntity];
		const std::string& entityName = agent.Name; 
		uint16_t colorRow = (iEntity == selectedRow) ? COLOR_YELLOW : COLOR_YELLOW << 4;

		printfToGridColored(display.Screen, display.TextAttributes, colorRow, offsetY+1+actualRowsDisplayed, offsetX, ::klib::SCREEN_LEFT, " %c - %-38.38s", ::klib::ascii_gender[agent.Flags.Tech.Gender], entityName.c_str());
		colorRow &= 0xF0;
		colorRow |= agent.Flags.Tech.Gender == ::klib::GENDER_FEMALE ? COLOR_MAGENTA : agent.Flags.Tech.Gender == ::klib::GENDER_MALE ? COLOR_CYAN : COLOR_GREEN;
		printfToGridColored(display.Screen, display.TextAttributes, colorRow, offsetY+1+actualRowsDisplayed, offsetX, ::klib::SCREEN_LEFT, " %c", ::klib::ascii_gender[agent.Flags.Tech.Gender]);
		
		++actualRowsDisplayed;
	} 

	return 0;
}

// 
int32_t drawEquipList(ENTITY_TYPE entityType, SGlobalDisplay& display, const ::klib::SInput& frameInput, const SPlayer& player, const CCharacter& agent, int32_t offsetY, int32_t offsetX, int32_t selectedRow) {
	const ::gpk::label& labelet = ::gpk::get_value_label(entityType);

	switch(entityType) {
	case ENTITY_TYPE_CHARACTER	:	drawAgentList(display, frameInput, player, offsetY	, offsetX + entityType, selectedRow, labelet, player.Army); break; 
	case ENTITY_TYPE_PROFESSION	:	drawEquipList(display, frameInput, player, agent	, offsetY + entityType, offsetX, selectedRow, labelet, player.Goods.Inventory.Profession	, definitionsProfession	, modifiersProfession	); break; 
	case ENTITY_TYPE_WEAPON		:	drawEquipList(display, frameInput, player, agent	, offsetY + entityType, offsetX, selectedRow, labelet, player.Goods.Inventory.Weapon		, definitionsWeapon		, modifiersWeapon		); break; 
	case ENTITY_TYPE_ARMOR		:	drawEquipList(display, frameInput, player, agent	, offsetY + entityType, offsetX, selectedRow, labelet, player.Goods.Inventory.Armor			, definitionsArmor		, modifiersArmor		); break; 
	case ENTITY_TYPE_ACCESSORY	:	drawEquipList(display, frameInput, player, agent	, offsetY + entityType, offsetX, selectedRow, labelet, player.Goods.Inventory.Accessory		, definitionsAccessory	, modifiersAccessory	); break; 
	case -1:
	default:
		break;
	}

	return -1;
}

int32_t processEquipInput(ENTITY_TYPE entityType, SGlobalDisplay& display, const ::klib::SInput& frameInput, const SPlayer& player, int32_t offsetY, int32_t offsetX)
{
	int32_t tempRow = -1;
				
	switch(entityType) {
	case ENTITY_TYPE_CHARACTER	:	tempRow = processEquipAgentInput(display, frameInput, player, player.Army.size(), offsetY + entityType, 1); break;
	case ENTITY_TYPE_PROFESSION	:	
	case ENTITY_TYPE_WEAPON		:	
	case ENTITY_TYPE_ARMOR		:	
	case ENTITY_TYPE_ACCESSORY	:	tempRow = mouseOverList(frameInput, player.Goods.Inventory.GetCount(entityType), 1, offsetY + 1 + entityType, AGENT_ROW_WIDTH);
	default:
		break;
	}

	return tempRow;
}

int32_t drawEquipDetail(ENTITY_TYPE entityType, SGlobalDisplay& display, const SPlayer& player, const CCharacter& agent, int32_t offsetY, int32_t offsetX) {
	const ::gpk::label& labelSelectedEquip = ::gpk::get_value_label(entityType);
	switch(entityType) {						
	case ENTITY_TYPE_CHARACTER	:	displayDetailedAgentSlot(display, offsetY, offsetX, agent, (COLOR_YELLOW<<4)| COLOR_BLUE );	break;
	case ENTITY_TYPE_PROFESSION	:	drawEquipDetail(display, player, agent, offsetY, offsetX, labelSelectedEquip, agent.CurrentEquip.Profession	, definitionsProfession	, modifiersProfession	);	break;
	case ENTITY_TYPE_WEAPON		:	drawEquipDetail(display, player, agent, offsetY, offsetX, labelSelectedEquip, agent.CurrentEquip.Weapon		, definitionsWeapon		, modifiersWeapon		);	break;
	case ENTITY_TYPE_ARMOR		:	drawEquipDetail(display, player, agent, offsetY, offsetX, labelSelectedEquip, agent.CurrentEquip.Armor		, definitionsArmor		, modifiersArmor		);	break;
	case ENTITY_TYPE_ACCESSORY	:	drawEquipDetail(display, player, agent, offsetY, offsetX, labelSelectedEquip, agent.CurrentEquip.Accessory	, definitionsAccessory	, modifiersAccessory	);	break;
	default:
		break;
	}
	return 0;
}

bool	equipIfResearchedProfession	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice);
bool	equipIfResearchedWeapon		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice);
bool	equipIfResearchedArmor		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice);
bool	equipIfResearchedAccessory	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice);

int32_t drawWelcomeGUI(SGame& instanceGame, const SGameState& returnValue) {
	SGlobalDisplay& display			= instanceGame.GlobalDisplay;
	int32_t startY					= TACTICAL_DISPLAY_POSY-2; 

	static int32_t selectedEquip	= -1;
	static int32_t selectedAgent	= -1;
	bool bHandledKey				= false;

	SPlayer& player					= instanceGame.Players[PLAYER_INDEX_USER];

	// Squad
	printfToGridColored(display.Screen, display.TextAttributes, COLOR_YELLOW << 4, TACTICAL_DISPLAY_POSY-2, 1, ::klib::SCREEN_LEFT, "%-43.43s", " Assigned squad:");
	int32_t agentsDisplayed			= 0;
	int32_t selectedRow				= -1;
	int32_t indexRow				= -1;
	for(int32_t iAgent=0; iAgent < player.Squad.Size; ++iAgent) {
		if(player.Squad.Agents[iAgent] == -1)
			continue;

		int32_t offsetY = TACTICAL_DISPLAY_POSY+((agentsDisplayed)*10);
		CCharacter& agent = *player.Army[player.Squad.Agents[iAgent]];

		int32_t selectedAgentField = mouseOverList(instanceGame.FrameInput, 5, 1, offsetY, AGENT_ROW_WIDTH);
		if(selectedAgentField != -1)
			selectedAgentField &= ~0x80000000;

		drawAgentResume(display, agent, offsetY, 1, selectedAgentField);

		if(selectedEquip != -1 && selectedAgent == iAgent) {
			ENTITY_TYPE selectedEntityType = (ENTITY_TYPE)selectedEquip;
			int32_t tempRow = processEquipInput(selectedEntityType, display, instanceGame.FrameInput, player, offsetY, 1);

			drawEquipDetail(selectedEntityType, display, player, agent, startY, 45+1);										

			if(tempRow != -1) {
				selectedRow = tempRow;
				indexRow	= tempRow & ~0x80000000;

				if(tempRow & 0x80000000) {
					switch(selectedEntityType) {
					case ENTITY_TYPE_CHARACTER	:	player.Squad.Agents[iAgent] = int16_t(indexRow); break;
					case ENTITY_TYPE_PROFESSION	:	equipIfResearchedProfession	(instanceGame, player.Squad.Agents[iAgent], indexRow); break;
					case ENTITY_TYPE_WEAPON		:	equipIfResearchedWeapon		(instanceGame, player.Squad.Agents[iAgent], indexRow); break;
					case ENTITY_TYPE_ARMOR		:	equipIfResearchedArmor		(instanceGame, player.Squad.Agents[iAgent], indexRow); break;
					case ENTITY_TYPE_ACCESSORY	:	equipIfResearchedAccessory	(instanceGame, player.Squad.Agents[iAgent], indexRow); break;
					default:
						break;
					}
					selectedAgent = -1;
					selectedEquip = -1;
					bHandledKey = true;
				}
				else {//if(tempRow != -1)
					int32_t offsetX = 45*2;
					const ::gpk::label& labelSelectedEquip = ::gpk::get_value_label(selectedEntityType);
					switch(selectedEntityType) {						
					case ENTITY_TYPE_CHARACTER	:	if(player.Army[indexRow]) displayDetailedAgentSlot(display, startY, offsetX, *player.Army[indexRow], (COLOR_YELLOW<<4)| COLOR_BLUE);	break;
					case ENTITY_TYPE_PROFESSION	:	drawEquipDetail(display, player, agent, startY, offsetX, labelSelectedEquip, player.Goods.Inventory.Profession	[indexRow].Entity, definitionsProfession	, modifiersProfession	);	break;
					case ENTITY_TYPE_WEAPON		:	drawEquipDetail(display, player, agent, startY, offsetX, labelSelectedEquip, player.Goods.Inventory.Weapon		[indexRow].Entity, definitionsWeapon		, modifiersWeapon		);	break;
					case ENTITY_TYPE_ARMOR		:	drawEquipDetail(display, player, agent, startY, offsetX, labelSelectedEquip, player.Goods.Inventory.Armor		[indexRow].Entity, definitionsArmor			, modifiersArmor		);	break;
					case ENTITY_TYPE_ACCESSORY	:	drawEquipDetail(display, player, agent, startY, offsetX, labelSelectedEquip, player.Goods.Inventory.Accessory	[indexRow].Entity, definitionsAccessory		, modifiersAccessory	);	break;
					default:
						break;
					}
				}
			}
		}
		++agentsDisplayed;
	}

	if(selectedEquip != -1) {
		agentsDisplayed = 0;
		for(int32_t iAgent=0; iAgent < player.Squad.Size; ++iAgent) {
			if(player.Squad.Agents[iAgent] == -1 || 0 == player.Army[player.Squad.Agents[iAgent]])
				continue;

			int32_t offsetY = TACTICAL_DISPLAY_POSY+((agentsDisplayed)*10);
			CCharacter& agent = *player.Army[player.Squad.Agents[iAgent]];

			if(selectedAgent == iAgent)
				drawEquipList((ENTITY_TYPE)selectedEquip, display, instanceGame.FrameInput, player, agent, offsetY, 1, selectedRow);

			++agentsDisplayed;
		}
	}

	if(selectedRow == -1) {
		agentsDisplayed = 0;
		for(int32_t iAgent=0; iAgent < player.Squad.Size; ++iAgent) {
			if(player.Squad.Agents[iAgent] == -1)
				continue;

			int32_t offsetY = TACTICAL_DISPLAY_POSY+((agentsDisplayed)*10);
			CCharacter& agent = *player.Army[player.Squad.Agents[iAgent]];
			int32_t indexEquip = mouseOverList(instanceGame.FrameInput, 5, 1, offsetY, AGENT_ROW_WIDTH);	//processAgentInput	(display, instanceGame.FrameInput, agent , offsetY, 1);
			int32_t rowIndex = indexEquip & ~(0x80000000);
			if(indexEquip != -1) {
				if(selectedEquip == -1 && selectedAgent == -1)
					drawEquipDetail((ENTITY_TYPE)rowIndex, display, player, agent, startY, 45+1);	

				if(indexEquip & 0x80000000) {
					selectedEquip	= rowIndex;
					selectedAgent	= iAgent;
					bHandledKey		= true;
				}
			}
			++agentsDisplayed;
		}
	}

	drawBalance(display, instanceGame.FrameInput, instanceGame.Players[PLAYER_INDEX_USER], display.Width-38, startY);

	if((!bHandledKey) && instanceGame.FrameInput.Mouse.Buttons[0])
		selectedEquip = selectedAgent = -1;

	// Score
	//drawScore(display, (display.Depth>>1)-(display.Depth>>2), display.Width>>2, player.Score);
	return 0;
}

SGameState drawWelcome(SGame& instanceGame, const SGameState& returnValue) {
	instanceGame.GlobalDisplay.Clear();

	const std::string				textToPrint			= "Welcome back commander " + std::string(instanceGame.Players[PLAYER_INDEX_USER].Name.begin()) + ".";
	SGlobalDisplay					& display			= instanceGame.GlobalDisplay;
	int32_t							lineOffset			= (display.Screen.Depth>>1)-1;
	int32_t							columnOffset		=  display.Screen.Width/2-(int32_t)textToPrint.size()/2;

	bool							bDonePrinting		= ::klib::getMessageSlow(instanceGame.SlowMessage, {textToPrint.data(), (uint32_t)textToPrint.size()}, instanceGame.FrameTimer.LastTimeSeconds);
	columnOffset				= printfToGridColored(display.Screen, display.TextAttributes, COLOR_GREEN, lineOffset, columnOffset, ::klib::SCREEN_LEFT, "%s", instanceGame.SlowMessage);

	if ( bDonePrinting ) {
		drawWelcomeGUI(instanceGame, returnValue);

		// Menu
		static const SMenu<SGameState>	menuControlCenter	({GAME_STATE_MENU_MAIN}, "Control Center", 28);
		bool							bInCourse			= ::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_TACTICAL) || ::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_TACTICAL_REMOTE);

		return drawMenu(display.Screen, &display.TextAttributes[0][0], menuControlCenter, (bInCourse) ? optionsControlCenterMissionInCourse : optionsControlCenter, instanceGame.FrameInput, returnValue);
	}
	return returnValue;
};
