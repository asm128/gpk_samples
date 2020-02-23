//#define NOMINMAX

#include "Game.h"
#include "draw.h"

#include "Accessory.h"
#include "Armor.h"
#include "Weapon.h"
#include "Profession.h"
#include "Vehicle.h"
#include "Facility.h"
#include "StageProp.h"

#include "projects.h"
#include "helper_projects.h"

using namespace klib;

SGameState drawResearchMenu(SGame& instanceGame, const SGameState& returnState)
{
	SPlayer&							player				= instanceGame.Players[PLAYER_INDEX_USER];
	SCharacterGoods&					playerCompany		= player.Goods;
	SCharacterInventory&				playerInventory		= playerCompany.Inventory;
	klib::SCharacterResearch&			researchCompleted	= playerCompany.CompletedResearch;
	::gpk::array_obj<SEntityResearch>&	queuedResearch		= player.Projects.QueuedResearch;

	klib::SCharacterResearch		researchableItems	= {};

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)							\
		generateResearchableList(researchableItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_	\
			, queuedResearch																									\
			, ProgressiveDefinitions_, ProgressiveModifiers_);																	\
		for(iAgent=0; iAgent<armySize; ++iAgent)																				\
		{																														\
			if( 0 == player.Army[iAgent] )																						\
				continue;																										\
																																\
			const CCharacter& playerAgent = *player.Army[iAgent];																\
			generateResearchableListFromAgent( researchableItems.EntityToken_ 													\
				, playerAgent.CurrentEquip.EntityToken_ 																		\
				, playerAgent.Goods.Inventory.EntityToken_ 																		\
				, researchCompleted.EntityToken_																				\
				, queuedResearch																								\
				, ProgressiveDefinitions_, ProgressiveModifiers_																\
				);																												\
		}																														\
																																\
		researchableDefinitions	+= researchableItems.EntityToken_.Definitions.Slots.size();										\
		researchableModifiers	+= researchableItems.EntityToken_.Modifiers.Slots.size();

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)				\
		generateResearchableList(researchableItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_	\
			, queuedResearch																									\
			, ProgressiveDefinitions_, ProgressiveModifiers_);																	\
		for(iAgent=0; iAgent<armySize; ++iAgent) {																				\
			if( 0 == player.Army[iAgent] )																						\
				continue;																										\
																																\
			const CCharacter& playerAgent = *player.Army[iAgent];																\
			generateResearchableListFromAgentNoEquip( researchableItems.EntityToken_ 											\
				, playerAgent.Goods.Inventory.EntityToken_ 																		\
				, researchCompleted.EntityToken_																				\
				, queuedResearch																								\
				, ProgressiveDefinitions_, ProgressiveModifiers_																\
				);																												\
		}																														\
																																\
		researchableDefinitions	+= researchableItems.EntityToken_.Definitions.Slots.size();										\
		researchableModifiers	+= researchableItems.EntityToken_.Modifiers.Slots.size();

	int32_t iAgent;
	const int32_t armySize	= (int32_t)player.Army.size();
	uint32_t researchableDefinitions=0, researchableModifiers=0;
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Accessory	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(StageProp	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Facility	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Vehicle	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Profession	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Weapon		, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Armor		, false, false);

#define MAX_RESEARCH_ITEMS					\
	 gpk::size(definitionsAccessory)		\
	+gpk::size(definitionsWeapon)		\
	+gpk::size(definitionsArmor)			\
	+gpk::size(definitionsProfession)	\
	+gpk::size(definitionsVehicle)		\
	+gpk::size(definitionsFacility)		\
	+gpk::size(definitionsStageProp)		\
	+gpk::size(modifiersAccessory)		\
	+gpk::size(modifiersWeapon)			\
	+gpk::size(modifiersArmor)			\
	+gpk::size(modifiersProfession)		\
	+gpk::size(modifiersVehicle)			\
	+gpk::size(modifiersFacility)		\
	+gpk::size(modifiersStageProp)

	static klib::SMenuItem<SEntityResearch> menuItems[MAX_RESEARCH_ITEMS] = {};

	uint32_t researchableCount=0;
	char composite[256] ={};
	char precompose[256] ={};
	const char* labelEntityType;

#define ADD_RESEARCH_DEFINITIONS(place, type, records)															\
	labelEntityType = ::gpk::get_value_label(type).begin();														\
	for(uint32_t i = 0, count = place.Definitions.Slots.size(); i < count; ++i) {								\
		menuItems[researchableCount].ReturnValue.ResearchIndex	= i;											\
		menuItems[researchableCount].ReturnValue.IsModifier		= false;										\
		int32_t priceUnit = records[place.Definitions[i].Entity].Points.PriceBuy/2;								\
		menuItems[researchableCount].ReturnValue.PriceUnit		= priceUnit;									\
		menuItems[researchableCount].ReturnValue.PricePaid		= 0;											\
		menuItems[researchableCount].ReturnValue.Entity			= {place.Definitions[i].Entity, 0, 1, -1};		\
		sprintf_s(composite, "%s: %s", labelEntityType, records[place.Definitions[i].Entity].Name.begin());		\
		menuItems[researchableCount].ReturnValue.Name			= composite;									\
		sprintf_s(composite, "%-40.40s $%i", menuItems[researchableCount].ReturnValue.Name.c_str(), priceUnit);	\
		menuItems[researchableCount].Text						= composite;									\
		menuItems[researchableCount].ReturnValue.Type			= type;											\
		researchableCount++;																					\
	}

