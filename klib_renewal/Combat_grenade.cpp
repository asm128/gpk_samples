#include "Combat.h"
#include "Item.h"
#include "Armor.h"

using namespace klib;

enum ATTACK_TARGET
	{	ATTACK_TARGET_MISS
	,	ATTACK_TARGET_SELF
	,	ATTACK_TARGET_OTHER
	};

COMBAT_STATUS getGrenadeStatusFromProperty(PROPERTY_TYPE grenadeProperty) {
	COMBAT_STATUS result = COMBAT_STATUS_NONE;
	switch(grenadeProperty) {
	case PROPERTY_TYPE_STUN		:	result = COMBAT_STATUS_STUN		; break;
	case PROPERTY_TYPE_SMOKE	:	result = COMBAT_STATUS_BLIND	; break;
	case PROPERTY_TYPE_PIERCING	:	result = COMBAT_STATUS_BLEEDING	; break;
	case PROPERTY_TYPE_FRAG		:	result = COMBAT_STATUS_BLEEDING	; break;
	case PROPERTY_TYPE_FIRE		:	result = COMBAT_STATUS_BURN		; break;
	case PROPERTY_TYPE_POISON	:	result = COMBAT_STATUS_POISON	; break;
	case PROPERTY_TYPE_COLD		:	result = COMBAT_STATUS_FREEZING	; break;
	case PROPERTY_TYPE_FREEZE	:	result = COMBAT_STATUS_FROZEN	; break;
	case PROPERTY_TYPE_SHOCK	:	result = COMBAT_STATUS_SHOCK	; break;
	case PROPERTY_TYPE_EMP		:	result = COMBAT_STATUS_BLACKOUT	; break;
	case PROPERTY_TYPE_SLEEP	:	result = COMBAT_STATUS_SLEEP	; break;
	}
	return result;
}

bool										klib::useGrenade						(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, const SItem& itemGrenade, CCharacter& thrower, CCharacter& target)											{
	const CItem										& itemDescription						= itemDescriptions[itemGrenade.Definition];

	if(0 == itemGrenade.Level) {
		::sprintf_s(messages.Aux, "The prop grenade thrown by %s puffs in the air and quickly falls to the ground.", thrower.Name.begin());
		return true;
	}

	const int										itemGrade								= itemGrenade.Level;

	// Currently the hit chance for all the grenade types are calculated with the same formula.
	int												lotteryRange							= 60+(10*itemGrade);	// calculate hit chance from item grade
	int												lotteryResult							= ::rand()%100;

	thrower.Recalculate(tables),
	target .Recalculate(tables);

	const SEntityPoints								& finalPointsThrower					= thrower	.FinalPoints;
	const SEntityPoints								& finalPointsTarget						= target	.FinalPoints;

	int32_t											itemEffectValue							= int32_t(finalPointsTarget .LifeMax.Health*(0.2f*itemGrade));
	int32_t											itemEffectValueSelf						= int32_t(finalPointsThrower.LifeMax.Health*(0.2f*itemGrade)) >> 1;

	ATTACK_TARGET									hitTarget								= ATTACK_TARGET_MISS;
	::gpk::array_pod<char_t>						itemName								= ::klib::getItemName(itemGrenade);
	::sprintf_s(messages.Aux, "%s throws %s to %s.", thrower.Name.begin(), itemName.begin(), target.Name.begin());
	messages.LogAuxMessage();
	bool											bAddStatus								= false;

	PROPERTY_TYPE									grenadeProperty							= itemDescription.Property;
	COMBAT_STATUS									grenadeStatus							= ::getGrenadeStatusFromProperty(grenadeProperty);
	const ::gpk::array_pod<char_t>					targetArmorName							= ::klib::getEntityName(tables.Armor, target.CurrentEquip.Armor);
	switch(grenadeProperty) {
	case PROPERTY_TYPE_SMOKE		:
	case PROPERTY_TYPE_STUN			:
	case PROPERTY_TYPE_FREEZE		:
	case PROPERTY_TYPE_COLD			:
	case PROPERTY_TYPE_SLEEP		:
	case PROPERTY_TYPE_FLASHBANG	:
	case PROPERTY_TYPE_EMP			: // Apply status with fixed 50% chance
		if( lotteryResult < lotteryRange )
			::klib::applyAttackStatus(tables, messages, target, grenadeStatus, itemGrade ? 1+itemGrade : 0, itemDescription.Name);
		else {
			::sprintf_s(messages.Aux, "%s throws the grenade too far away.", thrower.Name.begin());
			messages.LogAuxMessage();
		}
	break;

	case PROPERTY_TYPE_PIERCING		:
	case PROPERTY_TYPE_FRAG			:
	case PROPERTY_TYPE_FIRE			:
	case PROPERTY_TYPE_POISON		:
	case PROPERTY_TYPE_SHOCK		:
		itemEffectValue								>>= 1;
		itemEffectValueSelf							>>= 1;
		bAddStatus									= true;

	case PROPERTY_TYPE_BLAST:
		if(lotteryResult == lotteryRange) {
			::klib::applySuccessfulHit(tables, messages, thrower, thrower, itemEffectValueSelf, bAddStatus, grenadeStatus, 1+itemGrade, itemName);

			hitTarget									= ATTACK_TARGET_SELF;
			::sprintf_s(messages.Aux, "%s throws the grenade too close and explodes near %s doing %u damage!", thrower.Name.begin(), thrower.Name.begin(), itemEffectValueSelf);
		}
		else if( lotteryResult == (lotteryRange-1) ) {
			::klib::applySuccessfulHit(tables, messages, thrower, target,		itemEffectValue		>> 1, bAddStatus, grenadeStatus, itemGrade, itemName);
			::klib::applySuccessfulHit(tables, messages, thrower, thrower,	itemEffectValueSelf	>> 1, bAddStatus, grenadeStatus, (itemGrade > 0) ? itemGrade-1 : 0, itemName);

			hitTarget									= (ATTACK_TARGET)(ATTACK_TARGET_SELF | ATTACK_TARGET_OTHER);
			::sprintf_s(messages.Aux, "%s doesn't throw the grenade far enough so %s receives %u damage but also %s receives %u damage.", thrower.Name.begin(), target.Name.begin(), itemEffectValue >> 1, thrower.Name.begin(), itemEffectValueSelf >> 1);
		}
		else if( lotteryResult < lotteryRange ) {
			::klib::applySuccessfulHit(tables, messages, thrower, target, itemEffectValue, bAddStatus, grenadeStatus, 1+itemGrade, itemName);
			hitTarget									= ATTACK_TARGET_OTHER;
			::sprintf_s(messages.Aux, "The grenade hits the target doing %u damage.", itemEffectValue);
		}
		else
			::sprintf_s(messages.Aux, "%s throws the grenade too far away.", thrower.Name.begin());
		messages.LogAuxMessage();

		if(hitTarget & ATTACK_TARGET_OTHER) {
			thrower	.Score.DamageDealt					+= itemEffectValue;
			target	.Score.DamageTaken					+= itemEffectValue;
		}

		if(hitTarget & ATTACK_TARGET_SELF)  {
			thrower	.Score.DamageDealt					+= itemEffectValueSelf;
			thrower	.Score.DamageTaken					+= itemEffectValueSelf;
		}
		break;
	default:
		::sprintf_s(messages.Aux, "Grenade type not implemented!");
		messages.LogAuxMessage();
	}

	++thrower.Score.UsedGrenades;

	return true;
}
