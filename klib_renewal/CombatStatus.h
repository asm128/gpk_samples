#include "gpk_enum.h"

#ifndef	__COMBATSTATUS_H__2386498236498213469817263421__
#define	__COMBATSTATUS_H__2386498236498213469817263421__

namespace klib
{
#pragma pack(push, 1)

#define MAX_COMBAT_STATUS_COUNT 20
	GDEFINE_ENUM_TYPE(COMBAT_STATUS, int32_t);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, NONE			, 0x00000);

	GDEFINE_ENUM_VALUE(COMBAT_STATUS, BLIND			, 0x00001);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, STUN			, 0x00002);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, SHOCK			, 0x00004);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, BLEEDING		, 0x00008);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, BURN			, 0x00010);

	GDEFINE_ENUM_VALUE(COMBAT_STATUS, POISON		, 0x00020);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, FREEZING		, 0x00040);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, PETRIFY		, 0x00080);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, FROZEN		, 0x00100);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, BLACKOUT		, 0x00200);

	GDEFINE_ENUM_VALUE(COMBAT_STATUS, CHARMED		, 0x00400);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, DRUNK			, 0x00800);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, SLEEP			, 0x01000);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, SLOW			, 0x02000);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, WEAKNESS		, 0x04000);

	GDEFINE_ENUM_VALUE(COMBAT_STATUS, PANIC			, 0x08000);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, BERSERK		, 0x10000);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, INVISIBLE		, 0x20000);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, RAGE			, 0x40000);
	GDEFINE_ENUM_VALUE(COMBAT_STATUS, BULLIED		, 0x80000);

#define MAX_ATTACK_EFFECT_COUNT 4
	GDEFINE_ENUM_TYPE(ATTACK_EFFECT, uint16_t);
	GDEFINE_ENUM_VALUE(ATTACK_EFFECT, NONE			, 0x00	);
	GDEFINE_ENUM_VALUE(ATTACK_EFFECT, LEECH_HEALTH	, 0x01	);
	GDEFINE_ENUM_VALUE(ATTACK_EFFECT, LEECH_MANA	, 0x02	);
	GDEFINE_ENUM_VALUE(ATTACK_EFFECT, LEECH_SHIELD	, 0x04	);
	GDEFINE_ENUM_VALUE(ATTACK_EFFECT, STEAL			, 0x08	);

#define MAX_DEFEND_EFFECT_COUNT 4
	GDEFINE_ENUM_TYPE(DEFEND_EFFECT, uint16_t);
	GDEFINE_ENUM_VALUE(DEFEND_EFFECT, NONE			, 0x00	); 
	GDEFINE_ENUM_VALUE(DEFEND_EFFECT, REFLECT		, 0x01	); 	// Reflects damage to the attacker
	GDEFINE_ENUM_VALUE(DEFEND_EFFECT, BLIND			, 0x02	); 	// Blinds the attacker when inflicting 1 to 1 damage.
	GDEFINE_ENUM_VALUE(DEFEND_EFFECT, IMPENETRABLE	, 0x04	); 	// Reflects damage to the attacker
	GDEFINE_ENUM_VALUE(DEFEND_EFFECT, DEFLECT		, 0x08	); 	// Reflects damage to the attacker

#define MAX_PASSIVE_EFFECT_COUNT 3
	GDEFINE_ENUM_TYPE(PASSIVE_EFFECT, uint16_t);
	GDEFINE_ENUM_VALUE(PASSIVE_EFFECT, NONE				, 0x00);
	GDEFINE_ENUM_VALUE(PASSIVE_EFFECT, LIFE_REGEN		, 0x01);
	GDEFINE_ENUM_VALUE(PASSIVE_EFFECT, MANA_REGEN		, 0x02);
	GDEFINE_ENUM_VALUE(PASSIVE_EFFECT, SHIELD_REPAIR	, 0x04);

#define MAX_ENTITY_TECHNOLOGY_COUNT 15
	GDEFINE_ENUM_TYPE(ENTITY_TECHNOLOGY, uint16_t);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, UNKNOWN		, 0x0000);

	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, BASIC			, 0x0001);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, MECHANIC		, 0x0002);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, DIGITAL		, 0x0004);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, GENETIC		, 0x0008);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, CHEMICAL		, 0x0010);

	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, BALLISTIC		, 0x0020);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, LAUNCHER		, 0x0040);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, SOUND			, 0x0080);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, MICROWAVE		, 0x0100);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, RADIOACTIVE	, 0x0200);

	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, RAY			, 0x0400);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, SATELLITE		, 0x0800);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, MAGIC			, 0x1000);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, ARROW			, 0x2000);
	GDEFINE_ENUM_VALUE(ENTITY_TECHNOLOGY, SHELL			, 0x4000);

	// ------------------------------------ PROJECTILE_CLASS: Declares values for representing ammunition categories -------------------------------------------------
