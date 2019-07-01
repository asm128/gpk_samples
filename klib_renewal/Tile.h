#include "EntityImpl.h"
#include "EntityHelper.h"

#ifndef __TILE_H__89234092364098273468027364329__
#define __TILE_H__89234092364098273468027364329__

namespace klib
{

//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<STile> modifiersTile[] = 
//
{	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,100,1}	,{{},{}									, {ENTITY_TECHNOLOGY_BASIC				, ENTITY_GRADE_LIGHT}},	"%s"					}
//,	{{{1,0,0},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,100,1}	,{{},{}									, {ENTITY_TECHNOLOGY_BASIC				, ENTITY_GRADE_LIGHT}},	"%s of Resistance"		}
};

//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<STile> definitionsTile[] = 
// 6th grade	No weapon: five points of hit chance.
{	{{{0,0,0},{0,0,0}	,{0,0, {0,0,0},0,	 0},{0, 0, 0, 0},0	,10		, 1},{{},{COMBAT_STATUS_NONE				},	ENTITY_GRADE_STRUCT_ALL},	"Grass"						}
// 5th grade five points	
,	{{{ 0,0,0},{0,0,0}	,{0,0, {0,0,0},0,	 0},{0,-1, 0,-1},0	,100	, 2},{{},{COMBAT_STATUS_BURN				},	ENTITY_GRADE_STRUCT_ALL},	"Lava"						}
,	{{{ 0,0,0},{0,0,0}	,{0,0, {0,0,0},0,	 0},{0, 0, 0,-1},0	,100	, 2},{{},{COMBAT_STATUS_BURN				},	ENTITY_GRADE_STRUCT_ALL},	"Fire"						}
,	{{{ 0,0,0},{0,0,0}	,{0,0, {0,0,0},0,	 0},{0, 0, 0, 0},0	,100	, 2},{{},{COMBAT_STATUS_FREEZING			},	ENTITY_GRADE_STRUCT_ALL},	"Ice"						}
,	{{{ 0,0,0},{0,0,0}	,{0,0, {0,0,0},0,	 0},{0, 0, 0, 0},0	,100	, 2},{{},{COMBAT_STATUS_BLACKOUT			},	ENTITY_GRADE_STRUCT_ALL},	"EMP"						}
,	{{{ 0,0,0},{0,0,0}	,{0,0, {0,0,0},0,	 0},{0, 0, 0, 0},0	,100	, 2},{{},{COMBAT_STATUS_SHOCK				},	ENTITY_GRADE_STRUCT_ALL},	"Lightning"					}
,	{{{-1,0,0},{0,0,0}	,{0,0, {0,0,0},0,	 0},{0, 0, 0, 0},0	,100	, 2},{{},{COMBAT_STATUS_POISON				},	ENTITY_GRADE_STRUCT_ALL},	"Poison"					}
,	{{{ 0,0,0},{0,0,0}	,{0,0, {0,0,0},0,	 0},{0, 0, 0, 0},0	,100	, 2},{{},{COMBAT_STATUS_POISON				},	{ENTITY_TECHNOLOGY_ALL	, ENTITY_GRADE_ALL	,	(ATTACK_TYPE)(ATTACK_TYPE_ALL & (~ATTACK_TYPE_RANGED))	}},	"Trigger disable"			}
};

#endif // __TILE_H__89234092364098273468027364329__
