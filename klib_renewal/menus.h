#include "drawMenu.h"
#include "klib_ascii_screen.h"
#include "klib_draw_misc.h"
#include "klib_grid.h"

#ifndef __MENUS_H__9237409126340927634987234__
#define __MENUS_H__9237409126340927634987234__

#pragma pack(push, 1)
namespace klib
{
	// We can use 3 bits for the substate modifier and 5 bits for the substate itself?
	enum GAME_SUBSTATE_MODIFIER : uint8_t
		{	GAME_SUBSTATE_MODIFIER_MAIN						= 0x00
		,	GAME_SUBSTATE_MODIFIER_SMALL					= 0x01
		,	GAME_SUBSTATE_MODIFIER_REGULAR					= 0x02
		,	GAME_SUBSTATE_MODIFIER_LARGE					= 0x03
		,	GAME_SUBSTATE_MODIFIER_HUGE						= 0x04
		,	GAME_SUBSTATE_MODIFIER_PRIMARY					= 0x08
		,	GAME_SUBSTATE_MODIFIER_SECONDARY				= 0x10
		};

	enum GAME_SUBSTATE : int8_t
		{	GAME_SUBSTATE_MAIN								= 0
		,	GAME_SUBSTATE_CHARACTER							= 1
		,	GAME_SUBSTATE_PROFESSION						= 2
		,	GAME_SUBSTATE_WEAPON							= 3
		,	GAME_SUBSTATE_ARMOR								= 4
		,	GAME_SUBSTATE_ACCESSORY							= 5
		,	GAME_SUBSTATE_VEHICLE							= 6
		,	GAME_SUBSTATE_FACILITY							= 7
		,	GAME_SUBSTATE_STAGEPROP							= 8
		,	GAME_SUBSTATE_ITEM								= 9
		,	GAME_SUBSTATE_SKILL								= 10
		,	GAME_SUBSTATE_RESET								= 11
		,	GAME_SUBSTATE_SCREEN							= 12
		,	GAME_SUBSTATE_HOTKEY							= 13
		,	GAME_SUBSTATE_EQUIPMENT							= 14
		,	GAME_SUBSTATE_INVENTORY							= 15
		};

	enum GAME_STATE : uint8_t
		{	GAME_STATE_MENU_MAIN							= 0
		,	GAME_STATE_MENU_CONTROL_CENTER
		,	GAME_STATE_MENU_LAN_MISSION
		,	GAME_STATE_MENU_SQUAD_SETUP
		,	GAME_STATE_MENU_CHARACTER_SETUP
		,	GAME_STATE_MENU_RESEARCH
		,	GAME_STATE_MENU_EQUIPMENT
		,	GAME_STATE_MENU_INSPECT
		,	GAME_STATE_MENU_SENSE
		,	GAME_STATE_MENU_UPGRADE
		,	GAME_STATE_MENU_FACTORY
		,	GAME_STATE_MENU_BUY
		,	GAME_STATE_MENU_SELL
		,	GAME_STATE_MENU_USE
		,	GAME_STATE_MENU_OPTIONS
		,	GAME_STATE_MEMORIAL
		,	GAME_STATE_START_MISSION
		,	GAME_STATE_TACTICAL_CONTROL
		,	GAME_STATE_WELCOME_COMMANDER
		,	GAME_STATE_CREDITS
		,	GAME_STATE_EXIT
		};

	struct SGameState {
				GAME_STATE									State;
				GAME_SUBSTATE								Substate;
	};

	// 1
	static	const ::klib::SMenuItem<SGameState>			optionsMain								[] = 
		{	{ { GAME_STATE_WELCOME_COMMANDER	, GAME_SUBSTATE_RESET			}, "Start new game"					}
		,	{ { GAME_STATE_MENU_OPTIONS			, GAME_SUBSTATE_MAIN			}, "Options"						}
		,	{ { GAME_STATE_CREDITS				, GAME_SUBSTATE_MAIN			}, "Credits"						}
		};

	// 2
	static	const ::klib::SMenuItem<SGameState>			optionsMainInGame						[] = 
		{	{ { GAME_STATE_WELCOME_COMMANDER	, GAME_SUBSTATE_MAIN			}, "Continue game"					}
		,	{ { GAME_STATE_WELCOME_COMMANDER	, GAME_SUBSTATE_RESET			}, "Start new game"					}
		,	{ { GAME_STATE_MENU_OPTIONS			, GAME_SUBSTATE_MAIN			}, "Options"						}
		,	{ { GAME_STATE_CREDITS				, GAME_SUBSTATE_MAIN			}, "Credits"						}
		};

	// 3
	static	const ::klib::SMenuItem<SGameState>			optionsConfig							[] =
		{	{ { GAME_STATE_MENU_OPTIONS			, GAME_SUBSTATE_SCREEN			}, "Screen size"					}
		,	{ { GAME_STATE_MENU_OPTIONS			, GAME_SUBSTATE_HOTKEY			}, "Keyboard configuration"			}
		};

