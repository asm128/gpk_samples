#include "Entity.h"

#include "gpk_enum.h"
#include "gpk_size.h"
#include "gpk_bit.h"
#include "gpk_ascii_color.h"

#ifndef __ENTITYIMPL_H__928374092634923746298374__
#define __ENTITYIMPL_H__928374092634923746298374__


namespace klib
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(ENTITY_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, CHARACTER	, 0);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, PROFESSION	, 1);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, WEAPON		, 2);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, ARMOR		, 3);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, ACCESSORY	, 4);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, VEHICLE		, 5);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, FACILITY	, 6);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, STAGE_PROP	, 7);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, ITEM		, 8);
	GDEFINE_ENUM_VALUE(ENTITY_TYPE, TILE		, 9);
	stacxpr		const SEntityPointsMultiplier		g_MultipliersItem			= {{ .125,  .125,  .125}, { .125,  .125,  .125}, { .125,  .125, { .125,  .125,  .125},  .125,  .125}, {.125, .125, .125,  .125}, 0.125, 0.85, 1.0};
	stacxpr		const SEntityPointsMultiplier		g_MultipliersWeapon			= {{ .125,    .1, .0625}, { .125, .0625, .0625}, { .125,   .25, { .125,    .1,  .125}, .0625,   .25}, {.125,  .125, .125,   .1}, 0.125, 0.85, 1.0};
	stacxpr		const SEntityPointsMultiplier		g_MultipliersArmor			= {{ .125,    .1,   .25}, { .125,    .1,   .25}, {.0625, .0625, { .125, .0625,  .125},  .125, .0625}, {.125, .0625, .125,   .1}, 0.125, 0.85, 1.0};
	stacxpr		const SEntityPointsMultiplier		g_MultipliersProfession		= {{  .25,  .125, .0625}, {  .25,  .125, .0625}, { .125,  .125, {.0625,  .125, .0625},  .125,  .125}, {.125,    .1,   .1,   .1}, 0.125, 0.85, 1.0};
	stacxpr		const SEntityPointsMultiplier		g_MultipliersAccessory		= {{ .125,   .25, .0625}, { .125,   .25, .0625}, { .125,    .1, {.0625, .0625,  .125},    .1,    .1}, {.125,  .125, .125, .125}, 0.125, 0.85, 1.0};
	stacxpr		const SEntityPointsMultiplier		g_MultipliersVehicle		= {{.0625, .0625,   .25}, {.0625, .0625,   .25}, { .125,  .125, { .125,  .125,  .125},  .125,  .125}, {.125,    .1, .125,   .1},   0.1, 0.85, 1.0};
	stacxpr		const SEntityPointsMultiplier		g_MultipliersFacility		= {{  .25,   .25,   .25}, {  .25,   .25,   .25}, {  .25,   .25, {  .25,   .25,   .25},   .25,   .25}, { .25,   .25,  .25,   .1}, 0.125, 0.85, 1.0};
	stacxpr		const SEntityPointsMultiplier		g_MultipliersStageProp		= {{  .25,   .25,   .25}, {  .25,   .25,   .25}, {  .25,   .25, {  .25,   .25,   .25},   .25,   .25}, { .25,   .25,  .25,   .1}, 0.125, 0.85, 1.0};
	stacxpr		const SEntityPointsMultiplier		g_MultipliersTile			= {{  .25,   .25,   .25}, {  .25,   .25,   .25}, {  .25,   .25, {  .25,   .25,   .25},   .25,   .25}, { .25,   .25,  .25,   .1}, 0.125, 0.85, 1.0};

