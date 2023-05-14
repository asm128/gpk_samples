#include "Game.h"

#include "Item.h"
#include "Enemy.h"

#include "GameMenu.h"

bool																	useItems					(const ::klib::SEntityTables & tables, ::klib::SGameMessages & message, ::klib::CCharacter& user, ::klib::CCharacter& target, bool bIsAIControlled);	// While in combat, displays a list of the available items to use.

bool																	escape						(const ::gpk::view_const_char& escaperName, ::klib::SCharacterScore& escaperScore)									{
	printf("%s tries to escape!\n", escaperName.begin());
	if ((rand() % 100) < 30) {
		++escaperScore.EscapesSucceeded;
		printf("%s fled from the battle!", escaperName.begin());
		return true;
	}

	++escaperScore.EscapesFailed;
	printf("%s failed to escape!\n\n", escaperName.begin());
	return false;
}

void																	assignDrops					(const ::klib::SEntityTables & tables, ::klib::CCharacter& winner, ::klib::CCharacter& loser, bool bIsAIControlled)								{
	printf("%s is dead!\n", loser.Name.begin());
	int64_t																		drop						= rand() % ::gpk::max(1LL, loser.Points.Coins >> 2);

	if(bIsAIControlled)
		drop																	= loser.Points.Coins-drop;

	printf("\n%s dropped %lli coins!!\n", loser.Name.begin(), drop);
	winner	.Points.Coins													+= drop;
	loser	.Points.Coins													-= drop;
	for(uint32_t i=0; i<loser.Goods.Inventory.Items.Slots.size(); i++)
		if( 0 == (rand()%2) ) {
			const ::klib::SEntitySlot<klib::SItem>										& itemDrop					= loser.Goods.Inventory.Items[i];
			::gpk::array_pod<char>													itemDropName				= klib::getItemName(itemDrop.Entity);
			if(winner.Goods.Inventory.Items.AddElement(itemDrop.Entity)) {
				printf("\n%s dropped %s!!\n", loser.Name.begin(), itemDropName.begin());
				loser.Goods.Inventory.Items.DecreaseEntity(i);
			}
			else {
				printf("%s can't pick up %s by %s because the inventory is full!\n", winner.Name.begin(), itemDropName.begin(), loser.Name.begin());
			}
		}

	::gpk::array_pod<char>													loserWeaponName				= klib::getEntityName(tables.Weapon, loser.CurrentEquip.Weapon);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.begin(), loserWeaponName.begin(), loser.CurrentEquip.Armor.Level, loser.Name.begin());
		::klib::SWeapon																oldWinnerWeapon				= winner.CurrentEquip.Weapon;
		winner.Goods.Inventory.Weapon.AddElement(loser.CurrentEquip.Weapon);

		::klib::SWeapon																loserNewWeapon				=
			{	1 + (rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerWeapon.Definition	)	))
			,	1 + (rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerWeapon.Modifier		)	))
			,	1 + (rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerWeapon.Level		)	))
			};

		if(loserNewWeapon.Definition || loserNewWeapon.Modifier || loserNewWeapon.Level > 1)
			printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.begin(), klib::getEntityName(tables.Weapon, loserNewWeapon).begin(), loserNewWeapon.Level);
		loser.Goods.Inventory.Weapon.AddElement(loserNewWeapon);
		loser.CurrentEquip.Weapon												= { 0, 0, 1 };
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.begin(), loserWeaponName.begin(), loser.Name.begin());

	::gpk::array_pod<char>													loserAccessoryName			= ::klib::getEntityName(tables.Accessory, loser.CurrentEquip.Accessory);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.begin(), loserAccessoryName.begin(), loser.CurrentEquip.Accessory.Level, loser.Name.begin());
		::klib::SAccessory															oldWinnerAccessory			= winner.CurrentEquip.Accessory;
		winner.Goods.Inventory.Accessory.AddElement(loser.CurrentEquip.Accessory);

		::klib::SAccessory															loserNewAccessory			=
			{	1+(rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerAccessory.Definition	)	))
			,	1+(rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerAccessory.Modifier		)	))
			,	1+(rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerAccessory.Level			)	))
			};

		if(loserNewAccessory.Definition || loserNewAccessory.Modifier || loserNewAccessory.Level > 1)
			printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.begin(), klib::getEntityName(tables.Accessory, loserNewAccessory).begin(), loserNewAccessory.Level);
		loser.Goods.Inventory.Accessory.AddElement(loserNewAccessory);
		loser.CurrentEquip.Accessory											= {0,0,1};
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.begin(), loserAccessoryName.begin(), loser.Name.begin());

	::gpk::array_pod<char>													loserArmorName				= ::klib::getEntityName(tables.Armor, loser.CurrentEquip.Armor);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.begin(), loserArmorName.begin(), loser.CurrentEquip.Armor.Level, loser.Name.begin());
		::klib::SArmor																oldWinnerArmor				= winner.CurrentEquip.Armor;
		winner.Goods.Inventory.Armor.AddElement(loser.CurrentEquip.Armor);

		::klib::SArmor																loserNewArmor				=
			{	1+(rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerArmor.Definition	)))
			,	1+(rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerArmor.Modifier		)))
			,	1+(rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerArmor.Level			)))
			};

		if(loserNewArmor.Definition || loserNewArmor.Modifier || loserNewArmor.Level > 1)
			printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.begin(), ::klib::getEntityName(tables.Armor, loserNewArmor).begin(), loserNewArmor.Level);
		loser.Goods.Inventory.Armor.AddElement(loserNewArmor);
		loser.CurrentEquip.Armor												= {0,0,1};
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.begin(), loserArmorName.begin(), loser.Name.begin());

	::gpk::array_pod<char>													loserVehicleName			= ::klib::getEntityName(tables.Vehicle, loser.CurrentEquip.Vehicle);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.begin(), loserVehicleName.begin(), loser.CurrentEquip.Vehicle.Level, loser.Name.begin());
		::klib::SVehicle															oldWinnerVehicle			= winner.CurrentEquip.Vehicle;
		winner.Goods.Inventory.Vehicle.AddElement(loser.CurrentEquip.Vehicle);

		::klib::SVehicle															loserNewVehicle				=
			{	1 + (rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerVehicle.Definition	)))
			,	1 + (rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerVehicle.Modifier	)))
			,	1 + (rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerVehicle.Level		)))
			};

		if(loserNewVehicle.Definition || loserNewVehicle.Modifier || loserNewVehicle.Level > 1)
			printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.begin(), ::klib::getEntityName(tables.Vehicle, loserNewVehicle).begin(), loserNewVehicle.Level);
		loser.Goods.Inventory.Vehicle.AddElement(loserNewVehicle);
		loser.CurrentEquip.Vehicle												= { 0, 0, 1 };
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.begin(), loserVehicleName.begin(), loser.Name.begin());

	::gpk::array_pod<char>													loserFacilityName			= ::klib::getEntityName(tables.Facility, loser.CurrentEquip.Facility);
	if( 0 == (rand()%2) ) {
		printf("%s recovers %s level %u from %s.\n", winner.Name.begin(), loserFacilityName.begin(), loser.CurrentEquip.Facility.Level, loser.Name.begin());
		klib::SFacility																oldWinnerFacility			= winner.CurrentEquip.Facility;
		winner.Goods.Inventory.Facility.AddElement(loser.CurrentEquip.Facility);

		klib::SFacility																loserNewFacility			=
			{	1+(rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerFacility.Definition	)	))
			,	1+(rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerFacility.Modifier		)	))
			,	1+(rand() % ::gpk::max((int16_t)2, (int16_t)(oldWinnerFacility.Level		)	))
			};

		//if(loserNewFacility.Index || loserNewFacility.Modifier || loserNewFacility.Level > 1)
		//	printf("%s recovers a used %s level %u from the battlefield.\n", loser.Name.begin(), klib::getFacilityName(loserNewFacility).begin(), loserNewFacility.Level);
		//loser.Inventory.Facility.AddElement(loserNewFacility);
		//loser.CurrentEquip.Facility = {0,0,1};
	}
	else
		printf("%s doesn't get to recover %s from %s.\n", winner.Name.begin(), loserFacilityName.begin(), loser.Name.begin());

	::gpk::array_pod<char>													loserProfessionName			= ::klib::getEntityName(tables.Profession, loser.CurrentEquip.Profession);
	if(rand()%2) {
		printf("%s recovers a %s level %u from %s.\n", winner.Name.begin(), loserProfessionName.begin(), loser.CurrentEquip.Profession.Level, loser.Name.begin());
		winner.Goods.Inventory.Profession.AddElement(loser.CurrentEquip.Profession);
	}
	else
		printf("%s doesn't get to recover %s from %s because it's too damaged.\n", winner.Name.begin(), loserProfessionName.begin(), loser.Name.begin());

	::klib::SProfession															oldLoserProfession			= loser.CurrentEquip.Profession;

	bool																		bLowestFall					= true;
	if(loser.CurrentEquip.Profession.Modifier	)	{ bLowestFall = false; --loser.CurrentEquip.Profession.Modifier	; };
	if(loser.CurrentEquip.Profession.Level > 1	)	{ bLowestFall = false; --loser.CurrentEquip.Profession.Level	; };
	if( bLowestFall )
		printf("%s couldn't fall any lower!!\n", loser.Name.begin());
	else
		printf("%s loses his job as %s level %u and tries to get a new job as %s level %u instead.\n", loser.Name.begin(), loserProfessionName.begin(), oldLoserProfession.Level, klib::getEntityName(tables.Profession, loser.CurrentEquip.Profession).begin(), loser.CurrentEquip.Profession.Level);

	if(	winner.CurrentEquip.Profession.Modifier < winner.Goods.CompletedResearch.Profession.MaxResearch.Modifier ) {
		++winner.CurrentEquip.Profession.Modifier;
		printf("%s gets promoted to %s!!\n", winner.Name.begin(), getEntityName(tables.Profession, winner.CurrentEquip.Profession).begin());
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

static	void															determineOutcome			(const ::klib::SEntityTables & tables, ::klib::CCharacter& adventurer, klib::CCharacter& enemy)								{
		// Determine the outcome of the battle and give rewards if applicable.
	if (adventurer.Points.LifeCurrent.Health <= 0)
		assignDrops(tables, enemy, adventurer, false);
	if (enemy.Points.LifeCurrent.Health <= 0)
		assignDrops(tables, adventurer, enemy, true);
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
	(void)attacker; (void)target;
	return false;
}

void																	senseMenu					(const ::klib::SEntityTables & tables, ::klib::CCharacter& enemy)																				{
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
		int																			tavernChoice				= displayMenu(::gpk::view_const_string{"You wonder about what to do next.."}, ::gpk::view_array<const ::klib::SMenuItem<int>>{tavernOptions});

		// Interpret user input.
			 if( 1 == tavernChoice )	{	displayWeapon					(tables, enemy);									}	//
		else if( 2 == tavernChoice )	{	displayAccessory				(tables, enemy);									}	//
		else if( 3 == tavernChoice )	{	displayArmor					(tables, enemy);									}	//
		else if( 4 == tavernChoice )	{	displayProfession				(tables, enemy);									}	//
		else if( 5 == tavernChoice )	{	displayVehicle					(tables, enemy);									}	//
		else if( 6 == tavernChoice )	{	displayFacility					(tables, enemy);									}	//
		else if( 7 == tavernChoice )	{	displayInventory				(enemy.Goods.Inventory.Items, enemy.Name);	}	//
		else if( 8 == tavernChoice )	{	break;	}	// Exit the main loop, which effectively closes the game.
		else {
			printf("Option not supported yet.\n");
		}	// Exit the main loop, which effectively closes the game.
	}
}

