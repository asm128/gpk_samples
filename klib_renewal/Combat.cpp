#include "Combat.h"
#include "Item.h"

#include <algorithm>

using namespace klib;

SLifePoints klib::applyShieldableDamage(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, CCharacter& target, int32_t damageDealt, const ::gpk::view_const_char& sourceName) {
	return applyShieldableDamage(tables, messages, target, damageDealt, getArmorAbsorption(tables.Armor, target.CurrentEquip.Armor), sourceName);
}

// Returns final passthrough damage (not blocked by shield) to use by vampire and reflect effects.
SLifePoints klib::applyShieldableDamage(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, CCharacter& target, int32_t damageDealt, int32_t absorptionRate, const ::gpk::view_const_char& sourceName) {
	if(0 >= target.Points.LifeCurrent.Health)	// This character is already dead
		return {};

	const ::gpk::array_pod<char_t>		targetArmorName				= ::klib::getEntityName	(tables.Armor, target.CurrentEquip.Armor);
	const ::klib::SEntityPoints			& targetFinalPoints			= target.FinalPoints	;
	const ::klib::SEntityFlags			& targetFinalFlags			= target.FinalFlags		;
	const int32_t						targetArmorShield			= targetFinalPoints.LifeMax.Shield;

	// Impenetrable armors always have 60% extra absorption rate.
	if(targetFinalFlags.Effect.Defend & DEFEND_EFFECT_IMPENETRABLE) {
		if(target.Points.LifeCurrent.Shield) {
			absorptionRate += 60;
			sprintf_s(messages.Aux, "%s damage absorption rate for %s is raised to %%%u because of the impenetrable property.", targetArmorName.begin(), ::gpk::toString(sourceName).begin(), absorptionRate);
			messages.LogAuxMessage();
		}
	}
	else {
		// If the armor is not impenetrable, the absorption rate is affected by the shield damage.
		sprintf_s(messages.Aux, "%s damage absorption rate for %s is %%%u.", targetArmorName.begin(), ::gpk::toString(sourceName).begin(), absorptionRate);
		messages.LogAuxMessage();
		double								shieldDivisor				= (targetArmorShield > 1) ? targetArmorShield : 1;
		if(target.Points.LifeCurrent.Shield) {
			double possibleAbsorptionRate =  absorptionRate*(target.Points.LifeCurrent.Shield/shieldDivisor);
			absorptionRate	= absorptionRate ?
				((possibleAbsorptionRate > 1) ? (int32_t)possibleAbsorptionRate : 1) : 0;
		}
		else
			absorptionRate = 0;

		sprintf_s(messages.Aux, "%s final damage absorption rate taking deterioration into account is %%%u.", targetArmorName.begin(), absorptionRate);
		messages.LogAuxMessage();
	}

	absorptionRate = (absorptionRate < 100) ? absorptionRate : 100;
	const double	absorptionFraction	= 0.01*absorptionRate;
	int shieldedDamage		= (int)(damageDealt * absorptionFraction);
	int passthroughDamage	= (int)(damageDealt * ((0.0 > 1.0-absorptionFraction) ? 0.0 : 1.0 - absorptionFraction ));
	int totalDamage			= shieldedDamage+passthroughDamage;

	sprintf_s(messages.Aux, "Shielded damage: %u. Passthrough damage: %u. Expected sum: %u. Actual sum: %u. Absorption ratio: %%%u.", shieldedDamage, passthroughDamage, damageDealt, totalDamage, absorptionRate);
	messages.LogAuxMessage();
	if( totalDamage < damageDealt )	{// because of the lack of rounding when casting to integer, a difference of one may be found after calculating the health-shield proportion.
		int errorDamage = damageDealt-totalDamage;

		// if we have no health or the absorption rate is disabled we apply that error to the health. Otherwise apply it to the shield
		if(0 == absorptionRate || 0 == target.Points.LifeCurrent.Shield) {
			sprintf_s(messages.Aux, "%u damage error will be applied to the health.", errorDamage);
			passthroughDamage += errorDamage;
		}
		else {
			sprintf_s(messages.Aux, "%u damage error will be applied to the shield.", errorDamage);
			shieldedDamage += errorDamage;
		}
		messages.LogAuxMessage();
	}

	int finalPassthroughDamage = 0;

	// if damage has been inflicted to the shield, apply the damage and redirect the difference to the health if the damage to the shield was higher than what it could resist.
	if(shieldedDamage) {
		int remainingShield = target.Points.LifeCurrent.Shield-shieldedDamage;
		if(target.Points.LifeCurrent.Shield) {
			int32_t maxShieldedDamage = (target.Points.LifeCurrent.Shield < shieldedDamage) ? target.Points.LifeCurrent.Shield : shieldedDamage;
			sprintf_s(messages.Aux, "%s's shield absorbs %u damage from %s.", target.Name.begin(), maxShieldedDamage, ::gpk::toString(sourceName).begin());
			messages.LogAuxMessage();
			target.Points.LifeCurrent.Shield -= maxShieldedDamage;
			if(remainingShield < 0)
				sprintf_s(messages.Aux, "%s's shield ran out allowing some damage from %s to pass through.", target.Name.begin(), ::gpk::toString(sourceName).begin());
			else
				sprintf_s(messages.Aux, "%s's remaining shield is now %u.", target.Name.begin(), target.Points.LifeCurrent.Shield);
			messages.LogAuxMessage();
		}

		if(remainingShield < 0)	{// only apply damage to health if the shield didn't absorb all the damage.
			finalPassthroughDamage	= remainingShield*-1;
			sprintf_s(messages.Aux, "%s's was hurt by %u shieldable damage from %s for which there was no protection.", target.Name.begin(), finalPassthroughDamage, ::gpk::toString(sourceName).begin());
			messages.LogAuxMessage();
			target.Points.LifeCurrent.Health += remainingShield;
		}
	}

	if(passthroughDamage) {
		sprintf_s(messages.Aux, "%s receives %u passthrough damage from %s.", target.Name.begin(), passthroughDamage, ::gpk::toString(sourceName).begin());
		messages.LogAuxMessage();
	}
	finalPassthroughDamage	+= passthroughDamage;

	target.Points.LifeCurrent.Health -= passthroughDamage;

	target.Recalculate(tables);

	return { finalPassthroughDamage, 0, damageDealt-finalPassthroughDamage };
}


