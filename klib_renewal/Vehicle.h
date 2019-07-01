#include "EntityImpl.h"
#include "EntityHelper.h"

#ifndef __VEHICLES_H__91236982714698236423423__
#define __VEHICLES_H__91236982714698236423423__

namespace klib
{
#pragma pack(push, 1)

//	{SEntityPoints, SEntityFlags, Name}
static const SEntityRecord<SVehicle> modifiersVehicle[] = 
{	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,0,0,0},0, 000,  0}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"%s"					}
,	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,1,0,0},0, 100,  10}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Personal %s"			}
,	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,2,0,0},0, 200,  20}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Scout %s"				}
,	{{{0,0,5},{0,0,0}	,{0,0,{0,0,0},5,0}, {0,0,0,0},2, 300,  30}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_HEAVY	}},	"Cargo %s"				}
,	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,3,0,0},1, 400,  40}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Raid %s"				}
,	{{{0,0,3},{0,0,1}	,{2,5,{0,0,0},4,0}, {0,4,0,0},0, 500,  50}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Combat %s"				}
//-------------------------------0------0}, - U-gr,0de	 	   	0
,	{{{0,0,5},{0,0,0}	,{1,4,{0,0,0},5,0}, {0,5,0,0},0, 600,  60}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Military %s"			}
,	{{{0,0,7},{0,0,1}	,{0,0,{0,0,0},7,0}, {0,0,0,0},1, 700,  70}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_HEAVY	}},	"Armored %s"			}
,	{{{0,0,2},{0,0,0}	,{0,4,{0,0,2},2,0}, {0,5,0,0},0, 800,  80}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT	}},	"Nuclear %s"			}
,	{{{0,1,0},{0,1,0}	,{0,4,{1,0,0},1,0}, {0,7,0,0},0, 900,  90}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT	}},	"Supersonic %s"			}
// -------------------------------------0}, - Upda,0e 2			0
,	{{{2,1,4},{2,2,2}	,{5,7,{1,1,1},4,0}, {0,9,0,0},2,1000, 100}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_LIGHT	}},	"Warp %s"				}
}; //											   
//												   
//	{SEntityPoints, SEntityFlags, Name}			   
// Not sure how to deal with the fucntions		   
static const SEntityRecord<SVehicle> definitionsVehicle[] = 
{	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,1,0,0},0,  10,   1}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Socks"					}
,	{{{0,0,1},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,1,0,0},0,  50,   5}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Boat"					}
,	{{{0,0,1},{0,0,0}	,{0,1,{0,0,0},0,0}, {0,2,0,0},0, 100,  10}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Carriage"				}
,	{{{0,0,2},{0,0,0}	,{0,2,{0,0,0},0,0}, {0,1,0,0},0, 150,  15}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Ship"					}
,	{{{0,0,0},{0,0,0}	,{0,2,{0,0,0},0,0}, {0,2,0,0},0, 200,  20}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Bike"					}
//-------------------------------0------0}, - U-gr,0de	 	   	
,	{{{0,0,0},{0,0,0}	,{0,3,{0,0,0},0,0}, {0,5,0,0},0, 250,  25}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"Motorbike"				}
,	{{{0,0,3},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,5,0,0},0, 300,  30}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Car"					}
,	{{{0,0,4},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,4,0,0},0, 350,  35}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Van"					}
,	{{{0,0,4},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,3,0,0},0, 400,  40}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_HEAVY	}},	"Bus"					}
,	{{{0,0,3},{0,0,0}	,{0,3,{0,0,0},0,0}, {0,4,0,0},0, 450,  45}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"4x4"					}
,	{{{0,0,3},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,4,0,0},0, 500,  50}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Jeep"					}
,	{{{0,0,4},{0,0,0}	,{0,5,{0,0,0},0,0}, {0,3,0,0},0, 550,  55}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_HEAVY	}},	"Truck"					}
,	{{{0,0,0},{0,0,0}	,{0,0,{0,0,0},0,0}, {0,3,0,0},0, 600,  60}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_LIGHT	}},	"ATP"					}
,	{{{0,0,6},{0,0,0}	,{0,5,{0,0,0},0,0}, {0,5,0,0},0, 650,  65}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_HEAVY	}},	"Train"					}
,	{{{0,0,5},{0,0,0}	,{0,3,{0,0,0},0,0}, {0,6,0,0},0, 700,  70}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Plane"					}
,	{{{0,0,1},{0,0,0}	,{0,3,{0,0,0},0,0}, {0,5,0,0},0, 750,  75}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Hovercraft"			}
,	{{{0,0,4},{0,0,0}	,{0,5,{0,0,0},0,0}, {0,7,0,0},0, 800,  80}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_HEAVY	}},	"Aircraft"				}
,	{{{0,0,6},{0,0,0}	,{0,4,{0,0,0},0,0}, {0,0,0,0},0,1000, 100}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_HEAVY	}},	"Aircraft Carrier"		}
,	{{{0,0,6},{0,0,0}	,{0,5,{0,0,0},0,0}, {0,1,0,0},0, 900,  90}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_HEAVY	}},	"Submarine"				}
,	{{{0,0,2},{0,0,0}	,{0,2,{0,0,0},0,0}, {0,4,0,0},0, 950,  95}	,{{},{}	,{ENTITY_TECHNOLOGY_BASIC	, ENTITY_GRADE_MEDIUM	}},	"Helicopter"			}
,	{{{0,0,4},{0,0,0}	,{0,4,{0,0,0},0,0}, {0,8,0,0},0,1000, 100}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_MEDIUM	}},	"Jet"					}
,	{{{0,0,4},{0,0,0}	,{0,4,{0,0,0},0,0}, {0,4,0,0},0, 750,  75}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_HEAVY	}},	"Giant Robot"			}
// ------------------------------0------0}, - U-da,0e 2		  	
,	{{{0,0,4},{0,0,0}	,{0,4,{0,0,0},0,0}, {0,8,0,0},0,2000, 200}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_HEAVY	}},	"Spaceship"				}
,	{{{0,0,4},{0,0,0}	,{0,4,{0,0,0},0,0}, {0,8,0,0},0,3000, 300}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_HEAVY	}},	"Mothership"			}
,	{{{0,0,4},{0,0,4}	,{0,4,{0,0,0},0,0}, {0,8,0,0},0,5000, 500}	,{{},{}	,{ENTITY_TECHNOLOGY_DIGITAL	, ENTITY_GRADE_HEAVY	}},	"Death Planet"			}
};

	static inline	::std::string	getVehicleName		(const SVehicle& vehicle) { return getEntityName	(vehicle, definitionsVehicle, modifiersVehicle); }
	static inline	SEntityPoints	getVehiclePoints	(const SVehicle& vehicle) { return getEntityPoints	(vehicle, definitionsVehicle, modifiersVehicle); }
	static inline	SEntityFlags	getVehicleFlags		(const SVehicle& vehicle) { return getEntityFlags	(vehicle, definitionsVehicle, modifiersVehicle); }


#pragma pack(pop)
}

#endif // __VEHICLES_H__91236982714698236423423__