#define ADD_RESEARCH_MODIFIERS(place, type, records, text)														\
	labelEntityType = ::gpk::get_value_label(type).begin();														\
	for(uint32_t i = 0, count = place.Modifiers.Slots.size(); i < count; ++i) {									\
		menuItems[researchableCount].ReturnValue.ResearchIndex	= i;											\
		menuItems[researchableCount].ReturnValue.IsModifier		= true;											\
		int32_t priceUnit = records[place.Modifiers[i].Entity].Points.PriceBuy/2;								\
		menuItems[researchableCount].ReturnValue.PriceUnit		= priceUnit;									\
		menuItems[researchableCount].ReturnValue.PricePaid		= 0;											\
		menuItems[researchableCount].ReturnValue.Entity			= {0, place.Modifiers[i].Entity, 1, -1};		\
		sprintf_s(precompose, records[place.Modifiers[i].Entity].Name.begin(), text);							\
		sprintf_s(composite, "%s: %s", labelEntityType, precompose);											\
		menuItems[researchableCount].ReturnValue.Name			= composite;									\
		sprintf_s(composite, "%-40.40s $%i", menuItems[researchableCount].ReturnValue.Name.c_str(), priceUnit);	\
		menuItems[researchableCount].Text						= composite;									\
		menuItems[researchableCount].ReturnValue.Type			= type;											\
		researchableCount++;																					\
	}

	ADD_RESEARCH_DEFINITIONS(researchableItems.Weapon		, ENTITY_TYPE_WEAPON		, definitionsWeapon		);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Armor		, ENTITY_TYPE_ARMOR			, definitionsArmor		);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Accessory	, ENTITY_TYPE_ACCESSORY		, definitionsAccessory	);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Profession	, ENTITY_TYPE_PROFESSION	, definitionsProfession	);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Vehicle		, ENTITY_TYPE_VEHICLE		, definitionsVehicle	);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Facility		, ENTITY_TYPE_FACILITY		, definitionsFacility	);
	ADD_RESEARCH_DEFINITIONS(researchableItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, definitionsStageProp	);


	ADD_RESEARCH_MODIFIERS(researchableItems.Weapon		, ENTITY_TYPE_WEAPON		, modifiersWeapon		, "Science"				);
	ADD_RESEARCH_MODIFIERS(researchableItems.Armor		, ENTITY_TYPE_ARMOR			, modifiersArmor		, "Technology"			);
	ADD_RESEARCH_MODIFIERS(researchableItems.Accessory	, ENTITY_TYPE_ACCESSORY		, modifiersAccessory	, "Crafting Technique"	);
	ADD_RESEARCH_MODIFIERS(researchableItems.Profession	, ENTITY_TYPE_PROFESSION	, modifiersProfession	, "Rank"				);
	ADD_RESEARCH_MODIFIERS(researchableItems.Vehicle	, ENTITY_TYPE_VEHICLE		, modifiersVehicle		, "Transportation"		);
	ADD_RESEARCH_MODIFIERS(researchableItems.Facility	, ENTITY_TYPE_FACILITY		, modifiersFacility		, "Construction"		);
	ADD_RESEARCH_MODIFIERS(researchableItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, modifiersStageProp	, "Enhacement"			);

	SEntityResearch											selectedChoice							=	drawMenu
		( instanceGame.GlobalDisplay.Screen.View
		, instanceGame.GlobalDisplay.TextAttributes.begin()
		, (size_t)researchableCount
		, "Available Research"
		, ::gpk::view_array<const ::klib::SMenuItem<::klib::SEntityResearch>>{menuItems}
		, instanceGame.FrameInput
		, {"Exit research menu", (int32_t)researchableCount}
		, {"No action selected", -1}
		, 55U
		);
	if(selectedChoice.ResearchIndex == (int32_t)researchableCount)
		return {GAME_STATE_WELCOME_COMMANDER};

	if(selectedChoice.ResearchIndex == -1)
		return returnState;

	instanceGame.ClearMessages();
	switch(selectedChoice.Type) {
	case ENTITY_TYPE_ACCESSORY	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_STAGE_PROP	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_FACILITY	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_VEHICLE	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_PROFESSION	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_WEAPON		: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_ARMOR		: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.UserSuccess); instanceGame.LogSuccess(); break;
	default:
		break;
	}

	return returnState;
}

