#include "Game.h"

#include "Weapon.h"
#include "Accessory.h"
#include "Armor.h"
#include "Profession.h"
#include "Vehicle.h"
#include "Facility.h"

#include "Item.h"

#include "Enemy.h"

#include "GameMenu.h"

#include <algorithm>

bool																	useItems					(klib::CCharacter& user, klib::CCharacter& target, bool bIsAIControlled);	// While in combat, displays a list of the available items to use.

bool																	escape						(const std::string& escaperName, klib::SCharacterScore& escaperScore)									{
	printf("%s tries to escape!\n", escaperName.c_str());
	if ((rand() % 100) < 30) {
		++escaperScore.EscapesSucceeded;
		printf("%s fled from the battle!", escaperName.c_str());
		return true;
	}

	++escaperScore.EscapesFailed;
	printf("%s failed to escape!\n\n", escaperName.c_str());
	return false;
}

void																	assignDrops					(klib::CCharacter& winner, klib::CCharacter& loser, bool bIsAIControlled)								{
	printf("%s is dead!\n", loser.Name.c_str());
	int																			drop						= rand() % (std::max(1,loser.Points.Coins>>2));

	if(bIsAIControlled)
		drop																	= loser.Points.Coins-drop;

	printf("\n%s dropped %u coins!!\n", loser.Name.c_str(), drop);
	winner	.Points.Coins													+= drop;
	loser	.Points.Coins													-= drop;
	for(uint32_t i=0; i<loser.Goods.Inventory.Items.Count; i++) 
		if( 0 == (rand()%2) ) {
			const ::klib::SEntitySlot<klib::SItem>										& itemDrop					= loser.Goods.Inventory.Items[i];
			::std::string																itemDropName				= klib::getItemName(itemDrop.Entity);
			if(winner.Goods.Inventory.Items.AddElement(itemDrop.Entity)) {
				printf("\n%s dropped %s!!\n", loser.Name.c_str(), itemDropName.c_str());
				loser.Goods.Inventory.Items.DecreaseEntity(i);
			}
			else {
				printf("%s can't pick up %s by %s because the inventory is full!\n", winner.Name.c_str(), itemDropName.c_str(), loser.Name.c_str());
			}
		}

	::std::string																loserWeaponName				= klib::getWeaponName(loser.CurrentEquip.Weapon);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.c_str(), loserWeaponName.c_str(), loser.CurrentEquip.Armor.Level, loser.Name.c_str());
		::klib::SWeapon																oldWinnerWeapon				= winner.CurrentEquip.Weapon;
		winner.Goods.Inventory.Weapon.AddElement(loser.CurrentEquip.Weapon);

		::klib::SWeapon																loserNewWeapon				= 
			{	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerWeapon.Definition	)	))
			,	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerWeapon.Modifier		)	))
			,	1+(rand() % std::max((int16_t)2, (int16_t)(oldWinnerWeapon.Level		)	))
			};

		if(loserNewWeapon.Definition || loserNewWeapon.Modifier || loserNewWeapon.Level > 1)
			printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.c_str(), klib::getWeaponName(loserNewWeapon).c_str(), loserNewWeapon.Level);
		loser.Goods.Inventory.Weapon.AddElement(loserNewWeapon);
		loser.CurrentEquip.Weapon												= { 0, 0, 1 };
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.c_str(), loserWeaponName.c_str(), loser.Name.c_str());

	::std::string																loserAccessoryName			= klib::getAccessoryName(loser.CurrentEquip.Accessory);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.c_str(), loserAccessoryName.c_str(), loser.CurrentEquip.Accessory.Level, loser.Name.c_str());
		::klib::SAccessory															oldWinnerAccessory			= winner.CurrentEquip.Accessory;
		winner.Goods.Inventory.Accessory.AddElement(loser.CurrentEquip.Accessory);

		::klib::SAccessory															loserNewAccessory			= 
			{	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerAccessory.Definition	)	))
			,	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerAccessory.Modifier		)	))
			,	1+(rand() % std::max((int16_t)2, (int16_t)(oldWinnerAccessory.Level			)	))
			};

		if(loserNewAccessory.Definition || loserNewAccessory.Modifier || loserNewAccessory.Level > 1)
			printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.c_str(), klib::getAccessoryName(loserNewAccessory).c_str(), loserNewAccessory.Level);
		loser.Goods.Inventory.Accessory.AddElement(loserNewAccessory);
		loser.CurrentEquip.Accessory											= {0,0,1};
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.c_str(), loserAccessoryName.c_str(), loser.Name.c_str());

	::std::string																loserArmorName				= klib::getArmorName(loser.CurrentEquip.Armor);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.c_str(), loserArmorName.c_str(), loser.CurrentEquip.Armor.Level, loser.Name.c_str());
		::klib::SArmor																oldWinnerArmor				= winner.CurrentEquip.Armor;
		winner.Goods.Inventory.Armor.AddElement(loser.CurrentEquip.Armor);

		::klib::SArmor																loserNewArmor				= 
			{	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerArmor.Definition	)))
			,	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerArmor.Modifier		)))
			,	1+(rand() % std::max((int16_t)2, (int16_t)(oldWinnerArmor.Level			)))
			};

		if(loserNewArmor.Definition || loserNewArmor.Modifier || loserNewArmor.Level > 1)
			printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.c_str(), klib::getArmorName(loserNewArmor).c_str(), loserNewArmor.Level);
		loser.Goods.Inventory.Armor.AddElement(loserNewArmor);
		loser.CurrentEquip.Armor												= {0,0,1};
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.c_str(), loserArmorName.c_str(), loser.Name.c_str());

	::std::string																loserVehicleName			= klib::getVehicleName(loser.CurrentEquip.Vehicle);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.c_str(), loserVehicleName.c_str(), loser.CurrentEquip.Vehicle.Level, loser.Name.c_str());
		::klib::SVehicle															oldWinnerVehicle			= winner.CurrentEquip.Vehicle;
		winner.Goods.Inventory.Vehicle.AddElement(loser.CurrentEquip.Vehicle);

		::klib::SVehicle															loserNewVehicle				= 
			{	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerVehicle.Definition	)))
			,	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerVehicle.Modifier	)))
			,	1+(rand() % std::max((int16_t)2, (int16_t)(oldWinnerVehicle.Level		)))
			};

		if(loserNewVehicle.Definition || loserNewVehicle.Modifier || loserNewVehicle.Level > 1)
			printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.c_str(), klib::getVehicleName(loserNewVehicle).c_str(), loserNewVehicle.Level);
		loser.Goods.Inventory.Vehicle.AddElement(loserNewVehicle);
		loser.CurrentEquip.Vehicle												= { 0, 0, 1 };
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.c_str(), loserVehicleName.c_str(), loser.Name.c_str());

	::std::string																loserFacilityName			= klib::getFacilityName(loser.CurrentEquip.Facility);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.c_str(), loserFacilityName.c_str(), loser.CurrentEquip.Facility.Level, loser.Name.c_str());
		klib::SFacility																oldWinnerFacility			= winner.CurrentEquip.Facility;
		winner.Goods.Inventory.Facility.AddElement(loser.CurrentEquip.Facility);

		klib::SFacility																loserNewFacility			= 
			{	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerFacility.Definition	)	))
			,	1+(rand() %	std::max((int16_t)2, (int16_t)(oldWinnerFacility.Modifier	)	))
			,	1+(rand() % std::max((int16_t)2, (int16_t)(oldWinnerFacility.Level		)	))
			};

		//if(loserNewFacility.Index || loserNewFacility.Modifier || loserNewFacility.Level > 1)
		//	printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.c_str(), klib::getFacilityName(loserNewFacility).c_str(), loserNewFacility.Level);
		//loser.Inventory.Facility.AddElement(loserNewFacility);
		//loser.CurrentEquip.Facility = {0,0,1};
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.c_str(), loserFacilityName.c_str(), loser.Name.c_str());

	::std::string																loserProfessionName			= klib::getProfessionName(loser.CurrentEquip.Profession);
	if(rand()%2) {
		printf("%s recovers a %s level %u from %s.\n", winner.Name.c_str(), loserProfessionName.c_str(), loser.CurrentEquip.Profession.Level, loser.Name.c_str());
		winner.Goods.Inventory.Profession.AddElement(loser.CurrentEquip.Profession);
	}
	else
		printf("%s doesn't get to recover %s from %s because it's too damaged.\n", winner.Name.c_str(), loserProfessionName.c_str(), loser.Name.c_str());

	::klib::SProfession															oldLoserProfession			= loser.CurrentEquip.Profession;

	bool																		bLowestFall					= true;
	if(loser.CurrentEquip.Profession.Modifier	)	{ bLowestFall = false; --loser.CurrentEquip.Profession.Modifier	; };
	if(loser.CurrentEquip.Profession.Level > 1	)	{ bLowestFall = false; --loser.CurrentEquip.Profession.Level	; };
	if( bLowestFall )
		printf("%s couldn't fall any lower!!\n", loser.Name.c_str());
	else
		printf("%s loses his job as %s level %u and tries to get a new job as %s level %u instead.\n", loser.Name.c_str(), loserProfessionName.c_str(), oldLoserProfession.Level, klib::getProfessionName(loser.CurrentEquip.Profession).c_str(), loser.CurrentEquip.Profession.Level);

	if(	winner.CurrentEquip.Profession.Modifier < winner.Goods.CompletedResearch.Profession.MaxResearch.Modifier ) {
		++winner.CurrentEquip.Profession.Modifier;
		printf("%s gets promoted to %s!!\n", winner.Name.c_str(), getProfessionName(winner.CurrentEquip.Profession).c_str());
	}
		
		//winner.Points.LifeMax.Health += winner.Points.Attack;
	++winner.CurrentEquip.Profession	.Level;
	++winner.CurrentEquip.Armor			.Level;
	++winner.CurrentEquip.Weapon		.Level;

	++winner.Score.BattlesWon;
	++winner.Score.EnemiesKilled;
	winner.Score.MoneyEarned												+= drop;

	++loser.Score.BattlesLost;

}