TURN_OUTCOME															characterTurn				(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::TURN_ACTION combatOption, klib::CCharacter& attacker, klib::CCharacter& target, bool bIsAIControlled)	{
	// If the action is valid then we execute it and break the current while() so the attack turn executes.
	TURN_OUTCOME																outcome						= TURN_OUTCOME_CANCEL;
	if(TURN_ACTION_ATTACK == combatOption) {
		if( !klib::attack(tables, messages, attacker, target) )
			outcome																	= TURN_OUTCOME_CONTINUE;
	}
	else if(TURN_ACTION_INVENTORY == combatOption) {
		if( !useItems(tables, messages, attacker, target, bIsAIControlled) )
			outcome																	= TURN_OUTCOME_CONTINUE;
	}	// useItems requires to receive the current enemy as a parameter in order to modify its health if we use a grenade and hit.
	else if(TURN_ACTION_SKILL == combatOption) {
		if( !useSkills(attacker, target) )
			outcome																	= TURN_OUTCOME_CONTINUE;
	}	// useItems requires to receive the current enemy as a parameter in order to modify its health if we use a grenade and hit.
	else if(TURN_ACTION_SENSE == combatOption) {
		senseMenu(tables, target);	// sense applies to target only. You can't "sense yourself".
		outcome																	= TURN_OUTCOME_CONTINUE;
	}	//
	else if(TURN_ACTION_RUN == combatOption) {
		if( escape(attacker.Name, attacker.Score) )
			outcome																	= TURN_OUTCOME_ESCAPE; // Escape: if we succeed we just exit this combat() function, otherwise cancel this loop and execute the enemy turn.
	}
	else if(TURN_ACTION_CANCEL == combatOption) {
		sprintf_s(messages.Aux, "%s couldn't figure out what to do and skipped the turn.\n", attacker.Name.begin());
		outcome																	= TURN_OUTCOME_CANCEL; // Escape: if we succeed we just exit this combat() function, otherwise cancel this loop and execute the enemy turn.
	}

	if(outcome == TURN_OUTCOME_CANCEL && target.Points.LifeCurrent.Health > 0 && attacker.Points.LifeCurrent.Health > 0)
		::klib::applyTurnStatusAndBonusesAndSkipTurn(tables, messages, attacker);

	return outcome;
}