COMBAT_STATUS					klib::applyAttackStatus			(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& target, COMBAT_STATUS weaponStatus, int32_t absorbChance, int32_t turnCount, const ::gpk::view_const_char & sourceName)		{
	if(COMBAT_STATUS_NONE == weaponStatus || 0 >= target.Points.LifeCurrent.Health)
		return COMBAT_STATUS_NONE;

	const ::gpk::array_pod<char_t>		targetArmorName					= ::klib::getEntityName(tables.Armor, target.CurrentEquip.Armor);
	COMBAT_STATUS						appliedStatus					= COMBAT_STATUS_NONE;

	//target.Recalculate();
	const SEntityFlags					& targetFinalFlags				= target.FinalFlags;
	if((targetFinalFlags.Effect.Defend & DEFEND_EFFECT_IMPENETRABLE) && target.Points.LifeCurrent.Shield) {
		absorbChance					= (absorbChance > 30) ? absorbChance * 2 : 60;
		sprintf_s(messages.Aux, "%s absorb chance of status by %s is modified to %%%u because of the impenetrable property of %s.", target.Name.begin(), ::gpk::toString(sourceName).begin(), absorbChance, targetArmorName.begin());
		messages.LogAuxMessage();
	}

	for(int i = 0, statusCount = target.ActiveBonus.Status.MaxStatus; i < statusCount; ++i) {
		COMBAT_STATUS						bitStatus						= (COMBAT_STATUS)(1 << i);
		if(0 == (bitStatus & weaponStatus))
			continue;

		::gpk::view_const_char				statusLabel						= ::gpk::get_value_label(bitStatus);

		if(bitStatus & targetFinalFlags.Status.Immunity) {
			sprintf_s(messages.Aux, "%s is immune to %s!", target.Name.begin(), statusLabel.begin());
			messages.LogAuxMessage();
			continue;
		}

		if((rand()%100) < absorbChance) {
			sprintf_s(messages.Aux, "%s absorbs \"%s\" inflicted by %s with %%%u absorb chance.", targetArmorName.begin(), statusLabel.begin(), ::gpk::toString(sourceName).begin(), absorbChance);
			messages.LogAuxMessage();
			continue;
		}

		addStatus(target.ActiveBonus.Status, bitStatus, turnCount);
		target.Recalculate(tables);
		appliedStatus = (COMBAT_STATUS)(appliedStatus|bitStatus);

		sprintf_s(messages.Aux, "%s got inflicted \"%s\" status from %s that will last for the next %u turns.", target.Name.begin(), statusLabel.begin(), ::gpk::toString(sourceName).begin(), turnCount);
		messages.LogAuxMessage();
	}

	return appliedStatus;
}

COMBAT_STATUS				klib::applyAttackStatus			(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& target, COMBAT_STATUS weaponStatus, int32_t turnCount, const ::gpk::view_const_char & sourceName) {
	if(COMBAT_STATUS_NONE == weaponStatus || 0 >= target.Points.LifeCurrent.Health)
		return COMBAT_STATUS_NONE;

	const int32_t					targetArmorAbsorption			= getArmorAbsorption(tables.Armor, target.CurrentEquip.Armor);
	const ::gpk::array_pod<char_t>	targetArmorName					= getEntityName(tables.Armor, target.CurrentEquip.Armor);
	//target.Recalculate();
	const SEntityPoints				& targetFinalPoints				= target.FinalPoints;
	const int32_t					targetArmorShield				= targetFinalPoints.LifeMax.Shield;

	const double					absorptionRatio					= ::gpk::max(0.0, (target.Points.LifeCurrent.Shield/(double)targetArmorShield))/2.0;
	int32_t							absorbChance					= 50 + (int32_t)(absorptionRatio*targetArmorAbsorption);
	absorbChance				= ::gpk::min(absorbChance, 100);

	sprintf_s(messages.Aux, "%s status absorb chance after absorption calculation is %%%u.", target.Name.begin(), absorbChance);
	messages.LogAuxMessage();
	const COMBAT_STATUS				result							= applyAttackStatus(tables, messages, target, weaponStatus, absorbChance, turnCount, sourceName);
	return result;
}

