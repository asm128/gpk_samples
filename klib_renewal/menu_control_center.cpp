#include "draw.h"
#include "Agent_helper.h"
#include "EntityDetail.h"

#include "klib_ascii_reference.h"

using namespace klib;

#define AGENT_ROW_WIDTH 43

template <typename _TReturn>
struct SListItem {
	_TReturn					ReturnValue;
	uint32_t					Color;
	::gpk::array_pod<char_t>	Text;
};

int32_t mouseOverList(const ::klib::SInput& frameInput, uint32_t rowCount, int32_t offsetX, int32_t offsetY, int32_t width) {
	int32_t indexSelected = -1;
	for(uint32_t iRow = 0; iRow < rowCount; ++iRow)
		if(::klib::mouseOver({frameInput.Mouse.Deltas.x, frameInput.Mouse.Deltas.y}, {offsetX, (int32_t)(offsetY + iRow)}, width)) {
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

template <typename _TReturn, size_t _SizeList>
int32_t drawList(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, const SListItem<_TReturn> (&listItems)[_SizeList], uint32_t rowCount, int32_t offsetX, int32_t offsetY, int32_t width) {
	char formatRow[128] = {};
	sprintf_s(formatRow, "%%-%i.%is", width, width);
	rowCount = ::gpk::min((uint32_t)_SizeList, rowCount);
	for(uint32_t iRow = 0; iRow < rowCount; ++iRow)
		::klib::printfToGridColored(display, textAttributes, (uint16_t)listItems[iRow].Color, offsetY+iRow, offsetX, ::klib::SCREEN_LEFT, formatRow, listItems[iRow].Text.begin());

	return 0;
}

template <typename _TReturn>
int32_t drawList(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, const ::gpk::array_obj<SListItem<_TReturn>>& listItems, uint32_t rowCount, int32_t offsetX, int32_t offsetY, int32_t width) {
	char formatRow[128] = {};
	sprintf_s(formatRow, "%%-%i.%is", width, width);
	for(uint32_t iRow = 0; iRow < rowCount; ++iRow) {
		const ::gpk::view_const_char		& rowText		= listItems[iRow].Text;
		::klib::printfToGridColored(display, textAttributes, (uint16_t)listItems[iRow].Color, offsetY, offsetX, ::klib::SCREEN_LEFT, formatRow, rowText.begin());
	}
	return 0;
}

int32_t processEquipAgentInput(const ::klib::SInput& frameInput, const SPlayer& player, uint32_t rowCount, int32_t offsetY, int32_t offsetX) {
	const int32_t mouseX = frameInput.Mouse.Deltas.x;
	const int32_t mouseY = frameInput.Mouse.Deltas.y;
	int32_t indexEquip = -1;

	offsetY += 1;
	int32_t actualAgentsProcessed = 0;
	for(uint32_t iRow = 0; iRow < rowCount; ++iRow) {
		if(0 == player.Tactical.Army[iRow] || player.Tactical.Squad.IsAgentAssigned((int32_t)iRow))
			continue;

		if(mouseOver({mouseX, mouseY}, {offsetX, offsetY+actualAgentsProcessed}, AGENT_ROW_WIDTH)) { indexEquip = iRow | (frameInput.Mouse.Buttons[0] ? 0x80000000 : 0); break; }

		++actualAgentsProcessed;
	}

	return indexEquip;
}

int32_t							drawAgentResume				(const ::klib::SEntityTables & tables, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, const CCharacter& agent, int32_t offsetY, int32_t offsetX, int32_t selectedAgentField) {
	char								preformatted0[12]			= {};
	char								preformatted1[12]			= {};
	char								formatted[64]				= {};

	int32_t								columnOffset;
	int32_t								finalColor, genderColor = agent.Flags.Tech.Gender == GENDER_FEMALE ? COLOR_DARKMAGENTA : agent.Flags.Tech.Gender == GENDER_MALE ? COLOR_BLUE : COLOR_GREEN;
	::gpk::array_pod<char_t>			equipName;

	finalColor						= (selectedAgentField == ENTITY_TYPE_CHARACTER) ? (genderColor<<4)|COLOR_DARKGREY : (COLOR_DARKGREY<<4)|genderColor;
	columnOffset					= printfToGridColored(display, textAttributes, (uint16_t)finalColor, offsetY++, offsetX, ::klib::SCREEN_LEFT, " %c - %-38.38s", ::klib::ascii_gender[agent.Flags.Tech.Gender], agent.Name.begin());

	SListItem<int32_t>					equipSlots[4];
	for(uint32_t i=0; i < ::gpk::size(equipSlots); ++i)
		equipSlots[i].Color = (selectedAgentField == (int32_t)i + 1) ? COLOR_GREEN : COLOR_GREEN << 4;

	equipName = getEntityName(tables.Profession	, agent.CurrentEquip.Profession	);	sprintf_s(formatted, " %-10.10s: %-30.30s", "Job"		, equipName.begin());	equipSlots[ENTITY_TYPE_PROFESSION	-1].Text = ::gpk::view_const_string{formatted};
	equipName = getEntityName(tables.Weapon		, agent.CurrentEquip.Weapon		);	sprintf_s(formatted, " %-10.10s: %-30.30s", "Weapon"	, equipName.begin());	equipSlots[ENTITY_TYPE_WEAPON		-1].Text = ::gpk::view_const_string{formatted};
	equipName = getEntityName(tables.Armor		, agent.CurrentEquip.Armor		);	sprintf_s(formatted, " %-10.10s: %-30.30s", "Armor"		, equipName.begin());	equipSlots[ENTITY_TYPE_ARMOR		-1].Text = ::gpk::view_const_string{formatted};
	equipName = getEntityName(tables.Accessory	, agent.CurrentEquip.Accessory	);	sprintf_s(formatted, " %-10.10s: %-30.30s", "Accessory"	, equipName.begin());	equipSlots[ENTITY_TYPE_ACCESSORY	-1].Text = ::gpk::view_const_string(formatted);
	drawList(display, textAttributes, equipSlots, (uint32_t)::gpk::size(equipSlots), offsetX, offsetY, 43);

	offsetY += (uint32_t)::gpk::size(equipSlots);

	sprintf_s(preformatted0, "%i", agent.Points.LifeCurrent.Health	);	sprintf_s(preformatted1, "%i", agent.FinalPoints.LifeMax.Health	);	sprintf_s(formatted, " Health    : %7.7s/%7.7s"	, preformatted0, preformatted1); columnOffset = printfToGridColored(display, textAttributes, COLOR_GREEN<<0, offsetY++, offsetX, ::klib::SCREEN_LEFT, "%-43.43s", formatted);
	sprintf_s(preformatted0, "%i", agent.Points.LifeCurrent.Mana	);	sprintf_s(preformatted1, "%i", agent.FinalPoints.LifeMax.Mana	);	sprintf_s(formatted, " Mana      : %7.7s/%7.7s"	, preformatted0, preformatted1); columnOffset = printfToGridColored(display, textAttributes, COLOR_GREEN<<0, offsetY++, offsetX, ::klib::SCREEN_LEFT, "%-43.43s", formatted);
	sprintf_s(preformatted0, "%i", agent.Points.LifeCurrent.Shield	);	sprintf_s(preformatted1, "%i", agent.FinalPoints.LifeMax.Shield	);	sprintf_s(formatted, " Shield    : %7.7s/%7.7s"	, preformatted0, preformatted1); columnOffset = printfToGridColored(display, textAttributes, COLOR_GREEN<<0, offsetY++, offsetX, ::klib::SCREEN_LEFT, "%-43.43s", formatted);
	return 0;
}

int32_t processSliderInput(const ::klib::SInput& frameInput, int32_t offsetY, int32_t offsetX, int64_t& value, int32_t minValue, int32_t maxValue) {
	int32_t mouseX = frameInput.Mouse.Deltas.x;
	int32_t mouseY = frameInput.Mouse.Deltas.y;
		 if( mouseOver({mouseX, mouseY}, {offsetX + 11, offsetY}, 3) && frameInput.Mouse.Buttons[0]) {if( value > minValue ) --value; }
	else if( mouseOver({mouseX, mouseY}, {offsetX + 20, offsetY}, 3) && frameInput.Mouse.Buttons[0]) {if( value < maxValue ) ++value; }

	return 0;
}

int32_t drawValueSlider(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, int64_t value, int32_t labelMaxLen, const ::gpk::view_const_char & controlLabel ) {
	char preformatted[16] = {};

	sprintf_s(preformatted, "%%-%i.%is:", labelMaxLen, labelMaxLen);	; printfToGridColored(display, textAttributes, COLOR_GREEN<<0	, offsetY, offsetX+00				, ::klib::SCREEN_LEFT, preformatted	, controlLabel.begin());
	preformatted[0] = ::klib::ascii_arrow[1]; preformatted[1] = 0;		; printfToGridColored(display, textAttributes, COLOR_GREEN<<4	, offsetY, offsetX+labelMaxLen+ 1	, ::klib::SCREEN_LEFT, " %s "			, preformatted);
	sprintf_s(preformatted, "%lli", value);								; printfToGridColored(display, textAttributes, COLOR_GREEN<<0	, offsetY, offsetX+labelMaxLen+ 4	, ::klib::SCREEN_LEFT, " %3.3s%% "	, preformatted);
	preformatted[0] = ::klib::ascii_arrow[3]; preformatted[1] = 0;		; printfToGridColored(display, textAttributes, COLOR_GREEN<<4	, offsetY, offsetX+labelMaxLen+10	, ::klib::SCREEN_LEFT, " %s"			, preformatted);
	return 0;
};

static	void drawBalance(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, const ::klib::SInput& frameInput, SPlayer& player, int32_t offsetX, int32_t offsetY) {
	// Budgets
	printfToGridColored(display, textAttributes, COLOR_YELLOW << 4, offsetY, offsetX+14, ::klib::SCREEN_LEFT, "%s", "    Project budgets   ");
	drawValueSlider		(display	, textAttributes, offsetY+2, offsetX+14, player.Projects.BudgetProduction	.Money	, 10, "Production");
	drawValueSlider		(display	, textAttributes, offsetY+4, offsetX+14, player.Projects.BudgetResearch		.Money	, 10, "Research");
	processSliderInput	(frameInput	, offsetY+2, offsetX+14, player.Projects.BudgetProduction	.Money	, 0, 100);
	processSliderInput	(frameInput	, offsetY+4, offsetX+14, player.Projects.BudgetResearch		.Money	, 0, 100);

	// Funds/Costs
	int64_t funds			= player.Tactical.Money;
	int64_t cost			= ::klib::missionCost(player, player.Tactical.Squad, player.Tactical.Squad.Size);
	int64_t fundsAfterCost	= funds - cost;

	int64_t productionCost	= (int64_t)(fundsAfterCost * (player.Projects.BudgetProduction.Money / 100.0));
	productionCost			= ::gpk::max(0LL, ::gpk::min(productionCost, player.Projects.CostProduction	));

	int64_t researchCost	= (int64_t)((fundsAfterCost - productionCost) * (player.Projects.BudgetResearch.Money / 100.0));
	researchCost			= ::gpk::max(0LL, ::gpk::min(researchCost, player.Projects.CostResearch		));

	char formatted[64];
	sprintf_s(formatted, "%15.15s%s%14.14s", "", "Balance", "");
	printfToGridColored(display, textAttributes, COLOR_YELLOW << 4, offsetY+7, offsetX, ::klib::SCREEN_LEFT, "%s", formatted);

	// draw fields
	int32_t selectedIndex = mouseOverList(frameInput, 5, offsetX, offsetY+8, 37) & ~0x80000000;
	SListItem<int32_t> balanceOptions[5] = {};

	for(uint32_t i=0; i<::gpk::size(balanceOptions); ++i)
		balanceOptions[i].Color = (selectedIndex == (int32_t)i) ? COLOR_GREEN : COLOR_GREEN << 4;

	char preformatted[16];
	sprintf_s(preformatted, "%lli", funds										);	sprintf_s(formatted, " Funds               : %13.13s", preformatted); balanceOptions[0].Text = ::gpk::view_const_string{formatted};
	sprintf_s(preformatted, "%lli", cost										);	sprintf_s(formatted, " Mission Cost        : %13.13s", preformatted); balanceOptions[1].Text = ::gpk::view_const_string{formatted};
	sprintf_s(preformatted, "%lli", productionCost								);	sprintf_s(formatted, " Production Cost     : %13.13s", preformatted); balanceOptions[2].Text = ::gpk::view_const_string{formatted};
	sprintf_s(preformatted, "%lli", researchCost								);	sprintf_s(formatted, " Research Cost       : %13.13s", preformatted); balanceOptions[3].Text = ::gpk::view_const_string{formatted};
	sprintf_s(preformatted, "%lli", fundsAfterCost-productionCost-researchCost);	sprintf_s(formatted, " Funds after mission : %13.13s", preformatted); balanceOptions[4].Text = ::gpk::view_const_string{formatted};

	drawList(display, textAttributes, balanceOptions, (uint32_t)::gpk::size(balanceOptions), offsetX, offsetY+8, 36);

}

template <typename _TEntity>
int32_t												drawEquipDetail
	( ::gpk::view_grid<char>		display
	, ::gpk::view_grid<uint16_t>	textAttributes
	, int32_t						offsetY
	, int32_t						offsetX
	, const ::gpk::view_const_char	& entityTypeName
	, const _TEntity				& entity
	, const SEntityTable<_TEntity>	& table
	) {
	//const ::gpk::array_pod<char_t>						entityName = ::getEntityName(entity, tableDefinitions, tableModifiers);
	::drawEntityDetail(display, textAttributes, offsetY, offsetX, entity, table, entityTypeName);

	char													formattedTitle[32]					= {};
	sprintf_s(formattedTitle, " - %s:", entityTypeName.begin());
	for(uint32_t i = 4, count= (uint32_t)::gpk::size(formattedTitle); i<count; ++i)
		formattedTitle[i]									= (char)::tolower(formattedTitle[i]);

	printfToGridColored(display, textAttributes, COLOR_YELLOW << 4 | COLOR_BLUE, offsetY, offsetX, ::klib::SCREEN_LEFT, "%-37.37s", formattedTitle);
	return 0;
}

template <typename _tEntity>
int32_t										drawEquipList
(	::gpk::view_grid<char>						display
,	::gpk::view_grid<uint16_t>					textAttributes
,	int32_t										offsetY
,	int32_t										offsetX
,	int32_t										selectedRow
,	const ::gpk::view_const_char				& entityTypeName
,	const SEntityContainer		<_tEntity>		& entityContainer
,	const ::klib::SEntityTable	<_tEntity>		& table
) {
	char											formattedTitle[32] = {};
	sprintf_s(formattedTitle, " - %s:", entityTypeName.begin());
	for(uint32_t i=4; i<::gpk::size(formattedTitle); ++i)
		formattedTitle[i]							= (char)::tolower(formattedTitle[i]);

	selectedRow									&= ~0x80000000;
	::gpk::array_pod<char_t>						entityName;
	for(uint32_t iEntity = 0, entityCount = entityContainer.Slots.size(); iEntity < entityCount; ++iEntity) {
		entityName									= ::klib::getEntityName(table, entityContainer[iEntity].Entity);
		uint16_t										colorRow						= (iEntity == (uint32_t)selectedRow) ? COLOR_YELLOW : COLOR_YELLOW << 4;
		printfToGridColored(display, textAttributes, colorRow, offsetY+1+iEntity, offsetX, ::klib::SCREEN_LEFT, "%12.12s %-30.30s", "", entityName.begin());
	}
	return 0;
}

int32_t drawAgentList
	( ::gpk::view_grid<char>										display
	, ::gpk::view_grid<uint16_t>									textAttributes
	, const SPlayer													& player
	, int32_t														offsetY
	, int32_t														offsetX
	, int32_t														selectedRow
	, const ::gpk::array_obj<::gpk::ptr_obj<::klib::CCharacter>>	& army
	) {
	int32_t											actualRowsDisplayed				= 0;
	selectedRow									&= ~0x80000000;
	for(uint32_t iEntity = 0, entityCount = army.size(); iEntity < entityCount; ++iEntity) {
		if(0 == army[iEntity] || player.Tactical.Squad.IsAgentAssigned(iEntity))
			continue;

		const CCharacter								& agent							= *army[iEntity];
		const ::gpk::array_pod<char>					& entityName					= agent.Name;
		uint16_t										colorRow						= ((int32_t)iEntity == selectedRow) ? COLOR_YELLOW : COLOR_YELLOW << 4;

		printfToGridColored(display, textAttributes, colorRow, offsetY+1+actualRowsDisplayed, offsetX, ::klib::SCREEN_LEFT, " %c - %-38.38s", ::klib::ascii_gender[agent.Flags.Tech.Gender], entityName.begin());
		colorRow									&= 0xF0;
		colorRow									|= agent.Flags.Tech.Gender == ::klib::GENDER_FEMALE ? COLOR_MAGENTA : agent.Flags.Tech.Gender == ::klib::GENDER_MALE ? COLOR_CYAN : COLOR_GREEN;
		printfToGridColored(display, textAttributes, colorRow, offsetY+1+actualRowsDisplayed, offsetX, ::klib::SCREEN_LEFT, " %c", ::klib::ascii_gender[agent.Flags.Tech.Gender]);

		++actualRowsDisplayed;
	}

	return 0;
}

//
int32_t										drawEquipList					(::klib::SEntityTables & tables, ENTITY_TYPE entityType, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, const SPlayer& player, int32_t offsetY, int32_t offsetX, int32_t selectedRow) {
	const ::gpk::view_const_char					labelet						= ::gpk::get_value_label(entityType);

	switch(entityType) {
	case ENTITY_TYPE_CHARACTER	:	drawAgentList(display, textAttributes, player, offsetY	, offsetX + entityType, selectedRow, player.Tactical.Army); break;
	case ENTITY_TYPE_PROFESSION	:	drawEquipList(display, textAttributes, offsetY + entityType, offsetX, selectedRow, labelet, player.Inventory.Profession	, tables.Profession	); break;
	case ENTITY_TYPE_WEAPON		:	drawEquipList(display, textAttributes, offsetY + entityType, offsetX, selectedRow, labelet, player.Inventory.Weapon		, tables.Weapon		); break;
	case ENTITY_TYPE_ARMOR		:	drawEquipList(display, textAttributes, offsetY + entityType, offsetX, selectedRow, labelet, player.Inventory.Armor		, tables.Armor		); break;
	case ENTITY_TYPE_ACCESSORY	:	drawEquipList(display, textAttributes, offsetY + entityType, offsetX, selectedRow, labelet, player.Inventory.Accessory	, tables.Accessory	); break;
	case -1:
	default:
		break;
	}

	return -1;
}

int32_t processEquipInput(ENTITY_TYPE entityType, const ::klib::SInput& frameInput, const SPlayer& player, int32_t offsetY)
{
	int32_t tempRow = -1;

	switch(entityType) {
	case ENTITY_TYPE_CHARACTER	:	tempRow = processEquipAgentInput(frameInput, player, player.Tactical.Army.size(), offsetY + entityType, 1); break;
	case ENTITY_TYPE_PROFESSION	:
	case ENTITY_TYPE_WEAPON		:
	case ENTITY_TYPE_ARMOR		:
	case ENTITY_TYPE_ACCESSORY	:	tempRow = mouseOverList(frameInput, player.Inventory.GetCount(entityType), 1, offsetY + 1 + entityType, AGENT_ROW_WIDTH);
	default:
		break;
	}

	return tempRow;
}

int32_t drawEquipDetail(const ::klib::SEntityTables & tables, ENTITY_TYPE entityType, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, const CCharacter& agent, int32_t offsetY, int32_t offsetX) {
	const ::gpk::view_const_char					labelSelectedEquip = ::gpk::get_value_label(entityType);
	switch(entityType) {
	case ENTITY_TYPE_CHARACTER	:	displayDetailedAgentSlot(tables, display, textAttributes, offsetY, offsetX, agent, (COLOR_YELLOW<<4)| COLOR_BLUE );	break;
	case ENTITY_TYPE_PROFESSION	:	drawEquipDetail(display, textAttributes, offsetY, offsetX, labelSelectedEquip, agent.CurrentEquip.Profession, tables.Profession	); break;
	case ENTITY_TYPE_WEAPON		:	drawEquipDetail(display, textAttributes, offsetY, offsetX, labelSelectedEquip, agent.CurrentEquip.Weapon	, tables.Weapon		); break;
	case ENTITY_TYPE_ARMOR		:	drawEquipDetail(display, textAttributes, offsetY, offsetX, labelSelectedEquip, agent.CurrentEquip.Armor		, tables.Armor		); break;
	case ENTITY_TYPE_ACCESSORY	:	drawEquipDetail(display, textAttributes, offsetY, offsetX, labelSelectedEquip, agent.CurrentEquip.Accessory	, tables.Accessory	); break;
	default:
		break;
	}
	return 0;
}

bool	equipIfResearchedProfession	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice);
bool	equipIfResearchedWeapon		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice);
bool	equipIfResearchedArmor		(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice);
bool	equipIfResearchedAccessory	(SGame& instanceGame, int32_t indexAgent, int16_t selectedChoice);

