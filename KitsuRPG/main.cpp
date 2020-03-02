//#define NOMINMAX
#include "Game.h"

#include "Tile.h"
#include "StageProp.h"
#include "Armor.h"
#include "Weapon.h"
#include "Profession.h"
#include "Enemy.h"
#include "Item.h"
#include "Vehicle.h"
#include "Accessory.h"
#include "Facility.h"

#include "GameMenu.h"
#include "klib_ascii_screen.h"

#include <iostream>
#include <algorithm>
#include <time.h>

#include <Windows.h>

// Sets up initial equipment and items for the player to carry or wear.
void											initPlayerCharacter								(const ::klib::SEntityTables & tables, klib::CCharacter& adventurer, const ::gpk::view_const_char& name);

// This function seeds the rand() and enters the tavern() after initializing the player.
// If the player leaves the tavern() it means the game was requested to close.
// After leaving the tavern() we display the score of the player.
int												main											(int argc, char **argv)											{
	(void)argc, (void)argv;
//#if defined(GPK_DEBUG_ENABLED)
#if defined(DEBUG) || defined(_DEBUG)
	int													tmp												= _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
	(void)tmp;
#endif
		//|_CRTDBG_DELAY_FREE_MEM_DF);
//	tmp												= (tmp & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_16_DF;	// Clear the upper 16 bits and OR in the desired freqency
//	_CrtSetDbgFlag(tmp);	// Set the new bits
//#endif
	srand((unsigned int)time(NULL));

	::klib::initASCIIScreen(132, 60);

	printf("Welcome Stranger!! who are you?\n");
	printf("My name is: \n");
	::std::string											adventurerName;
	::std::getline(std::cin, adventurerName);

	::klib::CCharacter									* pAdventurer									= new ::klib::CCharacter(4, 50, 1, 100, {1,4}, {}, {klib::COMBAT_STATUS_NONE, klib::COMBAT_STATUS_STUN}, {adventurerName.data(), (uint32_t)adventurerName.size()});
	::klib::CCharacter									& adventurer									= *pAdventurer;

	static const ::klib::SMenuItem<int32_t>				playAgainMenu[]									=
		{	{ 1, "Yes"	}
		,	{ 0, "No"	}
		};

	bool												bPlayAgain										= true;
	::klib::SEntityTables								entityTables;
	entityTables.Profession		= {::klib::definitionsProfession, ::klib::modifiersProfession	};
	entityTables.Weapon			= {::klib::definitionsWeapon	, ::klib::modifiersWeapon		};
	entityTables.Armor			= {::klib::definitionsArmor		, ::klib::modifiersArmor		};
	entityTables.Accessory		= {::klib::definitionsAccessory	, ::klib::modifiersAccessory	};
	entityTables.Vehicle		= {::klib::definitionsVehicle	, ::klib::modifiersVehicle		};
	entityTables.Facility		= {::klib::definitionsFacility	, ::klib::modifiersFacility		};
	entityTables.StageProp		= {::klib::definitionsStageProp	, ::klib::modifiersStageProp	};
	entityTables.Tile			= {::klib::definitionsTile		, ::klib::modifiersTile			};

	while(bPlayAgain) {
		::initPlayerCharacter(entityTables, adventurer, {adventurerName.data(), (uint32_t)adventurerName.size()});

		std::cout << "\nSo, " << adventurer.Name.begin() << "... What brings you here?\n";
		::tavern(entityTables, adventurer);	// Tavern is the main menu of our game.

		printf("\n-- Game Over! --\n");
		::displayScore(adventurer.Score);

		bPlayAgain										= ::displayMenu("Play again? ..", ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{playAgainMenu}) ? true : false;
	}

	if( pAdventurer )
		delete(pAdventurer);

	::klib::shutdownASCIIScreen();
}

//
void											createPlayerCharacter							(klib::CCharacter& adventurer, const ::gpk::view_const_char& name)			{
	::klib::CCharacter									* tempadventurer								= new klib::CCharacter(4, 50, 1, 100, {1,4}, {}, {klib::COMBAT_STATUS_NONE, klib::COMBAT_STATUS_STUN}, name);
	adventurer										= *tempadventurer;
	delete(tempadventurer);
}

void											researchEquipped								(klib::CCharacter& agent)										{
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Weapon		, agent.CurrentEquip.Weapon		);
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Armor		, agent.CurrentEquip.Armor		);
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Accessory	, agent.CurrentEquip.Accessory	);
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Vehicle		, agent.CurrentEquip.Vehicle	);
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Facility	, agent.CurrentEquip.Facility	);
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.Profession	, agent.CurrentEquip.Profession	);
	::klib::completeAgentResearch(agent.Goods.CompletedResearch.StageProp	, agent.CurrentEquip.StageProp	);
}