void																	determineOutcome			(klib::CCharacter& adventurer, klib::CCharacter& enemy, int32_t enemyType)								{
		// Determine the outcome of the battle and give rewards if applicable.
	if (adventurer.Points.LifeCurrent.Health <= 0) 
		assignDrops(enemy, adventurer, false);
	if (enemy.Points.LifeCurrent.Health <= 0)
		assignDrops(adventurer, enemy, true);
}

enum TURN_ACTION
	{	TURN_ACTION_ATTACK
	,	TURN_ACTION_INVENTORY
	,	TURN_ACTION_SKILL
	,	TURN_ACTION_SENSE
	,	TURN_ACTION_RUN
	,	TURN_ACTION_CANCEL
	};

enum TURN_OUTCOME
	{	TURN_OUTCOME_CONTINUE
	,	TURN_OUTCOME_ESCAPE
	,	TURN_OUTCOME_CANCEL
	};

bool																	useSkills					(::klib::CCharacter& attacker, ::klib::CCharacter& target)												{
	printf("\n");
	printf("Skills are not implemented yet.\n");
	return false;
}

void																	senseMenu					(::klib::CCharacter& enemy)																				{
	// This is the main loop of the game and queries for user input until the exit option is selected.
	static const ::klib::SMenuItem<int>											tavernOptions[]				=
		{ {  1,	"Inspect enemy weapon"		}
		, {  2,	"Inspect enemy accessory"	}
		, {  3,	"Inspect enemy armor"		}
		, {  4,	"Inspect enemy agent"		}
		, {  5,	"Inspect enemy vehicle"		}
		, {  6,	"Inspect enemy building"	}
		, {  7,	"Peek enemy inventory"		}
		, {  8,	"Back to main menu"			}
		};

	while (true) { // Wait for exit request
		int																			tavernChoice				= displayMenu("You wonder about what to do next..", tavernOptions);

		// Interpret user input.
			 if( 1 == tavernChoice )	{	displayWeapon					(enemy);									}	// 
		else if( 2 == tavernChoice )	{	displayAccessory				(enemy);									}	// 
		else if( 3 == tavernChoice )	{	displayArmor					(enemy);									}	// 
		else if( 4 == tavernChoice )	{	displayProfession				(enemy);									}	// 
		else if( 5 == tavernChoice )	{	displayVehicle					(enemy);									}	// 
		else if( 6 == tavernChoice )	{	displayFacility					(enemy);									}	// 
		else if( 7 == tavernChoice )	{	displayInventory				(enemy.Goods.Inventory.Items, enemy.Name);	}	// 
		else if( 8 == tavernChoice )	{	break;	}	// Exit the main loop, which effectively closes the game.
		else {	
			printf("Option not supported yet.\n");
		}	// Exit the main loop, which effectively closes the game.
	}
}

