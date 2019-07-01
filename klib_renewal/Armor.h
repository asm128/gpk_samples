#include "EntityImpl.h"
#include "EntityHelper.h"

#ifndef __ARMOR_H__928364982734698273462834__
#define __ARMOR_H__928364982734698273462834__

namespace klib
{
#pragma pack(push, 1)

#define MECH_ARMOR_EFFECTS			{klib::ATTACK_EFFECT_NONE,	klib::DEFEND_EFFECT_NONE, klib::PASSIVE_EFFECT_SHIELD_REPAIR}
#define REGENERATIVE_TECHNOLOGY		((klib::ENTITY_TECHNOLOGY)(klib::ENTITY_TECHNOLOGY_DIGITAL|klib::ENTITY_TECHNOLOGY_GENETIC))
#define POLARIZED_EFFECTS			{{klib::ATTACK_EFFECT_NONE, (klib::DEFEND_EFFECT)(klib::DEFEND_EFFECT_REFLECT|klib::DEFEND_EFFECT_BLIND)}
#define POLARIZED_STATUS			{klib::COMBAT_STATUS_BLIND, klib::COMBAT_STATUS_BLIND}	

	//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<SArmor> modifiersArmor[] = 
{	{{{0,0,0},	{0,0,0}	,{0,0,{},0,0},{0,0,0,0}	,0,    00	,  0},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"%s"					}
,	{{{0,0,0},	{2,1,0}	,{0,0,{},0,0},{0,0,0,0}	,0,  10000	,  1},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"%s of Regeneration"	}
,	{{{0,0,1},	{0,0,0}	,{0,0,{},2,0},{0,0,0,0}	,0,  20000	,  2},{{}, {COMBAT_STATUS_NONE,COMBAT_STATUS_STUN		}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Tough %s"				}
,	{{{0,0,0},	{0,0,0}	,{0,0,{},0,0},{0,0,0,0}	,0,  30000	,  3},{{},{COMBAT_STATUS_NONE, COMBAT_STATUS_BURN		}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Fireproof %s"			}
,	{{{0,0,0},	{0,0,0}	,{0,0,{},0,0},{0,0,0,0}	,0,  30000	,  3},{{},{COMBAT_STATUS_NONE, COMBAT_STATUS_SHOCK		}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Insulating %s"			}
,	{{{0,0,0},	{0,0,0}	,{0,0,{},0,0},{0,0,0,0}	,0,  30000	,  3},{{},{COMBAT_STATUS_NONE, COMBAT_STATUS_POISON		}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Biohazard %s"			}
,	{{{0,0,0},	{0,0,0}	,{0,0,{},0,0},{0,0,0,0}	,0,  30000	,  3},{{},{COMBAT_STATUS_NONE, COMBAT_STATUS_FREEZING	}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Warm %s"				}
// --------------------- Update
,	{{{0,0,0},	{0,0,0}	,{0,0,{},0,0},{0,0,0,0}	,0,  30000	,  4},{{},{COMBAT_STATUS_NONE, COMBAT_STATUS_FROZEN		}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}},	"Self-Heating %s"		}
,	{{{0,0,0},	{0,0,0}	,{0,0,{},1,0},{0,0,0,0}	,0,  40000	,  5},{{ATTACK_EFFECT_NONE, DEFEND_EFFECT_REFLECT},	{}		,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Reflective %s"			}
,	{{{0,0,0},	{0,0,0}	,{0,0,{},0,0},{0,0,0,0}	,0,  40000	,  6},	POLARIZED_EFFECTS,	POLARIZED_STATUS				,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}},	"Polarized %s"			}
,	{{{0,0,0},	{1,0,0}	,{0,0,{},0,0},{0,0,0,0}	,0,  50000	,  7},{	MECH_ARMOR_EFFECTS, {}								,{REGENERATIVE_TECHNOLOGY	, ENTITY_GRADE_LIGHT}},	"Regenerative %s"		}
};

#define MECH_ARMOR_TECHNOLOGY		((klib::ENTITY_TECHNOLOGY)(klib::ENTITY_TECHNOLOGY_DIGITAL|klib::ENTITY_TECHNOLOGY_MECHANIC))
#define BIOMECH_ARMOR_TECHNOLOGY	((klib::ENTITY_TECHNOLOGY)(klib::ENTITY_TECHNOLOGY_DIGITAL|klib::ENTITY_TECHNOLOGY_MECHANIC|klib::ENTITY_TECHNOLOGY_GENETIC))
#define FORCE_FIELD_EFFECTS			{klib::ATTACK_EFFECT_NONE,	klib::DEFEND_EFFECT_IMPENETRABLE,	klib::PASSIVE_EFFECT_SHIELD_REPAIR}

//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<SArmor> definitionsArmor[] = 	
{	{{{0,0,0/2},	{0,0,0}	,{1,0,{0,0,0}, 00,0},{1,1,1,0} 	,0,  100*3,   10},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Underwear"				}
,	{{{0,0,10/2},	{0,0,0}	,{1,0,{0,0,0}, 10,0},{0,1,1,0} 	,0, 1000*3,  100},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Robe"					}
//,	{{{0,0,10/2},	{0,0,0}	,{1,0,{0,0,0}, 10,0},{0,1,1,0} 	,0, 1000*3,  100},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Cloak"					}
,	{{{0,0,20/2},	{0,0,0}	,{0,0,{0,0,0}, 20,0},{0,0,0,0} 	,0, 2000*3,  200},	{{},{COMBAT_STATUS_NONE, COMBAT_STATUS_FREEZING}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Leather Armor"			}
,	{{{0,0,30/2},	{0,0,0}	,{0,0,{0,0,0}, 30,0},{0,0,0,0} 	,0, 3000*3,  300},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Wooden Armor"			}
,	{{{0,0,40/2},	{0,0,0}	,{0,0,{0,0,0}, 40,0},{0,0,0,0} 	,0, 4000*3,  400},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Chainmail Armor"		}
,	{{{0,0,50/2},	{0,0,0}	,{0,0,{0,0,0}, 50,0},{0,0,0,0} 	,0, 5000*3,  500},	{{},{COMBAT_STATUS_NONE, COMBAT_STATUS_BLEEDING}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_HEAVY	}},	"Plate Armor"			}
//----------0/2--------------------0,0,0- -0-0}, Updat,0e 1		 	*30  	 0
,	{{{0,0,60/2},	{0,0,0}	,{0,0,{0,0,0}, 60,0},{0,0,0,0} 	,0, 6000*3,  600},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Kevlar Armor"			}
//,	{{{0,0,50/2},	{0,0,0}	,{0,0,{0,1,0}, 50,0},{0,0,0,0} 	,0, 6000*3,  600},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Combat Armor"			}
//,	{{{0,0,45/2},	{0,0,0}	,{1,0,{0,1,0}, 50,0},{0,0,0,0} 	,0, 6000*3,  600},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Tactical Vest"			}
,	{{{0,0,70/2},	{0,0,0}	,{0,0,{0,0,0}, 70,0},{0,0,0,0} 	,0, 7000*3,  700},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_HEAVY	}},	"Diamond Armor"			}
//,	{{{0,0,70/2},	{0,0,0}	,{0,0,{0,0,0}, 70,0},{0,0,0,0} 	,0, 7000*3,  700},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Mithril Armor"			}
,	{{{0,0,80/2},	{0,0,0}	,{0,0,{0,0,0}, 80,0},{0,0,0,0} 	,0, 8000*3,  800},	{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Dragon Scale Armor"	}
,	{{{0,0,80/2},	{0,1,0}	,{0,0,{0,0,0}, 90,0},{0,0,0,0} 	,0, 9000*3,  900},	{{},{COMBAT_STATUS_BLIND,COMBAT_STATUS_STUN}		,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_HEAVY	}},	"Stardust Armor"		}
//----------0/2--------------------0,0,0- -0-0}, Updat,0e 2			*30 	 0
,	{{{0,0,100/2},	{0,0,0}	,{0,1,{0,0,0}, 90,0},{0,0,0,0} 	,0,10000*3, 1000},	{MECH_ARMOR_EFFECTS	,{COMBAT_STATUS_SHOCK}			,{MECH_ARMOR_TECHNOLOGY		, ENTITY_GRADE_HEAVY	}},	"Mech Armor"			}
,	{{{0,0,90 /2},	{1,0,0}	,{0,1,{0,0,0}, 90,0},{0,0,0,0} 	,0,12000*3, 1200},	{MECH_ARMOR_EFFECTS	,{COMBAT_STATUS_SHOCK}			,{BIOMECH_ARMOR_TECHNOLOGY	, ENTITY_GRADE_MEDIUM	}},	"Biomech Armor"			}
,	{{{0,0,90 /2},	{0,0,0}	,{1,0,{0,0,0},100,0},{1,1,1,0} 	,0,15000*3, 1500},	{FORCE_FIELD_EFFECTS,{}								,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT	}},	"Force Field"			}
};

	static inline	::std::string	getArmorName	(const SArmor& armor) { return getEntityName	(armor, definitionsArmor, modifiersArmor);	}
	static inline	SEntityPoints	getArmorPoints	(const SArmor& armor) { return getEntityPoints	(armor, definitionsArmor, modifiersArmor);	}
	static inline	SEntityFlags	getArmorFlags	(const SArmor& armor) { return getEntityFlags	(armor, definitionsArmor, modifiersArmor);	}

	static inline constexpr int32_t getArmorAbsorption(const SArmor& armor) {
		return	(int32_t)((definitionsArmor[armor.Definition].Points.Attack.Absorption + modifiersArmor[armor.Modifier].Points.Attack.Absorption) * armor.Level);
	}

#pragma pack(pop)
}

#endif // __ARMOR_H__928364982734698273462834__
