#include "Item.h"
#include "Combat.h"

using namespace klib;

bool									potionRestore						(CCharacter& potionDrinker, const int32_t potionGrade, const int32_t maxPoints, int32_t& currentPoints, const std::string& pointName)				{
	if(maxPoints <= currentPoints) {
		printf("Your %s is full!\n", pointName.c_str());
		return false;
	}
	int32_t										lifeFractionBase					= (maxPoints>>3)+1;
	int32_t										lifeFractionExtra					= (maxPoints>>4);

	int32_t										itemEffectValue						= lifeFractionBase+(rand()%(lifeFractionExtra+1));
	itemEffectValue							*= potionGrade;
	int32_t										actualPointsRequiredToMax			= maxPoints-currentPoints;
	itemEffectValue							= (itemEffectValue < actualPointsRequiredToMax) ? itemEffectValue : actualPointsRequiredToMax;
	currentPoints							+= itemEffectValue;
	
	potionDrinker.Recalculate();
	
	const SEntityPoints							& finalPoints						= potionDrinker.FinalPoints;
	
	printf("The potion restores %u %s to %s! %s now has %u %s.\n", itemEffectValue, pointName.c_str(), potionDrinker.Name.c_str(), potionDrinker.Name.c_str(), currentPoints, pointName.c_str());
	return true;
}

bool									potionAttackBonus					(CCharacter& potionDrinker, const int32_t potionGrade, int32_t& currentPoints, int32_t& turnsLeft, const std::string& pointName)					{
	int32_t										pointsGainedBase					= 5*potionGrade;
	int32_t										pointsGainedExtra					= pointsGainedBase>>2;

	int32_t										itemEffectValue						= pointsGainedBase + rand() % (pointsGainedExtra+1);

	currentPoints							+= itemEffectValue;
	if(0 == turnsLeft)
		turnsLeft = 1;
	turnsLeft								+= potionGrade;
	
	potionDrinker.Recalculate();

	const SEntityPoints						& finalPoints							= potionDrinker.FinalPoints;
	printf("The potion gives %s %u %s points for %u turns. %s now has %u %s points for the next %u turns.\n", potionDrinker.Name.c_str(), itemEffectValue, pointName.c_str(), potionGrade, potionDrinker.Name.c_str(), finalPoints.Attack.Damage, pointName.c_str(), turnsLeft-1);

	return true;
}

bool									klib::usePotion						(const SItem& itemPotion, CCharacter& potionDrinker)																								{
	if(0 == itemPotion.Level) {
		printf("The prop potion drank by %s doesn't seem to taste very well...\n", potionDrinker.Name.c_str());
		return true;
	}

	const CItem									& itemDescription					= itemDescriptions[itemPotion.Definition];

	bool										bUsedItem							= false;

	SLifePoints									& currentPoints						= potionDrinker.Points.LifeCurrent;
	SCombatBonus								& drinkerBonus						= potionDrinker.ActiveBonus.Points;
	const SLifePoints							maxPoints							= potionDrinker.FinalPoints.LifeMax;

	if(	itemDescription.Property	&	 PROPERTY_TYPE_HEALTH			)	bUsedItem = bUsedItem || potionRestore		(potionDrinker, itemPotion.Level,	maxPoints.Health								, currentPoints.Health										, "Health"			);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_MANA				)	bUsedItem = bUsedItem || potionRestore		(potionDrinker, itemPotion.Level,	maxPoints.Mana									, currentPoints.Mana										, "Mana"			);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_SHIELD			)	bUsedItem = bUsedItem || potionRestore		(potionDrinker, itemPotion.Level,	maxPoints.Shield								, currentPoints.Shield										, "Shield"			);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_HIT				)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.Hit					, drinkerBonus.TurnsLeftPoints.Attack.Hit					, "Hit"				);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_DAMAGE			)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.Damage				, drinkerBonus.TurnsLeftPoints.Attack.Damage				, "Damage"			);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_HEALTH_DAMAGE	)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.DirectDamage.Health	, drinkerBonus.TurnsLeftPoints.Attack.DirectDamage.Health	, "Health Damage"	);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_MANA_DAMAGE		)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.DirectDamage.Mana	, drinkerBonus.TurnsLeftPoints.Attack.DirectDamage.Mana		, "Mana Damage"		);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_SHIELD_DAMAGE	)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.DirectDamage.Shield	, drinkerBonus.TurnsLeftPoints.Attack.DirectDamage.Shield	, "Shield Damage"	);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_ABSORPTION		)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.Absorption			, drinkerBonus.TurnsLeftPoints.Attack.Absorption			, "Absorption"		);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_RANGE			)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.Range				, drinkerBonus.TurnsLeftPoints.Attack.Absorption			, "Absorption"		);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_ATTACK_SPEED		)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Fitness.Attack				, drinkerBonus.TurnsLeftPoints.Fitness.Attack				, "Attack Speed"	);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_MOVEMENT_SPEED	)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Fitness.Movement			, drinkerBonus.TurnsLeftPoints.Fitness.Movement				, "Movement Speed"	);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_REFLEXES			)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Fitness.Reflexes			, drinkerBonus.TurnsLeftPoints.Fitness.Reflexes				, "Reflexes"		);
	if(	itemDescription.Property	&	 PROPERTY_TYPE_SIGHT			)	bUsedItem = bUsedItem || potionAttackBonus	(potionDrinker, itemPotion.Level,	drinkerBonus.Points.Fitness.Sight				, drinkerBonus.TurnsLeftPoints.Fitness.Sight				, "Sight"			);

	if(bUsedItem) {
		potionDrinker.Recalculate();
		++potionDrinker.Score.PotionsUsed;
	}

	return bUsedItem;
}