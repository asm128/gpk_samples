#include "EntityImpl.h"
#include "EntityHelper.h"

#ifndef __ACCESSORY_H__928402836490238648723432__
#define __ACCESSORY_H__928402836490238648723432__

namespace klib
{
#pragma pack(push, 1)
//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<SAccessory> modifiersAccessory[] = 
{	{SEntityPoints({SLifePoints({0,0,0}),SLifePoints({0,0,0}),SAttackPoints({0,0,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,0}),0,    0,  0}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s"				}
,	{SEntityPoints({SLifePoints({0,0,0}),SLifePoints({0,0,0}),SAttackPoints({1,0,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,0}),0, 5000,  5}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"Reinforced %s"		}
,	{SEntityPoints({SLifePoints({0,0,0}),SLifePoints({0,0,0}),SAttackPoints({1,0,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,0}),0,10000, 10}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"Iron %s"			}
,	{SEntityPoints({SLifePoints({0,0,0}),SLifePoints({0,0,0}),SAttackPoints({1,0,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,0}),1,15000, 15}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"Gold %s"			}
,	{SEntityPoints({SLifePoints({0,0,0}),SLifePoints({0,0,0}),SAttackPoints({1,1,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,0}),1,20000, 20}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"Steel %s"			}
,	{SEntityPoints({SLifePoints({0,0,0}),SLifePoints({0,0,0}),SAttackPoints({2,1,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,0}),1,25000, 25}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"Glass %s"			}
,	{SEntityPoints({SLifePoints({0,0,1}),SLifePoints({0,0,0}),SAttackPoints({2,1,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,0}),1,30000, 30}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"Titanium %s"		}
,	{SEntityPoints({SLifePoints({0,0,2}),SLifePoints({0,0,0}),SAttackPoints({2,1,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,0}),1,35000, 35}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"Diamond %s"		}
// - Update																																						  SEntityEffect(  )	SEntityStatus({}) SEntityGrade( 												 ) 
,	{SEntityPoints({SLifePoints({0,0,2}),SLifePoints({0,0,0}),SAttackPoints({2,3,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,0}),1,40000, 40}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_MAGIC	, ENTITY_GRADE_LIGHT}) }),	"Magic %s"			}
,	{SEntityPoints({SLifePoints({0,0,2}),SLifePoints({0,0,0}),SAttackPoints({2,3,SLifePoints({0,0,0}),0,0}),SFitnessPoints({0,0,0,1}),1,45000, 45}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}) }),	"Laser %s"			}
,	{SEntityPoints({SLifePoints({0,0,3}),SLifePoints({0,0,0}),SAttackPoints({2,3,SLifePoints({0,0,0}),0,0}),SFitnessPoints({1,0,0,0}),1,50000, 50}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}) }),	"Power %s"			}
,	{SEntityPoints({SLifePoints({1,1,2}),SLifePoints({0,0,0}),SAttackPoints({2,4,SLifePoints({0,0,0}),1,0}),SFitnessPoints({1,0,0,0}),1,55000, 55}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s of Ambition"	}
,	{SEntityPoints({SLifePoints({1,1,2}),SLifePoints({0,0,0}),SAttackPoints({2,4,SLifePoints({0,0,0}),1,0}),SFitnessPoints({1,0,0,0}),1,60000, 60}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s of Passion"		}
,	{SEntityPoints({SLifePoints({1,1,2}),SLifePoints({0,0,0}),SAttackPoints({2,4,SLifePoints({0,0,0}),1,0}),SFitnessPoints({1,0,0,0}),1,65000, 65}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s of Lust"		}
,	{SEntityPoints({SLifePoints({1,1,2}),SLifePoints({0,0,0}),SAttackPoints({2,4,SLifePoints({0,0,0}),1,0}),SFitnessPoints({1,0,0,0}),1,70000, 70}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s of Confidence"	}
,	{SEntityPoints({SLifePoints({1,1,2}),SLifePoints({0,0,0}),SAttackPoints({2,4,SLifePoints({0,0,0}),1,0}),SFitnessPoints({1,0,0,0}),1,75000, 75}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s of Trust"		}
,	{SEntityPoints({SLifePoints({1,1,2}),SLifePoints({0,0,0}),SAttackPoints({2,4,SLifePoints({0,0,0}),1,0}),SFitnessPoints({1,0,0,0}),1,80000, 80}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s of Anguish"		}
,	{SEntityPoints({SLifePoints({1,1,2}),SLifePoints({1,0,0}),SAttackPoints({2,4,SLifePoints({1,0,0}),1,0}),SFitnessPoints({1,0,0,0}),1,85000, 85}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s of Pain"		}
,	{SEntityPoints({SLifePoints({1,1,2}),SLifePoints({1,1,0}),SAttackPoints({2,4,SLifePoints({1,1,0}),1,0}),SFitnessPoints({1,0,0,0}),1,90000, 90}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s of Love"		}
,	{SEntityPoints({SLifePoints({1,1,2}),SLifePoints({1,1,1}),SAttackPoints({2,4,SLifePoints({1,1,1}),1,0}),SFitnessPoints({1,0,0,0}),1,95000, 95}),SEntityFlags({SEntityEffect({}),SEntityStatus({}),SEntityGrade( {ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}) }),	"%s of the Union"	}
};	
	
// Currently the amount of points available for a single character is 14.	
//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<SAccessory> definitionsAccessory[] =
{	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0}, {1,1,0,0},0, 100,  1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Bracelet"	}
,	{{{0,0,0},{0,0,0}	,{1,0,{0,0,0},0,0}, {0,1,0,0},0,2000, 20}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Hairband"	}
,	{{{1,0,0},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,0,0,0},1,2000, 20}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Hat"		}
,	{{{0,0,0},{0,1,0}	,{1,0,{0,0,0},0,0}, {0,0,0,0},0,2000, 20}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Ring"		}
,	{{{0,0,0},{0,0,0}	,{0,2,{0,0,0},0,0}, {0,0,0,0},0,2000, 20}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Gauntlet"	}
,	{{{0,0,0},{0,0,0}	,{1,0,{0,0,0},0,0}, {0,0,0,0},1,2000, 20}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Monocle"	}
,	{{{0,0,0},{0,0,0}	,{2,0,{0,0,0},0,0}, {0,0,0,2},0,2000, 20}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Scope"		}
//- Update
,	{{{0,1,0},{0,1,0}	,{1,0,{0,0,0},0,0}, {0,0,0,0},0,6000, 60}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Glasses"	}
,	{{{0,0,0},{0,0,0}	,{1,1,{0,0,0},0,0}, {0,1,0,0},0,6000, 60}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Watch"		}
,	{{{0,0,0},{0,0,0}	,{2,0,{0,0,0},0,0}, {0,1,0,0},0,6000, 60}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Leash"		}
,	{{{0,0,0},{0,1,0}	,{2,0,{0,0,0},0,0}, {0,1,1,1},0,6000, 60}	,{{},{COMBAT_STATUS_NONE, COMBAT_STATUS_SLEEP}	,{ENTITY_TECHNOLOGY_BASIC, ENTITY_GRADE_LIGHT	}},	"Implant"	}
};

	static inline	::std::string	getAccessoryName	(const SAccessory& accessory) { return getEntityName	(accessory, definitionsAccessory, modifiersAccessory); }
	static inline	SEntityPoints	getAccessoryPoints	(const SAccessory& accessory) { return getEntityPoints	(accessory, definitionsAccessory, modifiersAccessory); }
	static inline	SEntityFlags	getAccessoryFlags	(const SAccessory& accessory) { return getEntityFlags	(accessory, definitionsAccessory, modifiersAccessory); }

#pragma pack(pop)
}

#endif // __ACCESSORY_H__928402836490238648723432__
