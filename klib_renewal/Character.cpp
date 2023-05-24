#include "Character.h"
#include "CharacterTile.h"

#include "klib_draw_misc.h"

void									klib::SCharacter::RecalculateFinalPoints	(const ::klib::SEntityTables & tables)											{
	const ::klib::SEntityPoints					weaponPoints								= ::klib::getEntityPoints(tables.Weapon		, CurrentEquip.Weapon		);
	const ::klib::SEntityPoints					accessoryPoints								= ::klib::getEntityPoints(tables.Accessory	, CurrentEquip.Accessory	);
	const ::klib::SEntityPoints					armorPoints									= ::klib::getEntityPoints(tables.Armor		, CurrentEquip.Armor		);
	const ::klib::SEntityPoints					professionPoints							= ::klib::getEntityPoints(tables.Profession	, CurrentEquip.Profession	);
	const ::klib::SEntityPoints					& bonusPoints								= ActiveBonus.Points.Points;

	// Currently, SEntityPoints::Coins and SEntityPoints::LifeCurrent values of the equipment are used in a different way from the character's points so we avoid adding the character points to the result for these two.
	// The correct way of solving this would be to have different functions to calculate the points for the different actions/reactions.
	SEntityPoints								result										= bonusPoints + weaponPoints + accessoryPoints + armorPoints + professionPoints; // + facilityPoints + vehiclePoints;
	result.LifeMax							= result.LifeMax			+ Points.LifeMax;	//
	result.Attack							= result.Attack				+ Points.Attack;	//
	result.Fitness							= result.Fitness			+ Points.Fitness;	//
	result.PriceBuy							= result.PriceBuy			+ Points.PriceBuy;	//
	result.CostMaintenance					= result.CostMaintenance	+ Points.CostMaintenance;	//
	if(result.LifeMax.Health <= 0)
		result.LifeMax.Health					= 1;

	FinalPoints								= result;
}

void									klib::SCharacter::RecalculateFinalFlags		(const ::klib::SEntityTables & tables)											{
	::klib::SEntityFlags						result										= {};
	const ::klib::SEntityFlags					weaponFlags									= ::klib::getEntityFlags(tables.Weapon		, CurrentEquip.Weapon		);
	const ::klib::SEntityFlags					accessoryFlags								= ::klib::getEntityFlags(tables.Accessory	, CurrentEquip.Accessory	);
	const ::klib::SEntityFlags					armorFlags									= ::klib::getEntityFlags(tables.Armor		, CurrentEquip.Armor		);
	const ::klib::SEntityFlags					professionFlags								= ::klib::getEntityFlags(tables.Profession	, CurrentEquip.Profession	);
	const ::klib::SEntityFlags					& bonusFlags								= ActiveBonus.Points.Flags;

	result									= bonusFlags | weaponFlags | accessoryFlags | armorFlags | professionFlags | Flags; // | stagePropFlags | facilityFlags | vehicleFlags;
	FinalFlags								= result;
}

void									klib::addStatus								(SCombatStatus& characterStatus, COMBAT_STATUS statusType, int32_t turnCount)	{
	for(int i = 0, count = characterStatus.MaxStatus; i < count; ++i) {
		const COMBAT_STATUS							bitStatus									=  (COMBAT_STATUS)(1<<i);
		if(0 == (bitStatus & statusType))
			continue;

		characterStatus.TurnsLeft[i]			+= (int8_t)turnCount;
		characterStatus.Status					= (COMBAT_STATUS)(characterStatus.Status | bitStatus);
	}
}

bool									klib::isRelevantTeam						(::klib::TEAM_TYPE teamId)														{
		stacxpr	const ::klib::TEAM_TYPE			irrelevantTeams[]			= {::klib::TEAM_TYPE_CIVILIAN, ::klib::TEAM_TYPE_SPECTATOR, (::klib::TEAM_TYPE)-1};
		bool										bRelevant					= true;
		for(uint32_t i=0; i < ::gpk::size(irrelevantTeams); ++i)
			if(irrelevantTeams[i] == teamId) {
				bRelevant								= false;
				break;
			}

		return bRelevant;
	}