#define DECLARE_ENTITY(_multipliers, _type, _name)										\
	struct S##_name : public ::klib::SEntity { 											\
		using					::klib::SEntity					::SEntity; 				\
																						\
		inline constexpr										S##_name		()	{}	\
		inline constexpr										S##_name		(const SEntity& other)	: SEntity(other) {}	\
																												\
		static inline constexpr	::klib::ENTITY_TYPE				getType			()	{ return _type; }			\
		static inline constexpr	::klib::SEntityPointsMultiplier	getMultipliers	()	{ return _multipliers; }	\
		static					const ::gpk::vcc &				getName			()	{ static const ::gpk::vcc & name = ::gpk::vcs{#_name}; return name; }	\
	};

	DECLARE_ENTITY(g_MultipliersProfession	, ENTITY_TYPE_PROFESSION	, Profession	);
	DECLARE_ENTITY(g_MultipliersWeapon		, ENTITY_TYPE_WEAPON		, Weapon		);
	DECLARE_ENTITY(g_MultipliersArmor		, ENTITY_TYPE_ARMOR			, Armor			);
	DECLARE_ENTITY(g_MultipliersAccessory	, ENTITY_TYPE_ACCESSORY		, Accessory		);
	DECLARE_ENTITY(g_MultipliersVehicle		, ENTITY_TYPE_VEHICLE		, Vehicle		);
	DECLARE_ENTITY(g_MultipliersFacility	, ENTITY_TYPE_FACILITY		, Facility		);
	DECLARE_ENTITY(g_MultipliersStageProp	, ENTITY_TYPE_STAGE_PROP	, StageProp		);
	DECLARE_ENTITY(g_MultipliersItem		, ENTITY_TYPE_ITEM			, Item			);
	DECLARE_ENTITY(g_MultipliersTile		, ENTITY_TYPE_TILE			, Tile			);

	struct SCombatStatus {
		static					const uint32_t					MaxStatus						= MAX_COMBAT_STATUS_COUNT;
								COMBAT_STATUS					Status							= COMBAT_STATUS_NONE;
								int8_t							TurnsLeft						[MAX_COMBAT_STATUS_COUNT]								= {};

								void							Clear							()														{
			Status													= COMBAT_STATUS_NONE;
			memset(TurnsLeft, 0, sizeof(uint8_t)*::gpk::size(TurnsLeft));
		}
								int32_t							GetStatusTurns					(const COMBAT_STATUS status)			const			{
			int32_t														turns							= 0;
			for(uint32_t i=0, count = MaxStatus; i<count; ++i) {
				COMBAT_STATUS	statusBit	= (COMBAT_STATUS)(1 << i);
				if(0 == (statusBit & Status & status))
					continue;

				turns			+= TurnsLeft[i];
			}
			return turns;
		}

								void							NextTurn						()														{
			for(uint32_t i=0, count = MaxStatus; i<count; ++i) {
				COMBAT_STATUS												statusBit						= (COMBAT_STATUS)(1 << i);
				if(0 == (statusBit & Status))
					continue;

				if( ::gpk::bit_true(statusBit, COMBAT_STATUS_FROZEN	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_STUN		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_SLOW		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_PETRIFY	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_INVISIBLE	)

				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_SLEEP		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_BERSERK	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_PANIC		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_RAGE		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_CHARMED	)

				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_BLACKOUT	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_DRUNK		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_BULLIED	)
				)
					continue;

				// Clear bit if zero turns left.
				if(0 < TurnsLeft[i] && 0 == --TurnsLeft[i])
					Status			= (COMBAT_STATUS)(Status & ~statusBit);
				else if(0 > TurnsLeft[i] && 0 == ++TurnsLeft[i])
					Status			= (COMBAT_STATUS)(Status & ~statusBit);
			}
		}

								void							NextEnemyTurn					()														{
			for(uint32_t i=0, count = MaxStatus; i<count; ++i) {
				COMBAT_STATUS	statusBit	= (COMBAT_STATUS)(1 << i);
				if(0 == (statusBit & Status))
					continue;

				if( ::gpk::bit_true(statusBit, COMBAT_STATUS_BURN		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_FREEZING	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_POISON	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_BLEEDING	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_SHOCK		)

				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_FROZEN	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_STUN		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_SLOW		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_PETRIFY	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_INVISIBLE	)

				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_SLEEP		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_BERSERK	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_PANIC		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_RAGE		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_CHARMED	)

				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_BLACKOUT	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_FROZEN	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_DRUNK		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_BULLIED	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_WEAKNESS	)
				)
					continue;


				// Clear bit if zero turns left.
				if(0 < TurnsLeft[i] && 0 == --TurnsLeft[i])
					Status			= (COMBAT_STATUS)(Status & ~statusBit);
				else if(0 > TurnsLeft[i] && 0 == ++TurnsLeft[i])
					Status			= (COMBAT_STATUS)(Status & ~statusBit);
			}
		}

								void							NextRound						()														{
			for(uint32_t i=0, count = MaxStatus; i<count; ++i) {
				COMBAT_STATUS	statusBit	= (COMBAT_STATUS)(1 << i);
				if(0 == (statusBit & Status))
					continue;

				if( ::gpk::bit_true(statusBit, COMBAT_STATUS_BURN		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_FREEZING	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_POISON		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_BLEEDING	)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_SHOCK		)
				 || ::gpk::bit_true(statusBit, COMBAT_STATUS_WEAKNESS	)
				)
					continue;

				// Clear bit if zero turns left.
				if(0 < TurnsLeft[i] && 0 == --TurnsLeft[i])
					Status			= (COMBAT_STATUS)(Status & ~statusBit);
				else if(0 > TurnsLeft[i] && 0 == ++TurnsLeft[i])
					Status			= (COMBAT_STATUS)(Status & ~statusBit);
			}
		}
	};

							void							addStatus						(SCombatStatus& characterStatus, COMBAT_STATUS statusType, int32_t turnCount);

	typedef					SEntityPoints					SEntityPointsTurns;
	struct SCombatBonus {
								SEntityPoints					Points							= {};	// these are points that are calculated during combat depending on equipment or item consumption.
								SEntityPointsTurns				TurnsLeftPoints					= {};	// these are the amount of turns for which each bonus is valid. On each turn it should decrease by one and clear the bonus to zero when this counter reaches zero.
								SEntityFlags					Flags							= {};	// these are points that are calculated during combat depending on equipment or item consumption.
								SEntityFlagsTurns				TurnsLeftFlags					= {};	// these are the amount of turns for which each bonus is valid. On each turn it should decrease by one and clear the bonus to zero when this counter reaches zero.

								// Turn
								void							NextTurn						()														{
			if( 0 < TurnsLeftPoints.LifeMax.Health				)	{	if( 0 == --TurnsLeftPoints.LifeMax.Health				)	Points	.LifeMax		.Health		= 0										;} else if( 0 > TurnsLeftPoints.LifeMax.Health					)	{	if( 0 == ++TurnsLeftPoints.LifeMax.Health				)	Points.LifeMax.Health				= 0;	}
			if( 0 < TurnsLeftPoints.LifeMax.Mana				)	{	if( 0 == --TurnsLeftPoints.LifeMax.Mana					)	Points	.LifeMax		.Mana		= 0										;} else if( 0 > TurnsLeftPoints.LifeMax.Mana					)	{	if( 0 == ++TurnsLeftPoints.LifeMax.Mana					)	Points.LifeMax.Mana					= 0;	}
			if( 0 < TurnsLeftPoints.LifeMax.Shield				)	{	if( 0 == --TurnsLeftPoints.LifeMax.Shield				)	Points	.LifeMax		.Shield		= 0										;} else if( 0 > TurnsLeftPoints.LifeMax.Shield					)	{	if( 0 == ++TurnsLeftPoints.LifeMax.Shield				)	Points.LifeMax.Shield				= 0;	}
			if( 0 < TurnsLeftPoints.LifeCurrent.Health			)	{	if( 0 == --TurnsLeftPoints.LifeCurrent.Health			)	Points	.LifeCurrent	.Health		= 0										;} else if( 0 > TurnsLeftPoints.LifeCurrent.Health				)	{	if( 0 == ++TurnsLeftPoints.LifeCurrent.Health			)	Points.LifeCurrent.Health			= 0;	}
			if( 0 < TurnsLeftPoints.LifeCurrent.Mana			)	{	if( 0 == --TurnsLeftPoints.LifeCurrent.Mana				)	Points	.LifeCurrent	.Mana		= 0										;} else if( 0 > TurnsLeftPoints.LifeCurrent.Mana				)	{	if( 0 == ++TurnsLeftPoints.LifeCurrent.Mana				)	Points.LifeCurrent.Mana				= 0;	}
			if( 0 < TurnsLeftPoints.LifeCurrent.Shield			)	{	if( 0 == --TurnsLeftPoints.LifeCurrent.Shield			)	Points	.LifeCurrent	.Shield		= 0										;} else if( 0 > TurnsLeftPoints.LifeCurrent.Shield				)	{	if( 0 == ++TurnsLeftPoints.LifeCurrent.Shield			)	Points.LifeCurrent.Shield			= 0;	}
			if( 0 < TurnsLeftPoints.Coins						)	{	if( 0 == --TurnsLeftPoints.Coins						)	Points	.Coins						= 0										;} else if( 0 > TurnsLeftPoints.Coins							)	{	if( 0 == ++TurnsLeftPoints.Coins						)	Points.Coins						= 0;	}
			if( 0 < TurnsLeftFlags.Effect.Passive				)	{	if( 0 == --TurnsLeftFlags.Effect	.Passive			)	Flags	.Effect			.Passive	= PASSIVE_EFFECT_NONE					;} else if( 0 > TurnsLeftFlags.Effect	.Passive				)	{	if( 0 == ++TurnsLeftFlags.Effect.Passive				)	Flags.Effect.Passive				= PASSIVE_EFFECT_NONE;	}
		}

		// Player turn (round)
								void							NextRound						()														{
			if(0 < TurnsLeftPoints.Attack.Hit					) {	if(0 == --TurnsLeftPoints.Attack.Hit					)	Points	.Attack			.Hit					= 0							;} else if( 0 > TurnsLeftPoints.Attack.Hit					)	{	if( 0 == ++TurnsLeftPoints.Attack.Hit					) Points.Attack.Hit					= 0; }
			if(0 < TurnsLeftPoints.Attack.Damage				) {	if(0 == --TurnsLeftPoints.Attack.Damage					)	Points	.Attack			.Damage					= 0							;} else if( 0 > TurnsLeftPoints.Attack.Damage				)	{	if( 0 == ++TurnsLeftPoints.Attack.Damage				) Points.Attack.Damage				= 0; }
			if(0 < TurnsLeftPoints.Fitness.Attack				) {	if(0 == --TurnsLeftPoints.Fitness.Attack				)	Points	.Fitness		.Attack					= 0							;} else if( 0 > TurnsLeftPoints.Fitness.Attack				)	{	if( 0 == ++TurnsLeftPoints.Fitness.Attack				) Points.Fitness.Attack				= 0; }
			if(0 < TurnsLeftPoints.Fitness.Movement				) {	if(0 == --TurnsLeftPoints.Fitness.Movement				)	Points	.Fitness		.Movement				= 0							;} else if( 0 > TurnsLeftPoints.Fitness.Movement			)	{	if( 0 == ++TurnsLeftPoints.Fitness.Movement				) Points.Fitness.Movement			= 0; }
			if(0 < TurnsLeftPoints.Fitness.Sight				) {	if(0 == --TurnsLeftPoints.Fitness.Sight					)	Points	.Fitness		.Sight					= 0							;} else if( 0 > TurnsLeftPoints.Fitness.Sight				)	{	if( 0 == ++TurnsLeftPoints.Fitness.Sight				) Points.Fitness.Sight				= 0; }
			if(0 < TurnsLeftPoints.Attack.Range					) {	if(0 == --TurnsLeftPoints.Attack.Range					)	Points	.Attack			.Range					= 0							;} else if( 0 > TurnsLeftPoints.Attack.Range				)	{	if( 0 == ++TurnsLeftPoints.Attack.Range					) Points.Attack.Range				= 0; }
			if(0 < TurnsLeftPoints.Attack.DirectDamage.Health	) {	if(0 == --TurnsLeftPoints.Attack.DirectDamage.Health	)	Points	.Attack			.DirectDamage.Health	= 0							;} else if( 0 > TurnsLeftPoints.Attack.DirectDamage.Health	)	{	if( 0 == ++TurnsLeftPoints.Attack.DirectDamage.Health	) Points.Attack.DirectDamage.Health	= 0; }
			if(0 < TurnsLeftPoints.Attack.DirectDamage.Mana		) {	if(0 == --TurnsLeftPoints.Attack.DirectDamage.Mana		)	Points	.Attack			.DirectDamage.Mana		= 0							;} else if( 0 > TurnsLeftPoints.Attack.DirectDamage.Mana	)	{	if( 0 == ++TurnsLeftPoints.Attack.DirectDamage.Mana		) Points.Attack.DirectDamage.Mana	= 0; }
			if(0 < TurnsLeftPoints.Attack.DirectDamage.Shield	) {	if(0 == --TurnsLeftPoints.Attack.DirectDamage.Shield	)	Points	.Attack			.DirectDamage.Shield	= 0							;} else if( 0 > TurnsLeftPoints.Attack.DirectDamage.Shield	)	{	if( 0 == ++TurnsLeftPoints.Attack.DirectDamage.Shield	) Points.Attack.DirectDamage.Shield	= 0; }
			if(0 < TurnsLeftFlags.Effect.Attack					) {	if(0 == --TurnsLeftFlags.Effect	.Attack					)	Flags	.Effect			.Attack					= ATTACK_EFFECT_NONE		;} else if( 0 > TurnsLeftFlags.Effect	.Attack				)	{	if( 0 == ++TurnsLeftFlags.Effect	.Attack				) Flags.Effect.Attack				= ATTACK_EFFECT_NONE		; }
			if(0 < TurnsLeftFlags.Status.Inflict				) {	if(0 == --TurnsLeftFlags.Status	.Inflict				)	Flags	.Status			.Inflict				= COMBAT_STATUS_NONE		;} else if( 0 > TurnsLeftFlags.Status	.Inflict			)	{	if( 0 == ++TurnsLeftFlags.Status	.Inflict			) Flags.Status.Inflict				= COMBAT_STATUS_NONE		; }
			if(0 < TurnsLeftFlags.Tech.Tech						) {	if(0 == --TurnsLeftFlags.Tech.Tech						)	Flags	.Tech			.Tech					= ENTITY_TECHNOLOGY_UNKNOWN	;} else if( 0 > TurnsLeftFlags.Tech		.Tech				)	{	if( 0 == ++TurnsLeftFlags			.Tech.Tech			) Flags.Tech.Tech					= ENTITY_TECHNOLOGY_UNKNOWN	; }
			if(0 < TurnsLeftFlags.Tech.Grade					) {	if(0 == --TurnsLeftFlags.Tech.Grade						)	Flags	.Tech			.Grade					= ENTITY_GRADE_ILLUSION		;} else if( 0 > TurnsLeftFlags.Tech		.Grade				)	{	if( 0 == ++TurnsLeftFlags			.Tech.Grade			) Flags.Tech.Grade					= ENTITY_GRADE_ILLUSION		; }
		}

		// Enemy turn
								void							NextEnemyTurn					()														{
			if(0 < TurnsLeftPoints.Attack	.Absorption		) { if(0 == --TurnsLeftPoints.Attack	.Absorption			)	Points	.Attack			.Absorption				= 0							;} else if( 0 > TurnsLeftPoints.Attack.Absorption			)	{	if( 0 == ++TurnsLeftPoints.Attack.Absorption	)	Points.Attack.Absorption			= 0; }
			if(0 < TurnsLeftPoints.Fitness	.Reflexes		) { if(0 == --TurnsLeftPoints.Fitness	.Reflexes			)	Points	.Fitness		.Reflexes				= 0							;} else if( 0 > TurnsLeftPoints.Fitness.Reflexes			)	{	if( 0 == ++TurnsLeftPoints.Fitness.Reflexes		)	Points.Fitness.Reflexes				= 0; }
			if(0 < TurnsLeftFlags.Effect	.Defend			) { if(0 == --TurnsLeftFlags.Effect		.Defend				)	Flags	.Effect			.Defend					= DEFEND_EFFECT_NONE		;} else if( 0 > TurnsLeftFlags.Effect	.Defend				)	{	if( 0 == ++TurnsLeftFlags.Effect	.Defend		)	Flags.Effect.Defend					= DEFEND_EFFECT_NONE; }
			if(0 < TurnsLeftFlags.Status	.Immunity		) { if(0 == --TurnsLeftFlags.Status		.Immunity			)	Flags	.Status			.Immunity				= COMBAT_STATUS_NONE		;} else if( 0 > TurnsLeftFlags.Status	.Immunity			)	{	if( 0 == ++TurnsLeftFlags.Status	.Immunity	)	Flags.Status.Immunity				= COMBAT_STATUS_NONE; }
		}
	};

	struct SCharacterTurnBonus {
								SCombatBonus					Points							= SCombatBonus	();
								SCombatStatus					Status							= SCombatStatus	();
	};

	template<typename _tEntity>
	struct SEquip {
		typedef					_tEntity						TEntity;

								TEntity							Entity							= SEntity		();
								SEntityPoints					Points							= SEntityPoints	();
								SEntityFlags					Flags							= SEntityFlags	();

								SEntityPoints					FinalPoints						= SEntityPoints	();
								SEntityFlags					FinalFlags						= SEntityFlags	();

								SCharacterTurnBonus				ActiveBonus						= SCharacterTurnBonus();

								void							RecalculateFinalPoints			(const ::klib::SEntityTable<TEntity> & table)				noexcept		{
			const SEntityPoints											weaponPoints					= ::klib::getEntityPoints(Entity, table);
			const SEntityPoints											& bonusPoints					= ActiveBonus.Points.Points;

			// Currently, SEntityPoints::Coins and SEntityPoints::LifeCurrent values of the equipment are used in a different
			// way from the character's points so we avoid adding the character points to the result for these two.
			SEntityPoints												result							= bonusPoints + weaponPoints; // + facilityPoints + vehiclePoints;

			result.LifeMax											= result.LifeMax			+ Points.LifeMax;	//
			result.Attack											= result.Attack				+ Points.Attack;	//
			result.Fitness											= result.Fitness			+ Points.Fitness;	//
			result.PriceBuy											= result.PriceBuy			+ Points.PriceBuy;	//
			result.CostMaintenance									= result.CostMaintenance	+ Points.CostMaintenance;	//

			FinalPoints												= result;
		}
								void							RecalculateFinalFlags			(const ::klib::SEntityTable<TEntity> & table)				noexcept		{
			const SEntityFlags											weaponFlags						= ::klib::getEntityFlags(Entity, table);
			const SEntityFlags											& bonusFlags					= ActiveBonus.Points.Flags;
			FinalFlags												= bonusFlags | weaponFlags | Flags;
		}
								void							Recalculate						(const ::klib::SEntityTable<TEntity> & table)				noexcept		{
			RecalculateFinalPoints	(table);
			RecalculateFinalFlags	(table);
		}
	};


	template<typename _tEntity>
	inline					int32_t								getArmorAbsorption				(const ::klib::SEntityTable<_tEntity> & table, const SArmor& armor)			{
		return	(int32_t)((table.Definitions[armor.Definition].Points.Attack.Absorption + table.Modifiers[armor.Modifier].Points.Attack.Absorption) * armor.Level);
	}
