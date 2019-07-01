#ifndef __PROJECTS_H__9826348709234698723469823__
#define __PROJECTS_H__9826348709234698723469823__

namespace klib
{

	template<typename _TEntity, size_t _Size, size_t _SizeDefinitions, size_t _SizeModifiers>
	void																	completeProduction
		(	SEntityContainer<_TEntity, _Size>				& playerInventory
		,	const _TEntity									& entity
		,	::std::string									& messageSuccess
		,	const SEntityRecord<_TEntity>					(&definitionsTable	)[_SizeDefinitions	]
		,	const SEntityRecord<_TEntity>					(&modifiersTable	)[_SizeModifiers	]
		) 
	{
		playerInventory.AddElement(entity);
		::std::string																entityName						= getEntityName(entity, definitionsTable, modifiersTable);
		messageSuccess															= entityName + " production completed.";
	}

	template <typename _EntityType>
	void																	completeResearch
		(	const SEntityResearch							& selectedChoice
		,	_EntityType										& maxResearch
		,	SResearchGroup<_EntityType>						& researchCompleted
		,	::std::string									& successMessage
		)
	{
		if(selectedChoice.IsModifier) {
			researchCompleted.Modifiers.AddElement(selectedChoice.Entity.Modifier); 
			maxResearch.Modifier													= ::gpk::max(maxResearch.Modifier,	selectedChoice.Entity.Modifier);
		}
		else {
			researchCompleted.Definitions.AddElement(selectedChoice.Entity.Definition); 
			maxResearch.Definition													= ::gpk::max(maxResearch.Definition,	selectedChoice.Entity.Definition);
		}
		successMessage															= "You have successfully researched " + selectedChoice.Name + ".";
	}

	//-------------------------------------------------------------------------------------------
	static inline	void													acknowledgeResearch				(const SEntityResearch& selectedChoice, SPlayerProjects& playerProjects, std::string& successMessage)	{
		playerProjects.EnqueueResearch(selectedChoice); 
		successMessage															= selectedChoice.Name + " research has begun. Research cost: " + std::to_string(selectedChoice.PriceUnit - selectedChoice.PricePaid);
	}

	static inline	void													acknowledgeProduction			(const SEntityResearch& selectedChoice, SPlayerProjects& playerProjects, std::string& successMessage)	{
		playerProjects.EnqueueProduction(selectedChoice); 
		successMessage															= selectedChoice.Name + " production has begun. Cost: " + std::to_string(selectedChoice.PriceUnit - selectedChoice.PricePaid);
	}

} // namespace

#endif // __PROJECTS_H__9826348709234698723469823__