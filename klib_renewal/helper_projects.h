#ifndef __HELPER_PROJECTS_H__9826348709234698723469823__
#define __HELPER_PROJECTS_H__9826348709234698723469823__

namespace klib
{
	//------------------------------------------------------------ Researched
	template<typename _EntityType>
	void																							addIfResearched
		(	klib::SResearchGroup<_EntityType>						& researchedItems
		,	const _EntityType										& entity
		,	const ::klib::SResearchGroup<_EntityType>					& researchCompleted
		)
	{
		if(entity.Definition	!= 0 && -1 != researchCompleted.Definitions.FindElement(entity.Definition))
			researchedItems.Definitions.AddElement(entity.Definition);

		if(entity.Modifier		!= 0 && -1 != researchCompleted.Modifiers.FindElement(entity.Modifier))
			researchedItems.Modifiers.AddElement(entity.Modifier);
	}

	// Returns the possible research for a given type of researchable
	template<typename _EntityType>
	void																							generateResearchedList
		(	klib::SResearchGroup		<_EntityType>	& researchableItems
		,	const ::klib::SEntityContainer<_EntityType>	& playerInventory
		,	const ::klib::SResearchGroup	<_EntityType>	& researchCompleted
		)
	{
		for(uint32_t iSlot = 0; iSlot < playerInventory.Slots.size(); ++iSlot) {
			const _EntityType																				& entity					= playerInventory[iSlot].Entity;
			addIfResearched(researchableItems, entity, researchCompleted);
		}
	}

	template<typename _EntityType>
	void																							generateResearchedListFromAgent
		(	klib::SResearchGroup<_EntityType>			& researchableItems
		,	const _EntityType							& equippedEntity
		,	const ::klib::SEntityContainer<_EntityType>	& agentInventory
		,	const ::klib::SResearchGroup<_EntityType>		& researchCompleted
		)
	{
		addIfResearched			(researchableItems, equippedEntity, researchCompleted);
		generateResearchedList	(researchableItems, agentInventory, researchCompleted);
	}

	template<typename _EntityType>
	void																							generateResearchedListFromAgentNoEquip
		(	klib::SResearchGroup		<_EntityType>	& researchableItems
		,	const ::klib::SEntityContainer<_EntityType>	& agentInventory
		,	const ::klib::SResearchGroup	<_EntityType>	& researchCompleted
		)
	{
		generateResearchedList(researchableItems, agentInventory, researchCompleted);
	}


	//------------------------------------------------------------ Researchable
	template<typename _EntityType>
	void																							addIfResearchable
		(	klib::SResearchGroup		<_EntityType>				& researchableItems
		,	const _EntityType										& entity
		,	const ::klib::SResearchGroup	<_EntityType>				& researchCompleted
		,	const ::gpk::array_obj		<SEntityResearch>			& queuedResearch
		,	bool													bIsParallelDefinition
		,	bool													bIsParallelModifier
		)
	{
		if(entity.Definition && -1 == researchCompleted.Definitions.FindElement(entity.Definition) && (bIsParallelDefinition || entity.Definition <= (researchCompleted.MaxResearch.Definition+1)) ) {
			bool																							bNotFound					= true;
			for(uint32_t i=0, count = (uint32_t)queuedResearch.size(); i<count; ++i) {
				const SEntityResearch																			& queuedStuff				= queuedResearch[i];
				if(queuedStuff.Type == entity.getType() && queuedStuff.Entity.Definition == entity.Definition) {
					bNotFound																					= false;	// we just don't add it if it's already added
					break;
				}
			}

			if(bNotFound)
				researchableItems.Definitions.AddElement(entity.Definition);
		}

		if(entity.Modifier && -1 == researchCompleted.Modifiers.FindElement(entity.Modifier) && (bIsParallelModifier || entity.Modifier <= (researchCompleted.MaxResearch.Modifier+1)) ) {
			bool																							bNotFound					= true;
			for(uint32_t i=0, count = (uint32_t)queuedResearch.size(); i<count; ++i) {
				const SEntityResearch																			& queuedStuff				= queuedResearch[i];
				if(queuedStuff.Type == entity.getType() && queuedStuff.Entity.Modifier == entity.Modifier) {
					bNotFound																					= false;	// we just don't add it if it's already added
					break;
				}
			}

			if(bNotFound)
				researchableItems.Modifiers.AddElement(entity.Modifier);
		}
	}

	// Returns the possible research for a given type of researchable
	template<typename _EntityType>
	void																							generateResearchableList
		(	klib::SResearchGroup			<_EntityType>			& researchableItems
		,	const ::klib::SEntityContainer	<_EntityType>			& playerInventory
		,	const ::klib::SResearchGroup		<_EntityType>			& researchCompleted
		,	const ::gpk::array_obj			<SEntityResearch>		& queuedResearch
		,	bool													bIsParallelDefinition
		,	bool													bIsParallelModifier
		)
	{
		for(uint32_t iSlot = 0; iSlot < playerInventory.Slots.size(); ++iSlot) {
			const _EntityType																					& entity				= playerInventory[iSlot].Entity;
			addIfResearchable(researchableItems, entity, researchCompleted, queuedResearch, bIsParallelDefinition, bIsParallelModifier);
		}
	}

	template<typename _EntityType>
	void																							generateResearchableListFromAgent
		(	klib::SResearchGroup			<_EntityType>			& researchableItems
		,	const _EntityType										& equippedEntity
		,	const ::klib::SEntityContainer	<_EntityType>			& agentInventory
		,	const ::klib::SResearchGroup		<_EntityType>			& researchCompleted
		,	const ::gpk::array_obj			<SEntityResearch>		& queuedResearch
		,	bool													bIsParallelDefinition
		,	bool													bIsParallelModifier
		)
	{
		addIfResearchable		(researchableItems, equippedEntity, researchCompleted, queuedResearch, bIsParallelDefinition, bIsParallelModifier);
		generateResearchableList(researchableItems, agentInventory, researchCompleted, queuedResearch, bIsParallelDefinition, bIsParallelModifier);
	}


	template<typename _EntityType>
	void																							generateResearchableListFromAgentNoEquip
		(	klib::SResearchGroup			<_EntityType>			& researchableItems
		,	const ::klib::SEntityContainer	<_EntityType>			& agentInventory
		,	const ::klib::SResearchGroup		<_EntityType>			& researchCompleted
		,	const ::gpk::array_obj			<SEntityResearch>		& queuedResearch
		,	bool													bIsParallelDefinition
		,	bool													bIsParallelModifier
		)
	{
		generateResearchableList(researchableItems, agentInventory, researchCompleted, queuedResearch, bIsParallelDefinition, bIsParallelModifier);
	}

} // namespace

#endif // __HELPER_PROJECTS_H__9826348709234698723469823__