//
void											initPlayerCharacter								(const ::klib::SEntityTables & tables, ::klib::CCharacter& adventurer, const ::gpk::view_const_char& name)			{
	::createPlayerCharacter(adventurer, name);
	::klib::SCharacterEquip								& currentEquip									= adventurer.CurrentEquip;
	adventurer.Goods.CompletedResearch				= klib::SCharacterResearch();
#if defined(POWER_START)
	currentEquip.Weapon		.Index					= (int16_t)::gpk::min(3U, tables.Weapon		.Definitions.size()-1);
	currentEquip.Accessory	.Index					= (int16_t)::gpk::min(3U, tables.Accessory	.Definitions.size()-1);
	currentEquip.Armor		.Index					= (int16_t)::gpk::min(3U, tables.Armor		.Definitions.size()-1);
	currentEquip.Profession	.Index					= (int16_t)::gpk::min(3U, tables.Profession	.Definitions.size()-1);
	currentEquip.Vehicle	.Index					= (int16_t)::gpk::min(3U, tables.Vehicle	.Definitions.size()-1);
	currentEquip.Facility	.Index					= (int16_t)::gpk::min(3U, tables.Facility	.Definitions.size()-1);

	currentEquip.Weapon		.Modifier				= (int16_t)::gpk::min(3U, tables.Weapon		.Modifiers.size()-1);
	currentEquip.Accessory	.Modifier				= (int16_t)::gpk::min(3U, tables.Accessory	.Modifiers.size()-1);
	currentEquip.Armor		.Modifier				= (int16_t)::gpk::min(3U, tables.Armor		.Modifiers.size()-1);
	currentEquip.Profession	.Modifier				= (int16_t)::gpk::min(3U, tables.Profession	.Modifiers.size()-1);
	currentEquip.Vehicle	.Modifier				= (int16_t)::gpk::min(3U, tables.Vehicle	.Modifiers.size()-1);
	currentEquip.Facility	.Modifier				= (int16_t)::gpk::min(3U, tables.Facility	.Modifiers.size()-1);

	currentEquip.Weapon		.Level					= 10;
	currentEquip.Accessory	.Level					= 10;
	currentEquip.Armor		.Level					= 10;
	currentEquip.Profession	.Level					= 10;
	currentEquip.Vehicle	.Level					= 10;
	currentEquip.Facility	.Level					= 10;
#else
	memset(&currentEquip, 0, sizeof(klib::SCharacterEquip));
#endif

	adventurer.Goods.Inventory.Weapon		.AddElement({1,1,1});
	adventurer.Goods.Inventory.Accessory	.AddElement({1,1,1});
	adventurer.Goods.Inventory.Armor		.AddElement({1,1,1});
	adventurer.Goods.Inventory.Profession	.AddElement({adventurer.CurrentEquip.Profession.Definition,1,10});
	adventurer.Goods.Inventory.Vehicle		.AddElement({1,1,1});
	adventurer.Goods.Inventory.Facility		.AddElement({1,1,1});

	int16_t i = 5;
	adventurer.Goods.Inventory.Weapon		.AddElement({rand() % (int16_t)tables.Weapon		.Definitions.size(), rand()%(int16_t)tables.Weapon		.Modifiers.size(), ++i});
	adventurer.Goods.Inventory.Accessory	.AddElement({rand() % (int16_t)tables.Accessory		.Definitions.size(), rand()%(int16_t)tables.Accessory	.Modifiers.size(), ++i});
	adventurer.Goods.Inventory.Armor		.AddElement({rand() % (int16_t)tables.Armor			.Definitions.size(), rand()%(int16_t)tables.Armor		.Modifiers.size(), ++i});
	adventurer.Goods.Inventory.Profession	.AddElement({rand() % (int16_t)tables.Profession	.Definitions.size(), rand()%(int16_t)tables.Profession	.Modifiers.size(), ++i});
	adventurer.Goods.Inventory.Vehicle		.AddElement({rand() % (int16_t)tables.Vehicle		.Definitions.size(), rand()%(int16_t)tables.Vehicle		.Modifiers.size(), ++i});
	adventurer.Goods.Inventory.Facility		.AddElement({rand() % (int16_t)tables.Facility		.Definitions.size(), rand()%(int16_t)tables.Facility	.Modifiers.size(), ++i});

	adventurer.Goods.Inventory.Items		.AddElement({1,1,1});
	for(int32_t j = 1;  j < 3; ++j)
		adventurer.Goods.Inventory.Items		.AddElement({ 1+int16_t(rand()%(::gpk::size(klib::itemDescriptions)-1)), int16_t(1+rand() % ::gpk::size(klib::itemModifiers)), int16_t(rand() % ::gpk::size(klib::itemGrades)) });

	::researchEquipped(adventurer);
	adventurer.Recalculate(tables);
	adventurer.Points.LifeCurrent					= adventurer.FinalPoints.LifeMax;
}


int WINAPI										WinMain
	(	_In_		HINSTANCE	/*hInstance		*/
	,	_In_opt_	HINSTANCE	/*hPrevInstance	*/
	,	_In_		LPSTR		/*lpCmdLine		*/
	,	_In_		int			/*nShowCmd		*/
	)
{
	return (0 > main(__argc, __argv)) ? EXIT_FAILURE : EXIT_SUCCESS;
}