static	void															printStatuses				(const ::klib::CCharacter& character, ::klib::SGameMessages & messages)																	{
	for(uint32_t iStatus = 0, statusCount = character.ActiveBonus.Status.MaxStatus; iStatus < statusCount; ++iStatus) {
		::klib::COMBAT_STATUS														statusBit					= (::klib::COMBAT_STATUS)(1 << iStatus);
		if(0 == (statusBit & character.ActiveBonus.Status.Status))
			continue;

		const ::gpk::view_const_char												statusLabel					= ::gpk::get_value_label(statusBit);
		sprintf_s(messages.Aux, "%s is affected by \"%s\" for the next %u turn(s).\n", character.Name.begin(), statusLabel.begin(), character.ActiveBonus.Status.TurnsLeft[iStatus]);
	}
}


static	void															printBonuses				(const klib::CCharacter& character, ::klib::SGameMessages & messages)																		{
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.Hit					> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Hit.\n"						, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.Hit					,	character.ActiveBonus.Points.Points.Attack.Hit					);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.Damage				> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Damage.\n"					, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.Damage				,	character.ActiveBonus.Points.Points.Attack.Damage				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Health	> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Health Damage.\n"			, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Health	,	character.ActiveBonus.Points.Points.Attack.DirectDamage.Health	);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Mana	> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Mana Damage.\n"				, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Mana	,	character.ActiveBonus.Points.Points.Attack.DirectDamage.Mana	);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Shield	> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Shield Damage.\n"			, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.DirectDamage.Shield	,	character.ActiveBonus.Points.Points.Attack.DirectDamage.Shield	);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.Absorption			> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Absorption.\n"				, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.Absorption			,	character.ActiveBonus.Points.Points.Attack.Absorption			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Attack.Range				> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Range.\n"					, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Attack.Absorption			,	character.ActiveBonus.Points.Points.Attack.Absorption			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Attack				> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Attack Speed.\n"				, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Attack				,	character.ActiveBonus.Points.Points.Fitness.Attack				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Movement			> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Movement Speed.\n"			, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Movement			,	character.ActiveBonus.Points.Points.Fitness.Movement			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Reflexes			> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Reflexes.\n"					, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Reflexes			,	character.ActiveBonus.Points.Points.Fitness.Reflexes			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Sight				> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Sight.\n"					, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Fitness.Sight				,	character.ActiveBonus.Points.Points.Fitness.Sight				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Health				> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Max Health.\n"				, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Health				,	character.ActiveBonus.Points.Points.LifeMax.Health				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Mana				> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Max Mana.\n"					, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Mana				,	character.ActiveBonus.Points.Points.LifeMax.Mana				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Shield				> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Max Shield.\n"				, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeMax.Shield				,	character.ActiveBonus.Points.Points.LifeMax.Shield				);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Health			> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Health Recovery.\n"			, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Health			,	character.ActiveBonus.Points.Points.LifeCurrent.Health			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Mana			> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Mana Recovery.\n"			, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Mana			,	character.ActiveBonus.Points.Points.LifeCurrent.Mana			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Shield			> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %u turns: %i Shield Recovery.\n"			, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.LifeCurrent.Shield			,	character.ActiveBonus.Points.Points.LifeCurrent.Shield			);
	if( character.ActiveBonus.Points.TurnsLeftPoints.Coins						> 0 ) sprintf_s(messages.Aux, "%s has an additional bonus for the next %lli turns: %lli Coin Earning.\n"			, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftPoints.Coins						,	character.ActiveBonus.Points.Points.Coins						);

	// The following messages should be improved by taking every possible status into account.
	if( character.ActiveBonus.Points.TurnsLeftFlags.Effect.Attack				> 0 ) sprintf_s(messages.Aux, "%s has additional flags for the next %u turn(s): Attack Effect   (0x%.04X).\n"	, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftFlags.Effect.Attack				,	character.ActiveBonus.Points.Flags.Effect.Attack				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Effect.Defend				> 0 ) sprintf_s(messages.Aux, "%s has additional flags for the next %u turn(s): Defend Effect   (0x%.04X).\n"	, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftFlags.Effect.Defend				,	character.ActiveBonus.Points.Flags.Effect.Defend				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Effect.Passive				> 0 ) sprintf_s(messages.Aux, "%s has additional flags for the next %u turn(s): Passive Effect  (0x%.04X).\n"	, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftFlags.Effect.Passive				,	character.ActiveBonus.Points.Flags.Effect.Passive				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Status.Immunity				> 0 ) sprintf_s(messages.Aux, "%s has additional flags for the next %u turn(s): Status Immunity (0x%.04X).\n"	, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftFlags.Status.Immunity				,	character.ActiveBonus.Points.Flags.Status.Immunity				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Status.Inflict				> 0 ) sprintf_s(messages.Aux, "%s has additional flags for the next %u turn(s): Status Inflict  (0x%.04X).\n"	, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftFlags.Status.Inflict				,	character.ActiveBonus.Points.Flags.Status.Inflict				);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Tech.Tech					> 0 ) sprintf_s(messages.Aux, "%s has additional flags for the next %u turn(s): Technology      (0x%.04X).\n"	, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftFlags.Tech.Tech					,	character.ActiveBonus.Points.Flags.Tech.Tech					);
	if( character.ActiveBonus.Points.TurnsLeftFlags.Tech.Grade					> 0 ) sprintf_s(messages.Aux, "%s has additional level for the next %u turn(s): Level           (%.05i).\n"		, character.Name.begin(),	character.ActiveBonus.Points.TurnsLeftFlags.Tech.Grade					,	character.ActiveBonus.Points.Flags.Tech.Grade					);
}

