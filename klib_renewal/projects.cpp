#include "Character.h"
#include "projects.h"
#include "helper_projects.h"

static	void							completeProduction			(const ::klib::SEntityTables & entityTables, ::klib::SCharacterInventory & playerInventory, const ::klib::SEntityResearch & product, ::gpk::array_pod<char_t>& messageSuccess) {
	switch(product.Type) {
	case ::klib::ENTITY_TYPE_ACCESSORY	: ::klib::completeProduction(playerInventory.Accessory	, product.Entity, messageSuccess, entityTables.Accessory	);  break;
	case ::klib::ENTITY_TYPE_ARMOR		: ::klib::completeProduction(playerInventory.Armor		, product.Entity, messageSuccess, entityTables.Armor		);  break;
	case ::klib::ENTITY_TYPE_WEAPON		: ::klib::completeProduction(playerInventory.Weapon		, product.Entity, messageSuccess, entityTables.Weapon		);  break;
	case ::klib::ENTITY_TYPE_VEHICLE	: ::klib::completeProduction(playerInventory.Vehicle	, product.Entity, messageSuccess, entityTables.Vehicle		);  break;
	case ::klib::ENTITY_TYPE_PROFESSION	: ::klib::completeProduction(playerInventory.Profession	, product.Entity, messageSuccess, entityTables.Profession	);  break;
	case ::klib::ENTITY_TYPE_FACILITY	: ::klib::completeProduction(playerInventory.Facility	, product.Entity, messageSuccess, entityTables.Facility		);  break;
	case ::klib::ENTITY_TYPE_STAGE_PROP	: ::klib::completeProduction(playerInventory.StageProp	, product.Entity, messageSuccess, entityTables.StageProp	);  break;
	//case ::klib::ENTITY_TYPE_ITEM		:
	}
	char										price		[64]			= {};
	sprintf_s(price, "%lli", product.PricePaid);
	messageSuccess.append_string(" Cost: ");
	messageSuccess.append_string(price);
}

void									klib::handleProductionStep	(const ::klib::SEntityTables & entityTables, ::klib::SCharacterInventory & playerInventory, ::klib::SPlayerProjects & playerProjects, int64_t & playerMoney, ::klib::SCharacterScore & playerScore, ::klib::SGameMessages & messages) {
	int64_t										budgetProduction			= playerProjects.BudgetProduction.bIsRatio ? int32_t(playerMoney*(playerProjects.BudgetProduction.Money/100.0)) : ::gpk::min(playerProjects.BudgetProduction.Money, playerMoney);
	int64_t										actualCostProduction		= 0;
	for(uint32_t iProduct=0, productCount = playerProjects.QueuedProduction.size(); iProduct < productCount; ++iProduct) {
		if(actualCostProduction >= budgetProduction)
			break;

		::klib::SEntityResearch						& product					= playerProjects.QueuedProduction[iProduct];

		int64_t										assignedFunds				= ::gpk::min(budgetProduction-actualCostProduction, product.PriceUnit - product.PricePaid);
		product.PricePaid						+= assignedFunds;
		actualCostProduction					+= assignedFunds;
	}

	::gpk::array_obj<::klib::SEntityResearch>	remainingProduction;
	for(uint32_t iProduct = 0, productCount = playerProjects.QueuedProduction.size(); iProduct < productCount; ++iProduct) {
		const ::klib::SEntityResearch				& product					= playerProjects.QueuedProduction[iProduct];
		if((product.PriceUnit - product.PricePaid) > 0) {
			remainingProduction.push_back(product);
			continue; // This product is still in production
		}
		::completeProduction(entityTables, playerInventory, product, messages.UserSuccess);
		messages.LogSuccess();
		++playerScore.CompletedProduction;
	}
	playerProjects.CostProduction			-= actualCostProduction;
	playerProjects.QueuedProduction			= remainingProduction;
	playerMoney								-= actualCostProduction;
	playerScore.MoneySpent					+= actualCostProduction;
}

