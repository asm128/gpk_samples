#include "gontrill_base.h"

#ifndef SHIP_H_23627
#define SHIP_H_23627

enum SHIP_CLASS : int8_t
	{ SHIP_CLASS_ASSAULT
	, SHIP_CLASS_INTERCEPTOR
	, SHIP_CLASS_SAUCER
	, SHIP_CLASS_SPAWNER
	, SHIP_CLASS_MOTHER
	, SHIP_CLASS_MONSTER
	, SHIP_CLASS_COUNT
	, SHIP_CLASS_INVALID
	};

struct SShipProperties {
				::gpk::view_const_string				Name;		
				SHIP_CLASS								Class;
				float									Speed;
				SHealthPoints							BaseHealth;
};

static	const SShipProperties						shipProperties		[]			= 
	{ {"Assault"			, SHIP_CLASS_ASSAULT		, 10,    80,   100}
	, {"Interceptor"		, SHIP_CLASS_INTERCEPTOR	, 10,   100,    80}
	, {"Alien Saucer"		, SHIP_CLASS_SAUCER			, 10,   100,   100}
	, {"Mothership"			, SHIP_CLASS_MOTHER			, 10,  1000,  1000}
	, {"Monster Ship"		, SHIP_CLASS_MONSTER		, 10, 10000, 10000}
	, {"Factory Pod"		, SHIP_CLASS_SPAWNER		, 10,    50,   100}
	, {"Wormhole Endpoint"	, SHIP_CLASS_SPAWNER		, 10, 10000,     0}
	};

struct SShipState {
				bool									Thrust							: 1;
				bool									Firing							: 1;
				bool									Brakes							: 1;
};

enum SHIP_CONTROL_TYPE : int8_t
	{ SHIP_CONTROL_TYPE_USER
	, SHIP_CONTROL_TYPE_PATTERN
	, SHIP_CONTROL_TYPE_SEEK
	};

struct SShip {
				int32_t									PropertiesShip					;
				int32_t									PropertiesWeapon				;
				SHealthPoints							HealthCurrent					;
				SHIP_CONTROL_TYPE						ControlType						;
};

#endif // SHIP_H_23627