void																	printCharacterShortInfo		(const ::klib::SEntityTables & tables, ::klib::CCharacter& character)																			{
	const ::klib::SEntityPoints													& characterPoints			= character.FinalPoints;
	printf("\n----------------------- %s is a %s level %u.\nWeapon: %s level %u.\nArmor: %s level %u.\n"
		, character.Name.begin()
		, ::klib::getEntityName	(tables.Profession	, character.CurrentEquip.Profession	).begin(),	character.CurrentEquip.Profession.Level
		, ::klib::getEntityName	(tables.Weapon		, character.CurrentEquip.Weapon		).begin(),	character.CurrentEquip.Weapon.Level
		, ::klib::getEntityName	(tables.Armor		, character.CurrentEquip.Armor		).begin(),	character.CurrentEquip.Armor.Level
		);

	printf("-------------- Max points:\n");
	characterPoints.LifeMax.Print();
	printf("-------------- Current points:\n");
	character.Points.LifeCurrent.Print();
	characterPoints.Attack.Print();
}

TURN_OUTCOME															playerTurn					(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& adventurer, ::klib::CCharacter& currentEnemy)										{
	static const ::klib::SMenuItem<TURN_ACTION>									combatOptions[]				=
		{ { TURN_ACTION_ATTACK		, "Attack"		}
		, { TURN_ACTION_INVENTORY	, "Inventory"	}
		, { TURN_ACTION_SKILL		, "Skills"		}
		, { TURN_ACTION_SENSE		, "Sense"		}
		, { TURN_ACTION_CANCEL		, "Cancel turn"	}
		, { TURN_ACTION_RUN			, "Run"			}
		};

	TURN_OUTCOME																turnOutcome					= TURN_OUTCOME_CONTINUE;

	while (turnOutcome == TURN_OUTCOME_CONTINUE) {	// this while() process the input for this turn until the user enters a valid choice and then exits to the outer loop for executing the attack turn.
		printCharacterShortInfo	(tables, adventurer);
		printStatuses			(adventurer, messages);
		printBonuses			(adventurer, messages);

		printCharacterShortInfo	(tables, currentEnemy);
		printStatuses			(currentEnemy, messages);
		printBonuses			(currentEnemy, messages);

		const TURN_ACTION															actionChoice				= displayMenu("It's your turn to make a move", ::gpk::view_array<const ::klib::SMenuItem<TURN_ACTION>>{combatOptions});
		turnOutcome																= characterTurn(tables, messages, actionChoice, adventurer, currentEnemy, false);
	}
	return turnOutcome;
}