#define MAX_PROJECTILE_CLASS_COUNT 19
	GDEFINE_FLAG_TYPE(PROJECTILE_CLASS, int32_t);	
	// these are the ammo classes that I can think of right now
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	NONE		,	0x00000UL);	// 0000 0000 0000 0000

	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	ARROW		,	0x00001UL);	// This kind of ammo can be reused sometimes, is also very light and can be made from different materials. It can also be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	BULLET		,	0x00002UL);	// This kind of ammo is also very light and can be made from different materials. It can also be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	GRENADE		,	0x00004UL);	// This kind of ammo is heavy and can be made from different materials. It cannot be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	ROCKET		,	0x00008UL);	// This kind of ammo is heavy and can be made from different materials. It cannot be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	RAY			,	0x00010UL);	// This kind of ammo comes in batteries. It can't be enchanted. 

	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	DART		,	0x00020UL);	// This kind of ammo is very light and can be thrown without a weapon. It can cause effects from injecting substances to the target. It can't be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	FLAME		,	0x00040UL);	// This kind of ammo is meant for flamethrowers and has particular collision behaviours.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	FLUID		,	0x00080UL);	// This kind of ammo is meant for acid throwers and has particular collision behaviours--such as in "particle collision" behaviours.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	SHELL		,	0x00100UL);	// This kind of ammo throws very light and can be made from different materials. It cannot be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	KNIFE		,	0x00200UL);	// This kind of ammo is very light and can be thrown without a weapon. It can be soaked in toxin. It can also be enchanted.

	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	ROCK		,	0x00400UL);	// This kind of ammo can be reused sometimes, is also very light and can be made from different materials. It can also be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	SHRAPNEL	,	0x00800UL);	// This kind of ammo can be reused sometimes, is also very light and can be made from different materials. It can also be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	BOOMERANG	,	0x01000UL);	// This kind of ammo can be reused sometimes, is also very light and can be made from different materials. It can also be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	LIGHTNING	,	0x02000UL);	// This kind of ammo can be reused sometimes, is also very light and can be made from different materials. It can also be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	SPEAR		,	0x04000UL);	// This kind of ammo is very heavy and can be thrown without a weapon. It can also be enchanted.

	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	WAVE		,	0x08000UL);	// This kind of ammo is very heavy and can be thrown without a weapon. It can also be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	BLAST		,	0x10000UL);	// This kind of ammo is very heavy and can be thrown without a weapon. It can also be enchanted.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	SINGULARITY	,	0x20000UL);	// This kind of ammo is very heavy and can be thrown without a weapon. It can also be enchanted.

	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	TEMPORIZED	,	0x40000UL);	// The ammo can be thrown/shoot/placed and will explode/activate after a specified delay.
	GDEFINE_FLAG_VALUE(PROJECTILE_CLASS,	DETONABLE	,	0x80000UL);	// The ammo can be thrown/shoot/placed and is detonated by the player if he wants to. It may be combined with the AMMO_CLASS_TEMPORIZED flag. 

	// ------------------------------------ AMMO_EFFECT: Declares values for representing properties of ammunition -------------------------------------------------