TURN_OUTCOME															characterTurn				(TURN_ACTION combatOption, klib::CCharacter& attacker, klib::CCharacter& target, bool bIsAIControlled)	{
	// If the action is valid then we execute it and break the current while() so the attack turn executes.
	TURN_OUTCOME																outcome						= TURN_OUTCOME_CANCEL;
	if(TURN_ACTION_ATTACK == combatOption) {
		if( !klib::attack(attacker, target) )
			outcome																	= TURN_OUTCOME_CONTINUE;
	}
	else if(TURN_ACTION_INVENTORY == combatOption) { 
		if( !useItems(attacker, target, bIsAIControlled) )
			outcome																	= TURN_OUTCOME_CONTINUE;
	}	// useItems requires to receive the current enemy as a parameter in order to modify its health if we use a grenade and hit.
	else if(TURN_ACTION_SKILL == combatOption) { 
		if( !useSkills(attacker, target) )
			outcome																	= TURN_OUTCOME_CONTINUE;
	}	// useItems requires to receive the current enemy as a parameter in order to modify its health if we use a grenade and hit.
	else if(TURN_ACTION_SENSE == combatOption) { 
		senseMenu(target);	// sense applies to target only. You can't "sense yourself".
		outcome																	= TURN_OUTCOME_CONTINUE;
	}	// 
	else if(TURN_ACTION_RUN == combatOption) {
		if( escape(attacker.Name, attacker.Score) )
			outcome																	= TURN_OUTCOME_ESCAPE; // Escape: if we succeed we just exit this combat() function, otherwise cancel this loop and execute the enemy turn.
	}
	else if(TURN_ACTION_CANCEL == combatOption) {
		printf("%s couldn't figure out what to do and skipped the turn.\n", attacker.Name.c_str());
		outcome																	= TURN_OUTCOME_CANCEL; // Escape: if we succeed we just exit this combat() function, otherwise cancel this loop and execute the enemy turn.
	}

	if(outcome == TURN_OUTCOME_CANCEL && target.Points.LifeCurrent.Health > 0 && attacker.Points.LifeCurrent.Health > 0)
		::klib::applyTurnStatusAndBonusesAndSkipTurn(attacker);

	return outcome;
}