TURN_ACTION																resolveAI					(klib::CCharacter& enemy)													{
	TURN_ACTION																	action						= TURN_ACTION_ATTACK;
	if(enemy.Goods.Inventory.Items.Slots.size())
		action																	= (rand()%2) ? action : TURN_ACTION_INVENTORY;
	else if(enemy.Points.LifeCurrent.Health <= (enemy.Points.LifeMax.Health/9) && 0 == (rand()%7))	// chance of escape attempt if health is less than 11%.
		action																	= TURN_ACTION_RUN;

	return action;
}

TURN_OUTCOME															enemyTurn					(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& enemy, klib::CCharacter& adventurer)													{
	TURN_OUTCOME																turnOutcome					= TURN_OUTCOME_CONTINUE;
	while (turnOutcome == TURN_OUTCOME_CONTINUE) {	// this while() process the input for this turn until the user enters a valid choice and then exits to the outer loop for executing the attack turn.
		const TURN_ACTION															actionChoice				= (TURN_ACTION)resolveAI(enemy);
		turnOutcome																= characterTurn(tables, messages, actionChoice, enemy, adventurer, true);
	}
	return turnOutcome;
}

bool																	combatContinues				(TURN_OUTCOME turnOutcome, int adventurerHP, int enemyHP)												{
	return !( TURN_OUTCOME_ESCAPE == turnOutcome || 0 >= adventurerHP || 0 >= enemyHP );
}

