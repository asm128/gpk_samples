#include "Character.h"

#include <string>

#ifndef __COMBAT_H__923409826342897346920__
#define __COMBAT_H__923409826342897346920__

namespace klib
{
#pragma pack(push, 1)
	::klib::SLifePoints		applyShieldableDamage						(::klib::CCharacter& target		, int32_t damageDealt						, int32_t absorptionRate																											, const ::std::string& sourceName);
	::klib::SLifePoints		applyShieldableDamage						(::klib::CCharacter& target		, int32_t damageDealt																																							, const ::std::string& sourceName);
	::klib::COMBAT_STATUS	applyAttackStatus							(::klib::CCharacter& target		, ::klib::COMBAT_STATUS weaponStatus		, int32_t absorbChance			, int32_t turnCount																					, const ::std::string& sourceName);
	::klib::COMBAT_STATUS	applyAttackStatus							(::klib::CCharacter& target		, ::klib::COMBAT_STATUS weaponStatus		, int32_t turnCount																													, const ::std::string& sourceName);
	int32_t					applyArmorReflect							(::klib::CCharacter& attacker	, ::klib::CCharacter& targetReflecting		, int32_t damageDealt																												, const ::std::string& sourceName);
	void					applyCombatBonus							(::klib::CCharacter& character	, const ::klib::SEntityPoints& combatBonus																																		, const ::std::string& sourceName);
	void					applyWeaponLeechEffects						(::klib::CCharacter& attacker	, ::klib::CCharacter& targetReflecting		, const ::klib::SLifePoints& finalDamage																							, const ::std::string& sourceName);
	::klib::SLifePoints		applySuccessfulHit							(::klib::CCharacter& thrower	, ::klib::CCharacter& target				, int32_t damage				, int32_t absorptionRate, bool bAddStatus, ::klib::COMBAT_STATUS grenadeStatus, int32_t statusTurns	, const ::std::string& sourceName);
	::klib::SLifePoints		applySuccessfulHit							(::klib::CCharacter& thrower	, ::klib::CCharacter& target				, int32_t damage				, bool bAddStatus, ::klib::COMBAT_STATUS grenadeStatus, int32_t statusTurns							, const ::std::string& sourceName);
	void					applySuccessfulWeaponHit					(::klib::CCharacter& attacker	, ::klib::CCharacter& targetReflecting		, int32_t damageDealt			, int32_t absorptionRate																			, const ::std::string& sourceName);
	void					applySuccessfulWeaponHit					(::klib::CCharacter& attacker	, ::klib::CCharacter& targetReflecting		, int32_t damageDealt																												, const ::std::string& sourceName);
	bool					attack										(::klib::CCharacter& attacker	, ::klib::CCharacter& target);
	void					applyTurnStatus								(::klib::CCharacter& character);
	void					applyTurnStatusAndBonusesAndSkipTurn		(::klib::CCharacter& character);
	void					applyEnemyTurnStatus						(::klib::CCharacter& character);
	void					applyEnemyTurnStatusAndBonusesAndSkipTurn	(::klib::CCharacter& character);
	//void					applyRoundStatus							(::klib::CCharacter& character);
	void					applyRoundStatusAndBonusesAndSkipRound		(::klib::CCharacter& character);

	bool					useGrenade									(const ::klib::SItem& itemGrenade	, ::klib::CCharacter& thrower, ::klib::CCharacter& target);
	bool					usePotion									(const ::klib::SItem& itemPotion	, ::klib::CCharacter& potionDrinker);
	bool					executeItem									(int32_t indexInventory				, ::klib::CCharacter& user, ::klib::CCharacter& target);
#pragma pack(pop)
}


#endif // __COMBAT_H__923409826342897346920__