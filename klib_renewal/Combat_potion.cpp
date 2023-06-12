#include "Item.h"
#include "Combat.h"

static	bool							potionRestore						(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& potionDrinker, const int32_t potionGrade, const int32_t maxPoints, int32_t& currentPoints, const ::gpk::vcc& pointName)				{
	if(maxPoints <= currentPoints) {
		sprintf_s(messages.Aux, "Your %s is full!", pointName.begin());
		messages.LogAuxMessage();
		return false;
	}
	int32_t										lifeFractionBase					= (maxPoints>>3)+1;
	int32_t										lifeFractionExtra					= (maxPoints>>4);

	int32_t										itemEffectValue						= lifeFractionBase+(rand()%(lifeFractionExtra+1));
	itemEffectValue							*= potionGrade;
	int32_t										actualPointsRequiredToMax			= maxPoints-currentPoints;
	itemEffectValue							= (itemEffectValue < actualPointsRequiredToMax) ? itemEffectValue : actualPointsRequiredToMax;
	currentPoints							+= itemEffectValue;

	potionDrinker.Recalculate(tables);

	sprintf_s(messages.Aux, "The potion restores %u %s to %s! %s now has %u %s.", itemEffectValue, pointName.begin(), potionDrinker.Name.begin(), potionDrinker.Name.begin(), currentPoints, pointName.begin());
	messages.LogAuxMessage();
	return true;
}

static	bool							potionAttackBonus					(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& potionDrinker, const int32_t potionGrade, int32_t& currentPoints, int32_t& turnsLeft, const ::gpk::vcc& pointName)					{
	int32_t										pointsGainedBase					= 5*potionGrade;
	int32_t										pointsGainedExtra					= pointsGainedBase>>2;

	int32_t										itemEffectValue						= pointsGainedBase + rand() % (pointsGainedExtra+1);

	currentPoints							+= itemEffectValue;
	if(0 == turnsLeft)
		turnsLeft = 1;
	turnsLeft								+= potionGrade;

	potionDrinker.Recalculate(tables);

	const ::klib::SEntityPoints					& finalPoints						= potionDrinker.FinalPoints;
	sprintf_s(messages.Aux, "The potion gives %s %u %s points for %u turns. %s now has %u %s points for the next %u turns.", potionDrinker.Name.begin(), itemEffectValue, pointName.begin(), potionGrade, potionDrinker.Name.begin(), finalPoints.Attack.Damage, pointName.begin(), turnsLeft-1);
	messages.LogAuxMessage();
	return true;
}

bool									klib::usePotion						(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, const ::klib::SItem& itemPotion, CCharacter& potionDrinker)																								{
	if(0 == itemPotion.Level) {
		sprintf_s(messages.Aux, "The prop potion drank by %s doesn't seem to taste very well...", potionDrinker.Name.begin());
		messages.LogAuxMessage();
		return true;
	}

	const CItem									& itemDescription					= itemDescriptions[itemPotion.Definition];

	bool										bUsedItem							= false;

	::klib::SLifePoints							& currentPoints						= potionDrinker.Points.LifeCurrent;
	::klib::SCombatBonus						& drinkerBonus						= potionDrinker.ActiveBonus.Points;
	const ::klib::SLifePoints					maxPoints							= potionDrinker.FinalPoints.LifeMax;

	if(itemDescription.Property & PROPERTY_TYPE_HEALTH			) bUsedItem = bUsedItem || potionRestore		(tables, messages, potionDrinker, itemPotion.Level,	maxPoints.Health								, currentPoints.Health										, ::gpk::view_const_string{"Health"			});
	if(itemDescription.Property & PROPERTY_TYPE_MANA			) bUsedItem = bUsedItem || potionRestore		(tables, messages, potionDrinker, itemPotion.Level,	maxPoints.Mana									, currentPoints.Mana										, ::gpk::view_const_string{"Mana"			});
	if(itemDescription.Property & PROPERTY_TYPE_SHIELD			) bUsedItem = bUsedItem || potionRestore		(tables, messages, potionDrinker, itemPotion.Level,	maxPoints.Shield								, currentPoints.Shield										, ::gpk::view_const_string{"Shield"			});
	if(itemDescription.Property & PROPERTY_TYPE_HIT				) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.Hit					, drinkerBonus.TurnsLeftPoints.Attack.Hit					, ::gpk::view_const_string{"Hit"			});
	if(itemDescription.Property & PROPERTY_TYPE_DAMAGE			) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.Damage				, drinkerBonus.TurnsLeftPoints.Attack.Damage				, ::gpk::view_const_string{"Damage"			});
	if(itemDescription.Property & PROPERTY_TYPE_HEALTH_DAMAGE	) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.DirectDamage.Health	, drinkerBonus.TurnsLeftPoints.Attack.DirectDamage.Health	, ::gpk::view_const_string{"Health Damage"	});
	if(itemDescription.Property & PROPERTY_TYPE_MANA_DAMAGE		) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.DirectDamage.Mana	, drinkerBonus.TurnsLeftPoints.Attack.DirectDamage.Mana		, ::gpk::view_const_string{"Mana Damage"	});
	if(itemDescription.Property & PROPERTY_TYPE_SHIELD_DAMAGE	) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.DirectDamage.Shield	, drinkerBonus.TurnsLeftPoints.Attack.DirectDamage.Shield	, ::gpk::view_const_string{"Shield Damage"	});
	if(itemDescription.Property & PROPERTY_TYPE_ABSORPTION		) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.Absorption			, drinkerBonus.TurnsLeftPoints.Attack.Absorption			, ::gpk::view_const_string{"Absorption"		});
	if(itemDescription.Property & PROPERTY_TYPE_RANGE			) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Attack.Range				, drinkerBonus.TurnsLeftPoints.Attack.Absorption			, ::gpk::view_const_string{"Absorption"		});
	if(itemDescription.Property & PROPERTY_TYPE_ATTACK_SPEED	) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Fitness.Attack				, drinkerBonus.TurnsLeftPoints.Fitness.Attack				, ::gpk::view_const_string{"Attack Speed"	});
	if(itemDescription.Property & PROPERTY_TYPE_MOVEMENT_SPEED	) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Fitness.Movement			, drinkerBonus.TurnsLeftPoints.Fitness.Movement				, ::gpk::view_const_string{"Movement Speed"	});
	if(itemDescription.Property & PROPERTY_TYPE_REFLEXES		) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Fitness.Reflexes			, drinkerBonus.TurnsLeftPoints.Fitness.Reflexes				, ::gpk::view_const_string{"Reflexes"		});
	if(itemDescription.Property & PROPERTY_TYPE_SIGHT			) bUsedItem = bUsedItem || potionAttackBonus	(tables, messages, potionDrinker, itemPotion.Level,	drinkerBonus.Points.Fitness.Sight				, drinkerBonus.TurnsLeftPoints.Fitness.Sight				, ::gpk::view_const_string{"Sight"			});

	if(bUsedItem) {
		potionDrinker.Recalculate(tables);
		++potionDrinker.Score.UsedPotions;
	}

	return bUsedItem;
}