void																	printStatuses				(const ::klib::CCharacter& character)																	{
	for(uint32_t iStatus = 0, statusCount = character.ActiveBonus.Status.MaxStatus; iStatus < statusCount; ++iStatus) {
		::klib::COMBAT_STATUS														statusBit					= (::klib::COMBAT_STATUS)(1 << iStatus);
		if(0 == (statusBit & character.ActiveBonus.Status.Status))
			continue;

		const ::gpk::label															statusLabel					= ::gpk::get_value_label(statusBit);
		printf("%s is affected by \"%s\" for the next %u turn(s).\n", character.Name.c_str(), statusLabel.begin(), character.ActiveBonus.Status.TurnsLeft[iStatus]);	
	}
}


void																	printBonuses				(const klib::CCharacter& character)																		{
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.Hit					> 0 ) printf("%s has an additional bonus for the next %u turns: %i Hit.\n"						, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.Hit					,	character.ActiveBonus.Points.Points.Attack.Hit					);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.Damage				> 0 ) printf("%s has an additional bonus for the next %u turns: %i Damage.\n"					, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.Damage				,	character.ActiveBonus.Points.Points.Attack.Damage				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Health	> 0 ) printf("%s has an additional bonus for the next %u turns: %i Health Damage.\n"			, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Health	,	character.ActiveBonus.Points.Points.Attack.DirectDamage.Health	);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Mana	> 0 ) printf("%s has an additional bonus for the next %u turns: %i Mana Damage.\n"				, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Mana	,	character.ActiveBonus.Points.Points.Attack.DirectDamage.Mana	);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Shield	> 0 ) printf("%s has an additional bonus for the next %u turns: %i Shield Damage.\n"			, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Shield	,	character.ActiveBonus.Points.Points.Attack.DirectDamage.Shield	);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.Absorption			> 0 ) printf("%s has an additional bonus for the next %u turns: %i Absorption.\n"				, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.Absorption			,	character.ActiveBonus.Points.Points.Attack.Absorption			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.Range				> 0 ) printf("%s has an additional bonus for the next %u turns: %i Range.\n"					, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.Absorption			,	character.ActiveBonus.Points.Points.Attack.Absorption			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Attack				> 0 ) printf("%s has an additional bonus for the next %u turns: %i Attack Speed.\n"				, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Attack				,	character.ActiveBonus.Points.Points.Fitness.Attack				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Movement			> 0 ) printf("%s has an additional bonus for the next %u turns: %i Movement Speed.\n"			, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Movement			,	character.ActiveBonus.Points.Points.Fitness.Movement			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Reflexes			> 0 ) printf("%s has an additional bonus for the next %u turns: %i Reflexes.\n"					, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Reflexes			,	character.ActiveBonus.Points.Points.Fitness.Reflexes			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Sight				> 0 ) printf("%s has an additional bonus for the next %u turns: %i Sight.\n"					, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Sight				,	character.ActiveBonus.Points.Points.Fitness.Sight				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Health				> 0 ) printf("%s has an additional bonus for the next %u turns: %i Max Health.\n"				, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Health				,	character.ActiveBonus.Points.Points.LifeMax.Health				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Mana				> 0 ) printf("%s has an additional bonus for the next %u turns: %i Max Mana.\n"					, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Mana				,	character.ActiveBonus.Points.Points.LifeMax.Mana				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Shield				> 0 ) printf("%s has an additional bonus for the next %u turns: %i Max Shield.\n"				, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Shield				,	character.ActiveBonus.Points.Points.LifeMax.Shield				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Health			> 0 ) printf("%s has an additional bonus for the next %u turns: %i Health Recovery.\n"			, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Health			,	character.ActiveBonus.Points.Points.LifeCurrent.Health			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Mana			> 0 ) printf("%s has an additional bonus for the next %u turns: %i Mana Recovery.\n"			, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Mana			,	character.ActiveBonus.Points.Points.LifeCurrent.Mana			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Shield			> 0 ) printf("%s has an additional bonus for the next %u turns: %i Shield Recovery.\n"			, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Shield			,	character.ActiveBonus.Points.Points.LifeCurrent.Shield			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Coins						> 0 ) printf("%s has an additional bonus for the next %u turns: %i Coin Earning.\n"				, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftPoints.Coins						,	character.ActiveBonus.Points.Points.Coins						);

	// The following messages should be improved by taking every possible status into account.
	if( character.ActiveBonus.Points.TurnsLeftFlags.Effect.Attack				> 0 ) printf("%s has additional flags for the next %u turn(s): Attack Effect   (0x%.04X).\n"	, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftFlags.Effect.Attack				,	character.ActiveBonus.Points.Flags.Effect.Attack				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Effect.Defend				> 0 ) printf("%s has additional flags for the next %u turn(s): Defend Effect   (0x%.04X).\n"	, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftFlags.Effect.Defend				,	character.ActiveBonus.Points.Flags.Effect.Defend				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Effect.Passive				> 0 ) printf("%s has additional flags for the next %u turn(s): Passive Effect  (0x%.04X).\n"	, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftFlags.Effect.Passive				,	character.ActiveBonus.Points.Flags.Effect.Passive				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Status.Immunity				> 0 ) printf("%s has additional flags for the next %u turn(s): Status Immunity (0x%.04X).\n"	, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftFlags.Status.Immunity				,	character.ActiveBonus.Points.Flags.Status.Immunity				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Status.Inflict				> 0 ) printf("%s has additional flags for the next %u turn(s): Status Inflict  (0x%.04X).\n"	, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftFlags.Status.Inflict				,	character.ActiveBonus.Points.Flags.Status.Inflict				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Tech.Tech					> 0 ) printf("%s has additional flags for the next %u turn(s): Technology      (0x%.04X).\n"	, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftFlags.Tech.Tech					,	character.ActiveBonus.Points.Flags.Tech.Tech					);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Tech.Grade					> 0 ) printf("%s has additional level for the next %u turn(s): Level           (%.05i).\n"		, character.Name.c_str(),	character.ActiveBonus.Points.TurnsLeftFlags.Tech.Grade					,	character.ActiveBonus.Points.Flags.Tech.Grade					);
}