	// 5
	static	const ::klib::SMenuItem<SGameState>			optionsInspect							[] =
		{	{ { GAME_STATE_MENU_INSPECT			, GAME_SUBSTATE_PROFESSION		}, "Inspect job license"			}
		,	{ { GAME_STATE_MENU_INSPECT			, GAME_SUBSTATE_WEAPON			}, "Inspect weapon"					}
		,	{ { GAME_STATE_MENU_INSPECT			, GAME_SUBSTATE_ARMOR			}, "Inspect armor"					}
		,	{ { GAME_STATE_MENU_INSPECT			, GAME_SUBSTATE_ACCESSORY		}, "Inspect accessory"				}
		//,	{ { GAME_STATE_MENU_INSPECT			, GAME_SUBSTATE_VEHICLE			}, "Inspect vehicle"				}
		//,	{ { GAME_STATE_MENU_INSPECT			, GAME_SUBSTATE_FACILITY		}, "Inspect building"				}
		,	{ { GAME_STATE_MENU_INSPECT			, GAME_SUBSTATE_ITEM			}, "View items"						}
		,	{ { GAME_STATE_MENU_INSPECT			, GAME_SUBSTATE_CHARACTER		}, "Inspect general information"	}
		};

	// 6
	static	const ::klib::SMenuItem<SGameState>			optionsSense							[] =
		{	{ { GAME_STATE_MENU_SENSE			, GAME_SUBSTATE_PROFESSION		}, "Sense job license"				}
		,	{ { GAME_STATE_MENU_SENSE			, GAME_SUBSTATE_WEAPON			}, "Sense weapon"					}
		,	{ { GAME_STATE_MENU_SENSE			, GAME_SUBSTATE_ARMOR			}, "Sense armor"					}
		,	{ { GAME_STATE_MENU_SENSE			, GAME_SUBSTATE_ACCESSORY		}, "Sense accessory"				}
		//,	{ { GAME_STATE_MENU_SENSE			, GAME_SUBSTATE_VEHICLE			}, "Sense vehicle"					}
		//,	{ { GAME_STATE_MENU_SENSE			, GAME_SUBSTATE_FACILITY		}, "Sense building"					}
		,	{ { GAME_STATE_MENU_SENSE			, GAME_SUBSTATE_ITEM			}, "Peek items"						}
		,	{ { GAME_STATE_MENU_SENSE			, GAME_SUBSTATE_CHARACTER		}, "Sense general information"		}
		};

	// 7
	static	const ::klib::SMenuItem<SGameState>			optionsEquip							[] =
		{	{ { GAME_STATE_MENU_EQUIPMENT		, GAME_SUBSTATE_PROFESSION		}, "Assign job license"				}
		,	{ { GAME_STATE_MENU_EQUIPMENT		, GAME_SUBSTATE_WEAPON			}, "Equip weapon"					}
		,	{ { GAME_STATE_MENU_EQUIPMENT		, GAME_SUBSTATE_ARMOR			}, "Equip armor"					}
		,	{ { GAME_STATE_MENU_EQUIPMENT		, GAME_SUBSTATE_ACCESSORY		}, "Equip accessory"				}
		//,	{ { GAME_STATE_MENU_EQUIPMENT		, GAME_SUBSTATE_VEHICLE			}, "Equip vehicle"					}
		//,	{ { GAME_STATE_MENU_EQUIPMENT		, GAME_SUBSTATE_FACILITY		}, "Assign facility"				}
		,	{ { GAME_STATE_MENU_EQUIPMENT		, GAME_SUBSTATE_ITEM			}, "Equip items"					}
		,	{ { GAME_STATE_MENU_EQUIPMENT		, GAME_SUBSTATE_CHARACTER		}, "Assign another agent"			}
		};

