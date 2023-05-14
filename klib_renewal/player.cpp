#include "player.h"

void											klib::playerUpdateResearchLists		(const ::klib::SEntityTables & entityTables, ::klib::SGamePlayer & player)	{
	const ::klib::SCharacterInventory					& playerInventory					= player.Inventory;
	const ::klib::SCharacterResearch					& researchCompleted					= player.Tactical.Research;
	const ::gpk::array_obj<::klib::SEntityResearch>		& queuedResearch					= player.Projects.QueuedResearch;

	::gpk::array_pod<const ::klib::CCharacter*>			playerArmy					= {};
	for(uint32_t iAgent = 0; iAgent < player.Tactical.Army.size(); ++iAgent)
		playerArmy.push_back(player.Tactical.Army[iAgent]);

	::gpk::array_obj<::klib::SEntityResearch>			& researchablesValue				= player.ResearchablesValue	= {};
	::gpk::array_obj<::gpk::apod<char>>			& researchablesText					= player.ResearchablesText	= {};

	::klib::getResearchableItems	(entityTables, playerInventory, researchCompleted, queuedResearch, playerArmy, researchablesValue, researchablesText);

	::gpk::array_obj<::klib::SEntityResearch>			& researchedValue					= player.ResearchedValue	= {};
	::gpk::array_obj<::gpk::apod<char>>			& researchedText					= player.ResearchedText		= {};
	::klib::getResearchedItems		(entityTables, playerInventory, researchCompleted, playerArmy, researchedValue, researchedText);
}
