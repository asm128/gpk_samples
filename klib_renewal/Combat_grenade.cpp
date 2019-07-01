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

bool										klib::useGrenade						(const SItem& itemGrenade, CCharacter& thrower, CCharacter& target)											{
	const CItem										& itemDescription						= itemDescriptions[itemGrenade.Definition];

	if(0 == itemGrenade.Level) {
		::printf("The prop grenade thrown by %s puffs in the air and quickly falls to the ground.\n", thrower.Name.c_str());
		return true;
	}

	const int										itemGrade								= itemGrenade.Level; 

	// Currently the hit chance for all the grenade types are calculated with the same formula.
	int												lotteryRange							= 60+(10*itemGrade);	// calculate hit chance from item grade
	int												lotteryResult							= ::rand()%100;

	thrower.Recalculate(), 
	target .Recalculate();

	const SEntityPoints								& finalPointsThrower					= thrower	.FinalPoints; 
	const SEntityPoints								& finalPointsTarget						= target	.FinalPoints;
	const SEntityFlags								& finalFlagsThrower						= thrower	.FinalFlags;
	const SEntityFlags								& finalFlagsTarget						= target	.FinalFlags;

	int32_t											itemEffectValue							= int32_t(finalPointsTarget .LifeMax.Health*(0.2f*itemGrade));
	int32_t											itemEffectValueSelf						= int32_t(finalPointsThrower.LifeMax.Health*(0.2f*itemGrade)) >> 1;

	ATTACK_TARGET									hitTarget								= ATTACK_TARGET_MISS;

	::printf("%s throws %s to %s.\n", thrower.Name.c_str(), ::klib::getItemName(itemGrenade).c_str(), target.Name.c_str());
	bool											bAddStatus								= false;

	int32_t											targetArmorAbsorption					= ::klib::getArmorAbsorption(target.CurrentEquip.Armor), finalPassthroughDamage = 0, reflectedDamage = 0;
	DEFEND_EFFECT									attackerArmorEffect						= finalFlagsThrower.Effect.Defend;
	DEFEND_EFFECT									targetArmorEffect						= finalFlagsTarget.Effect.Defend;

	PROPERTY_TYPE									grenadeProperty							= itemDescription.Property;
	COMBAT_STATUS									grenadeStatus							= ::getGrenadeStatusFromProperty(grenadeProperty);
	const ::std::string								targetArmorName							= ::klib::getArmorName(target.CurrentEquip.Armor);
	switch(grenadeProperty) {
	case PROPERTY_TYPE_SMOKE		:
	case PROPERTY_TYPE_STUN			:
	case PROPERTY_TYPE_FREEZE		:
	case PROPERTY_TYPE_COLD			:
	case PROPERTY_TYPE_SLEEP		:
	case PROPERTY_TYPE_FLASHBANG	:
	case PROPERTY_TYPE_EMP			: // Apply status with fixed 50% chance
		if( lotteryResult < lotteryRange )
			::klib::applyAttackStatus(target, grenadeStatus, itemGrade ? 1+itemGrade : 0, itemDescription.Name);
		else
			::printf("%s throws the grenade too far away.\n", thrower.Name.c_str());
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
			::klib::applySuccessfulHit(thrower, thrower, itemEffectValueSelf, bAddStatus, grenadeStatus, 1+itemGrade, ::klib::getItemName(itemGrenade));

			hitTarget									= ATTACK_TARGET_SELF;
			::printf("%s throws the grenade too close...\n"		
				"The grenade explodes near %s doing %u damage!\n", thrower.Name.c_str(), thrower.Name.c_str(), itemEffectValueSelf);
		}
		else if( lotteryResult == (lotteryRange-1) ) {
			::klib::applySuccessfulHit(thrower, target,		itemEffectValue		>> 1, bAddStatus, grenadeStatus, itemGrade, ::klib::getItemName(itemGrenade));
			::klib::applySuccessfulHit(thrower, thrower,	itemEffectValueSelf	>> 1, bAddStatus, grenadeStatus, (itemGrade > 0) ? itemGrade-1 : 0, ::klib::getItemName(itemGrenade));

			hitTarget									= (ATTACK_TARGET)(ATTACK_TARGET_SELF | ATTACK_TARGET_OTHER);
			::printf("%s doesn't throw the grenade far enough so %s receives %u damage but also %s receives %u damage.\n", thrower.Name.c_str(), target.Name.c_str(), itemEffectValue >> 1, thrower.Name.c_str(), itemEffectValueSelf >> 1);
		}
		else if( lotteryResult < lotteryRange ) {
			::klib::applySuccessfulHit(thrower, target, itemEffectValue, bAddStatus, grenadeStatus, 1+itemGrade, ::klib::getItemName(itemGrenade));
			hitTarget									= ATTACK_TARGET_OTHER;
			::printf("The grenade hits the target doing %u damage.\n", itemEffectValue);
		}
		else
			::printf("%s throws the grenade too far away.\n", thrower.Name.c_str());

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
		::printf("Grenade type not implemented!");
	}

	++thrower.Score.GrenadesUsed;

	return true;
}
