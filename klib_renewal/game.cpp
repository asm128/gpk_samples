#include "Game.h"
//
#include "Tile.h"
#include "Item.h"
#include "Enemy.h"
#include "Accessory.h"
#include "Armor.h"
#include "Weapon.h"
#include "Profession.h"
#include "Vehicle.h"
#include "Facility.h"
#include "StageProp.h"

#include <iostream>

//#include "gref_definition.h"

#include <ctime>
#include <algorithm>
#include <string>

::gpk::apod<char>			klib::getItemName					(const ::klib::SItem& item)									{
	char									formattedName[128]					= {};
	sprintf_s(formattedName, ::klib::itemGrades[item.Level].Name.begin(), ::klib::itemDescriptions[item.Definition].Name.begin());
	return ::gpk::view_const_string(formattedName);
}

// Set up a nice prompt
static	::gpk::error_t					prompt					(::gpk::apod<char>& userInput, const ::gpk::vcc& displayText, ::klib::SASCIITarget& asciiTarget) {
	::klib::asciiTargetClear(asciiTarget, ' ', ::klib::ASCII_COLOR_INDEX_GREEN);
	uint32_t									screenWidth				=	asciiTarget.Width()
		,										screenHeight			=	asciiTarget.Height()
		;
	::klib::lineToRect((char*)asciiTarget.Characters.begin(), screenWidth, screenHeight, (screenHeight>>1)-1, 0, ::klib::SCREEN_CENTER, displayText.begin());
	::klib::presentASCIIBackBuffer();

	static const HANDLE							hConsoleOut				= ::GetStdHandle( STD_OUTPUT_HANDLE );
	COORD										cursorPos				= {((SHORT)screenWidth>>1)-5, (SHORT)screenHeight>>1};
	::SetConsoleCursorPosition	(hConsoleOut, cursorPos);
	::SetConsoleTextAttribute	(hConsoleOut, ::klib::ASCII_COLOR_INDEX_GREEN);
	::SetConsoleDisplayMode		(hConsoleOut, CONSOLE_FULLSCREEN_MODE, 0);
	static const HANDLE							hConsoleIn				= ::GetStdHandle( STD_INPUT_HANDLE );
	::FlushConsoleInputBuffer	(hConsoleIn);
	::std::string								consoleInput;
	::std::getline(::std::cin, consoleInput);
	userInput								= {consoleInput.data(), (uint32_t)consoleInput.size()};
	return 0;
}

int64_t								klib::missionCost				(const SGamePlayer& player, const SSquad& squadSetup, uint32_t maxAgents)	{
	int64_t									totalCost						= 0;
	for(uint32_t iAgent=0, agentCount= maxAgents < squadSetup.Agents.size() ? maxAgents : squadSetup.Agents.size(); iAgent<agentCount; ++iAgent) {
		if(squadSetup.Agents[iAgent] == -1)
			continue;

		const ::klib::CCharacter				& agent							= *player.Tactical.Army[squadSetup.Agents[iAgent]];
		const ::klib::SEntityPoints				& finalPoints					= agent.FinalPoints;
		int32_t									healthCurrent					= agent.Points.LifeCurrent.Health;
		if(healthCurrent <= 0) {
			healthCurrent						= 0;
			continue;
		}
		double									penaltyFromHealth				= healthCurrent / (double)finalPoints.LifeMax.Health;//agent.Points.LifeCurrent.Health/(double)finalPoints.LifeMax.Health;
		totalCost							+= finalPoints.CostMaintenance  +  (int64_t)((1.0 - penaltyFromHealth) * agent.Points.CostMaintenance);
		//totalCost							+= agent.Points.CostMaintenance + (int32_t)((penaltyFromHealth-1.0)*agent.Points.CostMaintenance);
	}
	return totalCost;
}

