#include "Entity.h"

#ifndef __ENTITYHELPER_H__98234092340283749023874920834234__
#define __ENTITYHELPER_H__98234092340283749023874920834234__

namespace klib
{

	// removes an entity from a container and sets it to zero
	template<typename _EntityType>
	void										unequipEntity					(SEntityContainer<_EntityType>& container, _EntityType& currentEntity)																								{
		if( 0 == currentEntity.Definition && 0 == currentEntity.Modifier && 0 == currentEntity.Level )
			return;
		container.AddElement(currentEntity);
		currentEntity								= {0,0,0};
	}
}	// namespace

#endif __ENTITYHELPER_H__98234092340283749023874920834234__
