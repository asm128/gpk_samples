#include "gpk_view.h"

#ifndef WEAPON_H_23627
#define WEAPON_H_23627

enum WEAPON_TYPE : int8_t
	{ WEAPON_TYPE_ARROW					= 0
	, WEAPON_TYPE_BULLET
	, WEAPON_TYPE_LASER
	, WEAPON_TYPE_PLASMA
	, WEAPON_TYPE_BUBBLE
	, WEAPON_TYPE_SPARK
	, WEAPON_TYPE_POISON
	, WEAPON_TYPE_FIRE
	, WEAPON_TYPE_ROCK
	, WEAPON_TYPE_GRAVITY
	, WEAPON_TYPE_COUNT
	, WEAPON_TYPE_INVALID				= -1
	};

enum SHOT_TYPE 
	{ SHOT_TYPE_SINGLE
	, SHOT_TYPE_SPLASH
	, SHOT_TYPE_OMNI
	, SHOT_TYPE_HOMING
	, SHOT_TYPE_COUNT
	};

struct SWeaponProperties {
			::gpk::view_const_string				Name;		
			WEAPON_TYPE								TypeWeapon;
			SHOT_TYPE								TypeShot;
			float									Speed;
			float									Delay;
			int32_t									Damage;
};

static	const int								MAX_PLAYER_WEAPONS					= 100;
static	const SWeaponProperties					weaponProperties	[]				=   
	{	{"Bow"				, WEAPON_TYPE_ARROW		, SHOT_TYPE_SINGLE,  800.f, 0.4f / 2, 2500}
	,	{"Gun"				, WEAPON_TYPE_BULLET	, SHOT_TYPE_SINGLE,  900.f, 0.2f / 2, 2000}
	,	{"Laser Gun"		, WEAPON_TYPE_LASER		, SHOT_TYPE_SINGLE,10000.f, 0.2f / 2, 2000}
	,	{"Plasma Gun"		, WEAPON_TYPE_PLASMA	, SHOT_TYPE_SINGLE, 5000.f, 0.3f / 2, 4000}
	,	{"Bubble Gun"		, WEAPON_TYPE_BUBBLE	, SHOT_TYPE_SINGLE, 2500.f, 0.2f / 2, 6000}
	,	{"Electricity Gun"	, WEAPON_TYPE_SPARK		, SHOT_TYPE_SINGLE, 8000.f, 0.2f / 2, 2000}
	,	{"Radiation Gun"	, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE,  800.f, 0.2f / 2, 2000}
	,	{"Fire Gun"			, WEAPON_TYPE_FIRE		, SHOT_TYPE_SINGLE,  100.f, 0.1f / 2, 2750}
	,	{"Stone Gun"		, WEAPON_TYPE_ROCK		, SHOT_TYPE_SINGLE,  100.f, 0.1f / 2, 4750}
	,	{"Gravity Gun"		, WEAPON_TYPE_GRAVITY	, SHOT_TYPE_SINGLE,  250.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f / 2,  750}
// 100 + --------------------------------------------------------------------------------------------------------------------------------------------------------------
	,	{"Fire Gun"			, WEAPON_TYPE_ARROW		, SHOT_TYPE_SINGLE, 100.f, 1.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_BULLET	, SHOT_TYPE_SINGLE, 200.f, 2.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_LASER		, SHOT_TYPE_SINGLE, 300.f, 3.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_PLASMA	, SHOT_TYPE_SINGLE, 400.f, 4.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_BUBBLE	, SHOT_TYPE_SINGLE, 500.f, 5.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_SPARK		, SHOT_TYPE_SINGLE, 100.f, 6.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 100.f, 7.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_ARROW		, SHOT_TYPE_SINGLE, 100.f, 8.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_BULLET	, SHOT_TYPE_SINGLE, 100.f, 9.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_LASER		, SHOT_TYPE_SINGLE, 100.f, .10f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_PLASMA	, SHOT_TYPE_SINGLE, 100.f, .20f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_BUBBLE	, SHOT_TYPE_SINGLE, 100.f, .30f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_SPARK		, SHOT_TYPE_SINGLE, 100.f, .40f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 100.f, .50f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_ARROW		, SHOT_TYPE_SINGLE, 100.f, .60f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_BULLET	, SHOT_TYPE_SINGLE, 100.f, .70f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_LASER		, SHOT_TYPE_SINGLE, 100.f, .80f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_PLASMA	, SHOT_TYPE_SINGLE, 100.f, .90f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_BUBBLE	, SHOT_TYPE_SINGLE, 100.f, 1.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_SPARK		, SHOT_TYPE_SINGLE, 100.f, 2.0f,  750}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 100.f, 3.0f,  750}
	};

#endif // WEAPON_H_298364239