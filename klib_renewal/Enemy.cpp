#include "Enemy.h"

#include "Item.h"
#include "klib_draw_misc.h"
#include "Weapon.h"
#include "Armor.h"
#include "Profession.h"
#include "Vehicle.h"
#include "Accessory.h"
#include "Facility.h"
#include "StageProp.h"

#include <algorithm>


template<typename _TEntity, typename _TEntityRecord, size_t _SizeDefinitions, size_t _SizeModifiers>
void											setupAgentEquip
	( _TEntity							& currentEnemyCurrentEquip
	, const _TEntity					& templateAgentCurrentEquip
	, const _TEntityRecord				(&definitionsTable	)[_SizeDefinitions	]
	, const _TEntityRecord				(&modifiersTable	)[_SizeModifiers	]
	) {
	int32_t												defin															= templateAgentCurrentEquip.Definition	;
	int32_t												modif															= templateAgentCurrentEquip.Modifier	;
	defin											= ( defin <= 2 ) ? (rand() % 3) + 1 : defin + ((rand() % 3) - 1);
	modif											= ( modif <= 2 ) ? (rand() % 3) + 1 : modif + ((rand() % 3) - 1);

	currentEnemyCurrentEquip.Definition				= (int16_t)::gpk::max(::gpk::min( defin, (int32_t)::gpk::size( definitionsTable	) - 1), 1);
	currentEnemyCurrentEquip.Modifier				= (int16_t)::gpk::max(::gpk::min( modif, (int32_t)::gpk::size( modifiersTable	) - 1), 1);
	currentEnemyCurrentEquip.Level					= (int16_t)::gpk::max(templateAgentCurrentEquip.Level	+ ((rand() % 3) - 2), 1);
}

void											klib::completeAgentResearch										(::klib::CCharacter& agent)									{
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Profession	, agent.CurrentEquip.Profession	);
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Weapon		, agent.CurrentEquip.Weapon		);
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Armor			, agent.CurrentEquip.Armor		);
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Accessory		, agent.CurrentEquip.Accessory	);
}

void											klib::setupAgent												(const CCharacter& adventurer, CCharacter& currentEnemy)	{
	currentEnemy.Goods.Inventory.Items.AddElement({ 1+int16_t(rand()%(::gpk::size(itemDescriptions)-1)), 1+int16_t(rand()%(::gpk::size(itemModifiers)-1)), 1+int16_t(rand() % (::gpk::size(itemGrades)-1)) });

	for(int32_t iSlot=0, slotCount=adventurer.Goods.Inventory.Items.Slots.size(); iSlot<slotCount; ++iSlot)
		for(int32_t iItemUnit=0, itemUnitCount=adventurer.Goods.Inventory.Items[iSlot].Count; iItemUnit<itemUnitCount; ++iItemUnit)
			currentEnemy.Goods.Inventory.Items.AddElement({ 1+int16_t(rand()%(::gpk::size(itemDescriptions)-1)), 1+int16_t(rand()%(::gpk::size(itemModifiers)-1)), int16_t(rand() % ::gpk::size(itemGrades)) });

	currentEnemy.Goods.CompletedResearch			= SCharacterResearch();
	setupAgentEquip(currentEnemy.CurrentEquip.Profession	, adventurer.CurrentEquip.	Profession		, definitionsProfession	, modifiersProfession	);
	setupAgentEquip(currentEnemy.CurrentEquip.Weapon		, adventurer.CurrentEquip.	Weapon			, definitionsWeapon		, modifiersWeapon		);
	setupAgentEquip(currentEnemy.CurrentEquip.Armor			, adventurer.CurrentEquip.	Armor			, definitionsArmor		, modifiersArmor		);
	setupAgentEquip(currentEnemy.CurrentEquip.Accessory		, adventurer.CurrentEquip.	Accessory		, definitionsAccessory	, modifiersAccessory	);
	completeAgentResearch(currentEnemy);

	currentEnemy.CurrentEquip.Vehicle				= {0,0,1,-1};
	currentEnemy.CurrentEquip.Facility				= {0,0,1,-1};
	currentEnemy.CurrentEquip.StageProp				= {0,0,1,-1};

	currentEnemy.Recalculate();
	const SEntityPoints									& finalEnemyPoints												= currentEnemy.FinalPoints;
	currentEnemy.Points.LifeCurrent					= finalEnemyPoints.LifeMax;
	currentEnemy.Points.Coins						= currentEnemy.Points.CostMaintenance;
	currentEnemy.Flags.Tech.Gender					= GENDER(1+rand()%2);
}
