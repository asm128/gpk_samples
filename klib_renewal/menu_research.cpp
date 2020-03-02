//#define NOMINMAX

#include "Game.h"
#include "draw.h"

#include "projects.h"
#include "helper_projects.h"

using namespace klib;

template <typename _tEntity>
static int32_t setupMenuItem(::klib::SEntityResearch & menuItemValue, bool bModifier, int32_t index, ::klib::ENTITY_TYPE type, ::gpk::view_const_char labelEntityType, const ::klib::SEntityContainer<int16_t> & place, const ::gpk::view_array<const ::klib::SEntityRecord<_tEntity>> & records, ::gpk::view_const_char text) {
	char															composite			[256]	= {};
	menuItemValue.ResearchIndex			= index;
	int64_t									priceUnit		= records[place[index].Entity].Points.PriceBuy / 2;
	menuItemValue.PriceUnit				= priceUnit;
	menuItemValue.PricePaid				= 0;
	menuItemValue.Entity				= {bModifier ? 0 : place[index].Entity, bModifier ? place[index].Entity : 0, 1, -1};
	menuItemValue.Type					= type;
	sprintf_s(composite, "%s: %s", labelEntityType.begin(), text.begin());
	menuItemValue.Name					= ::gpk::view_const_string{composite};
	return 0;
}