::gpk::error_t						klib::handleMissionEnd			(::klib::SGame& instanceGame) {
	::klib::SGamePlayer						& player						= instanceGame.Players[::klib::PLAYER_INDEX_USER];
	::klib::SPlayerProjects					& playerProjects				= player.Projects;

	if(playerProjects.QueuedProduction.size())
		::klib::handleProductionStep(instanceGame.EntityTables, player.Inventory, playerProjects, player.Tactical.Money, player.Tactical.Score, instanceGame.Messages);

	if(playerProjects.QueuedResearch.size())
		::klib::handleResearchStep(player.Tactical.Research, playerProjects, player.Tactical.Money, player.Tactical.Score, instanceGame.Messages);

	::klib::playerUpdateResearchLists(instanceGame.EntityTables, player);
	::klib::reinitBuyMenus(instanceGame.EntityTables, player.Inventory, instanceGame.ShopMenus);
	return 0;
}

::gpk::error_t			klib::resetGame(SGame& instanceGame) {
	instanceGame.Messages.UserLog.clear();
	::klib::initGame(instanceGame);

	//::klib::clearASCIIBackBuffer(' ', COLOR_WHITE);

	// Set up a nice prompt
	::gpk::apod<char>			playerName;
	::klib::SASCIITarget		asciiTarget;
	::klib::getASCIIBackBuffer(asciiTarget);

	::prompt(instanceGame.Players[::klib::PLAYER_INDEX_USER].Tactical.Name, ::gpk::vcc{"Enter your name:"}, asciiTarget);
	::std::string				password;
	//::prompt(password, "Enter password:");
	instanceGame.Flags		= ::gpk::bit_set  (instanceGame.Flags, ::klib::GAME_FLAGS_STARTED);
	instanceGame.Flags		= ::gpk::bit_clear(instanceGame.Flags, ::klib::GAME_FLAGS_TACTICAL);	// Tell the system that the tactical mode is over.
	instanceGame.Flags		= ::gpk::bit_clear(instanceGame.Flags, ::klib::GAME_FLAGS_TACTICAL_REMOTE);
	return 0;
}

struct SWearables {
	::klib::SProfession		Profession			= {0, 0, 1, -1};
	::klib::SWeapon			Weapon				= {0, 0, 1, -1};
	::klib::SArmor			Armor				= {0, 0, 1, -1};
	::klib::SAccessory		Accessory			= {0, 0, 1, -1};
};

