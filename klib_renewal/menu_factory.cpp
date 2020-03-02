//#define NOMINMAX
#include "Game.h"
#include "draw.h"

#include "projects.h"
#include "helper_projects.h"


using namespace klib;

SGameState													drawFactoryMenu										(SGame& instanceGame, const SGameState& returnState)										{
	SPlayer															& player											= instanceGame.Players[PLAYER_INDEX_USER];
	SCharacterInventory												& playerInventory									= player.Inventory;
	::klib::SCharacterResearch										& researchCompleted									= player.Tactical.Research;
	::klib::SCharacterResearch										researchedItems										= {};

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)						\
		generateResearchedList(researchedItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_);	\
		for(iAgent=0; iAgent<armySize; ++iAgent) {																			\
			if( 0 == player.Tactical.Army[iAgent] )																					\
				continue;																									\
																															\
			const CCharacter& playerAgent = *player.Tactical.Army[iAgent];															\
			generateResearchedListFromAgent( researchedItems.EntityToken_ 													\
				, playerAgent.CurrentEquip.EntityToken_ 																	\
				, playerAgent.Goods.Inventory.EntityToken_ 																	\
				, researchCompleted.EntityToken_																			\
			);																												\
		}																													\
		researchedDefinitions	+= researchedItems.EntityToken_.Definitions.Slots.size();									\
		researchedModifiers		+= researchedItems.EntityToken_.Modifiers.Slots.size();

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)			\
		generateResearchedList(researchedItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_);	\
		for(iAgent=0; iAgent<armySize; ++iAgent) {																			\
			if( 0 == player.Tactical.Army[iAgent] )																					\
				continue;																									\
																															\
			const CCharacter& playerAgent = *player.Tactical.Army[iAgent];															\
			generateResearchedListFromAgentNoEquip( researchedItems.EntityToken_ 											\
				, playerAgent.Goods.Inventory.EntityToken_ 																	\
				, researchCompleted.EntityToken_																			\
			);																												\
		}																													\
		researchedDefinitions	+= researchedItems.EntityToken_.Definitions.Slots.size();									\
		researchedModifiers		+= researchedItems.EntityToken_.Modifiers.Slots.size();

	int32_t															iAgent												= 0;
	const int32_t													armySize											= (int32_t)player.Tactical.Army.size();
	uint32_t														researchedDefinitions								= 0
		,															researchedModifiers									= 0
		;
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Accessory	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(StageProp	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Facility	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Vehicle	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Profession	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Weapon		, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Armor		, false, false);

#define MAX_RESEARCH_ITEMS							\
	 instanceGame.EntityTables.Accessory	.Definitions.size()	\
	+instanceGame.EntityTables.Weapon		.Definitions.size()	\
	+instanceGame.EntityTables.Armor		.Definitions.size()	\
	+instanceGame.EntityTables.Profession	.Definitions.size()	\
	+instanceGame.EntityTables.Vehicle		.Definitions.size()	\
	+instanceGame.EntityTables.Facility		.Definitions.size()	\
	+instanceGame.EntityTables.StageProp	.Definitions.size()	\

	::gpk::array_obj<::klib::SEntityResearch>	menuItemsValue								= {};
	::gpk::array_obj<::gpk::view_const_char>	menuItemsText								= {};
	menuItemsText	.resize(MAX_RESEARCH_ITEMS);
	menuItemsValue	.resize(MAX_RESEARCH_ITEMS);
	uint32_t														researchedCount										= 0;
	char															composite			[256]							= {};
	const char														* labelEntityType									= 0;