	// 8
	static	const ::klib::SMenuItem<SGameState>			optionsBuy								[] =
		{	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_PROFESSION		}, "Buy job license"				}
		,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_WEAPON			}, "Buy weapons"					}
		,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_ARMOR			}, "Buy armors"						}
		,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_ACCESSORY		}, "Buy accessories"				}
		//,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_VEHICLE			}, "Buy vehicles"					}
		,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_FACILITY		}, "Build facilities"				}
		,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_ITEM			}, "Buy items"						}
		,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_CHARACTER		}, "Hire agents"					}
		};

	// ? This isn't used yet but the idea would be to allow different gr	ades of potions and probably we could expand this menu from the option selected by the user. This way we could save some space as the list of items available for buying is huge already.
	static	const ::klib::SMenuItem<SGameState>			optionsBuySize							[] =
		{	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_ITEM			}, "Small"							}
		,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_ITEM			}, "Regular"						}
		,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_ITEM			}, "Large"							}
		,	{ { GAME_STATE_MENU_BUY				, GAME_SUBSTATE_ITEM			}, "Huge"							}
		};

	// 9
	static	const ::klib::SMenuItem<SGameState>			optionsSell								[] =
		{	{ { GAME_STATE_MENU_SELL			, GAME_SUBSTATE_PROFESSION		}, "Sell job license"				}
		,	{ { GAME_STATE_MENU_SELL			, GAME_SUBSTATE_WEAPON			}, "Sell weapons"					}
		,	{ { GAME_STATE_MENU_SELL			, GAME_SUBSTATE_ARMOR			}, "Sell armors"					}
		,	{ { GAME_STATE_MENU_SELL			, GAME_SUBSTATE_ACCESSORY		}, "Sell accessories"				}
		//,	{ { GAME_STATE_MENU_SELL			, GAME_SUBSTATE_VEHICLE			}, "Sell vehicles"					}
		,	{ { GAME_STATE_MENU_SELL			, GAME_SUBSTATE_FACILITY		}, "Dismantle facilities"			}
		,	{ { GAME_STATE_MENU_SELL			, GAME_SUBSTATE_ITEM			}, "Sell items"						}
		,	{ { GAME_STATE_MENU_SELL			, GAME_SUBSTATE_CHARACTER		}, "Sack agents"					}
		};

	// 12
	static	const ::klib::SMenuItem<SGameState>			optionsControlCenter					[] = 
		{	{ { GAME_STATE_START_MISSION		}, "Start new Mission"			}
		//,	{ { GAME_STATE_MENU_LAN_MISSION		}, "Join LAN Mission"			}
		,	{ { GAME_STATE_MENU_SQUAD_SETUP		}, "Set up Squad"				}
		,	{ { GAME_STATE_MENU_RESEARCH		}, "Visit Labs"					}
		,	{ { GAME_STATE_MENU_UPGRADE			}, "Upgrade center"				}
		,	{ { GAME_STATE_MENU_FACTORY			}, "Factory"					}
		,	{ { GAME_STATE_MENU_BUY				}, "Buy items and equipment"	}
		,	{ { GAME_STATE_MENU_SELL			}, "Sell items and equipment"	}
		,	{ { GAME_STATE_MEMORIAL				}, "Memorial"					}
		};

	// 13
	static	const ::klib::SMenuItem<SGameState>			optionsControlCenterMissionInCourse		[] = 
		{	{ { GAME_STATE_TACTICAL_CONTROL		}, "Continue Mission"			}
		,	{ { GAME_STATE_MENU_SQUAD_SETUP		}, "Set up Squad"				}
		,	{ { GAME_STATE_MENU_RESEARCH		}, "Visit Labs"					}
		,	{ { GAME_STATE_MENU_UPGRADE			}, "Upgrade center"				}
		,	{ { GAME_STATE_MENU_FACTORY			}, "Factory"					}
		,	{ { GAME_STATE_MENU_BUY				}, "Buy items and equipment"	}
		,	{ { GAME_STATE_MENU_SELL			}, "Sell items and equipment"	}
		,	{ { GAME_STATE_MEMORIAL				}, "Memorial"					}
		};

	enum TURN_ACTION : int8_t
		{	TURN_ACTION_MAIN
		,	TURN_ACTION_ATTACK
		,	TURN_ACTION_ITEM
		,	TURN_ACTION_INVENTORY
		,	TURN_ACTION_EQUIPMENT
		,	TURN_ACTION_SKILL
		,	TURN_ACTION_SENSE
		,	TURN_ACTION_RUN
		,	TURN_ACTION_SECURE
		,	TURN_ACTION_CANCEL
		,	TURN_ACTION_CONTINUE
		,	TURN_ACTION_ABORT_MISSION
		,	TURN_ACTION_MOVE
		,	TURN_ACTION_MENUS
		,	TURN_ACTION_AUTOPLAY
		};

	// 14
	static	const ::klib::SMenuItem<TURN_ACTION>		optionsCombatTurn						[] =
		{ { TURN_ACTION_MOVE					, "Move"						}
		, { TURN_ACTION_ATTACK					, "Attack"						}
		, { TURN_ACTION_SKILL					, "Skills"						}
		, { TURN_ACTION_INVENTORY				, "Use items"					}
		, { TURN_ACTION_EQUIPMENT				, "Equip"						}
		, { TURN_ACTION_SENSE					, "Sense"						}
		, { TURN_ACTION_SECURE					, "Secure loot"					}
		, { TURN_ACTION_CANCEL					, "Cancel turn"					}
		, { TURN_ACTION_RUN						, "Run"							}
		, { TURN_ACTION_ABORT_MISSION			, "Abort mission"				}
		, { TURN_ACTION_AUTOPLAY				, "Autoplay"					}
		};

	// 15
	static	const ::klib::SMenuItem<SGameState>			optionsCombatTurnEquip					[] =
		{ {	{ GAME_STATE_TACTICAL_CONTROL, GAME_SUBSTATE_WEAPON					}, "Equip weapon"		}
		, {	{ GAME_STATE_TACTICAL_CONTROL, GAME_SUBSTATE_ARMOR					}, "Equip armor"		}
		, {	{ GAME_STATE_TACTICAL_CONTROL, GAME_SUBSTATE_ACCESSORY				}, "Equip accessory"	}
		};

	// 16
	static	const ::klib::SMenuItem<int32_t>			optionsAreYouSure						[] = 
		{	{ 1, "Yes"	}
		,	{ 0, "No"	}
		};
}

#pragma pack(pop)


#endif // __MENUS_H__9237409126340927634987234__