// Sets up initial equipment and items for the player to carry or wear.
::gpk::error_t			klib::initGame		(SGame& instanceGame) {
	instanceGame.EntityTables.Profession	= {definitionsProfession, modifiersProfession	};
	instanceGame.EntityTables.Weapon		= {definitionsWeapon	, modifiersWeapon		};
	instanceGame.EntityTables.Armor			= {definitionsArmor		, modifiersArmor		};
	instanceGame.EntityTables.Accessory		= {definitionsAccessory	, modifiersAccessory	};
	instanceGame.EntityTables.Vehicle		= {definitionsVehicle	, modifiersVehicle		};
	instanceGame.EntityTables.Facility		= {definitionsFacility	, modifiersFacility		};
	instanceGame.EntityTables.StageProp		= {definitionsStageProp	, modifiersStageProp	};
	instanceGame.EntityTables.Tile			= {definitionsTile		, modifiersTile			};
	//instanceGame.EntityTables.Item			= {definitionsItem		, modifiersItem};
	info_printf("sizeof(SEntityPoints): %u"							, (uint32_t) sizeof(::klib::SEntityPoints)										);
	info_printf("sizeof(SEntityFlags): %u"							, (uint32_t) sizeof(::klib::SEntityFlags)										);
	info_printf("sizeof(SCombatBonus): %u"							, (uint32_t) sizeof(::klib::SCombatBonus)										);
	info_printf("sizeof(SCombatStatus): %u"							, (uint32_t) sizeof(::klib::SCombatStatus)										);
	info_printf("sizeof(SCharacterEquip): %u"						, (uint32_t) sizeof(::klib::SCharacterEquip)									);
	info_printf("sizeof(SCharacterTurnBonus): %u"					, (uint32_t) sizeof(::klib::SCharacterTurnBonus)								);
	info_printf("sizeof(SCharacterScore): %u"						, (uint32_t) sizeof(::klib::SCharacterScore)									);
	info_printf("sizeof(SCharacterGoods): %u"						, (uint32_t) sizeof(::klib::SCharacterGoods)									);
	info_printf("sizeof(SCharacter): %u"							, (uint32_t) sizeof(::klib::SCharacter)											);
	info_printf("sizeof(SCharacter)-sizeof(SCharacterGoods): %u"	, (uint32_t)(sizeof(::klib::SCharacter) - sizeof(::klib::SCharacterGoods))		);
	info_printf("sizeof(SGamePlayer): %u"							, (uint32_t) sizeof(::klib::SGamePlayer)										);
	info_printf("sizeof(SGamePlayer)-sizeof(SCharacterGoods): %u"	, (uint32_t)(sizeof(::klib::SGamePlayer) - sizeof(::klib::SCharacterGoods))		);
	info_printf("sizeof(STacticalInfo): %u"							, (uint32_t) sizeof(::klib::STacticalInfo)										);
	info_printf("sizeof(STacticalBoard): %u"						, (uint32_t) sizeof(::klib::STacticalBoard)										);
	info_printf("sizeof(SMapInventory): %u"							, (uint32_t) sizeof(::klib::SMapInventory)										);
	info_printf("sizeof(STacticalInfo)-sizeof(SMapInventory): %u"	, (uint32_t)(sizeof(::klib::STacticalInfo) - sizeof(::klib::SMapInventory))		);
	info_printf("sizeof(STacticalInfo)-sizeof(STacticalBoard): %u"	, (uint32_t)(sizeof(::klib::STacticalInfo) - sizeof(::klib::STacticalBoard))	);
	info_printf("sizeof(SGame): %u"									, (uint32_t) sizeof(::klib::SGame)												);

	instanceGame.Flags = ::gpk::bit_clear(instanceGame.Flags, GAME_FLAGS_STARTED			);
	instanceGame.Flags = ::gpk::bit_clear(instanceGame.Flags, GAME_FLAGS_TACTICAL			);
	instanceGame.Flags = ::gpk::bit_clear(instanceGame.Flags, GAME_FLAGS_TACTICAL_REMOTE	);
	instanceGame.Seed										= time(0);
	::srand((unsigned int)instanceGame.Seed);
	static ::klib::SMessageSlow					slowMessage						= {};
	::klib::resetCursorString(slowMessage);
	instanceGame.ClearDisplays();
	instanceGame.TacticalInfo.Clear();
#define CAMPAIGN_AGENT_COUNT 12

	::SWearables												wearablesDeath					= {};
	::SWearables												wearablesTiamat					= {};
	::SWearables												wearablesThor					= {};
	::SWearables												wearablesZeus					= {};
	::SWearables												wearablesAnhur					= {};
	::SWearables												wearablesCthulhu				= {};
	::SWearables												wearablesPerseus				= {};
	::SWearables												wearablesFenrir					= {};
	::SWearables												wearablesGilgamesh				= {};
	::SWearables												wearablesDragon					= {};
	::SWearables												wearablesJesus					= {};
	::SWearables												wearablesBehemoth				= {};
	::SWearables												wearablesAssault				= {};
	::SWearables												wearablesHeavy					= {};
	::SWearables												wearablesSniper					= {};

	wearablesDeath		.Weapon								= {45, 0/*17*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Gauss Death Scythe
	wearablesDeath		.Accessory							= { 1, 0/*16*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Hairband of Anguish
	wearablesDeath		.Armor								= { 1, 9					, 10 + (int16_t)(::rand() % 5), - 1};	// Polarized Robe
	wearablesDeath		.Profession							= {26, 22					, 10 + (int16_t)(::rand() % 5), - 1};	// Spy God

	wearablesTiamat		.Weapon								= {44, 0/*17*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Gauss Tiamat's Roar
	wearablesTiamat		.Accessory							= { 3, 0/*16*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Ring of Anguish
	wearablesTiamat		.Armor								= { 9, 0/* 9*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Polarized Stardust Armor
	wearablesTiamat		.Profession							= {18, 22					, 10 + (int16_t)(::rand() % 5), - 1};	// Sorcerer God

	wearablesAnhur		.Weapon								= {43, 0/*25*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Neutron Anhur's Spear
	wearablesAnhur		.Accessory							= { 8, 0/*11*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Watch of Ambition
	wearablesAnhur		.Armor								= { 2, 0/* 8*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Reflective Leather Armor
	wearablesAnhur		.Profession							= {20, 22					, 10 + (int16_t)(::rand() % 5), - 1};	// Hunter God

	wearablesThor		.Weapon								= {41, 0/*16*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Acid Thor's Mace
	wearablesThor		.Accessory							= { 4, 0/* 6*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Titanium Gauntlet
	wearablesThor		.Armor								= { 5, 0/* 5*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Biohazard Plate Armor
	wearablesThor		.Profession							= {28, 22					, 10 + (int16_t)(::rand() % 5), - 1};	// Commando God

	wearablesZeus		.Weapon								= {42, 0/*21*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Executioner's Zeus' Staff
	wearablesZeus		.Accessory							= { 5, 0/* 8*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Magic Monocle
	wearablesZeus		.Armor								= { 8, 0/* 4*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Insulating Dragon Scale Armor
	wearablesZeus		.Profession							= { 5, 22					, 10 + (int16_t)(::rand() % 5), - 1};	// Lightning Mage God

	wearablesCthulhu	.Weapon								= {40, 0/*21*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Executioner's Cthulhu's Voice
	wearablesCthulhu	.Accessory							= { 2, 8					, 10 + (int16_t)(::rand() % 5), - 1};	// Magic Hat
	wearablesCthulhu	.Armor								= { 7, 0/* 4*/				, 10 + (int16_t)(::rand() % 5), - 1};	// Insulating Diamond Armor
	wearablesCthulhu	.Profession							= {12, 22					, 10 + (int16_t)(::rand() % 5), - 1};	// Illusion Mage God

	wearablesPerseus	.Weapon								= {39,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Perseus' Sword
	wearablesPerseus	.Accessory							= { 1,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Hairband
	wearablesPerseus	.Armor								= { 2,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Leather Armor
	wearablesPerseus	.Profession							= { 1,  20					, 10 + (int16_t)(::rand() % 5), - 1};	// Swordman Demigod

	wearablesFenrir		.Weapon								= {38,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Fenrir's Claw
	wearablesFenrir		.Accessory							= { 9,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Leash
	wearablesFenrir		.Armor								= { 0,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Underwear
	wearablesFenrir		.Profession							= {19,  17					, 10 + (int16_t)(::rand() % 5), - 1};	// Demon Assassin

	wearablesGilgamesh	.Weapon								= {37,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Gilgamesh Whip
	wearablesGilgamesh	.Accessory							= { 0,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Bracelet
	wearablesGilgamesh	.Armor								= { 2,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Leather Armor
	wearablesGilgamesh	.Profession							= {27,  20					, 10 + (int16_t)(::rand() % 5), - 1};	// Engineer Demigod

	wearablesDragon		.Weapon								= {36,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Dragon's Nail
	wearablesDragon		.Accessory							= { 3,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Ring
	wearablesDragon		.Armor								= { 8,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Dragon Scale Armor
	wearablesDragon		.Profession							= { 9,  20					, 10 + (int16_t)(::rand() % 5), - 1};	// Fire Mage Demigod

	wearablesJesus		.Weapon								= {35,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Jesus' Staff
	wearablesJesus		.Accessory							= { 1,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Hairband
	wearablesJesus		.Armor								= { 1,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Robe
	wearablesJesus		.Profession							= { 0,  20					, 10 + (int16_t)(::rand() % 5), - 1};	// John Doe Demigod

	wearablesBehemoth	.Weapon								= {34,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Behemoth Tooth
	wearablesBehemoth	.Accessory							= { 0,   0					, 10 + (int16_t)(::rand() % 5), - 1};	//
	wearablesBehemoth	.Armor								= { 0,   0					, 10 + (int16_t)(::rand() % 5), - 1};	// Underwear
	wearablesBehemoth	.Profession							= {21,  17					, 10 + (int16_t)(::rand() % 5), - 1};	// Demon Heavy

	wearablesAssault	.Weapon								= {27,	1					,  3 + (int16_t)(::rand() % 5), - 1};
	wearablesAssault	.Accessory							= { 8,	3					,  3 + (int16_t)(::rand() % 5), - 1};
	wearablesAssault	.Armor								= { 6,	2					,  3 + (int16_t)(::rand() % 5), - 1};
	wearablesAssault	.Profession							= {22,	1 + (rand() & 3)	,  3 + (int16_t)(::rand() % 5), - 1};

	wearablesHeavy		.Weapon								= {30,	8					,  3 + (int16_t)(::rand() % 5), - 1};
	wearablesHeavy		.Accessory							= { 4,	4					,  3 + (int16_t)(::rand() % 5), - 1};
	wearablesHeavy		.Armor								= {10,	1					,  3 + (int16_t)(::rand() % 5), - 1};
	wearablesHeavy		.Profession							= {21,	1 + (rand() & 3)	,  3 + (int16_t)(::rand() % 5), - 1};

	wearablesSniper		.Weapon								= {29, 5					,  5 + (int16_t)(::rand() % 5), - 1};
	wearablesSniper		.Accessory							= { 6, 9					,  5 + (int16_t)(::rand() % 5), - 1};
	wearablesSniper		.Armor								= {12, 4					,  5 + (int16_t)(::rand() % 5), - 1};
	wearablesSniper		.Profession							= {24, 1 + (rand() & 3)		,  5 + (int16_t)(::rand() % 5), - 1};


	for(uint32_t iPlayer=0, count=1/*MAX_PLAYER_TYPES*/; iPlayer < count; ++iPlayer) {
		::klib::SGamePlayer												& player						= instanceGame.Players[iPlayer]	= SGamePlayer();
		::gpk::pobj<::klib::CCharacter> newAgent;// = ::klib::enemyDefinitions[1 + ::rand() % (::gpk::size(klib::enemyDefinitions)-1)];
		newAgent.create(::klib::enemyDefinitions[1 + ::rand() % (::gpk::size(klib::enemyDefinitions)-1)]);

		for(uint32_t iAgent=0, agentCount=CAMPAIGN_AGENT_COUNT*2; iAgent<agentCount; iAgent++) {
			int32_t iAgentType = (int32_t)((iAgent < CAMPAIGN_AGENT_COUNT) ? ::gpk::size(klib::enemyDefinitions)-iAgent-2 : 1);
			::gpk::pobj<::klib::CCharacter>							newAgentNext;
			newAgentNext.create(klib::enemyDefinitions[iAgentType]);
			::klib::CCharacter											& adventurer					= *newAgentNext;
			::klib::setupAgent(instanceGame.EntityTables, adventurer, adventurer);
			if(iPlayer == 0) {
				::SWearables wearablesSelected = {};
				//::klib::GENDER genderSelected = GENDER_FEMALE;
				switch(iAgent) {
				case  0:	wearablesSelected	= wearablesDeath		;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_FEMALE	;										break;
				case  1:	wearablesSelected	= wearablesTiamat		;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_FEMALE	;										break;
				case  2:	wearablesSelected	= wearablesAnhur		;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_MALE	;										break;
				case  3:	wearablesSelected	= wearablesThor			;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_MALE	;										break;
				case  4:	wearablesSelected	= wearablesZeus			;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_MALE	;										break;
				case  5:	wearablesSelected	= wearablesCthulhu		;	adventurer.Flags.Tech.Gender	= ::klib::GENDER(::klib::GENDER_MALE | ::klib::GENDER_FEMALE);	break;
				case  6:	wearablesSelected	= wearablesPerseus		;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_MALE;											break;	// "Perseus"
				case  7:	wearablesSelected	= wearablesFenrir		;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_MALE;											break;	// "Fenrir"
				case  8:	wearablesSelected	= wearablesGilgamesh	;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_MALE;											break;	// "Gilgamesh"
				case  9:	wearablesSelected	= wearablesDragon		;	adventurer.Flags.Tech.Gender	= (::klib::GENDER)(1+(::rand()&1));								break;
				case 10:	wearablesSelected	= wearablesJesus		;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_MALE;											break;	// "Jesus"
				case 11:	wearablesSelected	= wearablesBehemoth		;	adventurer.Flags.Tech.Gender	= ::klib::GENDER_MALE;											break;	// "Behemoth"
				case 12:	// Assault
				case 15:
				case 18:
				case 21:
					wearablesSelected										= wearablesAssault;
					wearablesAssault.Weapon.Definition						+= iAgent&1;
					adventurer.Name											= ::gpk::view_const_string{"Slave - Assault"};
					break;
				case 13:	// Heavy
				case 16:
				case 19:
				case 22:
					wearablesSelected										= wearablesHeavy;
					adventurer.Name											= ::gpk::view_const_string{"Slave - Heavy"};
					break;
				case 14:	// Sniper
				case 17:
				case 20:
				case 23:
					wearablesSelected										= wearablesSniper;	// Squaddie Sniper
					adventurer.Name											= ::gpk::view_const_string{"Slave - Sniper"};
					break;
				}

				if(iAgent > CAMPAIGN_AGENT_COUNT) {
					wearablesSelected.Profession	.Modifier				= 1+(::rand()&3);	// Sergeant Assault
					wearablesSelected.Weapon		.Level					= 3+(int16_t)(::rand()%5);	// Shotgun of Resistance
					wearablesSelected.Accessory		.Level					= 3+(int16_t)(::rand()%5);	// Gold Watch
					wearablesSelected.Armor			.Level					= 3+(int16_t)(::rand()%5);	// Tough Kevlar Armor
					wearablesSelected.Profession	.Level					= 3+(int16_t)(::rand()%5);	// Sergeant Assault
					adventurer.Flags.Tech.Gender							= (GENDER)(1+(::rand()&1));
				}

				//wearablesSelected.Weapon		.Level	= 5;
				//wearablesSelected.Accessory		.Level	= 5;
				//wearablesSelected.Armor			.Level	= 5;
				//wearablesSelected.Profession	.Level	= 5;
				switch(iAgent) {
				default: break;
				case 12:
				case 13:
				case 14:
					adventurer.Flags.Tech.Gender							= ::klib::GENDER_FEMALE;
				}

				adventurer.CurrentEquip.Weapon							= wearablesSelected.Weapon		;	// Gauss Death Scythe
				adventurer.CurrentEquip.Accessory						= wearablesSelected.Accessory	;	// Hairband of Anguish
				adventurer.CurrentEquip.Armor							= wearablesSelected.Armor		;	// Polarized Robe
				adventurer.CurrentEquip.Profession						= wearablesSelected.Profession	;	// Spy God

				adventurer.Goods.CompletedResearch						= SCharacterResearch();
				::klib::completeAgentResearch(instanceGame.EntityTables, adventurer);
				adventurer.Recalculate(instanceGame.EntityTables);
				adventurer.Points.LifeCurrent							= adventurer.FinalPoints.LifeMax;
			}

			player.Tactical.Army.push_back(newAgentNext);
		}

		for(uint32_t i=0; i < 2; ++i) {
			player.Inventory.Weapon		.AddElement({2 + ::rand() % 2, 1 + ::rand() % 2, 1, -1});
			player.Inventory.Accessory	.AddElement({2 + ::rand() % 2, 1 + ::rand() % 2, 1, -1});
			player.Inventory.Armor		.AddElement({2 + ::rand() % 2, 1 + ::rand() % 2, 1, -1});
			player.Inventory.Profession	.AddElement({2 + ::rand() % 2, 1 + ::rand() % 2, 1, -1});
		}

		player.Tactical.Squad.Clear(-1);
		player.Tactical.Squad.Agents[0]			= 3;
		for(uint32_t i=0; i < 2; ++i)
			player.Tactical.Squad.Agents[1 + i]		= (int16_t)(CAMPAIGN_AGENT_COUNT + i);

		player.Tactical.Selection				= {0, 0, -1, -1, -1};
		::gpk::apod<char>					& playerName			= player.Tactical.Name;
		char										striPlayer [64]	;
		sprintf_s(striPlayer, " #%i", iPlayer);
		playerName.append_string(striPlayer);

		//player.Name = ::gpk::get_value_label((PLAYER_INDEX)iPlayer);
	}

	::klib::initBuyMenus(instanceGame.EntityTables, instanceGame.ShopMenus);
	::klib::reinitBuyMenus(instanceGame.EntityTables, instanceGame.Players[::klib::PLAYER_INDEX_USER].Inventory, instanceGame.ShopMenus);

	::klib::initTacticalMap(instanceGame);
	instanceGame.Flags = ::gpk::bit_set(instanceGame.Flags, GAME_FLAGS_RUNNING);
	return 0;
}
