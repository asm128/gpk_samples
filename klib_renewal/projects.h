#ifndef __PROJECTS_H__9826348709234698723469823__
#define __PROJECTS_H__9826348709234698723469823__

namespace klib
{
	template<typename _tEntity>
	void													completeProduction
		(	SEntityContainer<_tEntity>				& playerInventory
		,	const _tEntity							& entity
		,	::gpk::array_pod<char_t>				& messageSuccess
		,	const ::klib::SEntityTable<_tEntity>	& entityTable
		) {
		playerInventory.AddElement(entity);
		messageSuccess									= ::klib::getEntityName(entityTable, entity);
		messageSuccess.append_string(" production completed.");
	}

	template <typename _EntityType>
	void													completeResearch
		(	const SEntityResearch			& selectedChoice
		,	_EntityType						& maxResearch
		,	SResearchGroup<_EntityType>		& researchCompleted
		,	::gpk::array_pod<char_t>		& successMessage
		) {
		if(selectedChoice.IsModifier) {
			researchCompleted.Modifiers.AddElement(selectedChoice.Entity.Modifier);
			maxResearch.Modifier									= ::gpk::max(maxResearch.Modifier,	selectedChoice.Entity.Modifier);
		}
		else {
			researchCompleted.Definitions.AddElement(selectedChoice.Entity.Definition);
			maxResearch.Definition									= ::gpk::max(maxResearch.Definition,	selectedChoice.Entity.Definition);
		}
		successMessage											= ::gpk::view_const_string{"You have successfully researched "};
		successMessage.append(selectedChoice.Name);
		successMessage.append_string(".");
	}

	//-------------------------------------------------------------------------------------------
	static inline	void									acknowledgeResearch				(const SEntityResearch& selectedChoice, SPlayerProjects& playerProjects, ::gpk::array_pod<char_t> & successMessage)	{
		playerProjects.EnqueueResearch(selectedChoice);
		successMessage											= selectedChoice.Name;
		successMessage.append_string(" research has begun. Research cost: ");
		char														cost	[32];
		sprintf_s(cost, "%lli", selectedChoice.PriceUnit - selectedChoice.PricePaid);
		successMessage.append_string(cost);
	}
	static inline	void									acknowledgeProduction			(const SEntityResearch& selectedChoice, SPlayerProjects& playerProjects, ::gpk::array_pod<char_t> & successMessage)	{
		playerProjects.EnqueueProduction(selectedChoice);
		successMessage											= selectedChoice.Name;
		successMessage.append_string(" production has begun. Cost: ");
		char														cost	[32];
		sprintf_s(cost, "%lli", selectedChoice.PriceUnit - selectedChoice.PricePaid);
		successMessage.append_string(cost);
	}
} // namespace

#endif // __PROJECTS_H__9826348709234698723469823__