#define ADD_RESEARCH_DEFINITIONS(place, type, records)																		\
	labelEntityType												= ::gpk::get_value_label(type).begin();						\
	for(uint32_t i=0, count=place.Definitions.Slots.size(); i<count; ++i) {													\
		menuItemsValue[researchedCount].ResearchIndex	= i;														\
		menuItemsValue[researchedCount].IsModifier		= false;													\
		int64_t												priceUnit		= records[place.Definitions[i].Entity].Points.PriceBuy / 2;	\
		menuItemsValue[researchedCount].PriceUnit		= priceUnit;												\
		menuItemsValue[researchedCount].PricePaid		= 0;														\
		menuItemsValue[researchedCount].Entity			= {place.Definitions[i].Entity, 0, 1, -1};					\
		sprintf_s(composite, "%s: %s", labelEntityType, records[place.Definitions[i].Entity].Name.begin());				\
		menuItemsValue[researchedCount].Name			= ::gpk::view_const_string{composite};						\
		sprintf_s(composite, "%-40.40s $%lli", menuItemsValue[researchedCount].Name.begin(), priceUnit);			\
		menuItemsText	[researchedCount]				= menuItemsValue[researchedCount].Name;						\
		menuItemsValue	[researchedCount].Type			= type;														\
		++researchedCount;																									\
	}

	ADD_RESEARCH_DEFINITIONS(researchedItems.Profession	, ENTITY_TYPE_PROFESSION	, instanceGame.EntityTables.Profession	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Weapon		, ENTITY_TYPE_WEAPON		, instanceGame.EntityTables.Weapon		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Armor		, ENTITY_TYPE_ARMOR			, instanceGame.EntityTables.Armor		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Accessory	, ENTITY_TYPE_ACCESSORY		, instanceGame.EntityTables.Accessory	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Vehicle	, ENTITY_TYPE_VEHICLE		, instanceGame.EntityTables.Vehicle		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Facility	, ENTITY_TYPE_FACILITY		, instanceGame.EntityTables.Facility	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, instanceGame.EntityTables.StageProp	.Definitions);

	static ::klib::SDrawMenuState							menuState;
	SEntityResearch											selectedChoice								= ::klib::drawMenu
		( menuState
		, instanceGame.GlobalDisplay.Screen.View
		, instanceGame.GlobalDisplay.TextAttributes.begin()
		, ::gpk::view_const_string{"Available Production"}
		, ::gpk::view_array<const ::klib::SEntityResearch>	{menuItemsValue	.begin(), (uint32_t)researchedCount}
		, ::gpk::view_array<const ::gpk::view_const_char>	{menuItemsText	.begin(), (uint32_t)researchedCount}
		, instanceGame.FrameInput
		, {::gpk::view_const_string{"Exit production menu"	}, (int32_t)researchedCount}
		, {::gpk::view_const_string{"No action selected"	}, -1}
		, 50U
		);
	if(selectedChoice.ResearchIndex == (int32_t)researchedCount)
		return {GAME_STATE_WELCOME_COMMANDER};

	if(selectedChoice.ResearchIndex == -1)
		return returnState;

	instanceGame.Messages.ClearMessages();
	switch(selectedChoice.Type) {
	case ENTITY_TYPE_ACCESSORY	: acknowledgeProduction(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_STAGE_PROP	: acknowledgeProduction(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_FACILITY	: acknowledgeProduction(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_VEHICLE	: acknowledgeProduction(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_PROFESSION	: acknowledgeProduction(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_WEAPON		: acknowledgeProduction(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	case ENTITY_TYPE_ARMOR		: acknowledgeProduction(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess); instanceGame.LogSuccess(); break;
	default:
		break;
	}

	return returnState;
}

SGameState									drawFactory							(SGame& instanceGame, const SGameState& returnState)																						{
	const ::gpk::view_const_string					textToPrint							= "Factory.";
	static ::klib::SMessageSlow						slowMessage;
	bool											bDonePrinting						= ::klib::getMessageSlow(slowMessage, textToPrint, instanceGame.FrameTimer.LastTimeSeconds);
	::gpk::SCoord2<uint32_t>						position							= instanceGame.TacticalDisplay.Screen.metrics() / 2;
	const uint32_t									messageLen							= (uint32_t)strlen(slowMessage.Message);
	memcpy(&instanceGame.TacticalDisplay.Screen[position.y][position.x - (messageLen + 1) / 2], slowMessage.Message, messageLen);
	if ( !bDonePrinting )
		return returnState;

	drawFireBackground(instanceGame.TacticalDisplay, instanceGame.FrameTimer.LastTimeSeconds);
	return drawFactoryMenu(instanceGame, returnState);
};