void																	printCharacterShortInfo		(::klib::CCharacter& character)																			{
	const ::klib::SEntityPoints													& characterPoints			= character.FinalPoints;
	printf("\n----------------------- %s is a %s level %u.\nWeapon: %s level %u.\nArmor: %s level %u.\n",  
		character.Name.c_str(), 
		::klib::getProfessionName	(character.CurrentEquip.Profession)	.c_str(),	character.CurrentEquip.Profession.Level,	
		::klib::getWeaponName		(character.CurrentEquip.Weapon)		.c_str(),	character.CurrentEquip.Weapon.Level,	
		::klib::getArmorName		(character.CurrentEquip.Armor )		.c_str(),	character.CurrentEquip.Armor.Level);
		
	printf("-------------- Max points:\n");
	characterPoints.LifeMax.Print();
	printf("-------------- Current points:\n");
	character.Points.LifeCurrent.Print();
	characterPoints.Attack.Print();
}

TURN_OUTCOME															playerTurn					(::klib::CCharacter& adventurer, ::klib::CCharacter& currentEnemy)										{
	static const klib::SMenuItem<TURN_ACTION>									combatOptions[]				=
		{ { TURN_ACTION_ATTACK		, "Attack"		}
		, { TURN_ACTION_INVENTORY	, "Inventory"	}
		, { TURN_ACTION_SKILL		, "Skills"		}
		, { TURN_ACTION_SENSE		, "Sense"		}
		, { TURN_ACTION_CANCEL		, "Cancel turn"	}
		, { TURN_ACTION_RUN			, "Run"			}
		};

	TURN_OUTCOME																turnOutcome					= TURN_OUTCOME_CONTINUE;

	while (turnOutcome == TURN_OUTCOME_CONTINUE) {	// this while() process the input for this turn until the user enters a valid choice and then exits to the outer loop for executing the attack turn.
		printCharacterShortInfo	(adventurer);
		printStatuses			(adventurer);
		printBonuses			(adventurer);
		
		printCharacterShortInfo	(currentEnemy);
		printStatuses			(currentEnemy);
		printBonuses			(currentEnemy);

		const TURN_ACTION															actionChoice				= displayMenu("It's your turn to make a move", combatOptions);
		turnOutcome																= characterTurn(actionChoice, adventurer, currentEnemy, false);
	}
	return turnOutcome;
}

