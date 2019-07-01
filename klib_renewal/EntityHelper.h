#include "Entity.h"

#include <string>

#ifndef __ENTITYHELPER_H__98234092340283749023874920834234__
#define __ENTITYHELPER_H__98234092340283749023874920834234__

namespace klib
{
#pragma pack(push, 1)
	template<typename _EntityType>
	struct SResearchGroup {
		_EntityType												MaxResearch;
		SEntityContainer<int16_t, 128>							Definitions;
		SEntityContainer<int16_t, 128>							Modifiers;
	};


	// Combines two record tables to get the names and combine them as one for display purposes.
	template<typename _EntityType, size_t _DefinitionCount, size_t _ModifierCount> 
	::std::string											getEntityName						
		( const SEntity						& entity
		, const SEntityRecord<_EntityType>	(&tableDefinitions	)[_DefinitionCount	]
		, const SEntityRecord<_EntityType>	(&tableModifiers	)[_ModifierCount	]
		)
	{
		char														formattedName	[128]					= {};
		sprintf_s(formattedName, tableModifiers[entity.Modifier].Name.begin(), tableDefinitions[entity.Definition].Name.begin());
		return formattedName;
	}

	// removes an entity from a container and sets it to zero
	template<typename _EntityType, size_t _DefinitionCount>
	void													unequipEntity							(SEntityContainer<_EntityType, _DefinitionCount>& container, _EntityType& currentEntity)																	{
		if( 0 == currentEntity.Definition && 0 == currentEntity.Modifier && 0 == currentEntity.Level )
			return; 
		container.AddElement(currentEntity);	
		currentEntity											= {0,0,0};
	}

	template<typename _EntityType, size_t _definitionCount, size_t _modifierCount>
	static SEntityPoints								getEntityPoints								(const SEntity& entity,const SEntityRecord<_EntityType> (&definitions)[_definitionCount], const SEntityRecord<_EntityType> (&modifiers)[_modifierCount])	{
		return (definitions[entity.Definition].Points + modifiers[entity.Modifier].Points)*(_EntityType::getMultipliers()*entity.Level);
	}

	template<typename _EntityType, size_t _definitionCount, size_t _modifierCount>
	static SEntityFlags getEntityFlags(const SEntity& entity,const SEntityRecord<_EntityType> (&definitions)[_definitionCount], const SEntityRecord<_EntityType> (&modifiers)[_modifierCount])																	{
		return (definitions[entity.Definition].Flags | modifiers[entity.Modifier].Flags);
	}



#pragma pack(pop)
}	// namespace

#endif __ENTITYHELPER_H__98234092340283749023874920834234__