//5736	// gasty.bellino@gmail.com
void																	combat						(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& adventurer, int32_t enemyType)														{
	if(adventurer.Points.LifeCurrent.Health <= 1) {
		printf("You don't have enough health to engage in combat. Please go rest and come back later.\n");
		return;
	}

	::klib::CCharacter															* pEnemy					= new ::klib::CCharacter(::klib::enemyDefinitions[enemyType]);
	::klib::CCharacter															& currentEnemy				= *pEnemy;	// Request the enemy data.
	::klib::setupAgent(tables, adventurer, currentEnemy);

	adventurer.ActiveBonus.Status.Clear();	// We need to clear the combat status before starting the combat.

	TURN_OUTCOME																turnOutcome					= TURN_OUTCOME_CONTINUE;
	while(combatContinues(turnOutcome, adventurer.Points.LifeCurrent.Health, currentEnemy.Points.LifeCurrent.Health)) {	// This while() executes the attack turns, requesting for user input at the beginning of each turn.
		++adventurer	.Score.TurnsPlayed;
		++currentEnemy	.Score.TurnsPlayed;

		if(adventurer.DidLoseTurn()) {
			printf("%s is stunned/asleep/frozen and loses his turn!\n", adventurer.Name.begin());
			turnOutcome															= TURN_OUTCOME_CANCEL;
			::klib::applyTurnStatusAndBonusesAndSkipTurn(tables, messages, adventurer);
		}
		else
			turnOutcome															= playerTurn(tables, messages, adventurer, currentEnemy);

		for(uint32_t iMessage = 0; iMessage < messages.UserLog.size(); ++iMessage)
			printf("%s", messages.UserLog[iMessage].Message.begin());
		messages.UserLog.clear();
		if(!combatContinues(turnOutcome, adventurer.Points.LifeCurrent.Health, currentEnemy.Points.LifeCurrent.Health))
			break;

		// Execute enemy attack turn
		if(currentEnemy.DidLoseTurn()) {
			printf("%s is stunned/asleep and loses his turn!\n", currentEnemy.Name.begin());
			turnOutcome															= TURN_OUTCOME_CANCEL;
			::klib::applyTurnStatusAndBonusesAndSkipTurn(tables, messages, currentEnemy);
		}
		else
			turnOutcome															= enemyTurn(tables, messages, currentEnemy, adventurer);
		for(uint32_t iMessage = 0; iMessage < messages.UserLog.size(); ++iMessage)
			printf("%s", messages.UserLog[iMessage].Message.begin());
		messages.UserLog.clear();
	}

	determineOutcome(tables, adventurer, currentEnemy);

	if(pEnemy)
		delete(pEnemy);
}