static	void							completeResearch				(const ::klib::SEntityResearch& product, ::klib::SCharacterResearch& playerResearch, ::gpk::array_pod<char_t> & successMessage)		{
	switch(product.Type) {
	case ::klib::ENTITY_TYPE_ACCESSORY	: ::klib::completeResearch(product, playerResearch.Accessory	.MaxResearch, playerResearch.Accessory	, successMessage); break;
	case ::klib::ENTITY_TYPE_ARMOR		: ::klib::completeResearch(product, playerResearch.Armor		.MaxResearch, playerResearch.Armor		, successMessage); break;
	case ::klib::ENTITY_TYPE_WEAPON		: ::klib::completeResearch(product, playerResearch.Weapon		.MaxResearch, playerResearch.Weapon		, successMessage); break;
	case ::klib::ENTITY_TYPE_VEHICLE	: ::klib::completeResearch(product, playerResearch.Vehicle		.MaxResearch, playerResearch.Vehicle	, successMessage); break;
	case ::klib::ENTITY_TYPE_PROFESSION	: ::klib::completeResearch(product, playerResearch.Profession	.MaxResearch, playerResearch.Profession	, successMessage); break;
	case ::klib::ENTITY_TYPE_FACILITY	: ::klib::completeResearch(product, playerResearch.Facility		.MaxResearch, playerResearch.Facility	, successMessage); break;
	case ::klib::ENTITY_TYPE_STAGE_PROP	: ::klib::completeResearch(product, playerResearch.StageProp	.MaxResearch, playerResearch.StageProp	, successMessage); break;
	//case ENTITY_TYPE_ITEM		:
	}

	successMessage.append_string(" Cost: ");
	char										price	[64]							= {};
	sprintf_s(price, "%lli", product.PricePaid);
	successMessage.append_string(price);
}

void									klib::handleResearchStep
	( ::klib::SCharacterResearch	& playerResearch
	, ::klib::SPlayerProjects		& playerProjects
	, int64_t						& playerMoney
	, ::klib::SCharacterScore		& playerScore
	, ::klib::SGameMessages			& messages
	) {
	int64_t										budgetResearch					= playerProjects.BudgetResearch.bIsRatio ? int64_t(playerMoney * (playerProjects.BudgetResearch.Money/100.0)) : ::gpk::min(playerProjects.BudgetResearch.Money, playerMoney);
	int64_t										actualCostResearch				= 0;
	for(uint32_t iProduct = 0, productCount = playerProjects.QueuedResearch.size(); iProduct < productCount; ++iProduct) {
		if(actualCostResearch >= budgetResearch)
			break;

		::klib::SEntityResearch						& product						= playerProjects.QueuedResearch[iProduct];
		int64_t										assignedFunds					= ::gpk::min(budgetResearch-actualCostResearch, product.PriceUnit - product.PricePaid);
		//product.PriceUnit						-= assignedFunds;
		product.PricePaid						+= assignedFunds;
		actualCostResearch						+= assignedFunds;
	}

	::gpk::array_obj<::klib::SEntityResearch>		remainingResearch;
	for(uint32_t iProduct = 0, productCount = playerProjects.QueuedResearch.size(); iProduct < productCount; ++iProduct) {
		const ::klib::SEntityResearch					& product						= playerProjects.QueuedResearch[iProduct];
		if((product.PriceUnit-product.PricePaid) > 0) {
			remainingResearch.push_back(product);
			continue; // This product is still in production
		}
		::completeResearch(product, playerResearch, messages.UserSuccess);
		messages.LogSuccess();
		++playerScore.CompletedResearch;
	}
	playerProjects.CostResearch				-=	actualCostResearch;
	playerProjects.QueuedResearch			=	remainingResearch;
	playerMoney								-=	actualCostResearch;
	playerScore.MoneySpent					+=	actualCostResearch;
}

template <typename _tEntity>
static int32_t							setupMenuItem					(::klib::SEntityResearch & menuItemValue, bool bModifier, int32_t index, ::klib::ENTITY_TYPE type, ::gpk::view_const_char labelEntityType, const ::klib::SEntityContainer<int16_t> & place, const ::gpk::view_array<const ::klib::SEntityRecord<_tEntity>> & records, ::gpk::view_const_char text) {
	char										composite	[256]				= {};
	menuItemValue.ResearchIndex				= index;
	int64_t										priceUnit						= records[place[index].Entity].Points.PriceBuy / 2;
	menuItemValue.PriceUnit					= priceUnit;
	menuItemValue.PricePaid					= 0;
	menuItemValue.Entity					= {bModifier ? 0 : place[index].Entity, bModifier ? place[index].Entity : 0, 1, -1};
	menuItemValue.Type						= type;
	sprintf_s(composite, "%s: %s", labelEntityType.begin(), text.begin());
	menuItemValue.Name						= ::gpk::view_const_string{composite};
	return 0;
}