int32_t						klib::applyArmorReflect			(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker, CCharacter& targetReflecting, int32_t damageDealt, const ::gpk::view_const_char & sourceName) {
	const SEntityFlags				& targetFinalFlags				= targetReflecting.FinalFlags	;
	if( 0 == damageDealt
	 ||	0 == (targetFinalFlags.Effect.Defend & DEFEND_EFFECT_REFLECT)
	 ||	0 >= targetReflecting.Points.LifeCurrent.Shield
	 ||	0 >= attacker.Points.LifeCurrent.Health
	 ||	0 >= targetReflecting.Points.LifeCurrent.Health
	 )
		return 0;

	const ::gpk::array_pod<char_t>			targetArmorName			= getEntityName(tables.Armor, targetReflecting.CurrentEquip.Armor);

	if(damageDealt > 0)
		sprintf_s(messages.Aux, "%s reflects %i damage from %s with %s.", targetReflecting.Name.begin(), damageDealt, ::gpk::toString(sourceName).begin(), targetArmorName.begin());
	else if(damageDealt < 0)
		sprintf_s(messages.Aux, "%s reflects %i health from %s with %s.", targetReflecting.Name.begin(), damageDealt, ::gpk::toString(sourceName).begin(), targetArmorName.begin());
	messages.LogAuxMessage();

	::klib::SLifePoints						finalDamage				= applyShieldableDamage(tables, messages, attacker, damageDealt, {targetArmorName.begin(), (uint32_t)targetArmorName.size()});
	// If the attacker was killed by the reflect we need to avoid reflecting from her armor.
	if(finalDamage.Shield && 0 < attacker.Points.LifeCurrent.Health) {
		//attacker.Recalculate();
		const SEntityFlags&						attackerFinalFlags		= attacker.FinalFlags;
		DEFEND_EFFECT							attackerArmorEffect		= attackerFinalFlags.Effect.Defend;
		if((attackerArmorEffect & DEFEND_EFFECT_REFLECT) && attacker.Points.LifeCurrent.Shield) {
			const ::gpk::array_pod<char_t>			attackerArmorName		= getEntityName(tables.Armor, attacker.CurrentEquip.Armor);
			sprintf_s(messages.Aux, "%s causes a recursive reflection with %s dealing %u damage.", attackerArmorName.begin(), targetArmorName.begin(), damageDealt);
			messages.LogAuxMessage();
			applyArmorReflect(tables, messages, targetReflecting, attacker, finalDamage.Shield, {targetArmorName.begin(), (uint32_t)targetArmorName.size()});
		}
	}
	return finalDamage.Health;
}

static	_Check_return_ int64_t				applyWeaponLeech			(::klib::SGameMessages & messages, ATTACK_EFFECT testEffectBit, ATTACK_EFFECT attackerWeaponEffect, int32_t finalPassthroughDamage, int32_t maxPoints, int64_t currentPoints, const ::gpk::view_const_char& attackerName, const ::gpk::view_const_char& targetName, const ::gpk::view_const_char& attackerWeaponName, const ::gpk::view_const_char& pointName, const ::gpk::view_const_char& gainVerb, const ::gpk::view_const_char& loseVerb ) {
	if(attackerWeaponEffect & testEffectBit) {
		int64_t						actualHPGained				= ::gpk::min((int64_t)finalPassthroughDamage, ::gpk::max(0LL, maxPoints - ::gpk::max(0LL, currentPoints)));
		if(actualHPGained > 0)
			sprintf_s(messages.Aux, "%s %s %lli %s from %s with %s.", attackerName.begin(), gainVerb.begin(), actualHPGained, pointName.begin(), targetName.begin(), ::gpk::toString(attackerWeaponName).begin());
		else if(actualHPGained < 0)
			sprintf_s(messages.Aux, "%s %s %lli %s to %s with %s.", attackerName.begin(), loseVerb.begin(),   actualHPGained, pointName.begin(), targetName.begin(), ::gpk::toString(attackerWeaponName).begin());
		messages.LogAuxMessage();
		currentPoints			+= actualHPGained;
	}
	return currentPoints;
}

SLifePoints									applyUnshieldableDamage		(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker, CCharacter& target, const SLifePoints& damageDealt) {
	const SLifePoints								maxPossibleDamage			=
	{	(0 > target.Points.LifeCurrent.Health	) ? 0 : target.Points.LifeCurrent.Health
	,	(0 > target.Points.LifeCurrent.Mana		) ? 0 : target.Points.LifeCurrent.Mana
	,	(0 > target.Points.LifeCurrent.Shield	) ? 0 : target.Points.LifeCurrent.Shield
	};

	const SLifePoints finalDamage =
	{	(	maxPossibleDamage.Health	< damageDealt.Health	) ? maxPossibleDamage.Health	: damageDealt.Health
	,	(	maxPossibleDamage.Mana		< damageDealt.Mana		) ? maxPossibleDamage.Mana		: damageDealt.Mana
	,	(	maxPossibleDamage.Shield	< damageDealt.Shield	) ? maxPossibleDamage.Shield	: damageDealt.Shield
	};


	if( finalDamage.Health	) { sprintf_s(messages.Aux, "%s does %i direct damage to %s's %s.", attacker.Name.begin(), finalDamage.Health	, target.Name.begin(), "Health"	); messages.LogAuxMessage(); }
	if( finalDamage.Mana	) { sprintf_s(messages.Aux, "%s does %i direct damage to %s's %s.", attacker.Name.begin(), finalDamage.Mana	, target.Name.begin(), "Mana"	); messages.LogAuxMessage(); }
	if( finalDamage.Shield	) { sprintf_s(messages.Aux, "%s does %i direct damage to %s's %s.", attacker.Name.begin(), finalDamage.Shield	, target.Name.begin(), "Shield"	); messages.LogAuxMessage(); }

	target.Points.LifeCurrent.Health	-= finalDamage.Health	;
	target.Points.LifeCurrent.Mana		-= finalDamage.Mana		;
	target.Points.LifeCurrent.Shield	-= finalDamage.Shield	;
	//attacker	.Recalculate();
	target		.Recalculate(tables);
	return finalDamage;
}

