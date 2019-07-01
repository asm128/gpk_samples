#include "EntityImpl.h"
#include "EntityHelper.h"

#ifndef __PROFESSION_H__986349827346982734__
#define __PROFESSION_H__986349827346982734__

namespace klib
{
#pragma pack(push, 1)

#define DRONE_EFFECTS			{klib::ATTACK_EFFECT_NONE,klib::DEFEND_EFFECT_NONE,klib::PASSIVE_EFFECT_SHIELD_REPAIR}
#define MUTANT_EFFECTS DRONE_EFFECTS

//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<SProfession> modifiersProfession[] = 
{	{{{2+0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0,  100	,	  100	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Rookie %s"			}
,	{{{2+1,0,0},{0,0,0}	,{0,0,{0,0,0},0,0},{0,0,0,0},0, 1000	,	 1000	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Squaddie %s"		}	// points: 1
,	{{{2+1,0,0},{0,0,0}	,{1,0,{0,0,0},0,0},{0,0,0,0},0, 1500	,	 1500	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Corporal %s"		}	// points: 2
,	{{{2+1,1,0},{0,0,0}	,{1,0,{0,0,0},0,0},{0,0,0,0},0, 2000	,	 2000	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Sergeant %s"		}	// points: 3
,	{{{2+1,1,1},{0,0,0}	,{1,0,{0,0,0},0,0},{0,0,0,0},0, 2500	,	 2500	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Lieutenant %s"		}	// points: 4
,	{{{2+1,1,1},{0,0,0}	,{1,0,{0,0,0},0,0},{0,0,0,0},1, 3000	,	 3000	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Captain %s"		}	// points: 5
,	{{{2+1,1,1},{0,0,0}	,{1,1,{0,0,0},0,0},{0,0,0,0},1, 3500	,	 3500	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Major %s"			}	// points: 6
,	{{{2+1,1,1},{0,1,0}	,{1,1,{0,0,0},0,0},{0,0,0,0},1, 4000	,	 4000	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Colonel %s"		}	// points: 7
,	{{{2+2,2,2},{0,1,0}	,{2,2,{0,0,0},0,0},{0,0,0,0},1, 4500	,	 4500	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Champion %s"		}	// points: 12
,	{{{2+2,2,1},{0,1,0}	,{2,1,{0,0,0},0,0},{0,0,0,0},1, 5000	,	 5000	},{{},{COMBAT_STATUS_NONE,COMBAT_STATUS_STUN}			,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Possessed %s"		}	// points: 10
,	{{{2+2,2,2},{1,1,0}	,{2,2,{0,0,0},0,0},{0,0,0,0},1, 5500	,	 5500	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Elite %s"			}	// points: 13
,	{{{2+2,2,2},{1,1,0}	,{2,2,{0,0,0},0,0},{0,0,0,0},2, 6000	,	 6000	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "King %s"			}	// points: 14
// ----2+--------------------- Update						0,		   00	
,	{{{2+1,1,1},{0,1,0}	,{2,1,{0,0,0},0,0},{0,0,0,0},1, 6500	,	 6500	},{{},{COMBAT_STATUS_NONE,COMBAT_STATUS_POISON}			,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Zombie %s"			}	// points: 8
,	{{{2+1,2,1},{0,1,0}	,{2,1,{0,0,0},0,0},{0,0,0,0},1, 7000	,	 7000	},{{},{COMBAT_STATUS_NONE,COMBAT_STATUS_BLIND}			,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Undead %s"			}	// points: 9
,	{{{2+2,2,1},{0,1,0}	,{2,2,{0,0,0},0,0},{0,0,0,0},1, 7500	,	 7500	},{MUTANT_EFFECTS ,{}									,{ENTITY_TECHNOLOGY_GENETIC	, ENTITY_GRADE_LIGHT}}, "Mutant %s"			}	// points: 11
,	{{{2+3,2,2},{1,1,0}	,{2,2,{0,0,0},0,0},{0,0,0,0},2, 8000	,	 8000	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "%s CEO"			}	// points: 15
,	{{{2+3,2,2},{1,1,1}	,{2,3,{0,0,0},0,0},{0,0,0,0},2, 8500	,	 8500	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "%s Mad Scientist"	}	// points: 17
,	{{{2+3,2,2},{1,1,1}	,{2,2,{0,0,0},0,0},{0,0,0,0},2, 9000	,	 9000	},{{},{}												,{ENTITY_TECHNOLOGY_MAGIC	, ENTITY_GRADE_LIGHT}}, "Demon %s"			}	// points: 16
,	{{{2+3,2,2},{1,1,1}	,{3,3,{0,0,0},0,0},{0,0,0,0},2, 9500	,	 9500	},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "Emperor %s"		}	// points: 19
,	{{{2+3,2,2},{1,1,2}	,{3,3,{0,0,0},0,0},{0,0,0,0},2,15000	,	15000	},{{},{COMBAT_STATUS_NONE, COMBAT_STATUS_BLEEDING}		,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Robot %s"			}	// points: 18
,	{{{2+3,2,2},{1,2,2}	,{3,3,{0,0,0},0,0},{0,0,0,0},2,30000	,	30000	},{{},{}												,{ENTITY_TECHNOLOGY_MAGIC	, ENTITY_GRADE_LIGHT}}, "Demigod %s"		}	// points: 20
,	{{{2+3,2,2},{2,2,2}	,{3,3,{0,0,0},0,0},{0,0,0,0},2,35000	,	35000	},{{},{}												,{ENTITY_TECHNOLOGY_MAGIC	, ENTITY_GRADE_LIGHT}}, "Angel %s"			}	// points: 21
,	{{{2+3,2,3},{2,2,2}	,{3,3,{0,0,0},0,0},{0,0,0,0},2,70000	,	70000	},{{},{}												,{ENTITY_TECHNOLOGY_MAGIC	, ENTITY_GRADE_LIGHT}}, "%s God"			}	// points: 22
//,	{{{2+3,2,2},{2,2,2}	,{3,3,{0,0,0},0,0},{0,0,0,0},2,150	,	150},{{},{}												,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT}}, "%s God"			}	// points: 23
};

#define DRONE_COMBAT_STATUS		{klib::COMBAT_STATUS_BURN,klib::COMBAT_STATUS_BLEEDING}
#define VAMPIRE_EFFECTS			{klib::ATTACK_EFFECT_LEECH_HEALTH,klib::DEFEND_EFFECT_NONE,klib::PASSIVE_EFFECT_MANA_REGEN}
#define ANDROID_TECHNOLOGY		(klib::ENTITY_TECHNOLOGY)(klib::ENTITY_TECHNOLOGY_GENETIC|klib::ENTITY_TECHNOLOGY_DIGITAL)

// Currently the amount of points available for a single character is 16.	
//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<SProfession> definitionsProfession[] =
{	{{{1,1,1},{1,1,1}	,{1,1,{1,1,1},1,0},{1,1,1,0},1,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "John Doe"			}	// Low profile - Play dead
,	{{{4,0,2},{0,0,0}	,{2,4,{0,0,0},2,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Swordman"			}	// Critical - Shield
,	{{{2,0,1},{0,0,0}	,{5,1,{0,0,0},0,0},{4,0,0,0},2,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Thief"				}	// Steal - Cloak walk
,	{{{0,5,0},{2,5,0}	,{0,0,{0,0,0},0,0},{2,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Acolyte"			}	// Heal - Blessing
//														  0
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Telekinetic Mage"	}	// AOE - Summon - Split mages into elements?
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Lightning Mage"	}	// AOE - Summon - Split mages into elements?
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Earth Mage"		}	// AOE - Summon - Split mages into elements?
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Poison Mage"		}	// AOE - Summon - Split mages into elements?
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Ice Mage"			}	// AOE - Summon - Split mages into elements?
//														  0
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Fire Mage"			}	// AOE - Summon - Split mages into elements?
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Dark Mage"			}	// AOE - Summon - Split mages into elements?
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Air Mage"			}	// AOE - Summon - Split mages into elements?
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Illusion Mage"		}	// AOE - Summon - Split mages into elements?
,	{{{0,3,0},{0,3,0}	,{3,5,{0,0,0},0,0},{0,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "AIO Mage"			}	// AOE - Summon - Split mages into elements?
//											
,	{{{1,0,1},{0,0,0}	,{4,4,{0,0,0},0,0},{4,0,0,0},0,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Archer"			}	// Arrow shower - Traps
,	{{{2,0,1},{0,0,0}	,{2,4,{0,0,0},2,0},{0,0,0,0},3,1000, 1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Merchant"			}	// Persuade - Coin blow
// ---------------------------				
,	{{{2,0,1},{0,0,0}	,{4,6,{0,0,0},0,0},{0,0,0,0},0,2000, 2}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Executioner"		}	// Push Attack - Combo de danio
,	{{{0,2,0},{0,0,0}	,{4,4,{0,0,0},0,0},{4,0,0,0},0,2000, 2}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Ninja"				}	// Quick walk - Trunk
,	{{{0,2,0},{1,4,0}	,{1,6,{0,0,0},0,0},{0,0,0,0},0,2000, 2}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Sorcerer"			}	// Possess - Summon
,	{{{2,0,1},{0,0,0}	,{3,5,{0,0,0},0,0},{3,0,0,0},0,2000, 2}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Assassin"			}	// Fatal blow - Poison attack
,	{{{1,0,1},{0,0,0}	,{4,5,{0,0,0},0,0},{3,0,0,0},0,2000, 2}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Hunter"			}	// Fatal blow - Poison attack
// ---------------------------				
,	{{{4,0,4},{0,0,0}	,{0,4,{0,0,0},3,0},{0,0,0,0},0,2500, 3}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Heavy"				}	// Defend - Wall break
,	{{{2,0,1},{0,0,0}	,{2,5,{0,0,0},2,0},{2,0,0,0},0,2500, 3}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Assault"			}	// Double attack - Disarm 
,	{{{1,4,0},{1,3,1}	,{2,0,{0,0,0},0,0},{2,0,0,0},0,2500, 3}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Support"			}	// Cure - Cheerleader
,	{{{0,1,0},{0,0,0}	,{7,6,{0,0,0},0,0},{0,0,0,0},0,2500, 3}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Sniper"			}	// Headshot - Pierce wall
,	{{{2,0,2},{1,0,0}	,{1,6,{0,0,0},2,0},{0,0,0,0},0,2500, 3}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Grenadier"			}	// Mine - Demolition
,	{{{0,0,0},{1,1,0}	,{1,6,{0,0,0},2,0},{0,0,0,0},0,2500, 3}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Spy"				}	// Disguise - Incommunicate
,	{{{0,0,1},{1,0,1}	,{5,5,{0,0,0},0,0},{1,0,0,0},0,2500, 3}	,{DRONE_EFFECTS, {},{ENTITY_TECHNOLOGY_BASIC							, ENTITY_GRADE_LIGHT}}, "Engineer"			}	// Turret deploy - Remote vehicle
// ---------------------------				
,	{{{1,0,1},{0,0,0}	,{5,5,{0,0,0},0,0},{2,0,0,0},0,3000, 4}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Commando"			}	// Open fire - Airstrike flare
,	{{{0,0,0},{0,0,1}	,{5,5,{0,0,0},0,0},{3,0,0,0},0,3000, 4}	,{DRONE_EFFECTS, DRONE_COMBAT_STATUS,{ENTITY_TECHNOLOGY_DIGITAL			, ENTITY_GRADE_LIGHT}}, "Drone"				}	// Analyze weak points - Distance Jump
,	{{{2,1,0},{1,1,0}	,{0,6,{0,0,0},3,0},{0,0,0,0},0,3000, 4}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC										, ENTITY_GRADE_LIGHT}}, "Biochemist"		}	// Transform - Experiment
,	{{{2,0,2},{0,0,0}	,{4,5,{0,0,0},0,0},{1,0,0,0},0,5000, 5}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL										, ENTITY_GRADE_LIGHT}}, "Cyborg"			}	// Laser eye - Rocket punch
,	{{{1,0,1},{0,0,1}	,{6,4,{0,0,0},0,0},{1,0,0,0},0,5000, 5}	,{{},{}	,{ANDROID_TECHNOLOGY											, ENTITY_GRADE_LIGHT}}, "Android"			}	// Attack jump - Decoy hologram
,	{{{3,2,0},{1,1,0}	,{1,5,{0,0,0},0,0},{1,0,0,0},0,5000, 5}	,{VAMPIRE_EFFECTS,{COMBAT_STATUS_BLEEDING},{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT}}, "Vampire"			}	// Hipnotize - Bat swarm 
};


	static inline	::std::string	getProfessionName	(const SProfession& profession) { return getEntityName		(profession, definitionsProfession, modifiersProfession); }
	static inline	SEntityPoints	getProfessionPoints	(const SProfession& profession) { return getEntityPoints	(profession, definitionsProfession, modifiersProfession); }
	static inline	SEntityFlags	getProfessionFlags	(const SProfession& profession) { return getEntityFlags		(profession, definitionsProfession, modifiersProfession); }
	
#pragma pack(pop)
}

#endif __PROFESSION_H__986349827346982734__