void				drawBubblesBackground		( SWeightedDisplay & display, double lastTimeSeconds, uint32_t disturbance=2 ) {
		uint32_t				displayWidth				= (int32_t)display.Screen.metrics().x;
		uint32_t				displayDepth				= (int32_t)display.Screen.metrics().y;

		uint64_t				seed						= (uint64_t)(disturbance+lastTimeSeconds*100000*(1+(rand()%100)));
		uint32_t				randBase					= (uint32_t)(lastTimeSeconds*(disturbance+654)*100000			);
		for(uint32_t x=0; x < displayWidth; ++x)
			//if(display.DisplayWeights[displayDepth-1][x] == 0)
			if(	display.Screen[displayDepth-1][x] != '0' &&
				display.Screen[displayDepth-1][x] != 'o' &&
				display.Screen[displayDepth-1][x] != '.' &&
				display.Screen[displayDepth-1][x] != 'O'
			)
			{
				if( rand()%2 ) {
					display.Screen			[displayDepth-1][x] = (::gpk::noise1D(randBase + x, seed + 1203) > 0.0) ? 'o' : (::gpk::noise1D(randBase+561+x, seed+2135) > 0.0) ? '0' : (::gpk::noise1D(randBase+x+6, seed+103) > 0.0) ? '.' : 'O';
					display.DisplayWeights	[displayDepth-1][x] = .000001f;
					display.Speed			[displayDepth-1][x] = rand()*.001f+0.001f;
					display.SpeedTarget		[displayDepth-1][x] = rand()*.0025f+0.001f;
					display.TextAttributes	[displayDepth-1][x] = (rand() % 2)?COLOR_GREEN:COLOR_DARKGREEN;
				}
			}

		for(uint32_t z=1; z<displayDepth; ++z)
			for(uint32_t x=0; x<displayWidth; ++x) {
				if(display.Screen[z][x] == ' ')
					continue;

				display.DisplayWeights[z][x] += (float)(lastTimeSeconds * display.Speed[z][x]);

				if(display.Speed[z][x] < display.SpeedTarget[z][x])
					display.Speed[z][x] += (float)((display.Speed[z][x] * lastTimeSeconds));
				else
					display.Speed[z][x] -= (float)((display.Speed[z][x] * lastTimeSeconds));

				display.Speed[z][x] *= .999f;
			}

		for(uint32_t z=1; z<displayDepth; ++z)
			for(uint32_t x=0; x<displayWidth; ++x) {
				if(display.Screen[z][x] == ' ')
					continue;

				if(display.DisplayWeights[z][x] > 1.0) {
					int randX = (rand()%2) ? rand()%(1+disturbance*2)-disturbance : 0;
					if(1 == z) {
						display.Screen			[0][x]	= ' ';
						display.DisplayWeights	[0][x]	= 0;
						display.Speed			[0][x]	= 0;
						display.SpeedTarget		[0][x]	= 0;
						display.TextAttributes	[0][x]	= COLOR_WHITE;
					}
					else {
						int32_t									xpos				= ::gpk::min(x + randX, displayWidth - 1);
						if((rand()%10) == 0)  {
							display.Screen[z-1][xpos]			= ' ';
							display.DisplayWeights[z-1][xpos]	= 0;
						}
						else {
								 if( '0' == display.Screen[z][x] && z < (displayDepth/5*4))
									display.Screen[z-1][xpos] = 'O';
							else if( 'O' == display.Screen[z][x] && z < (displayDepth/3*2))
									display.Screen[z-1][xpos] = (::gpk::noise1D(randBase+x, seed+12345) > 0.0) ? 'o' : '\'';
							else if( 'o' == display.Screen[z][x] && z < (displayDepth>>1))
									display.Screen[z-1][xpos] = '.';
							else
									display.Screen[z-1][xpos]	= display.Screen[z][x];

							display.TextAttributes	[z-1][xpos]	= (::gpk::noise1D(randBase+x+x, seed+41203) > 0.0) ? COLOR_DARKGREEN : COLOR_GREEN;
							display.DisplayWeights	[z-1][xpos]	= 0.00001f;
							display.Speed			[z-1][xpos]	= display.Speed[z][x];
							display.SpeedTarget		[z-1][xpos]	= (float)::gpk::noiseNormal1D(x, seed) * 20.0f * (z*1.0f/displayDepth)+0.001f;
						}
					}

					display.Screen			[z][x]		= ' ';
					display.DisplayWeights	[z][x]		= 0;
					display.Speed			[z][x]		= 0;
					display.SpeedTarget		[z][x]		= 0;
					display.TextAttributes	[z][x]		= COLOR_WHITE;
				}
			}
	}


SGameState drawResearch(SGame& instanceGame, const SGameState& returnState) {
	const std::string textToPrint = "Research center.";

	bool bDonePrinting = ::klib::getMessageSlow(instanceGame.SlowMessage, {textToPrint.data(), (uint32_t)textToPrint.size()}, instanceGame.FrameTimer.LastTimeSeconds);
	memcpy(&instanceGame.TacticalDisplay.Screen[instanceGame.TacticalDisplay.Screen.metrics().y >> 1][instanceGame.TacticalDisplay.Screen.metrics().x / 2-((uint32_t)strlen(instanceGame.SlowMessage)+1)/2], instanceGame.SlowMessage, strlen(instanceGame.SlowMessage));
	if ( !bDonePrinting )
		return returnState;

	drawBubblesBackground(instanceGame.TacticalDisplay, instanceGame.FrameTimer.LastTimeSeconds);
	return drawResearchMenu(instanceGame, returnState);
};
