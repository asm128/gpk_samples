#include "EntityImpl.h"

#ifndef __PROJECTS_H__9826348709234698723469823__
#define __PROJECTS_H__9826348709234698723469823__

namespace klib
{
	struct SEntityResearch  {
		::gpk::array_pod<char_t>					Name;
		int32_t										ResearchIndex;
		int64_t										PriceUnit;
		int64_t										PricePaid;
		::klib::SEntity								Entity;
		bool										IsModifier;
		::klib::ENTITY_TYPE							Type;
	};

#pragma pack(push, 1)
	struct SProjectBudget {
		bool										bIsRatio	;
		int64_t										Money		;
	};
#pragma pack(pop)

	struct SPlayerProjects {
		::klib::SProjectBudget						BudgetProduction			= {true, 10};
		::klib::SProjectBudget						BudgetResearch				= {true, 10};
		::klib::SProjectBudget						BudgetUpgrade				= {true, 10};

		::gpk::array_obj<::klib::SEntityResearch>	QueuedProduction			= {};
		::gpk::array_obj<::klib::SEntityResearch>	QueuedResearch				= {};
		::gpk::array_obj<::klib::SEntityResearch>	QueuedUpgrade				= {};

		int64_t										CostProduction				= 0;
		int64_t										CostResearch				= 0;
		int64_t										CostUpgrade					= 0;

		void										EnqueueProduction			( const SEntityResearch&	production	)	{ QueuedProduction	.push_back(	production	); CostProduction	 +=	production	.PriceUnit -	production	.PricePaid; };
		void										EnqueueResearch				( const SEntityResearch&	research	)	{ QueuedResearch	.push_back(	research	); CostResearch		 +=	research	.PriceUnit -	research	.PricePaid; };
		void										EnqueueUpgrade				( const SEntityResearch&	upgrade		)	{ QueuedUpgrade		.push_back(	upgrade		); CostUpgrade		 +=	upgrade		.PriceUnit -	upgrade		.PricePaid; };

		void										DequeueProduction			( int32_t index	)							{ const SEntityResearch & production	= QueuedProduction	[index]; CostProduction	 -=	production	.PriceUnit -	production	.PricePaid; QueuedProduction	.remove(index); }
		void										DequeueResearch				( int32_t index	)							{ const SEntityResearch & research		= QueuedResearch	[index]; CostResearch	 -=	research	.PriceUnit -	research	.PricePaid; QueuedResearch		.remove(index); }
		void										DequeueUpgrade				( int32_t index	)							{ const SEntityResearch & upgrade		= QueuedUpgrade		[index]; CostUpgrade	 -=	upgrade		.PriceUnit -	upgrade		.PricePaid; QueuedUpgrade		.remove(index); }
	};

	::gpk::error_t								getResearchableItems
		(	const ::klib::SEntityTables								& entityTables
		,	const ::klib::SCharacterInventory						& playerInventory
		,	const ::klib::SCharacterResearch						& researchCompleted
		,	const ::gpk::view_array<const ::klib::SEntityResearch>	& queuedResearch
		,	const ::gpk::view_array<const ::klib::CCharacter*>		& playerArmy
		,	::gpk::array_obj<::klib::SEntityResearch>				& menuItemsValue
		,	::gpk::array_obj<::gpk::array_pod<char_t>>				& menuItemsText
		);

	::gpk::error_t								getResearchedItems
		(	const ::klib::SEntityTables								& entityTables
		,	const ::klib::SCharacterInventory						& playerInventory
		,	const ::klib::SCharacterResearch						& researchCompleted
		,	const ::gpk::view_array<const ::klib::CCharacter*>		& playerArmy
		,	::gpk::array_obj<::klib::SEntityResearch>				& menuItemsValue
		,	::gpk::array_obj<::gpk::array_pod<char_t>>				& menuItemsText
		);

	void										handleProductionStep
		( const ::klib::SEntityTables	& entityTables
		, ::klib::SCharacterInventory	& playerInventory
		, ::klib::SPlayerProjects		& playerProjects
		, int64_t						& playerMoney
		, ::klib::SCharacterScore		& playerScore
		, ::klib::SGameMessages			& messages
		);
	void										handleResearchStep
		( ::klib::SCharacterResearch	& playerResearch
		, ::klib::SPlayerProjects		& playerProjects
		, int64_t						& playerMoney
		, ::klib::SCharacterScore		& playerScore
		, ::klib::SGameMessages			& messages
		);
} // namespace

#endif // __PROJECTS_H__9826348709234698723469823__