TURN_ACTION																resolveAI					(klib::CCharacter& enemy, klib::CCharacter& adventurer)													{
	TURN_ACTION																	action						= TURN_ACTION_ATTACK;
	if(enemy.Goods.Inventory.Items.Count)
		action																	= (rand()%2) ? action : TURN_ACTION_INVENTORY;
	else if(enemy.Points.LifeCurrent.Health <= (enemy.Points.LifeMax.Health/9) && 0 == (rand()%7))	// chance of escape attempt if health is less than 11%.
		action																	= TURN_ACTION_RUN;

	return action;
}

TURN_OUTCOME															enemyTurn					(klib::CCharacter& enemy, klib::CCharacter& adventurer)													{
	TURN_OUTCOME																turnOutcome					= TURN_OUTCOME_CONTINUE;
	while (turnOutcome == TURN_OUTCOME_CONTINUE) {	// this while() process the input for this turn until the user enters a valid choice and then exits to the outer loop for executing the attack turn.
		const TURN_ACTION															actionChoice				= (TURN_ACTION)resolveAI(enemy, adventurer);
		turnOutcome																= characterTurn(actionChoice, enemy, adventurer, true);
	}
	return turnOutcome;
}

bool																	combatContinues				(TURN_OUTCOME turnOutcome, int adventurerHP, int enemyHP)												{
	return !( TURN_OUTCOME_ESCAPE == turnOutcome || 0 >= adventurerHP || 0 >= enemyHP );
}