SGameState						drawResearchMenu		(SGame& instanceGame, const SGameState& returnState)
{
	::klib::SPlayer						& player				= instanceGame.Players[PLAYER_INDEX_USER];
	::klib::SCharacterInventory			& playerInventory		= player.Inventory;
	::klib::SCharacterResearch			& researchCompleted		= player.Tactical.Research;
	::gpk::array_obj<SEntityResearch>	& queuedResearch		= player.Projects.QueuedResearch;

	::klib::SCharacterResearch			researchableItems		= {};

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)							\
		generateResearchableList(researchableItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_	\
			, queuedResearch																									\
			, ProgressiveDefinitions_, ProgressiveModifiers_);																	\
		for(iAgent=0; iAgent<armySize; ++iAgent) {																														\
			if( 0 == player.Tactical.Army[iAgent] )																				\
				continue;																										\
																																\
			const CCharacter				& playerAgent			= *player.Tactical.Army[iAgent];							\
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
			if( 0 == player.Tactical.Army[iAgent] )																						\
				continue;																										\
																																\
			const CCharacter& playerAgent = *player.Tactical.Army[iAgent];																\
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

	int32_t						iAgent;
	const int32_t				armySize				= (int32_t)player.Tactical.Army.size();
	uint32_t					researchableDefinitions	= 0
		,						researchableModifiers	= 0
		;
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Accessory	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Profession	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Weapon		, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Armor		, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(StageProp	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Facility	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Vehicle	, false, false);

#define MAX_RESEARCH_ITEMS										\
	 instanceGame.EntityTables.Accessory	.Definitions.size()	\
	+instanceGame.EntityTables.Weapon		.Definitions.size()	\
	+instanceGame.EntityTables.Armor		.Definitions.size()	\
	+instanceGame.EntityTables.Profession	.Definitions.size()	\
	+instanceGame.EntityTables.Vehicle		.Definitions.size()	\
	+instanceGame.EntityTables.Facility		.Definitions.size()	\
	+instanceGame.EntityTables.StageProp	.Definitions.size()	\
	+instanceGame.EntityTables.Accessory	.Modifiers	.size()	\
	+instanceGame.EntityTables.Weapon		.Modifiers	.size()	\
	+instanceGame.EntityTables.Armor		.Modifiers	.size()	\
	+instanceGame.EntityTables.Profession	.Modifiers	.size()	\
	+instanceGame.EntityTables.Vehicle		.Modifiers	.size()	\
	+instanceGame.EntityTables.Facility		.Modifiers	.size()	\
	+instanceGame.EntityTables.StageProp	.Modifiers	.size()

	::gpk::array_obj<::klib::SEntityResearch>	menuItemsValue								= {};
	::gpk::array_obj<::gpk::array_pod<char_t>>	menuItemsText								= {};
	::gpk::array_obj<::gpk::view_const_char>	menuItemsView								= {};
	menuItemsText	.resize(MAX_RESEARCH_ITEMS);
	menuItemsValue	.resize(MAX_RESEARCH_ITEMS);
	menuItemsView	.resize(MAX_RESEARCH_ITEMS);

	uint32_t														researchableCount							= 0;
	::gpk::view_const_char											labelEntityType								= {};
	char															precompose			[256]	= {};

#define ADD_RESEARCH_DEFINITIONS(place, type, records)																											\
	labelEntityType = ::gpk::get_value_label(type);																												\
	for(uint32_t i = 0, count = place.Definitions.Slots.size(); i < count; ++i) {																				\
		menuItemsValue[researchableCount].IsModifier	= false;																								\
		::setupMenuItem(menuItemsValue[researchableCount], false, i, type, labelEntityType, place.Definitions, records, records[place.Definitions[i].Entity].Name);	\
		sprintf_s(precompose, "%-40.40s $%lli", menuItemsValue[researchableCount].Name.begin(), menuItemsValue[researchableCount].PriceUnit);					\
		menuItemsText[researchableCount]							= ::gpk::view_const_string{precompose};														\
		++researchableCount;																																	\
	}

#define ADD_RESEARCH_MODIFIERS(place, type, records, text)																								\
	labelEntityType = ::gpk::get_value_label(type);																										\
	for(uint32_t i = 0, count = place.Modifiers.Slots.size(); i < count; ++i) {																			\
		sprintf_s(precompose, records[place.Modifiers[i].Entity].Name.begin(), text);																	\
		menuItemsValue[researchableCount].IsModifier			= true;																					\
		::setupMenuItem(menuItemsValue[researchableCount], true, i, type, labelEntityType, place.Modifiers, records, ::gpk::view_const_string{precompose});	\
		sprintf_s(precompose, "%-40.40s $%lli", menuItemsValue[researchableCount].Name.begin(), menuItemsValue[researchableCount].PriceUnit);			\
		menuItemsText[researchableCount]						= ::gpk::view_const_string{precompose};													\
		++researchableCount;																															\
	}

	ADD_RESEARCH_DEFINITIONS(researchableItems.Weapon		, ENTITY_TYPE_WEAPON		, instanceGame.EntityTables.Weapon		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Armor		, ENTITY_TYPE_ARMOR			, instanceGame.EntityTables.Armor		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Accessory	, ENTITY_TYPE_ACCESSORY		, instanceGame.EntityTables.Accessory	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Profession	, ENTITY_TYPE_PROFESSION	, instanceGame.EntityTables.Profession	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Vehicle		, ENTITY_TYPE_VEHICLE		, instanceGame.EntityTables.Vehicle		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.Facility		, ENTITY_TYPE_FACILITY		, instanceGame.EntityTables.Facility	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchableItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, instanceGame.EntityTables.StageProp	.Definitions);

	ADD_RESEARCH_MODIFIERS(researchableItems.Weapon		, ENTITY_TYPE_WEAPON		, instanceGame.EntityTables.Weapon		.Modifiers, "Science"				);
	ADD_RESEARCH_MODIFIERS(researchableItems.Armor		, ENTITY_TYPE_ARMOR			, instanceGame.EntityTables.Armor		.Modifiers, "Technology"			);
	ADD_RESEARCH_MODIFIERS(researchableItems.Accessory	, ENTITY_TYPE_ACCESSORY		, instanceGame.EntityTables.Accessory	.Modifiers, "Crafting Technique"	);
	ADD_RESEARCH_MODIFIERS(researchableItems.Profession	, ENTITY_TYPE_PROFESSION	, instanceGame.EntityTables.Profession	.Modifiers, "Rank"					);
	ADD_RESEARCH_MODIFIERS(researchableItems.Vehicle	, ENTITY_TYPE_VEHICLE		, instanceGame.EntityTables.Vehicle		.Modifiers, "Transportation"		);
	ADD_RESEARCH_MODIFIERS(researchableItems.Facility	, ENTITY_TYPE_FACILITY		, instanceGame.EntityTables.Facility	.Modifiers, "Construction"			);
	ADD_RESEARCH_MODIFIERS(researchableItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, instanceGame.EntityTables.StageProp	.Modifiers, "Enhacement"			);

	for(uint32_t i = 0, count = menuItemsText.size(); i < count; ++i)
		menuItemsView[i] = menuItemsText[i];

	static ::klib::SDrawMenuState	menuState;
	if(instanceGame.PreviousState.State != instanceGame.State.State)
		::klib::resetCursorString(menuState.SlowTitle);
	::klib::SEntityResearch									selectedChoice							= ::klib::drawMenu
		( menuState
		, instanceGame.GlobalDisplay.Screen.View
		, instanceGame.GlobalDisplay.TextAttributes.begin()
		, ::gpk::view_const_string{"Available Research"}
		, ::gpk::view_array<const ::klib::SEntityResearch>	{menuItemsValue.begin(), (uint32_t)researchableCount}
		, {menuItemsView.begin(), (uint32_t)researchableCount}
		, instanceGame.FrameInput
		, {::gpk::view_const_string{"Exit research menu"}, (int32_t)researchableCount}
		, {::gpk::view_const_string{"No action selected"}, -1}
		, 55U
		);
	if(selectedChoice.ResearchIndex == (int32_t)researchableCount)
		return {GAME_STATE_WELCOME_COMMANDER};

	if(selectedChoice.ResearchIndex == -1)
		return returnState;

	instanceGame.ClearMessages();
	switch(selectedChoice.Type) {
	case ENTITY_TYPE_ACCESSORY	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_STAGE_PROP	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_FACILITY	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_VEHICLE	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_PROFESSION	: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_WEAPON		: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_ARMOR		: acknowledgeResearch(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
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
								 if( '0' == display.Screen[z][x] && z < (displayDepth/5*4))	display.Screen[z-1][xpos] = 'O';
							else if( 'O' == display.Screen[z][x] && z < (displayDepth/3*2))	display.Screen[z-1][xpos] = (::gpk::noise1D(randBase+x, seed+12345) > 0.0) ? 'o' : '\'';
							else if( 'o' == display.Screen[z][x] && z < (displayDepth>>1))	display.Screen[z-1][xpos] = '.';
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
	const ::gpk::view_const_string					textToPrint					= "Research center.";

	static ::klib::SMessageSlow						slowMessage;
	bool bDonePrinting = ::klib::getMessageSlow(slowMessage, textToPrint, instanceGame.FrameTimer.LastTimeSeconds);
	::gpk::SCoord2<uint32_t>						position					= instanceGame.TacticalDisplay.Screen.metrics() / 2;
	const uint32_t									messageLen					= (uint32_t)strlen(slowMessage.Message);
	memcpy(&instanceGame.TacticalDisplay.Screen[position.y][position.x - (messageLen + 1) / 2], slowMessage.Message, messageLen);
	if ( !bDonePrinting )
		return returnState;

	drawBubblesBackground(instanceGame.TacticalDisplay, instanceGame.FrameTimer.LastTimeSeconds);
	return drawResearchMenu(instanceGame, returnState);
};
