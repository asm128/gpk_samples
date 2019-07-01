#include "EntityImpl.h"
#include "EntityHelper.h"

#ifndef __FACILITIES_H__986349827346982734__
#define __FACILITIES_H__986349827346982734__

namespace klib
{
#pragma pack(push, 1)
//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<SFacility> modifiersFacility[] = 
{	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,	    0	,    0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Small %s"		}
,	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,	10000	, 1000}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Large %s"		}
,	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,	20000	,10000}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Advanced %s"	}
,	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,	50000	,50000}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "%s Hub"		}
};	
	
#define DEFAULT_FACILITY_EFFECTS	{ATTACK_EFFECT_NONE, DEFEND_EFFECT_NONE, }

// Currently the amount of points available for a single character is 14.	
//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<SFacility> definitionsFacility[] =
{	{{{00,0,00},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,     100	, 100}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Dog House"			}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,   10000	,   10}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Training Room"		}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,   20000	,   20}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Cult Center"		}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,   40000	,   40}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Docks"				}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,   80000	,   80}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Foundry"			}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,  160000	,  160}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Warfare Labs"		}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,  320000	,  320}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Hospital"			}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,  640000	,  640}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Physics Labs"		}
// ----------- Update 1					0},		, 0	, 	  00	0  00	00									, ENTITY_GRADE_LIGHT
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0, 1280000	, 1280}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Chemical Labs"		}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0, 2560000	, 2560}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Mechanics Labs"	}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0, 5120000	, 5120}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Airport"			}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,10240000	,10240}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Digital Labs"		}
// ----------- Update 2					0},		, 0	, 	 	00	0 										, ENTITY_GRADE_LIGHT
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,20480000	,20480}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Launch Base"		}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,40960000	,40960}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Quantum Labs"		}
,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,81920000	,81920}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Genetic Labs"		}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,15000	,15}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Space Academy"		}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,10000 ,10}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Landing Drone"		}
//,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,15000	,15}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Planetgate"		}
//,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,15000	,15}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Galaxygate"		}
//,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,15000	,15}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Univergate"		}
//,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,15000	,15}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Darkgate"			}
//,	{{{10,0,10},{1,0,1}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,15000	,15}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Nothingate"		}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Shack"				}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Barn"				}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Wall"				}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Farm"				}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Town Hall"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Barracks"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Tower"				}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Store"				}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Castle"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Lumber Mill"		}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Shipyard"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Foundry"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Coal Mine"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Iron Refinery"		}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Steel Mill"		}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Paper Mill"		}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Oil Wells"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Oil Refinery"		}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Oil Rig"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Coal Power Plant"	}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Airbase"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Seaport"			}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Space Academy"		}
//,	{{{10,0,10},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Lanch Platform"	}
};

	static inline	::std::string	getFacilityName		(const SFacility& facility) { return getEntityName	(facility, definitionsFacility, modifiersFacility); }
	static inline	SEntityPoints	getFacilityPoints	(const SFacility& facility) { return getEntityPoints(facility, definitionsFacility, modifiersFacility); }
	static inline	SEntityFlags	getFacilityFlags	(const SFacility& facility) { return getEntityFlags	(facility, definitionsFacility, modifiersFacility); }

#pragma pack(pop)
}

#endif __FACILITIES_H__986349827346982734__