int32_t																	selectItemsPlayer						(::klib::CCharacter& user)														{
	int32_t indexInventory = user.Goods.Inventory.Items.size();	// this initial value exits the menu

	::klib::SMenuItem<int32_t>													itemOptions[4096]						= {};
	char																		itemOption[128]							= {};
	for(uint32_t i = 0; i < user.Goods.Inventory.Items.size(); ++i) {
		const ::gpk::array_pod<char>												itemName								= ::klib::getItemName(user.Goods.Inventory.Items[i].Entity);
		sprintf_s(itemOption, "- x%.2u %s", user.Goods.Inventory.Items[i].Count, itemName.begin());
		itemOptions[i].ReturnValue												= i;
 		itemOptions[i].Text														= itemOption;
	}
	itemOptions[user.Goods.Inventory.Items.size()].ReturnValue				= user.Goods.Inventory.Items.size();
	itemOptions[user.Goods.Inventory.Items.size()].Text						= "Back to combat options";
	indexInventory															= displayMenu(user.Goods.Inventory.Items.size() + 1, "Select an item to use", ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{itemOptions});

	if(indexInventory == (int32_t)user.Goods.Inventory.Items.Slots.size())	// exit option
		indexInventory															= user.Goods.Inventory.Items.size();	// Exit menu
	else if (user.Goods.Inventory.Items[indexInventory].Count <= 0) {
		printf("You don't have anymore of that. Use something else...\n");
		indexInventory															= user.Goods.Inventory.Items.size();
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
			const ::gpk::view_const_string												userMessage			= "You don't need to use %s!\n";
			const ::gpk::array_pod<char>												itemName			= ::klib::getItemName(user.Goods.Inventory.Items[indexInventory].Entity);
			printf(userMessage.begin(), itemName.begin());
			indexInventory															= user.Goods.Inventory.Items.size();
		}
	}
	return indexInventory;
}