//5736	// gasty.bellino@gmail.com
void																	combat						(::klib::CCharacter& adventurer, int32_t enemyType)														{
	if(adventurer.Points.LifeCurrent.Health <= 1) {
		printf("You don't have enough health to engage in combat. Please go rest and come back later.\n");
		return;
	}

	klib::CCharacter															* pEnemy					= new klib::CCharacter(klib::enemyDefinitions[enemyType]);
	klib::CCharacter															& currentEnemy				= *pEnemy;	// Request the enemy data.
	klib::setupAgent(adventurer, currentEnemy);

	adventurer.ActiveBonus.Status.Clear();	// We need to clear the combat status before starting the combat.

	TURN_OUTCOME																turnOutcome					= TURN_OUTCOME_CONTINUE;
	while(combatContinues(turnOutcome, adventurer.Points.LifeCurrent.Health, currentEnemy.Points.LifeCurrent.Health)) {	// This while() executes the attack turns, requesting for user input at the beginning of each turn.
		++adventurer	.Score.TurnsPlayed;
		++currentEnemy	.Score.TurnsPlayed;

		if(adventurer.DidLoseTurn()) {
			printf("%s is stunned/asleep/frozen and loses his turn!\n", adventurer.Name.c_str());
			turnOutcome															= TURN_OUTCOME_CANCEL;
			klib::applyTurnStatusAndBonusesAndSkipTurn(adventurer);
		}
		else
			turnOutcome															= playerTurn(adventurer, currentEnemy);

		if(!combatContinues(turnOutcome, adventurer.Points.LifeCurrent.Health, currentEnemy.Points.LifeCurrent.Health))
			break;

		// Execute enemy attack turn
		if(currentEnemy.DidLoseTurn()) {
			printf("%s is stunned/asleep and loses his turn!\n", currentEnemy.Name.c_str());
			turnOutcome															= TURN_OUTCOME_CANCEL;
			klib::applyTurnStatusAndBonusesAndSkipTurn(currentEnemy);
		}
		else
			turnOutcome															= enemyTurn(currentEnemy, adventurer);
	}

	determineOutcome(adventurer, currentEnemy, enemyType);

	if(pEnemy)
		delete(pEnemy);
}

int32_t																	selectItemsPlayer			(klib::CCharacter& user, klib::CCharacter& target)														{
	int32_t indexInventory = user.Goods.Inventory.Items.Count;	// this initial value exits the menu

	klib::SMenuItem<int32_t>													itemOptions[MAX_INVENTORY_SLOTS+1]	;
	char																		itemOption[128]						= {};
	for(uint32_t i = 0; i < user.Goods.Inventory.Items.Count; ++i) {
		std::string itemName = klib::getItemName(user.Goods.Inventory.Items[i].Entity);
		sprintf_s(itemOption, "- x%.2u %s", user.Goods.Inventory.Items[i].Count, itemName.c_str());
		itemOptions[i].ReturnValue												= i;
 		itemOptions[i].Text														= itemOption;
	}
	itemOptions[user.Goods.Inventory.Items.Count].ReturnValue				= user.Goods.Inventory.Items.Count;
	itemOptions[user.Goods.Inventory.Items.Count].Text						= "Back to combat options";
	indexInventory															= displayMenu(user.Goods.Inventory.Items.Count+1, "Select an item to use", itemOptions);

	if(indexInventory == user.Goods.Inventory.Items.Count)	// exit option
		indexInventory															= user.Goods.Inventory.Items.Count;	// Exit menu
	else if (user.Goods.Inventory.Items[indexInventory].Count <= 0) {
		printf("You don't have anymore of that. Use something else...\n"); 
		indexInventory															= user.Goods.Inventory.Items.Count;
	}
	else {
		// Only use potions if we have less than Max HP
		const ::klib::SItem															& entityItem		= user.Goods.Inventory.Items[indexInventory].Entity;
		const ::klib::CItem															& itemDescription	= ::klib::itemDescriptions[entityItem.Definition];
		const ::klib::SEntityPoints													& userFinalPoints	= user.FinalPoints;
		if( ::klib::ITEM_TYPE_POTION == itemDescription.Type 
			&&  (	(::klib::PROPERTY_TYPE_HEALTH	== itemDescription.Property && user.Points.LifeCurrent.Health	>= (userFinalPoints.LifeMax.Health	))
				||	(::klib::PROPERTY_TYPE_SHIELD	== itemDescription.Property && user.Points.LifeCurrent.Shield	>= (userFinalPoints.LifeMax.Shield	))
				||	(::klib::PROPERTY_TYPE_MANA		== itemDescription.Property && user.Points.LifeCurrent.Mana		>= (userFinalPoints.LifeMax.Mana	))
				)
		)
		{
			// if we reached here it means that the input was valid so we select the description and exit the loop
			const ::std::string															userMessage			= "You don't need to use %s!\n";
			const ::std::string															itemName			= klib::getItemName(user.Goods.Inventory.Items[indexInventory].Entity);
			printf(userMessage.c_str(), itemName.c_str());
			indexInventory															= user.Goods.Inventory.Items.Count;
		}
	}
	return indexInventory;
}