::klib::SLifePoints klib::applySuccessfulHit(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker, ::klib::CCharacter& target, int32_t damage, bool bAddStatus, ::klib::COMBAT_STATUS grenadeStatus, int32_t statusTurns, const ::gpk::view_const_char& sourceName) {
	SLifePoints finalDamage = ::klib::applyShieldableDamage(tables, messages, target, damage, sourceName);
	klib::applyArmorReflect(tables, messages, attacker, target, finalDamage.Shield, sourceName);

	attacker.Recalculate(tables);
	const SLifePoints& directDamage = attacker.FinalPoints.Attack.DirectDamage;
	applyUnshieldableDamage(tables, messages, attacker, target, directDamage);

	// Clear sleep on successful hit.
	if(finalDamage.Health || finalDamage.Shield || finalDamage.Mana) {
		for(uint32_t i=0, statusCount = attacker.ActiveBonus.Status.MaxStatus; i < statusCount; ++i) {
			COMBAT_STATUS statusBit = (COMBAT_STATUS)(1<<i);
			if( target.ActiveBonus.Status.Status & statusBit & COMBAT_STATUS_SLEEP ) {
				if(target.Points.LifeCurrent.Health < 0)
					sprintf_s(messages.Aux, "Sweet Dreams, %s!", target.Name.begin());
				else
					sprintf_s(messages.Aux, "%s awakes from his induced nap!", target.Name.begin());
				messages.LogAuxMessage();
				target.ActiveBonus.Status.Status = (COMBAT_STATUS)(target.ActiveBonus.Status.Status & ~statusBit);
				target.ActiveBonus.Status.TurnsLeft[i]	= 0;
				target.Recalculate(tables);

			}
		}
	}

	if(bAddStatus)
		applyAttackStatus(tables, messages, target, grenadeStatus, statusTurns, sourceName);

	return finalDamage;
}


void klib::applyWeaponLeechEffects(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker, CCharacter& targetReflecting, const SLifePoints& finalDamage, const ::gpk::view_const_char& sourceName) {
	SEntityPoints							& attackerPoints			= attacker.FinalPoints;
	SEntityFlags							& attackerFlags			= attacker.FinalFlags;
	attacker.Points.LifeCurrent.Health	= (int32_t)applyWeaponLeech(messages, ATTACK_EFFECT_LEECH_HEALTH	, attackerFlags.Effect.Attack, finalDamage.Health	, attackerPoints.LifeMax.Health	, attacker.Points.LifeCurrent.Health	, attacker.Name	, targetReflecting.Name	, sourceName, ::gpk::view_const_string{"Health"	}, ::gpk::view_const_string{"drains"}, ::gpk::view_const_string{"loses"});
	attacker.Points.LifeCurrent.Mana	= (int32_t)applyWeaponLeech(messages, ATTACK_EFFECT_LEECH_MANA		, attackerFlags.Effect.Attack, finalDamage.Mana		, attackerPoints.LifeMax.Mana	, attacker.Points.LifeCurrent.Mana		, attacker.Name	, targetReflecting.Name	, sourceName, ::gpk::view_const_string{"Mana"	}, ::gpk::view_const_string{"drains"}, ::gpk::view_const_string{"loses"});
	attacker.Points.LifeCurrent.Shield	= (int32_t)applyWeaponLeech(messages, ATTACK_EFFECT_LEECH_SHIELD	, attackerFlags.Effect.Attack, finalDamage.Shield	, attackerPoints.LifeMax.Shield	, attacker.Points.LifeCurrent.Shield	, attacker.Name	, targetReflecting.Name	, sourceName, ::gpk::view_const_string{"Shield"	}, ::gpk::view_const_string{"steals"}, ::gpk::view_const_string{"gives"});
	attacker.Points.Coins				= applyWeaponLeech(messages, ATTACK_EFFECT_STEAL					, attackerFlags.Effect.Attack, finalDamage.Health+finalDamage.Shield+finalDamage.Mana, 0x7FFFFFFF, attacker.Points.Coins	, attacker.Name	, targetReflecting.Name	, sourceName, ::gpk::view_const_string{"Coins"	}, ::gpk::view_const_string{"steals"}, ::gpk::view_const_string{"drops"});
	attacker			.Recalculate(tables);
	targetReflecting	.Recalculate(tables);
}

void klib::applySuccessfulWeaponHit(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker, CCharacter& targetReflecting, int32_t damageDealt, const ::gpk::view_const_char& sourceName) {
	if(targetReflecting.FinalFlags.Effect.Defend & DEFEND_EFFECT_BLIND)	{
		const ::gpk::array_pod<char_t>						armorName						= getEntityName(tables.Armor, targetReflecting.CurrentEquip.Armor);
		applyAttackStatus(tables, messages, attacker, COMBAT_STATUS_BLIND, 1, {armorName.begin(), (uint32_t)armorName.size()});
	}

	if( 0 == damageDealt )
		return;

	const	SEntityFlags	&	attackerFlags	= attacker.FinalFlags	;
	const	SLifePoints			finalDamage 	= applySuccessfulHit(tables, messages, attacker, targetReflecting, damageDealt, attackerFlags.Status.Inflict != COMBAT_STATUS_NONE, attackerFlags.Status.Inflict, 1, sourceName);

	// Apply combat bonuses from weapon for successful hits.
	const SEntityPoints attackerWeaponPoints = getWeaponPoints(tables, attacker.CurrentEquip.Weapon);
	applyCombatBonus(tables, messages, attacker, attackerWeaponPoints, sourceName);

	// Apply weapon effects for successful hits.
	applyWeaponLeechEffects(tables, messages, attacker, targetReflecting, finalDamage, sourceName);
}