int32_t drawWelcomeGUI(SGame& instanceGame) {

	::gpk::view_grid<char>				display					= instanceGame.GlobalDisplay.Screen;
	::gpk::view_grid<uint16_t>			textAttributes			= instanceGame.GlobalDisplay.TextAttributes;

	int32_t								startY					= 1;

	static int32_t						selectedEquip			= -1;
	static int32_t						selectedAgent			= -1;
	bool								bHandledKey				= false;

	SPlayer								& player				= instanceGame.Players[PLAYER_INDEX_USER];

	// Squad
	printfToGridColored(display, textAttributes, COLOR_YELLOW << 4, startY, 1, ::klib::SCREEN_LEFT, "%-43.43s", " Assigned squad:");
	int32_t								agentsDisplayed			= 0;
	int32_t								selectedRow				= -1;
	int32_t								indexRow				= -1;
	startY							+= 2;
	for(uint32_t iAgent=0; iAgent < player.Tactical.Squad.Size; ++iAgent) {
		const int32_t						offsetY					= startY + ((agentsDisplayed)*10);
		if(player.Tactical.Squad.Agents[iAgent] == -1)
			continue;

		CCharacter& agent = *player.Tactical.Army[player.Tactical.Squad.Agents[iAgent]];

		int32_t selectedAgentField = mouseOverList(instanceGame.FrameInput, 5, 1, offsetY, AGENT_ROW_WIDTH);
		if(selectedAgentField != -1)
			selectedAgentField &= ~0x80000000;

		drawAgentResume(instanceGame.EntityTables, display, textAttributes, agent, offsetY, 1, selectedAgentField);

		if(selectedEquip != -1 && selectedAgent == (int32_t)iAgent) {
			ENTITY_TYPE		selectedEntityType	= (ENTITY_TYPE)selectedEquip;
			int32_t			tempRow				= processEquipInput(selectedEntityType, instanceGame.FrameInput, player, offsetY);

			drawEquipDetail(instanceGame.EntityTables, selectedEntityType, display, textAttributes, agent, startY, 45+1);

			if(tempRow != -1) {
				selectedRow = tempRow;
				indexRow	= tempRow & ~0x80000000;

				if(tempRow & 0x80000000) {
					switch(selectedEntityType) {
					case ENTITY_TYPE_CHARACTER	:	player.Tactical.Squad.Agents[iAgent] = int16_t(indexRow); break;
					case ENTITY_TYPE_PROFESSION	:	equipIfResearchedProfession	(instanceGame, player.Tactical.Squad.Agents[iAgent], (int16_t)indexRow); break;
					case ENTITY_TYPE_WEAPON		:	equipIfResearchedWeapon		(instanceGame, player.Tactical.Squad.Agents[iAgent], (int16_t)indexRow); break;
					case ENTITY_TYPE_ARMOR		:	equipIfResearchedArmor		(instanceGame, player.Tactical.Squad.Agents[iAgent], (int16_t)indexRow); break;
					case ENTITY_TYPE_ACCESSORY	:	equipIfResearchedAccessory	(instanceGame, player.Tactical.Squad.Agents[iAgent], (int16_t)indexRow); break;
					default:
						break;
					}
					selectedAgent = -1;
					selectedEquip = -1;
					bHandledKey = true;
				}
				else {//if(tempRow != -1)
					int32_t offsetX = 45*2;
					const ::gpk::view_const_char			labelSelectedEquip = ::gpk::get_value_label(selectedEntityType);
					switch(selectedEntityType) {
					case ENTITY_TYPE_CHARACTER	:	if(player.Tactical.Army[indexRow]) displayDetailedAgentSlot(instanceGame.EntityTables, display, textAttributes, startY, offsetX, *player.Tactical.Army[indexRow], (COLOR_YELLOW<<4)| COLOR_BLUE);	break;
					case ENTITY_TYPE_PROFESSION	:	drawEquipDetail(display, textAttributes, startY, offsetX, labelSelectedEquip, player.Inventory.Profession	[indexRow].Entity, instanceGame.EntityTables.Profession	);	break;
					case ENTITY_TYPE_WEAPON		:	drawEquipDetail(display, textAttributes, startY, offsetX, labelSelectedEquip, player.Inventory.Weapon		[indexRow].Entity, instanceGame.EntityTables.Weapon		);	break;
					case ENTITY_TYPE_ARMOR		:	drawEquipDetail(display, textAttributes, startY, offsetX, labelSelectedEquip, player.Inventory.Armor		[indexRow].Entity, instanceGame.EntityTables.Armor		);	break;
					case ENTITY_TYPE_ACCESSORY	:	drawEquipDetail(display, textAttributes, startY, offsetX, labelSelectedEquip, player.Inventory.Accessory	[indexRow].Entity, instanceGame.EntityTables.Accessory	);	break;
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
		for(uint32_t iAgent=0; iAgent < player.Tactical.Squad.Size; ++iAgent) {
			if(player.Tactical.Squad.Agents[iAgent] == -1 || 0 == player.Tactical.Army[player.Tactical.Squad.Agents[iAgent]])
				continue;

			if(selectedAgent == (int32_t)iAgent) {
				int32_t								offsetY					= startY + ((agentsDisplayed)*10);
				drawEquipList(instanceGame.EntityTables, (ENTITY_TYPE)selectedEquip, display, textAttributes, player, offsetY, 1, selectedRow);
			}

			++agentsDisplayed;
		}
	}

	if(selectedRow == -1) {
		agentsDisplayed = 0;
		for(uint32_t iAgent=0; iAgent < player.Tactical.Squad.Size; ++iAgent) {
			if(player.Tactical.Squad.Agents[iAgent] == -1)
				continue;
			CCharacter& agent = *player.Tactical.Army[player.Tactical.Squad.Agents[iAgent]];
			int32_t								offsetY					= startY + ((agentsDisplayed)*10);
			int32_t								indexEquip				= mouseOverList(instanceGame.FrameInput, 5, 1, offsetY, AGENT_ROW_WIDTH);	//processAgentInput	(display, instanceGame.FrameInput, agent , offsetY, 1);
			int32_t								rowIndex				= indexEquip & ~(0x80000000);
			if(indexEquip != -1) {
				if(selectedEquip == -1 && selectedAgent == -1)
					drawEquipDetail(instanceGame.EntityTables, (ENTITY_TYPE)rowIndex, display, textAttributes, agent, startY, 45+1);

				if(indexEquip & 0x80000000) {
					selectedEquip	= rowIndex;
					selectedAgent	= iAgent;
					bHandledKey		= true;
				}
			}
			++agentsDisplayed;
		}
	}

	drawBalance(display, textAttributes, instanceGame.FrameInput, instanceGame.Players[PLAYER_INDEX_USER], display.metrics().x - 38, display.metrics().y - 14);

	if((!bHandledKey) && instanceGame.FrameInput.Mouse.Buttons[0])
		selectedEquip = selectedAgent = -1;

	// Score
	//drawScore(display, (display.Depth>>1)-(display.Depth>>2), display.Width>>2, player.Score);
	return 0;
}

SGameState								drawWelcome			(SGame& instanceGame, const SGameState& returnValue) {
	instanceGame.GlobalDisplay.Clear();

	::gpk::array_pod<char_t>					textToPrint			= ::gpk::view_const_string{"Welcome back commander "};
	textToPrint.append(instanceGame.Players[PLAYER_INDEX_USER].Tactical.Name);
	textToPrint.append_string(".");
	SWeightedDisplay							& display			= instanceGame.GlobalDisplay;
	int32_t										lineOffset			= (display.Screen.metrics().y >> 1) - 1;
	int32_t										columnOffset		=  display.Screen.metrics().x / 2 - (int32_t)textToPrint.size() / 2;

	static ::klib::SMessageSlow					slowMessage;
	bool										bDonePrinting		= ::klib::getMessageSlow(slowMessage, textToPrint, instanceGame.FrameTimer.LastTimeSeconds);
	columnOffset							= printfToGridColored(display.Screen.View, display.TextAttributes, COLOR_GREEN, lineOffset, columnOffset, ::klib::SCREEN_LEFT, "%s", slowMessage);

	if ( bDonePrinting ) {
		drawWelcomeGUI(instanceGame);

		// Menu
		static SMenuHeader<SGameState>				menuControlCenter	({GAME_STATE_MENU_MAIN}, ::gpk::view_const_string{"Control Center"}, 28);
		bool										bInCourse			= ::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_TACTICAL) || ::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_TACTICAL_REMOTE);
		return drawMenu(display.Screen.View, display.TextAttributes.begin(), menuControlCenter, ::gpk::view_array<const ::klib::SMenuItem<SGameState>>{(bInCourse) ? optionsControlCenterMissionInCourse : optionsControlCenter}, instanceGame.FrameInput, returnValue);
	}
	return returnValue;
};
