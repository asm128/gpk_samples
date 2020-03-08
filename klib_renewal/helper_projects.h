#ifndef __HELPER_PROJECTS_H__9826348709234698723469823__
#define __HELPER_PROJECTS_H__9826348709234698723469823__

namespace klib
{
	//------------------------------------------------------------ Researched
	template<typename _EntityType>
	void										addIfResearched
		(	::klib::SResearchGroup<_EntityType>			& researchedItems
		,	const _EntityType							& entity
		,	const ::klib::SResearchGroup<_EntityType>	& researchCompleted
		) {
		if(entity.Definition	!= 0 && -1 != researchCompleted.Definitions.FindElement(entity.Definition))
			researchedItems.Definitions.AddElement(entity.Definition);

		if(entity.Modifier		!= 0 && -1 != researchCompleted.Modifiers.FindElement(entity.Modifier))
			researchedItems.Modifiers.AddElement(entity.Modifier);
	}

	// Returns the possible research for a given type of researchable
	template<typename _EntityType>
	void										generateResearchedList
		(	::klib::SResearchGroup			<_EntityType> & researchableItems
		,	const ::klib::SEntityContainer	<_EntityType> & playerInventory
		,	const ::klib::SResearchGroup	<_EntityType> & researchCompleted
		) {
		for(uint32_t iSlot = 0; iSlot < playerInventory.Slots.size(); ++iSlot) {
			const _EntityType								& entity					= playerInventory[iSlot].Entity;
			::klib::addIfResearched(researchableItems, entity, researchCompleted);
		}
	}

	template<typename _EntityType>
	void										generateResearchedListFromAgent
		(	::klib::SResearchGroup<_EntityType>			& researchableItems
		,	const _EntityType							& equippedEntity
		,	const ::klib::SEntityContainer<_EntityType>	& agentInventory
		,	const ::klib::SResearchGroup<_EntityType>	& researchCompleted
		) {
		::klib::addIfResearched			(researchableItems, equippedEntity, researchCompleted);
		::klib::generateResearchedList	(researchableItems, agentInventory, researchCompleted);
	}

	template<typename _EntityType>
	void										generateResearchedListFromAgentNoEquip
		(	::klib::SResearchGroup			<_EntityType>		& researchableItems
		,	const ::klib::SEntityContainer	<_EntityType>		& agentInventory
		,	const ::klib::SResearchGroup	<_EntityType>		& researchCompleted
		) {
		::klib::generateResearchedList(researchableItems, agentInventory, researchCompleted);
	}

	//------------------------------------------------------------ Researchable
	template<typename _EntityType>
	void										addIfResearchable
		(	::klib::SResearchGroup		<_EntityType>				& researchableItems
		,	const _EntityType										& entity
		,	const ::klib::SResearchGroup<_EntityType>				& researchCompleted
		,	const ::gpk::view_array		<const SEntityResearch>		& queuedResearch
		,	bool													bIsParallelDefinition
		,	bool													bIsParallelModifier
		) {
		static constexpr	const ENTITY_TYPE			curType					= entity.getType();
		if(entity.Definition && -1 == researchCompleted.Definitions.FindElement(entity.Definition) && (bIsParallelDefinition || entity.Definition <= (researchCompleted.MaxResearch.Definition+1)) ) {
			bool											bNotFound				= true;
			for(uint32_t i=0, count = (uint32_t)queuedResearch.size(); i<count; ++i) {
				const SEntityResearch							& queuedStuff			= queuedResearch[i];
				if(queuedStuff.Type == curType && queuedStuff.Entity.Definition == entity.Definition) {
					bNotFound									= false;	// we just don't add it if it's already added
					break;
				}
			}

			if(bNotFound)
				researchableItems.Definitions.AddElement(entity.Definition);
		}

		if(entity.Modifier && -1 == researchCompleted.Modifiers.FindElement(entity.Modifier) && (bIsParallelModifier || entity.Modifier <= (researchCompleted.MaxResearch.Modifier+1)) ) {
			bool											bNotFound					= true;
			for(uint32_t i=0, count = (uint32_t)queuedResearch.size(); i<count; ++i) {
				const SEntityResearch							& queuedStuff				= queuedResearch[i];
				if(queuedStuff.Type == curType && queuedStuff.Entity.Modifier == entity.Modifier) {
					bNotFound									= false;	// we just don't add it if it's already added
					break;
				}
			}

			if(bNotFound)
				researchableItems.Modifiers.AddElement(entity.Modifier);
		}
	}