#define MAX_AMMO_EFFECT_COUNT 17
	GDEFINE_FLAG_TYPE(AMMO_EFFECT, int32_t);	
	GDEFINE_FLAG_VALUE(AMMO_EFFECT,	NONE		,	0x00000UL);// 0000 0000 0000 0000

	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00001UL,	FRAGMENTARY	);// This kind of ammo explodes on impact and release its dangerous fragments to fly around the impact point.
	GDEFINE_FLAG_VALUE(AMMO_EFFECT,	INCENDIARY	,	0x00002UL);// This kind of ammo sets on fire things within a range from the impact point.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00004UL,	SMOKE		);// This kind of ammo reduces visibility.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00008UL,	POISON		);	// This kind of ammo poisons the target.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00010UL,	STUN		);	// This kind of ammo lets the target unconscioucs.

	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00020UL,	EMP			);	// This kind of ammo can damage electronic artifacts.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00040UL,	ACID		);	// This kind of ammo can corrode things, even heavy metal armor plates.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00080UL,	LIGHT		);	// This kind of ammo emits a light that can damage darkness beings. It also sheds light on the environment.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00100UL,	SPORE		);	// This kind of ammo spawns some being on impact.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00200UL,	IMPACT		);	// This kind of ammo damages enemies without heavy armor.

	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x00400UL,	PIERCING	);	// This kind of ammo is better to pierce heavy armor.
	GDEFINE_FLAG_VALUE(AMMO_EFFECT,	BLAST		,	0x00800UL);	// This kind of ammo damages enemies without heavy armor.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x01000UL,	LASER		);	// This kind of ammo burns the objective.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x02000UL,	PLASMA		);	// This kind of ammo burns the objective.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x04000UL,	NEUTRON		);	// This kind of ammo causes direct damage.

	GDEFINE_FLAG_VALUE(AMMO_EFFECT,	EXPLOSIVE	,	0x08000UL);	// This kind of ammo causes direct damage.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x10000UL,	SINGULARITY	);	// This kind of ammo causes the surrounding of the impact to be drifted into a singularity event.
	//GDEFINE_FLAG_VALUE(AMMO_EFFECT,	0x0800UL,	RAY			);	// This kind of ammo damages enemies without heavy armor.


#define MAX_ENTITY_GRADE_COUNT 3
	GDEFINE_ENUM_TYPE(ENTITY_GRADE, uint8_t);
	GDEFINE_ENUM_VALUE(ENTITY_GRADE, ILLUSION	, 0x00);
	GDEFINE_ENUM_VALUE(ENTITY_GRADE, LIGHT		, 0x01);
	GDEFINE_ENUM_VALUE(ENTITY_GRADE, MEDIUM		, 0x02);
	GDEFINE_ENUM_VALUE(ENTITY_GRADE, HEAVY		, 0x03);

#define MAX_ATTACK_TYPE_COUNT 13
	GDEFINE_ENUM_TYPE(ATTACK_TYPE, uint16_t);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, NONE			, 0x0000);

	GDEFINE_ENUM_VALUE(ATTACK_TYPE, MELEE			, 0x0001);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, RANGED			, 0x0002);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, BURST			, 0x0004);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, TELEKINETIC		, 0x0020);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, MAGIC			, 0x0040);

	GDEFINE_ENUM_VALUE(ATTACK_TYPE, PSYCHIC			, 0x0080);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, SATELLITE		, 0x0100);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, SPLASH			, 0x0200);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, ANTIPERSONNEL	, 0x0400);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, ANTIAIR			, 0x0800);

	GDEFINE_ENUM_VALUE(ATTACK_TYPE, ANTISHIP		, 0x1000);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, NUKE			, 0x2000);
	GDEFINE_ENUM_VALUE(ATTACK_TYPE, ANTITANK		, 0x4000);

#define COMBAT_STATUS_ALL		((COMBAT_STATUS)		0xFFFFFFFF)
#define ENTITY_TECHNOLOGY_ALL	((ENTITY_TECHNOLOGY)	0xFFFFFFFF)
#define ENTITY_GRADE_ALL		((ENTITY_GRADE)			0xFFFFFFFF)
#define ATTACK_TYPE_ALL			((ATTACK_TYPE)			0xFFFFFFFF)
#define PROJECTILE_CLASS_ALL	((PROJECTILE_CLASS)		0xFFFFFFFF)
#define AMMO_EFFECT_ALL			((AMMO_EFFECT)			0xFFFFFFFF)

