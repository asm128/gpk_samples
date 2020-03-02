#include "Character.h"
#include "CombatStatus.h"

#ifndef __ENEMY_H__923409234902364897234__
#define __ENEMY_H__923409234902364897234__

//#define BEHEMOT_STATUS_INFLICT		((klib::COMBAT_STATUS)(klib::COMBAT_STATUS_POISON	|klib::COMBAT_STATUS_BLEEDING	))
//#define ZEUS_STATUS_INFLICT			((klib::COMBAT_STATUS)(klib::COMBAT_STATUS_BLIND	|klib::COMBAT_STATUS_SHOCK		))
//#define TIAMAT_STATUS_INFLICT		((klib::COMBAT_STATUS)(klib::COMBAT_STATUS_CHARMED	|klib::COMBAT_STATUS_STUN		|klib::COMBAT_STATUS_BLEEDING	|klib::COMBAT_STATUS_PETRIFY	))
#define TIAMAT_STATUS_IMMUNITY		((::klib::COMBAT_STATUS)(::klib::COMBAT_STATUS_BLIND	|::klib::COMBAT_STATUS_BLEEDING	|::klib::COMBAT_STATUS_BURN		|::klib::COMBAT_STATUS_FROZEN	|::klib::COMBAT_STATUS_POISON	|::klib::COMBAT_STATUS_PETRIFY	))
//#define DEATH_STATUS_INFLICT		((::klib::COMBAT_STATUS)(::klib::COMBAT_STATUS_BLIND	|::klib::COMBAT_STATUS_STUN		|::klib::COMBAT_STATUS_BLEEDING	|::klib::COMBAT_STATUS_BURN		|::klib::COMBAT_STATUS_FREEZING	|::klib::COMBAT_STATUS_POISON	|::klib::COMBAT_STATUS_PETRIFY	|::klib::COMBAT_STATUS_PANIC	))
#define DEATH_STATUS_IMMUNITY		((::klib::COMBAT_STATUS)(::klib::COMBAT_STATUS_BLIND	|::klib::COMBAT_STATUS_BLEEDING	|::klib::COMBAT_STATUS_BURN		|::klib::COMBAT_STATUS_FROZEN	|::klib::COMBAT_STATUS_POISON	|::klib::COMBAT_STATUS_PANIC	))
//#define SELDON_STATUS_INFLICT		((::klib::COMBAT_STATUS)(::klib::COMBAT_STATUS_BLIND	|::klib::COMBAT_STATUS_STUN		|::klib::COMBAT_STATUS_SHOCK	|::klib::COMBAT_STATUS_BLACKOUT	|::klib::COMBAT_STATUS_PETRIFY	|::klib::COMBAT_STATUS_SLOW		|::klib::COMBAT_STATUS_SLEEP		))
#define SELDON_STATUS_IMMUNITY		((::klib::COMBAT_STATUS)(::klib::COMBAT_STATUS_BLIND	|::klib::COMBAT_STATUS_BLEEDING	|::klib::COMBAT_STATUS_FREEZING	|::klib::COMBAT_STATUS_SLEEP	|::klib::COMBAT_STATUS_SHOCK	|::klib::COMBAT_STATUS_BLACKOUT	|::klib::COMBAT_STATUS_PETRIFY	))

namespace klib
{
#pragma pack(push, 1)
	// (maxHP, hitChance, attack, coins, SEntityFitness, SEntityEffect, SEntityStatus, name)
	static const CCharacter enemyDefinitions[] =
		{ {	    1,		   50,	        1,     100, {1, 4+ 2,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_NONE		},	::gpk::view_const_string{"Normal guy"	}}
		, {	    5,		   50,	        5,    1000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_NONE		},	::gpk::view_const_string{"Slave"		}}
		, {	   10,		   50,	       10,    4000, {1, 4+ 6,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_NONE		},	::gpk::view_const_string{"Terrorist"	}}
		, {	   15,		   50,	       15,   16000, {1, 4+ 8,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_NONE		},	::gpk::view_const_string{"Genocide"		}}
		, {10+10+10,	50+10,		15+10,	 20000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_POISON	},	::gpk::view_const_string{"Behemoth"		}}
		, {10+10+11,	50+11,		15+11,	 21000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_POISON	},	::gpk::view_const_string{"Jesus"		}}
		, {10+10+12,	50+12,		15+12,	 22000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_POISON	},	::gpk::view_const_string{"Dragon"		}}
		, {10+10+13,	50+13,		15+13,	 23000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_POISON	},	::gpk::view_const_string{"Gilgamesh"	}}
		, {10+10+14,	50+14,		15+14,	 24000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_POISON	},	::gpk::view_const_string{"Fenrir"		}}
		, {10+10+15,	50+15,		15+15,	 25000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_POISON	},	::gpk::view_const_string{"Perseus"		}}
		, {10+20+16,	50+16,		15+16,	 26000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_SLEEP		},	::gpk::view_const_string{"Cthulhu"		}}
		, {10+20+17,	50+17,		15+17,	 27000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_BLIND		},	::gpk::view_const_string{"Zeus"			}}
		, {10+20+18,	50+18,		15+18,	 28000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_STUN		},	::gpk::view_const_string{"Thor"			}}
		, {10+20+19,	50+19,		15+19,	 29000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, COMBAT_STATUS_BLEEDING	},	::gpk::view_const_string{"Anhur"		}}
		, {10+20+20,	50+20,		15+20,	 30000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, TIAMAT_STATUS_IMMUNITY	},	::gpk::view_const_string{"Tiamat"		}}
		, {10+40+21,	50+21,		15+21,	 31000, {1, 4+ 4,  0, 10}, {}, {COMBAT_STATUS_NONE, DEATH_STATUS_IMMUNITY	},	::gpk::view_const_string{"Death"		}}
		, {10+80+23,	50+23,		15+23,	 33000, {1, 4+ 4, 10, 10}, {}, {COMBAT_STATUS_NONE, SELDON_STATUS_IMMUNITY	},	::gpk::view_const_string{"asm128"		}}
		};

	// set
	void setupAgent				(const ::klib::SEntityTables & tables, const CCharacter& adventurer, CCharacter& currentEnemy);
	void completeAgentResearch	(const ::klib::SEntityTables & tables, CCharacter& agent);
#pragma pack(pop)
}	// namespace
#endif // __ENEMY_H__923409234902364897234__
