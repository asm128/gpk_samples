#include "Entity.h"
#include "gpk_eval.h"

#include "GameMenu.h"

#ifndef __ENTITYRESEARCH_H__92803472098473209847239084702934__
#define __ENTITYRESEARCH_H__92803472098473209847239084702934__


#pragma pack(push, 1)

// This hell creates an user menu for the entity records available for research
template <typename _TEquipClass, typename _TInventory, typename _TResearched>
void																		research
	( ::klib::SEntityContainer<_TInventory>			& equipInventory
	, ::klib::SEntityContainer<_TResearched>		& researchedList
	, const ::gpk::view<const _TEquipClass>	& table
	, _TInventory									& adventurerMaxEquip
	, bool											bIsModifier
	, bool											bIsProgressive
	, const ::gpk::view_const_char					& itemFormat
	, const ::gpk::view_const_char					& allResearchComplete
	, const ::gpk::view_const_char					& noResearchAvailable
	, const ::gpk::view_const_char					& selectItemToResearch
	, const ::gpk::view_const_char					& startResearching
	, const ::gpk::view_const_char					& doneResearching
	)
{
	if(researchedList.size() >= table.size() - 1) {	// No more research items in the game.
		printf(allResearchComplete.begin(), researchedList.size());
		return;
	}

	// These variables are obviously to store the menuitems and compose the item menu text
	static const int32_t															maxItemCount										= 256;
	static char																		menuItemText	[maxItemCount]						= {};
	static klib::SMenuItem<int32_t>													menuItems		[maxItemCount]						= {};

	adventurerMaxEquip.Modifier													= (adventurerMaxEquip.Modifier		> 1) ? adventurerMaxEquip.Modifier		: 1;
	adventurerMaxEquip.Definition												= (adventurerMaxEquip.Definition	> 1) ? adventurerMaxEquip.Definition	: 1;
	adventurerMaxEquip.Level													= (adventurerMaxEquip.Level			> 1) ? adventurerMaxEquip.Level			: 1;

	int32_t																			menuItemCount										= 0;
	for( uint32_t i=0, count = equipInventory.size(); i<count; ++i ) {
		int32_t																			value												= 0;
		const char																		* stringLeft										= nullptr
			,																			* stringRight										= nullptr
			;
		if(bIsModifier) {
			int32_t																			selectedEntityModifier								= equipInventory[i].Entity.Modifier;
			if( 0 != selectedEntityModifier && (-1) == researchedList.FindElement((int16_t)selectedEntityModifier) ) {
				stringLeft																	= table[selectedEntityModifier].Name.begin();
				stringRight																	= itemFormat.begin();
				value																		= selectedEntityModifier;
				sprintf_s(menuItemText, stringLeft, stringRight);
#ifndef DISABLE_RESEARCH_REQUIREMENTS
				if(bIsProgressive) {
					if(value > adventurerMaxEquip.Modifier) {
						printf("%s can't be pursued because it's grade %u and your max research grade allowed for is %u.\n", menuItemText, value, adventurerMaxEquip.Modifier);
						continue;
					}
				}
#endif
			}
			else
				continue;
		}
		else {
			int32_t																			selectedEntityDefinition							= equipInventory[i].Entity.Definition;
			if( 0 != selectedEntityDefinition && (-1) == researchedList.FindElement((int16_t)selectedEntityDefinition) ) {
				stringRight																	= table[selectedEntityDefinition].Name.begin();
				stringLeft																	= itemFormat.begin();
				value																		= selectedEntityDefinition;
				sprintf_s(menuItemText, stringLeft, stringRight);
#ifndef DISABLE_RESEARCH_REQUIREMENTS
				if(bIsProgressive) {
					if(value > adventurerMaxEquip.Definition) {
						printf("%s can't be pursued because it's grade %u and your max research grade allowed for is %u.\n", menuItemText, value, adventurerMaxEquip.Definition);
						continue;
					}
				}
#endif
			}
			else
				continue;
		}

		bool																		bRequiresInserting										= true;
		for(int32_t j = 0; j < menuItemCount; ++j)
			if(menuItems[j].ReturnValue == value) {
				printf("You seem to have an additional research point for %s.\n", menuItemText);
				bRequiresInserting														= false;
				break;
			}

		if(bRequiresInserting)
			menuItems[menuItemCount++]												= { value, menuItemText };
	}

	if( 0 == menuItemCount ) {
		printf("%s", noResearchAvailable.begin());
		return;
	}

	menuItems[menuItemCount++]												= {maxItemCount, "Exit this menu"};

	sprintf_s(menuItemText, "%s", selectItemToResearch.begin());
	int32_t																		selectedValue											= displayMenu(menuItemText, ::gpk::view<const ::klib::SMenuItem<int32_t>>{menuItems}, menuItemCount);

	if(maxItemCount == selectedValue) {
		printf("You exit the labs.\n");
		return;
	}

	if(bIsModifier) {
		sprintf_s(menuItemText, table[selectedValue].Name.begin(), itemFormat.begin());
		printf(startResearching.begin(), menuItemText);
	}
	else
		printf(startResearching.begin(), table[selectedValue].Name.begin());

	researchedList.AddElement((int16_t)selectedValue);

	if(bIsModifier) {
		adventurerMaxEquip.Modifier												= ::gpk::max(adventurerMaxEquip.Modifier, (int16_t)(selectedValue+1));
		printf(doneResearching.begin(), menuItemText);
	}
	else {
		adventurerMaxEquip.Definition											= ::gpk::max(adventurerMaxEquip.Definition, (int16_t)(selectedValue+1));
		printf(doneResearching.begin(), table[selectedValue].Name.begin());
	}
	research
		( equipInventory
		, researchedList
		, table
		, adventurerMaxEquip
		, bIsModifier
		, bIsProgressive
		, itemFormat
		, allResearchComplete
		, noResearchAvailable
		, selectItemToResearch
		, startResearching
		, doneResearching
		);
};

#pragma pack(pop)


#endif // __ENTITYRESEARCH_H__92803472098473209847239084702934__