	// Returns the possible research for a given type of researchable
	template<typename _EntityType>
	void										generateResearchableList
		(	::klib::SResearchGroup			<_EntityType>			& researchableItems
		,	const ::klib::SEntityContainer	<_EntityType>			& playerInventory
		,	const ::klib::SResearchGroup	<_EntityType>			& researchCompleted
		,	const ::gpk::view_array			<const SEntityResearch>	& queuedResearch
		,	bool													bIsParallelDefinition
		,	bool													bIsParallelModifier
		) {
		for(uint32_t iSlot = 0; iSlot < playerInventory.Slots.size(); ++iSlot) {
			const _EntityType							& entity				= playerInventory[iSlot].Entity;
			::klib::addIfResearchable(researchableItems, entity, researchCompleted, queuedResearch, bIsParallelDefinition, bIsParallelModifier);
		}
	}

	template<typename _EntityType>
	void										generateResearchableListFromAgent
		(	::klib::SResearchGroup			<_EntityType>			& researchableItems
		,	const _EntityType										& equippedEntity
		,	const ::klib::SEntityContainer	<_EntityType>			& agentInventory
		,	const ::klib::SResearchGroup	<_EntityType>			& researchCompleted
		,	const ::gpk::view_array			<const SEntityResearch>	& queuedResearch
		,	bool													bIsParallelDefinition
		,	bool													bIsParallelModifier
		) {
		::klib::addIfResearchable		(researchableItems, equippedEntity, researchCompleted, queuedResearch, bIsParallelDefinition, bIsParallelModifier);
		::klib::generateResearchableList(researchableItems, agentInventory, researchCompleted, queuedResearch, bIsParallelDefinition, bIsParallelModifier);
	}


	template<typename _EntityType>
	void										generateResearchableListFromAgentNoEquip
		(	::klib::SResearchGroup			<_EntityType>			& researchableItems
		,	const ::klib::SEntityContainer	<_EntityType>			& agentInventory
		,	const ::klib::SResearchGroup	<_EntityType>			& researchCompleted
		,	const ::gpk::view_array			<const SEntityResearch>	& queuedResearch
		,	bool													bIsParallelDefinition
		,	bool													bIsParallelModifier
		) {
		::klib::generateResearchableList(researchableItems, agentInventory, researchCompleted, queuedResearch, bIsParallelDefinition, bIsParallelModifier);
	}
	template<typename _tEntity>
	void										completeProduction
		(	SEntityContainer<_tEntity>				& playerInventory
		,	const ::klib::SEntity					& entity
		,	::gpk::array_pod<char_t>				& messageSuccess
		,	const ::klib::SEntityTable<_tEntity>	& entityTable
		) {
		playerInventory.AddElement(entity);
		messageSuccess								= ::klib::getEntityName(entityTable, entity);
		messageSuccess.append_string(" production completed.");
	}

	template <typename _EntityType>
	void										completeResearch
		(	const SEntityResearch			& selectedChoice
		,	::klib::SEntity					& maxResearch
		,	SResearchGroup<_EntityType>		& researchCompleted
		,	::gpk::array_pod<char_t>		& successMessage
		) {
		if(selectedChoice.IsModifier) {
			researchCompleted.Modifiers.AddElement(selectedChoice.Entity.Modifier);
			maxResearch.Modifier						= ::gpk::max(maxResearch.Modifier,	selectedChoice.Entity.Modifier);
		}
		else {
			researchCompleted.Definitions.AddElement(selectedChoice.Entity.Definition);
			maxResearch.Definition						= ::gpk::max(maxResearch.Definition,	selectedChoice.Entity.Definition);
		}
		successMessage								= ::gpk::view_const_string{"You have successfully researched "};
		successMessage.append(selectedChoice.Name);
		successMessage.append_string(".");
	}

	//-------------------------------------------------------------------------------------------
	static inline	void						acknowledgeResearch				(const ::klib::SEntityResearch& selectedChoice, ::klib::SPlayerProjects& playerProjects, ::gpk::array_pod<char_t> & successMessage)	{
		playerProjects.EnqueueResearch(selectedChoice);
		successMessage								= selectedChoice.Name;
		successMessage.append_string(" research has begun. Research cost: ");
		char											cost	[32];
		sprintf_s(cost, "%lli", selectedChoice.PriceUnit - selectedChoice.PricePaid);
		successMessage.append_string(cost);
	}
	static inline	void						acknowledgeProduction			(const ::klib::SEntityResearch& selectedChoice, ::klib::SPlayerProjects& playerProjects, ::gpk::array_pod<char_t> & successMessage)	{
		playerProjects.EnqueueProduction(selectedChoice);
		successMessage								= selectedChoice.Name;
		successMessage.append_string(" production has begun. Cost: ");
		char											cost	[32];
		sprintf_s(cost, "%lli", selectedChoice.PriceUnit - selectedChoice.PricePaid);
		successMessage.append_string(cost);
	}
} // namespace

#endif // __HELPER_PROJECTS_H__9826348709234698723469823__