// This function returns the damage dealt to the target
bool klib::attack(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker, CCharacter& target) {
	// Calculate success from the hit chance and apply damage to target or just print the miss message.
	int32_t									damageDealt				= 0;

	const ::gpk::array_pod<char_t>			attackerWeaponName		= ::klib::getEntityName(tables.Weapon, attacker.CurrentEquip.Weapon);
	::klib::SEntityPoints					& attackerPoints		= attacker.FinalPoints;

	bool bIsBlind = true_if(attacker.ActiveBonus.Status.Status & COMBAT_STATUS_BLIND);

	if(true_if(attacker.ActiveBonus.Status.Status & COMBAT_STATUS_BLACKOUT) && (getWeaponFlags(tables, attacker.CurrentEquip.Weapon).Tech.Tech & ENTITY_TECHNOLOGY_DIGITAL)) {
		sprintf_s(messages.Aux, "This weapon was disabled by an electromagnetic pulse.");
		messages.LogAuxMessage();
		return false;
	}

	int finalChance = attackerPoints.Attack.Hit;
	if(bIsBlind) {
		sprintf_s(messages.Aux, "Blindness causes %s to have %u hit chance for this turn.", attacker.Name.begin(), attackerPoints.Attack.Hit >>= 1);
		messages.LogAuxMessage();
	}

	if(true_if(target.ActiveBonus.Status.Status & COMBAT_STATUS_STUN)) {
		sprintf_s(messages.Aux, "As %s is stunned, %s gains %u hit chance for this turn.", target.Name.begin(), attacker.Name.begin(), attackerPoints.Attack.Hit>>1);
		messages.LogAuxMessage();
		finalChance	+= attackerPoints.Attack.Hit>>1;
	}
	else if(true_if(target.ActiveBonus.Status.Status & COMBAT_STATUS_SLEEP)) {
		sprintf_s(messages.Aux, "As %s is asleep, %s gains %u hit chance for this turn.", target.Name.begin(), attacker.Name.begin(), attackerPoints.Attack.Hit/3);
		messages.LogAuxMessage();
		finalChance	+= attackerPoints.Attack.Hit/3;
	}
	else if(true_if(target.ActiveBonus.Status.Status & COMBAT_STATUS_BLIND)) {
		sprintf_s(messages.Aux, "As %s is blind, %s gains %u hit chance for this turn.", target.Name.begin(), attacker.Name.begin(), attackerPoints.Attack.Hit>>2);
		messages.LogAuxMessage();
		finalChance	+= attackerPoints.Attack.Hit>>2;
	}

	if ((rand() % 100) < finalChance ) {
		damageDealt = attackerPoints.Attack.Damage+(rand()%(attackerPoints.Attack.Damage/10+1));
		sprintf_s(messages.Aux, "%s hits %s for: %u.", attacker.Name.begin(), target.Name.begin(), damageDealt);
		applySuccessfulWeaponHit(tables, messages, attacker, target, damageDealt, {attackerWeaponName.begin(), (uint32_t)attackerWeaponName.size()});
	}
	else {
		sprintf_s(messages.Aux, "%s misses the attack!", attacker.Name.begin());
	}
	messages.LogAuxMessage();

	if(damageDealt) {
		attacker	.Score.DamageDealt += damageDealt;
		attacker	.Score.AttacksHit++;
		target		.Score.DamageTaken += damageDealt;
		target		.Score.AttacksReceived++;
		attacker	.Recalculate(tables);
		target		.Recalculate(tables);
	}
	else  {
		attacker	.Score.AttacksMissed++;
		target		.Score.AttacksAvoided++;
	}

	return true;
};

static	void	applyTurnBonus		(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character, int32_t& characterCurrentPoint, const int32_t characterMaxPoint, const int32_t combatBonus, const ::gpk::view_const_char& characterName, const ::gpk::view_const_char& pointName, const ::gpk::view_const_char & sourceName) {
	if(combatBonus > 0 && characterCurrentPoint < characterMaxPoint) {
		sprintf_s(messages.Aux, "%s gains %u %s from %s.", characterName.begin(), combatBonus, pointName.begin(), ::gpk::toString(sourceName).begin());
		messages.LogAuxMessage();
		characterCurrentPoint	+= combatBonus;
		character.Recalculate(tables);
	}
	else if(combatBonus < 0 && characterCurrentPoint) {
		sprintf_s(messages.Aux, "%s loses %u %s from %s.", characterName.begin(), combatBonus, pointName.begin(), ::gpk::toString(sourceName).begin());
		messages.LogAuxMessage();
		characterCurrentPoint	-= combatBonus;
		character.Recalculate(tables);
	}
}

