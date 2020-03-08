#include "player.h"

void											klib::playerUpdateResearchLists		(const ::klib::SEntityTables & entityTables, ::klib::SPlayer & player)	{
	const ::klib::SCharacterInventory					& playerInventory					= player.Inventory;
	const ::klib::SCharacterResearch					& researchCompleted					= player.Tactical.Research;
	const ::gpk::array_obj<::klib::SEntityResearch>		& queuedResearch					= player.Projects.QueuedResearch;

	::gpk::array_obj<::klib::SEntityResearch>			& menuItemsValue					= player.ResearchablesValue;
	::gpk::array_obj<::gpk::array_pod<char_t>>			& menuItemsText						= player.ResearchablesText;
	menuItemsValue									= {};
	menuItemsText									= {};

	::gpk::array_pod<const ::klib::CCharacter*>		playerArmy					= {};
	for(uint32_t iAgent = 0; iAgent < player.Tactical.Army.size(); ++iAgent)
		playerArmy.push_back(player.Tactical.Army[iAgent]);

	::klib::getResearchableItems	(entityTables, playerInventory, researchCompleted, queuedResearch, playerArmy, menuItemsValue, menuItemsText);
	::klib::getResearchedItems		(entityTables, playerInventory, researchCompleted, playerArmy, menuItemsValue, menuItemsText);
}