int32_t									klib::getResearchableItems
	(	const ::klib::SEntityTables								& entityTables
	,	const ::klib::SCharacterInventory						& playerInventory
	,	const ::klib::SCharacterResearch						& researchCompleted
	,	const ::gpk::view_array<const ::klib::SEntityResearch>	& queuedResearch
	,	const ::gpk::view_array<const ::klib::CCharacter*>		& playerArmy
	,	::gpk::array_obj<::klib::SEntityResearch>				& menuItemsValue
	,	::gpk::array_obj<::gpk::array_pod<char_t>>				& menuItemsText
	) {
	::klib::SCharacterResearch					researchableItems				= {};

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)									\
		::klib::generateResearchableList(researchableItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_	\
			, queuedResearch																											\
			, ProgressiveDefinitions_, ProgressiveModifiers_);																			\
		for(uint32_t iAgent = 0; iAgent < armySize; ++iAgent) {											\
			if( 0 == playerArmy[iAgent] )																								\
				continue;																												\
			const ::klib::CCharacter				& playerAgent			= *playerArmy[iAgent];										\
			::klib::generateResearchableListFromAgent(researchableItems.EntityToken_, playerAgent.CurrentEquip.EntityToken_, playerAgent.Goods.Inventory.EntityToken_, researchCompleted.EntityToken_, queuedResearch, ProgressiveDefinitions_, ProgressiveModifiers_);	\
		}

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)						\
		::klib::generateResearchableList(researchableItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_	\
			, queuedResearch																											\
			, ProgressiveDefinitions_, ProgressiveModifiers_);																			\
		for(uint32_t iAgent=0; iAgent < armySize; ++iAgent) {												\
			if( 0 == playerArmy[iAgent] )																								\
				continue;																												\
			const ::klib::CCharacter				& playerAgent			= *playerArmy[iAgent];										\
			::klib::generateResearchableListFromAgentNoEquip(researchableItems.EntityToken_, playerAgent.Goods.Inventory.EntityToken_, researchCompleted.EntityToken_, queuedResearch, ProgressiveDefinitions_, ProgressiveModifiers_);									\
		}

	const uint32_t								armySize					= playerArmy.size();
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Accessory	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Profession	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Weapon		, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Armor		, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(StageProp	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Facility	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Vehicle	, false, false);

	const uint32_t								MAX_RESEARCH_ITEMS
		= entityTables.Accessory	.Definitions.size()
		+ entityTables.Weapon		.Definitions.size()
		+ entityTables.Armor		.Definitions.size()
		+ entityTables.Profession	.Definitions.size()
		+ entityTables.Vehicle		.Definitions.size()
		+ entityTables.Facility		.Definitions.size()
		+ entityTables.StageProp	.Definitions.size()
		+ entityTables.Accessory	.Modifiers	.size()
		+ entityTables.Weapon		.Modifiers	.size()
		+ entityTables.Armor		.Modifiers	.size()
		+ entityTables.Profession	.Modifiers	.size()
		+ entityTables.Vehicle		.Modifiers	.size()
		+ entityTables.Facility		.Modifiers	.size()
		+ entityTables.StageProp	.Modifiers	.size()
		;
	menuItemsText	.reserve(MAX_RESEARCH_ITEMS);
	menuItemsValue	.reserve(MAX_RESEARCH_ITEMS);

	::gpk::view_const_char						labelEntityType				= {};
	char										precompose			[256]	= {};

	::klib::SEntityResearch						menuItemValue				= {};
#define ADD_RESEARCH_DEFINITIONS(place, type, records)																							\
	labelEntityType							= ::gpk::get_value_label(type);																		\
	for(uint32_t i = 0, count = place.Definitions.Slots.size(); i < count; ++i) {																\
		menuItemValue.IsModifier				= false;																						\
		::setupMenuItem(menuItemValue, false, i, type, labelEntityType, place.Definitions, records, records[place.Definitions[i].Entity].Name);	\
		sprintf_s(precompose, "%-40.40s $%lli", menuItemValue.Name.begin(), menuItemValue.PriceUnit);											\
		menuItemsText	.push_back(::gpk::view_const_string{precompose});																		\
		menuItemsValue	.push_back(menuItemValue);																								\
	}

