#include "Character.h"

#ifndef __COMBAT_H__923409826342897346920__
#define __COMBAT_H__923409826342897346920__

namespace klib
{
#pragma pack(push, 1)
	::klib::SLifePoints		applyShieldableDamage						(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& target		, int32_t						damageDealt			, int32_t absorptionRate																					, const ::gpk::view_const_char& sourceName);
	::klib::SLifePoints		applyShieldableDamage						(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& target		, int32_t						damageDealt			, const ::gpk::view_const_char	& sourceName);
	::klib::COMBAT_STATUS	applyAttackStatus							(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& target		, ::klib::COMBAT_STATUS			weaponStatus		, int32_t absorbChance			, int32_t turnCount															, const ::gpk::view_const_char& sourceName);
	::klib::COMBAT_STATUS	applyAttackStatus							(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& target		, ::klib::COMBAT_STATUS			weaponStatus		, int32_t turnCount																							, const ::gpk::view_const_char& sourceName);
	int32_t					applyArmorReflect							(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker		, ::klib::CCharacter			& targetReflecting	, int32_t damageDealt																						, const ::gpk::view_const_char& sourceName);
	void					applyCombatBonus							(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character	, const ::klib::SEntityPoints	& combatBonus																													, const ::gpk::view_const_char& sourceName);
	void					applyWeaponLeechEffects						(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker		, ::klib::CCharacter			& targetReflecting	, const ::klib::SLifePoints		& finalDamage																, const ::gpk::view_const_char& sourceName);
	::klib::SLifePoints		applySuccessfulHit							(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& thrower		, ::klib::CCharacter			& target			, int32_t damage				, bool bAddStatus, ::klib::COMBAT_STATUS grenadeStatus, int32_t statusTurns	, const ::gpk::view_const_char& sourceName);
	void					applySuccessfulWeaponHit					(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker		, ::klib::CCharacter			& targetReflecting	, int32_t damageDealt																						, const ::gpk::view_const_char& sourceName);
	bool					attack										(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& attacker		, ::klib::CCharacter			& target);
	void					applyTurnStatus								(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character);
	void					applyTurnStatusAndBonusesAndSkipTurn		(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character);
	void					applyEnemyTurnStatus						(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character);
	void					applyEnemyTurnStatusAndBonusesAndSkipTurn	(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character);
	void					applyRoundStatusAndBonusesAndSkipRound		(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& character);
	bool					useGrenade									(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, const ::klib::SItem& itemGrenade		, ::klib::CCharacter& thrower, ::klib::CCharacter& target);
	bool					usePotion									(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, const ::klib::SItem& itemPotion		, ::klib::CCharacter& potionDrinker);
	bool					executeItem									(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, int32_t				indexInventory	, ::klib::CCharacter& user, ::klib::CCharacter& target);
#pragma pack(pop)
} // namespace


#endif // __COMBAT_H__923409826342897346920__