#pragma pack(pop)

	struct SLogLine {
							uint16_t							Color;
							const ::gpk::apod<char>		Message;
	};
	struct SGameMessages {
							char								Aux		[4096]					= {};
							::gpk::apod<char>			UserMessage						= "";
							::gpk::apod<char>			UserSuccess						= "";
							::gpk::apod<char>			UserMiss						= "";
							::gpk::apod<char>			UserError						= "";
							::gpk::apod<char>			StateMessage					= "";
							::gpk::array_obj<SLogLine>			UserLog							= {};
							void								ClearMessages					()	{
			UserError	.clear();
			UserMessage	.clear();
			UserMiss	.clear();
			UserSuccess	.clear();
			StateMessage.clear();
		}

							void									LogStateMessage					()																						{ StateMessage	.push_back(0); UserLog.push_back({::gpk::ASCII_COLOR_CYAN	, StateMessage	});StateMessage	[0] = 0; StateMessage	.clear(); }
							void									LogMessage						()																						{ UserMessage	.push_back(0); UserLog.push_back({::gpk::ASCII_COLOR_YELLOW	, UserMessage	});UserMessage	[0] = 0; UserMessage	.clear(); }
							void									LogSuccess						()																						{ UserSuccess	.push_back(0); UserLog.push_back({::gpk::ASCII_COLOR_CYAN	, UserSuccess	});UserSuccess	[0] = 0; UserSuccess	.clear(); }
							void									LogError						()																						{ UserError		.push_back(0); UserLog.push_back({::gpk::ASCII_COLOR_RED	, UserError		});UserError	[0] = 0; UserError		.clear(); }
							void									LogMiss							()																						{ UserMiss		.push_back(0); UserLog.push_back({::gpk::ASCII_COLOR_RED	, UserMiss		});UserMiss		[0] = 0; UserMiss		.clear(); }
							void									LogAuxStateMessage				()																						{ if(0 == Aux[0]) return; UserLog.push_back({::gpk::ASCII_COLOR_CYAN	, ::gpk::view_const_string{Aux}}); Aux[0] = 0; }
							void									LogAuxMessage					()																						{ if(0 == Aux[0]) return; UserLog.push_back({::gpk::ASCII_COLOR_YELLOW	, ::gpk::view_const_string{Aux}}); Aux[0] = 0; }
							void									LogAuxSuccess					()																						{ if(0 == Aux[0]) return; UserLog.push_back({::gpk::ASCII_COLOR_CYAN	, ::gpk::view_const_string{Aux}}); Aux[0] = 0; }
							void									LogAuxError						()																						{ if(0 == Aux[0]) return; UserLog.push_back({::gpk::ASCII_COLOR_RED		, ::gpk::view_const_string{Aux}}); Aux[0] = 0; }
							void									LogAuxMiss						()																						{ if(0 == Aux[0]) return; UserLog.push_back({::gpk::ASCII_COLOR_RED		, ::gpk::view_const_string{Aux}}); Aux[0] = 0; }
	};

	struct SEntityTables {
		::klib::SEntityTable<::klib::SProfession>		Profession			= {};
		::klib::SEntityTable<::klib::SWeapon	>		Weapon				= {};
		::klib::SEntityTable<::klib::SArmor		>		Armor				= {};
		::klib::SEntityTable<::klib::SAccessory	>		Accessory			= {};
		::klib::SEntityTable<::klib::SVehicle	>		Vehicle				= {};
		::klib::SEntityTable<::klib::SFacility	>		Facility			= {};
		::klib::SEntityTable<::klib::SStageProp	>		StageProp			= {};
		::klib::SEntityTable<::klib::STile		>		Tile				= {};
		::klib::SEntityTable<::klib::SItem		>		Item				= {};
	};

	template<typename _EntityType>
	struct SResearchGroup {
		_EntityType										MaxResearch;
		SEntityContainer<int16_t>						Definitions;
		SEntityContainer<int16_t>						Modifiers;
	};
} // namespace

#endif //__ENTITYIMPL_H__928374092634923746298374__