#define ADD_RESEARCH_MODIFIERS(place, type, records, text)																					\
	labelEntityType							= ::gpk::get_value_label(type);																	\
	for(uint32_t i = 0, count = place.Modifiers.Slots.size(); i < count; ++i) {																\
		sprintf_s(precompose, records[place.Modifiers[i].Entity].Name.begin(), text);														\
		menuItemValue.IsModifier				= true;																						\
		::setupMenuItem(menuItemValue, true, i, type, labelEntityType, place.Modifiers, records, ::gpk::view_const_string{precompose});		\
		sprintf_s(precompose, "%-40.40s $%lli", menuItemValue.Name.begin(), menuItemValue.PriceUnit);										\
		menuItemsText	.push_back(::gpk::view_const_string{precompose});																	\
		menuItemsValue	.push_back(menuItemValue);																							\
	}

	ADD_RESEARCH_DEFINITIONS(researchableItems.Weapon		, ENTITY_TYPE_WEAPON		, entityTables.Weapon		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Armor		, ENTITY_TYPE_ARMOR			, entityTables.Armor		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Accessory	, ENTITY_TYPE_ACCESSORY		, entityTables.Accessory	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Profession	, ENTITY_TYPE_PROFESSION	, entityTables.Profession	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Vehicle		, ENTITY_TYPE_VEHICLE		, entityTables.Vehicle		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Facility		, ENTITY_TYPE_FACILITY		, entityTables.Facility		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, entityTables.StageProp	.Definitions);

	ADD_RESEARCH_MODIFIERS(researchableItems.Weapon		, ENTITY_TYPE_WEAPON		, entityTables.Weapon		.Modifiers, "Science"				);
	ADD_RESEARCH_MODIFIERS(researchableItems.Armor		, ENTITY_TYPE_ARMOR			, entityTables.Armor		.Modifiers, "Technology"			);
	ADD_RESEARCH_MODIFIERS(researchableItems.Accessory	, ENTITY_TYPE_ACCESSORY		, entityTables.Accessory	.Modifiers, "Crafting Technique"	);
	ADD_RESEARCH_MODIFIERS(researchableItems.Profession	, ENTITY_TYPE_PROFESSION	, entityTables.Profession	.Modifiers, "Rank"					);
	ADD_RESEARCH_MODIFIERS(researchableItems.Vehicle	, ENTITY_TYPE_VEHICLE		, entityTables.Vehicle		.Modifiers, "Transportation"		);
	ADD_RESEARCH_MODIFIERS(researchableItems.Facility	, ENTITY_TYPE_FACILITY		, entityTables.Facility		.Modifiers, "Construction"			);
	ADD_RESEARCH_MODIFIERS(researchableItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, entityTables.StageProp	.Modifiers, "Enhacement"			);
	return (::gpk::error_t)menuItemsValue.size();
}