int32_t																	selectItemsAI				(::klib::CCharacter& user)														{
	int32_t																		indexInventory				= (int32_t)(rand() % user.Goods.Inventory.Items.size());

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
		const ::gpk::view_const_string												userMessage											= "The enemy changes his mind about consuming %s because it doens't seem to be necessary!\n\n";
		const ::gpk::array_pod<char>												itemName											= klib::getItemName(user.Goods.Inventory.Items[indexInventory].Entity);
		printf(userMessage.begin(), itemName.begin());
		indexInventory															= user.Goods.Inventory.Items.size();
	}

	return indexInventory;
}


// This function returns true if an item was used or false if the menu was exited without doing anything.
bool																	useItems					(const ::klib::SEntityTables & tables, ::klib::SGameMessages & messages, ::klib::CCharacter& user, klib::CCharacter& target, bool bIsAIControlled)								{
	uint32_t																	indexInventory				= ~0U;
	static const size_t															inventorySize				= user.Goods.Inventory.Items.Slots.size();
	if(0 == user.Goods.Inventory.Items.size()) {
		printf("%s has no items in the inventory.\n", user.Name.begin());
		return false;
	}

	bool																		bUsedItem					= false;
	if(!bIsAIControlled) {
		indexInventory															= selectItemsPlayer(user);
		if(indexInventory < user.Goods.Inventory.Items.size())
			bUsedItem																= true;
	}
	else { // not a player so execute choice by AI
		indexInventory															= selectItemsAI(user);
		if(indexInventory < user.Goods.Inventory.Items.size())
			bUsedItem																= true;
	}

	if(bUsedItem) {
		const ::klib::SItem															& entityItem				= user.Goods.Inventory.Items[indexInventory].Entity;
		const ::klib::CItem															& itemDescription			= klib::itemDescriptions[entityItem.Definition];
		const ::klib::SEntityPoints													& userFinalPoints			= user.FinalPoints;
		// Only use potions if we have less than 60% HP
		if( ::klib::ITEM_TYPE_POTION == itemDescription.Type
			&&  (	(::klib::PROPERTY_TYPE_HEALTH	== itemDescription.Property && user.Points.LifeCurrent.Health	>= userFinalPoints.LifeMax.Health	)
				||	(::klib::PROPERTY_TYPE_SHIELD	== itemDescription.Property && user.Points.LifeCurrent.Shield	>= userFinalPoints.LifeMax.Shield	)
				||	(::klib::PROPERTY_TYPE_MANA		== itemDescription.Property && user.Points.LifeCurrent.Mana		>= userFinalPoints.LifeMax.Mana		)
				)
			)
		{
			bUsedItem																= false;
		}
		else
			bUsedItem																= true;
	}

	if(bUsedItem)
		bUsedItem																= klib::executeItem(tables, messages, indexInventory, user, target);

	return bUsedItem;
}
