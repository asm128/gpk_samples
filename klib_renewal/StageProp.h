#include "EntityImpl.h"
#include "EntityHelper.h"


#ifndef __STAGEPROP_H__98723409213640987236498273__
#define __STAGEPROP_H__98723409213640987236498273__

namespace klib
{
#pragma pack(push, 1)
static const SEntityRecord<SStageProp> modifiersStageProp	[]	= 
{	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0}, {0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"%s"			}
,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0}, {0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Reinforced %s"	}
};																										
																										
static const SEntityRecord<SStageProp> definitionsStageProp	[]	= 												
{	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Mailbox"		}
//,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Pot"			}
//,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Lamp"			}
,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Chest"			}
,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Semaphore"		}
//,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Door"			}
,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Table"			}
,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Streetlight"	}
,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Chair"			}
//,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Glass"			}
//,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Screen"		}
,	{{{0,0,0},	{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0}	,0,0	, 1},{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Wall"			}
};
	static inline	::std::string	getStagePropName	(const SStageProp& stageProp) { return getEntityName	(stageProp, definitionsStageProp, modifiersStageProp); }
	static inline	SEntityPoints	getStagePropPoints	(const SStageProp& stageProp) { return getEntityPoints	(stageProp, definitionsStageProp, modifiersStageProp); }
	static inline	SEntityFlags	getStagePropFlags	(const SStageProp& stageProp) { return getEntityFlags	(stageProp, definitionsStageProp, modifiersStageProp); }

#pragma pack(pop)
}


#endif // __STAGEPROP_H__98723409213640987236498273__