::gpk::error_t							klib::getResearchedItems
	(	const ::klib::SEntityTables								& entityTables
	,	const ::klib::SCharacterInventory						& playerInventory
	,	const ::klib::SCharacterResearch						& researchCompleted
	,	const ::gpk::view_array<const ::klib::CCharacter*>		& playerArmy
	,	::gpk::array_obj<::klib::SEntityResearch>				& menuItemsValue
	,	::gpk::array_obj<::gpk::array_pod<char_t>>				& menuItemsText
	) {
	::klib::SCharacterResearch					researchedItems										= {};

#define GET_RESEARCHED_FOR_ENTITY(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)								\
		::klib::generateResearchedList(researchedItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_);	\
		for(iAgent = 0; iAgent < armySize; ++iAgent) {																				\
			if( 0 == playerArmy[iAgent] )																							\
				continue;																											\
																																	\
			const ::klib::CCharacter				& playerAgent				= *playerArmy[iAgent];								\
			::klib::generateResearchedListFromAgent(researchedItems.EntityToken_, playerAgent.CurrentEquip.EntityToken_, playerAgent.Goods.Inventory.EntityToken_, researchCompleted.EntityToken_);	\
		}																															\

#define GET_RESEARCHED_FOR_ENTITY_NO_EQUIP(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)					\
		::klib::generateResearchedList(researchedItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_);	\
		for(iAgent = 0; iAgent < armySize; ++iAgent) {																				\
			if( 0 == playerArmy[iAgent] )																							\
				continue;																											\
																																	\
			const ::klib::CCharacter				& playerAgent				= *playerArmy[iAgent];								\
			::klib::generateResearchedListFromAgentNoEquip(researchedItems.EntityToken_, playerAgent.Goods.Inventory.EntityToken_, researchCompleted.EntityToken_);	\
		}																															\

	int32_t										iAgent												= 0;
	const int32_t								armySize											= (int32_t)playerArmy.size();
	GET_RESEARCHED_FOR_ENTITY			(Accessory	, false, false);
	GET_RESEARCHED_FOR_ENTITY_NO_EQUIP	(StageProp	, true , false);
	GET_RESEARCHED_FOR_ENTITY_NO_EQUIP	(Facility	, false, false);
	GET_RESEARCHED_FOR_ENTITY_NO_EQUIP	(Vehicle	, false, false);
	GET_RESEARCHED_FOR_ENTITY			(Profession	, true , false);
	GET_RESEARCHED_FOR_ENTITY			(Weapon		, false, false);
	GET_RESEARCHED_FOR_ENTITY			(Armor		, false, false);

	const uint32_t								MAX_RESEARCH_ITEMS
		= entityTables.Accessory	.Definitions.size()
		+ entityTables.Weapon		.Definitions.size()
		+ entityTables.Armor		.Definitions.size()
		+ entityTables.Profession	.Definitions.size()
		+ entityTables.Vehicle		.Definitions.size()
		+ entityTables.Facility		.Definitions.size()
		+ entityTables.StageProp	.Definitions.size()
		;
	menuItemsText	.reserve(MAX_RESEARCH_ITEMS);
	menuItemsValue	.reserve(MAX_RESEARCH_ITEMS);
	char										precompose			[256]							= {};
	::gpk::view_const_char						labelEntityType										= {};

	::klib::SEntityResearch						menuItemValue										= {};

	ADD_RESEARCH_DEFINITIONS(researchedItems.Profession	, ENTITY_TYPE_PROFESSION	, entityTables.Profession	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Weapon		, ENTITY_TYPE_WEAPON		, entityTables.Weapon		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Armor		, ENTITY_TYPE_ARMOR			, entityTables.Armor		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Accessory	, ENTITY_TYPE_ACCESSORY		, entityTables.Accessory	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Vehicle	, ENTITY_TYPE_VEHICLE		, entityTables.Vehicle		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Facility	, ENTITY_TYPE_FACILITY		, entityTables.Facility		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, entityTables.StageProp	.Definitions);

	ADD_RESEARCH_MODIFIERS(researchedItems.Weapon		, ENTITY_TYPE_WEAPON		, entityTables.Weapon		.Modifiers, "Mod");
	ADD_RESEARCH_MODIFIERS(researchedItems.Armor		, ENTITY_TYPE_ARMOR			, entityTables.Armor		.Modifiers, "Mod");
	ADD_RESEARCH_MODIFIERS(researchedItems.Accessory	, ENTITY_TYPE_ACCESSORY		, entityTables.Accessory	.Modifiers, "Mod");
	ADD_RESEARCH_MODIFIERS(researchedItems.Profession	, ENTITY_TYPE_PROFESSION	, entityTables.Profession	.Modifiers, "Badge" );
	ADD_RESEARCH_MODIFIERS(researchedItems.Vehicle		, ENTITY_TYPE_VEHICLE		, entityTables.Vehicle		.Modifiers, "Mod");
	ADD_RESEARCH_MODIFIERS(researchedItems.Facility		, ENTITY_TYPE_FACILITY		, entityTables.Facility		.Modifiers, "Mod");
	ADD_RESEARCH_MODIFIERS(researchedItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, entityTables.StageProp	.Modifiers, "Mod");
	return menuItemsValue.size();
}