void	klib::applyCombatBonus(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character, const SEntityPoints& combatBonus, const ::gpk::view_const_char& sourceName) {
	if(0 >= character.Points.LifeCurrent.Health)	// This character is already dead
		return;

	const SEntityPoints						& characterPoints		= character.FinalPoints;
	const SLifePoints						& lifeMax				= characterPoints.LifeMax;
	SLifePoints								& lifeCurrent			= character.Points.LifeCurrent;

	int finalHPAdded = 0;

	finalHPAdded = ::gpk::min(combatBonus.LifeCurrent.Health	, ::gpk::max(0, lifeMax.Health	-lifeCurrent.Health	));	applyTurnBonus(tables, messages, character, lifeCurrent.Health	, lifeMax.Health	, finalHPAdded, character.Name, "Health"	, sourceName);
	finalHPAdded = ::gpk::min(combatBonus.LifeCurrent.Mana		, ::gpk::max(0, lifeMax.Mana	-lifeCurrent.Mana	));	applyTurnBonus(tables, messages, character, lifeCurrent.Mana		, lifeMax.Mana		, finalHPAdded, character.Name, "Mana"		, sourceName);
	finalHPAdded = ::gpk::min(combatBonus.LifeCurrent.Shield	, ::gpk::max(0, lifeMax.Shield	-lifeCurrent.Shield	));	applyTurnBonus(tables, messages, character, lifeCurrent.Shield	, lifeMax.Shield	, finalHPAdded, character.Name, "Shield"	, sourceName);

	if(combatBonus.Coins) {
		if(combatBonus.Coins > 0) {
			sprintf_s(messages.Aux, "%s gains %llu Coins from %s.", character.Name.begin(), combatBonus.Coins, ::gpk::toString(sourceName).begin());
			messages.LogAuxMessage();
		}
		else if(combatBonus.Coins < 0 && character.Points.Coins) {
			sprintf_s(messages.Aux, "%s loses %llu Coins from %s.", character.Name.begin(), combatBonus.Coins*-1, ::gpk::toString(sourceName).begin());
			messages.LogAuxMessage();
		}
		character.Points.Coins	+= combatBonus.Coins;

		character.Recalculate(tables);
	}
};

void						applyRegenBonus				(::klib::SGameMessages & messages, PASSIVE_EFFECT testEffectBit, PASSIVE_EFFECT characterActiveEffects, int32_t maxPoints, int32_t& characterCurrentPoints, const ::gpk::view_const_char& pointName, const ::gpk::view_const_char& armorName) {
	if((testEffectBit & characterActiveEffects) && (characterCurrentPoints < maxPoints)) {
		int32_t						pointsToAdd						= maxPoints / 20;
		pointsToAdd				= ::gpk::max(1, ::gpk::min(pointsToAdd, maxPoints-characterCurrentPoints));
		characterCurrentPoints	+= pointsToAdd;
		sprintf_s(messages.Aux, "%s regenerates %s by %i.", armorName.begin(), pointName.begin(), pointsToAdd);
		messages.LogAuxMessage();
		//getchar();
	}
}


void						applyPassive					(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character, PASSIVE_EFFECT equipmentEffects, const ::gpk::view_const_char& sourceName) {
	const SEntityPoints				& characterFinalPoints			= character.FinalPoints;

	applyRegenBonus(messages, PASSIVE_EFFECT_LIFE_REGEN		,	equipmentEffects,	characterFinalPoints.LifeMax.Health	,	character.Points.LifeCurrent.Health	, "Health"	, sourceName);
	applyRegenBonus(messages, PASSIVE_EFFECT_MANA_REGEN		,	equipmentEffects,	characterFinalPoints.LifeMax.Mana	,	character.Points.LifeCurrent.Mana	, "Mana"	, sourceName);
	applyRegenBonus(messages, PASSIVE_EFFECT_SHIELD_REPAIR	,	equipmentEffects,	characterFinalPoints.LifeMax.Shield	,	character.Points.LifeCurrent.Shield	, "Shield"	, sourceName);
	character.Recalculate(tables);
}

void						klib::applyTurnStatus			(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character) {
	if(0 >= character.Points.LifeCurrent.Health)	// This character is already dead
		return;

	int								amount							= 0;
	const SEntityPoints&	finalPoints	= character.FinalPoints;
	for(uint32_t i=0, statusCount = character.ActiveBonus.Status.MaxStatus; i<statusCount; ++i) {
		COMBAT_STATUS statusBit = (COMBAT_STATUS)(1 << i);
		if(0 == (statusBit & character.ActiveBonus.Status.Status))
			continue;

		switch(statusBit) {
		case COMBAT_STATUS_BLEEDING	:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, 0, ::gpk::view_const_string{"bleeding" });																; break;	//getchar(); break;
		case COMBAT_STATUS_POISON	:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, 0, ::gpk::view_const_string{"poisoning"});																; break;	//getchar(); break;
		case COMBAT_STATUS_BURN		:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, getArmorAbsorption(tables.Armor, character.CurrentEquip.Armor), ::gpk::view_const_string{"burning"	});	; break;	//getchar(); break;
		case COMBAT_STATUS_FREEZING	:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, getArmorAbsorption(tables.Armor, character.CurrentEquip.Armor), ::gpk::view_const_string{"freezing"	});	; break;	//getchar(); break;
		case COMBAT_STATUS_SHOCK	:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, getArmorAbsorption(tables.Armor, character.CurrentEquip.Armor), ::gpk::view_const_string{"shocked"	});	; break;	//getchar(); break;
		//case COMBAT_STATUS_STUN	:		break;
		//case COMBAT_STATUS_BLIND	:	break;
		}
	}
}