#define MAX_EQUIP_MATERIAL_COUNT 16
	enum EQUIP_MATERIAL : uint16_t
		{	EQUIP_MATERIAL_UNKNOWN				= 0x000
		,	EQUIP_MATERIAL_WOOD					= 0x001
		,	EQUIP_MATERIAL_METAL				= 0x002
		,	EQUIP_MATERIAL_BIO					= 0x004
		,	EQUIP_MATERIAL_CLOTH				= 0x008
		,	EQUIP_MATERIAL_POWER				= 0x010
		};

	struct SEntityEffect {
							ATTACK_EFFECT		Attack;
							DEFEND_EFFECT		Defend;
							PASSIVE_EFFECT		Passive;

		inline constexpr	bool				operator ==	(const SEntityEffect& other)	const	noexcept	{ return	Attack == other.Attack && Defend == other.Defend && Passive == other.Passive; }
		inline constexpr	SEntityEffect		operator |	(const SEntityEffect& other)	const	noexcept	{ return{ (ATTACK_EFFECT)(Attack | other.Attack), (DEFEND_EFFECT)(Defend | other.Defend), (PASSIVE_EFFECT)(Passive | other.Passive) }; }
							void				Print		()								const				{
			printf("- Flags for Attack Effect ...: 0x%.08x.\n"	, (int32_t)	Attack	);
			printf("- Flags for Defend Effect ---: 0x%.08x.\n"	, (int32_t)	Defend	);
			printf("- Flags for Passive Effect ..: 0x%.08x.\n"	, (int32_t)	Passive	);
		}
	};

	struct SEntityStatus {
							COMBAT_STATUS		Inflict;
							COMBAT_STATUS		Immunity;

		inline constexpr	bool				operator ==	(const SEntityStatus& other)	const	noexcept	{ return Inflict == other.Inflict && Immunity == other.Immunity;	};
		inline constexpr	SEntityStatus		operator |	(const SEntityStatus& other)	const	noexcept	{ return { (COMBAT_STATUS)(Inflict | other.Inflict), (COMBAT_STATUS)(Immunity | other.Immunity) }; };
							void				Print		()								const
		{
			printf("- Flags for Status Inflict ..: 0x%.08x.\n"	, (int32_t)	Inflict		);
			printf("- Flags for Status Immunity -: 0x%.08x.\n"	, (int32_t)	Immunity	);
		}										
	};

	// ------------------------------------ GENDER: Defines values for character gender and sexual orientation (!!) ------------------------------------ 
	GDEFINE_FLAG_TYPE(GENDER, int8_t);	// 2 bits for gender and 2 bits for sexual orientation, everything else for padding
	GDEFINE_FLAG_VALUE(GENDER,	HYBRID			,	0x00UL);
	GDEFINE_FLAG_VALUE(GENDER,	FEMALE			,	0x01UL);
	GDEFINE_FLAG_VALUE(GENDER,	MALE			,	0x02UL);
	GDEFINE_FLAG_VALUE(GENDER,	HERMAPHRODITE	,	0x03UL);

	struct SEntityGrade {
							ENTITY_TECHNOLOGY	Tech			;
							ENTITY_GRADE		Grade			;
							ATTACK_TYPE			AttackType		;
							PROJECTILE_CLASS	ProjectileClass	;
							AMMO_EFFECT			AmmoEffect		;
							GENDER				Gender			;

		inline constexpr	bool				operator ==	(const SEntityGrade& other)		const	noexcept	{ return Tech == other.Tech && Grade == other.Grade && AttackType == other.AttackType && ProjectileClass == other.ProjectileClass && AmmoEffect == other.AmmoEffect && Gender == other.Gender; }
		inline constexpr	SEntityGrade		operator |	(const SEntityGrade& other)		const	noexcept	{ return{ (ENTITY_TECHNOLOGY)(Tech | other.Tech), (Grade > other.Grade) ? Grade : other.Grade, (ATTACK_TYPE)(AttackType | other.AttackType), (PROJECTILE_CLASS)(ProjectileClass|other.ProjectileClass), (AMMO_EFFECT)(AmmoEffect|other.AmmoEffect), (GENDER)(Gender|other.Gender)}; }
							void				Print		()								const	{
			printf("- Flags for Tech ............: 0x%.08x.\n"	, (int32_t)	Tech		);
			printf("- Grade ---------------------: 0x%.08x.\n"	, (int32_t)	Grade		);
			printf("- Flags for Attack Type .....: 0x%.08x.\n"	, (int32_t)	AttackType	);
			printf("- Flags for Gender ----------: 0x%.08x.\n"	, (int32_t)	Gender		);
		}
	};

#define ENTITY_GRADE_STRUCT_ALL	{ENTITY_TECHNOLOGY_ALL	, ENTITY_GRADE_ALL	,	ATTACK_TYPE_ALL, PROJECTILE_CLASS_ALL, AMMO_EFFECT_ALL, GENDER_HERMAPHRODITE}

	struct SEntityEffectTurns {
							int32_t				Attack;
							int32_t				Defend;
							int32_t				Passive;
	};

	struct SEntityStatusTurns {
							int32_t				Inflict;
							int32_t				Immunity;
	};

	struct SEntityTechTurns {
							int32_t				Tech;
							int32_t				Grade;
							int32_t				AttackType;
	};

	struct SEntityFlagsTurns {
							SEntityEffectTurns	Effect;
							SEntityStatusTurns	Status;
							SEntityTechTurns	Tech;
	};

#pragma pack(pop)
}

#endif // __COMBATSTATUS_H__2386498236498213469817263421__