int32_t																	selectItemsAI				(klib::CCharacter& user, klib::CCharacter& target)														{
	int32_t																		indexInventory				= (int32_t)(rand() % user.Goods.Inventory.Items.Count);

	const ::klib::SItem															& entityItem				= user.Goods.Inventory.Items[indexInventory].Entity;
	const ::klib::CItem															& itemDescription			= klib::itemDescriptions[entityItem.Definition];
	const ::klib::SEntityPoints													& userFinalPoints			= user.FinalPoints;
	// Only use potions if we have less than 60% HP
	if( ::klib::ITEM_TYPE_POTION == itemDescription.Type 
		&&  (	(::klib::PROPERTY_TYPE_HEALTH	== itemDescription.Property && user.Points.LifeCurrent.Health	> (userFinalPoints.LifeMax.Health	*.60))
			||	(::klib::PROPERTY_TYPE_SHIELD	== itemDescription.Property && user.Points.LifeCurrent.Shield	> (userFinalPoints.LifeMax.Shield	*.60))
			||	(::klib::PROPERTY_TYPE_MANA		== itemDescription.Property && user.Points.LifeCurrent.Mana		> (userFinalPoints.LifeMax.Mana		*.60))
			)
	)
	{
		const ::std::string userMessage											= "The enemy changes his mind about consuming %s because it doens't seem to be necessary!\n\n";
		const ::std::string itemName											= klib::getItemName(user.Goods.Inventory.Items[indexInventory].Entity);
		printf(userMessage.c_str(), itemName.c_str());
		indexInventory															= user.Goods.Inventory.Items.Count;
	}
		
	return indexInventory;
}


// This function returns true if an item was used or false if the menu was exited without doing anything.
bool																	useItems					(klib::CCharacter& user, klib::CCharacter& target, bool bIsAIControlled)								{
	uint32_t																	indexInventory				= ~0U;
	static const size_t															inventorySize				= ::gpk::size(user.Goods.Inventory.Items.Slots);
	if(0 == user.Goods.Inventory.Items.Count) {
		printf("%s has no items in the inventory.\n", user.Name.c_str());
		return false;
	}

	::std::string																userMessage					= "%s";
	bool																		bUsedItem					= false;
	if(!bIsAIControlled) {
		indexInventory															= selectItemsPlayer(user, target);
		if(indexInventory < user.Goods.Inventory.Items.Count)
			bUsedItem																= true;
	}
	else { // not a player so execute choice by AI
		indexInventory															= selectItemsAI(user, target);
		if(indexInventory < user.Goods.Inventory.Items.Count)
			bUsedItem																= true;
	}

	if(bUsedItem) {
		const ::klib::SItem															& entityItem				= user.Goods.Inventory.Items[indexInventory].Entity;
		const ::klib::CItem															& itemDescription			= klib::itemDescriptions[entityItem.Definition];
		const ::klib::SEntityPoints													& userFinalPoints			= user.FinalPoints;
		// Only use potions if we have less than 60% HP
		if( klib::ITEM_TYPE_POTION == itemDescription.Type 
			&&  (	(klib::PROPERTY_TYPE_HEALTH	== itemDescription.Property && user.Points.LifeCurrent.Health	>= userFinalPoints.LifeMax.Health	)
				||	(klib::PROPERTY_TYPE_SHIELD	== itemDescription.Property && user.Points.LifeCurrent.Shield	>= userFinalPoints.LifeMax.Shield	)
				||	(klib::PROPERTY_TYPE_MANA	== itemDescription.Property && user.Points.LifeCurrent.Mana		>= userFinalPoints.LifeMax.Mana		)
					)
			)
		{
			bUsedItem																= false;
		}
		else
			bUsedItem																= true;
	}

	if(bUsedItem)
		bUsedItem																= klib::executeItem(indexInventory, user, target);
	
	return bUsedItem;
}