void klib::applyTurnStatusAndBonusesAndSkipTurn(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character) {
	if(0 >= character.Points.LifeCurrent.Health)	// This character is already dead
		return;

	//printf("");
	applyTurnStatus		(tables, messages, character);																																		if(0 >= character.Points.LifeCurrent.Health) return;
	applyCombatBonus	(tables, messages, character, character.ActiveBonus.Points.Points, "Turn Combat Bonus");																				if(0 >= character.Points.LifeCurrent.Health) return;
	::gpk::array_pod<char_t> nameProfession			= ::klib::getEntityName		(tables.Profession, character.CurrentEquip.Profession)	; applyCombatBonus(tables, messages, character, getProfessionPoints	(tables, character.CurrentEquip.Profession	), {nameProfession	.begin(), (uint32_t)nameProfession.size()});	if(0 >= character.Points.LifeCurrent.Health) return;
	::gpk::array_pod<char_t> nameArmor				= ::klib::getEntityName		(tables.Armor, character.CurrentEquip.Armor)			; applyCombatBonus(tables, messages, character, getArmorPoints		(tables, character.CurrentEquip.Armor		), {nameArmor		.begin(), (uint32_t)nameArmor		.size()});	if(0 >= character.Points.LifeCurrent.Health) return;
	::gpk::array_pod<char_t> nameAccessory			= ::klib::getEntityName		(tables.Accessory, character.CurrentEquip.Accessory)	; applyCombatBonus(tables, messages, character, getAccessoryPoints	(tables, character.CurrentEquip.Accessory	), {nameAccessory	.begin(), (uint32_t)nameAccessory	.size()});	if(0 >= character.Points.LifeCurrent.Health) return;
	//applyCombatBonus	(tables, character, getVehiclePoints	(tables, character.CurrentEquip.Vehicle		), getVehicleName	(character.CurrentEquip.Vehicle));							if(0 >= character.Points.LifeCurrent.Health) return;
	::gpk::array_pod<char_t>
	nameWeapon				= ::klib::getEntityName(tables.Weapon		, character.CurrentEquip.Weapon		); applyPassive(tables, messages, character, getWeaponFlags		(tables, character.CurrentEquip.Weapon		).Effect.Passive, nameWeapon		); if(0 >= character.Points.LifeCurrent.Health) return;
	nameProfession			= ::klib::getEntityName(tables.Profession	, character.CurrentEquip.Profession	); applyPassive(tables, messages, character, getProfessionFlags	(tables, character.CurrentEquip.Profession	).Effect.Passive, nameProfession	); if(0 >= character.Points.LifeCurrent.Health) return;
	nameArmor				= ::klib::getEntityName(tables.Armor		, character.CurrentEquip.Armor		); applyPassive(tables, messages, character, getArmorFlags		(tables, character.CurrentEquip.Armor		).Effect.Passive, nameArmor			); if(0 >= character.Points.LifeCurrent.Health) return;
	nameAccessory			= ::klib::getEntityName(tables.Accessory	, character.CurrentEquip.Accessory	); applyPassive(tables, messages, character, getAccessoryFlags	(tables, character.CurrentEquip.Accessory	).Effect.Passive, nameAccessory		); if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive		(tables, character, getVehicleFlags		(tables, character.CurrentEquip.Vehicle		).Effect.Passive, ::klib::getEntityName(tables.Vehicle		, character.CurrentEquip.Vehicle	)); if(0 >= character.Points.LifeCurrent.Health) return;

	character.ActiveBonus.Points	.NextTurn();
	character.Recalculate(tables);
	character.ActiveBonus.Status	.NextTurn();
	character.Recalculate(tables);
	//printf("");
}

void klib::applyEnemyTurnStatus(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character) {
	if(0 >= character.Points.LifeCurrent.Health)	// This character is already dead
		return;

	int amount=0;
	const SEntityPoints&	finalPoints	= character.FinalPoints;
	for(uint32_t i=0, statusCount = character.ActiveBonus.Status.MaxStatus; i<statusCount; ++i) {
		COMBAT_STATUS statusBit = (COMBAT_STATUS)(1 << i);
		if(0 == (statusBit & character.ActiveBonus.Status.Status))
			continue;

		switch(statusBit) {
		case COMBAT_STATUS_BLEEDING	:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, 0, "bleeding");													; break;	//getchar(); break;
		case COMBAT_STATUS_POISON	:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, 0, "poisoning");													; break;	//getchar(); break;
		case COMBAT_STATUS_BURN		:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, getArmorAbsorption(tables.Armor, character.CurrentEquip.Armor), "burning");		; break;	//getchar(); break;
		case COMBAT_STATUS_FREEZING	:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, getArmorAbsorption(tables.Armor, character.CurrentEquip.Armor), "freezing");	; break;	//getchar(); break;
		case COMBAT_STATUS_SHOCK	:	amount = ::gpk::max(1, finalPoints.LifeMax.Health/20); if( amount > 0 ) character.Score.DamageTaken += amount; applyShieldableDamage(tables, messages, character, amount, getArmorAbsorption(tables.Armor, character.CurrentEquip.Armor), "shocked");		; break;	//getchar(); break;
		//case COMBAT_STATUS_STUN	:		break;
		//case COMBAT_STATUS_BLIND	:	break;
		}
	}
}

void klib::applyEnemyTurnStatusAndBonusesAndSkipTurn(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character) {
	(void)messages;
	if(0 >= character.Points.LifeCurrent.Health)	// This character is already dead
		return;

	//printf("");
	//applyEnemyTurnStatus	(character);																																		if(0 >= character.Points.LifeCurrent.Health) return;
	//applyCombatBonus		(character, character.ActiveBonus.Points.Points, "Turn Combat Bonus");																				if(0 >= character.Points.LifeCurrent.Health) return;
	//applyCombatBonus		(character, getProfessionPoints	(character.CurrentEquip.Profession	), getProfessionName(character.CurrentEquip.Profession));						if(0 >= character.Points.LifeCurrent.Health) return;
	//applyCombatBonus		(character, getArmorPoints		(character.CurrentEquip.Armor		), getArmorName		(character.CurrentEquip.Armor));							if(0 >= character.Points.LifeCurrent.Health) return;
	//applyCombatBonus		(character, getAccessoryPoints	(character.CurrentEquip.Accessory	), getAccessoryName	(character.CurrentEquip.Accessory));						if(0 >= character.Points.LifeCurrent.Health) return;
	//applyCombatBonus		(character, getVehiclePoints	(character.CurrentEquip.Vehicle		), getVehicleName	(character.CurrentEquip.Vehicle));							if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive			(character, getProfessionFlags	(character.CurrentEquip.Profession	).Effect.Passive	, getProfessionName	(character.CurrentEquip.Profession	)); if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive			(character, getArmorFlags		(character.CurrentEquip.Armor		).Effect.Passive	, getArmorName		(character.CurrentEquip.Armor		)); if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive			(character, getAccessoryFlags	(character.CurrentEquip.Accessory	).Effect.Passive	, getAccessoryName	(character.CurrentEquip.Accessory	)); if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive			(character, getWeaponFlags		(character.CurrentEquip.Weapon		).Effect.Passive	, getWeaponName		(character.CurrentEquip.Weapon		)); if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive			(character, getVehicleFlags		(character.CurrentEquip.Vehicle		).Effect.Passive	, getVehicleName	(character.CurrentEquip.Vehicle		)); if(0 >= character.Points.LifeCurrent.Health) return;

	character.ActiveBonus.Points	.NextEnemyTurn();
	character.Recalculate(tables);
	character.ActiveBonus.Status	.NextEnemyTurn();
	character.Recalculate(tables);
	//printf("");
}

void klib::applyRoundStatusAndBonusesAndSkipRound(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character) {
	if(0 >= character.Points.LifeCurrent.Health)	// This character is already dead
		return;
	(void)tables, (void)messages;
	//printf("");
	//applyRoundStatus	(character);																																		if(0 >= character.Points.LifeCurrent.Health)	return;
	//applyCombatBonus	(character, character.ActiveBonus.Points.Points, "Round Combat Bonus");																				if(0 >= character.Points.LifeCurrent.Health)	return;
	//applyCombatBonus	(character, getProfessionPoints	(character.CurrentEquip.Profession	), getProfessionName(character.CurrentEquip.Profession));						if(0 >= character.Points.LifeCurrent.Health) return;
	//applyCombatBonus	(character, getArmorPoints		(character.CurrentEquip.Armor		), getArmorName		(character.CurrentEquip.Armor));							if(0 >= character.Points.LifeCurrent.Health) return;
	//applyCombatBonus	(character, getAccessoryPoints	(character.CurrentEquip.Accessory	), getAccessoryName	(character.CurrentEquip.Accessory));						if(0 >= character.Points.LifeCurrent.Health) return;
	//applyCombatBonus	(character, getVehiclePoints	(character.CurrentEquip.Vehicle		), getVehicleName	(character.CurrentEquip.Vehicle));							if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive		(character, getProfessionFlags	(character.CurrentEquip.Profession	).Effect.Passive	, getProfessionName	(character.CurrentEquip.Profession	));	if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive		(character, getArmorFlags		(character.CurrentEquip.Armor		).Effect.Passive	, getArmorName		(character.CurrentEquip.Armor		));	if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive		(character, getAccessoryFlags	(character.CurrentEquip.Accessory	).Effect.Passive	, getAccessoryName	(character.CurrentEquip.Accessory	));	if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive		(character, getWeaponFlags		(character.CurrentEquip.Weapon		).Effect.Passive	, getWeaponName		(character.CurrentEquip.Weapon		));	if(0 >= character.Points.LifeCurrent.Health) return;
	//applyPassive		(character, getVehicleFlags		(character.CurrentEquip.Vehicle		).Effect.Passive	, getVehicleName	(character.CurrentEquip.Vehicle		));	if(0 >= character.Points.LifeCurrent.Health) return;

	character.ActiveBonus.Points	.NextRound();
	character.ActiveBonus.Status	.NextRound();

	//printf("");
}

bool klib::executeItem(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, int32_t indexInventory, CCharacter& user, CCharacter& target) {

	const SItem& item = user.Goods.Inventory.Items[indexInventory].Entity;
	::gpk::array_pod<char_t>					itemName = getItemName(item);

	bool bUsedItem = false;

	sprintf_s(messages.Aux, "%s uses: %s.", user.Name.begin(), itemName.begin());
	messages.LogAuxMessage();
	switch( itemDescriptions[item.Definition].Type ) {
	case ITEM_TYPE_POTION:
		bUsedItem = usePotion(tables, messages, item, user);
		break;

	case ITEM_TYPE_GRENADE:
		bUsedItem = useGrenade(tables, messages, item, user, target);
		break;

	default:
		sprintf_s(messages.Aux, "This item type does nothing yet... But we still remove it from your inventory!");
		messages.LogAuxMessage();
	}

	if(bUsedItem)
		user.Goods.Inventory.Items.DecreaseEntity(indexInventory);

	return bUsedItem;
}
