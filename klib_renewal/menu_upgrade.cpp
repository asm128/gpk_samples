//#define NOMINMAX
#include "Game.h"
#include "draw.h"

#include "projects.h"
#include "helper_projects.h"

using namespace klib;

SGameState								drawUpgradeMenu				(SGame& instanceGame, const SGameState& returnState) {
	::klib::SPlayer								& player					= instanceGame.Players[PLAYER_INDEX_USER];
	::klib::SCharacterInventory					& playerInventory			= player.Inventory;
	::klib::SCharacterResearch					& researchCompleted			= player.Tactical.Research;

	::klib::SCharacterResearch					researchedItems				= {};

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)					\
	generateResearchedList(researchedItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_);	\
	for(iAgent = 0; iAgent < armySize; ++iAgent) {																		\
		if( 0 == player.Tactical.Army[iAgent] )																			\
			continue;																									\
																														\
		const CCharacter							& playerAgent			= *player.Tactical.Army[iAgent];			\
		generateResearchedListFromAgent( researchedItems.EntityToken_ 													\
			, playerAgent.CurrentEquip.EntityToken_ 																	\
			, playerAgent.Goods.Inventory.EntityToken_ 																	\
			, researchCompleted.EntityToken_																			\
		);																												\
	}																													\
	researchedDefinitions					+= researchedItems.EntityToken_.Definitions.Slots.size();					\
	researchedModifiers						+= researchedItems.EntityToken_.Modifiers.Slots.size();

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)		\
	generateResearchedList(researchedItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_);	\
	for(iAgent = 0; iAgent < armySize; ++iAgent) {																		\
		if( 0 == player.Tactical.Army[iAgent] )																			\
			continue;																									\
																														\
		const CCharacter							& playerAgent					= *player.Tactical.Army[iAgent];	\
		generateResearchedListFromAgentNoEquip( researchedItems.EntityToken_ 											\
			, playerAgent.Goods.Inventory.EntityToken_ 																	\
			, researchCompleted.EntityToken_																			\
		);																												\
	}																													\
	researchedDefinitions					+= researchedItems.EntityToken_.Definitions.Slots.size();					\
	researchedModifiers						+= researchedItems.EntityToken_.Modifiers.Slots.size();

	int32_t										iAgent;
	const int32_t								armySize						= (int32_t)player.Tactical.Army.size();
	uint32_t									researchedDefinitions			= 0
		,										researchedModifiers				= 0
		;
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Accessory	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(StageProp	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Facility	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP	(Vehicle	, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Profession	, true , false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Weapon		, false, false);
	GET_AVAILABLE_RESEARCH_FOR_ENTITY			(Armor		, false, false);

#define MAX_RESEARCH_ITEMS										\
	  instanceGame.EntityTables.Accessory	.Definitions.size()	\
	+ instanceGame.EntityTables.Weapon		.Definitions.size()	\
	+ instanceGame.EntityTables.Armor		.Definitions.size()	\
	+ instanceGame.EntityTables.Profession	.Definitions.size()	\
	+ instanceGame.EntityTables.Vehicle		.Definitions.size()	\
	+ instanceGame.EntityTables.Facility	.Definitions.size()	\
	+ instanceGame.EntityTables.StageProp	.Definitions.size()

	typedef ::klib::SMenuItem<SEntityResearch>	TMenuItem;
	::gpk::array_obj<TMenuItem>					menuItems						= {};
	menuItems.resize(MAX_RESEARCH_ITEMS);
	uint32_t									researchedCount					= 0;
	char										composite	[256]				= {};
	const char									* labelEntityType				= nullptr;

#define ADD_RESEARCH_DEFINITIONS(place, type, records)																		\
	labelEntityType							= ::gpk::get_value_label(type).begin();											\
	for(uint32_t i = 0, count = place.Definitions.Slots.size(); i < count; ++i) {											\
		TMenuItem									& menuItem						= menuItems[researchedCount];			\
		int16_t										idEntity						= place.Definitions[i].Entity;			\
		menuItem.ReturnValue.ResearchIndex		= i;																		\
		menuItem.ReturnValue.IsModifier			= false;																	\
		int64_t										priceUnit						= records[idEntity].Points.PriceBuy/2;	\
		menuItem.ReturnValue.PriceUnit			= priceUnit;																\
		menuItem.ReturnValue.PricePaid			= 0;																		\
		menuItem.ReturnValue.Entity				= {idEntity, 0, 1, -1};														\
		sprintf_s(composite, "%s: %s", labelEntityType, records[idEntity].Name.begin());									\
		menuItem.ReturnValue.Name				= ::gpk::view_const_string{composite};										\
		sprintf_s(composite, "%-40.40s $%lli", menuItem.ReturnValue.Name.begin(), priceUnit);								\
		menuItem.Text							= ::gpk::view_const_string{composite};										\
		menuItem.ReturnValue.Type				= type;																		\
		++researchedCount;																									\
	}

#define ADD_RESEARCH_MODIFIERS(place, type, records, text)																	\
	labelEntityType							= ::gpk::get_value_label(type).begin();											\
	for(uint32_t i = 0, count = place.Modifiers.Slots.size(); i < count; ++i) {												\
		TMenuItem									& menuItem						= menuItems[researchedCount];			\
		int16_t										idEntity						= place.Modifiers[i].Entity;			\
		menuItem.ReturnValue.ResearchIndex		= i;																		\
		menuItem.ReturnValue.IsModifier			= true;																		\
		int64_t										priceUnit						= records[idEntity].Points.PriceBuy/2;	\
		menuItem.ReturnValue.PriceUnit			= priceUnit;																\
		menuItem.ReturnValue.PricePaid			= 0;																		\
		menuItem.ReturnValue.Entity				= {0, idEntity, 1, -1};														\
		sprintf_s(precompose, records[idEntity].Name.begin(), text);														\
		sprintf_s(composite, "%s: %s", labelEntityType, precompose);														\
		menuItem.ReturnValue.Name				= ::gpk::view_const_string{composite};										\
		sprintf_s(composite, "%-40.40s $%lli", menuItem.ReturnValue.Name.begin(), priceUnit);								\
		menuItem.Text							= ::gpk::view_const_string{composite};										\
		menuItem.ReturnValue.Type				= type;																		\
		++researchedCount;																									\
	}

	ADD_RESEARCH_DEFINITIONS(researchedItems.Profession	, ENTITY_TYPE_PROFESSION	, instanceGame.EntityTables.Profession	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Weapon		, ENTITY_TYPE_WEAPON		, instanceGame.EntityTables.Weapon		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Armor		, ENTITY_TYPE_ARMOR			, instanceGame.EntityTables.Armor		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Accessory	, ENTITY_TYPE_ACCESSORY		, instanceGame.EntityTables.Accessory	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Vehicle	, ENTITY_TYPE_VEHICLE		, instanceGame.EntityTables.Vehicle		.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Facility	, ENTITY_TYPE_FACILITY		, instanceGame.EntityTables.Facility	.Definitions);
	ADD_RESEARCH_DEFINITIONS(researchedItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, instanceGame.EntityTables.StageProp	.Definitions);

//	char precompose[256] ={};
//	ADD_RESEARCH_MODIFIERS(researchedItems.Profession	, ENTITY_TYPE_PROFESSION	, modifiersProfession	, "Rank"				);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Weapon		, ENTITY_TYPE_WEAPON		, modifiersWeapon		, "Science"				);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Armor		, ENTITY_TYPE_ARMOR			, modifiersArmor		, "Technology"			);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Accessory	, ENTITY_TYPE_ACCESSORY		, modifiersAccessory	, "Crafting Technique"	);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Vehicle		, ENTITY_TYPE_VEHICLE		, modifiersVehicle		, "Transportation"		);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Facility		, ENTITY_TYPE_FACILITY		, modifiersFacility		, "Construction"		);
//	ADD_RESEARCH_MODIFIERS(researchedItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, modifiersStageProp	, "Enhacement"			);

	static ::klib::SDrawMenuState				menuState;
	SEntityResearch								selectedChoice					= drawMenu
		(	menuState
		,	instanceGame.GlobalDisplay.Screen.View
		,	instanceGame.GlobalDisplay.TextAttributes.begin()
		,	::gpk::view_const_string{"Available Production"}
		,	::gpk::view_array<const TMenuItem>{menuItems.begin(), researchedCount}
		,	instanceGame.FrameInput
		,	{::gpk::view_const_string{"Exit upgrade menu"	}, (int32_t)researchedCount}
		,	{::gpk::view_const_string{"No action selected"	}, -1}
		,	50U
		);
	if(selectedChoice.ResearchIndex == (int32_t)researchedCount)
		return {GAME_STATE_WELCOME_COMMANDER};

	if(selectedChoice.ResearchIndex == -1)
		return returnState;

	instanceGame.ClearMessages();

	switch(selectedChoice.Type) {
	case ENTITY_TYPE_ACCESSORY	:
	case ENTITY_TYPE_STAGE_PROP	:
	case ENTITY_TYPE_FACILITY	:
	case ENTITY_TYPE_VEHICLE	:
	case ENTITY_TYPE_PROFESSION	:
	case ENTITY_TYPE_WEAPON		:
	case ENTITY_TYPE_ARMOR		:
		acknowledgeProduction(selectedChoice, player.Projects, instanceGame.Messages.UserSuccess);
		instanceGame.LogSuccess();
		break;
	default:
		break;
	}

	return returnState;
}

SGameState								drawUpgrade				(SGame& instanceGame, const SGameState& returnState) {
	static const ::gpk::view_const_string		textToPrint				= "Upgrade.";
	static ::klib::SMessageSlow					slowMessage;
	bool										bDonePrinting			= ::klib::getMessageSlow(slowMessage, textToPrint.begin(), textToPrint.size(), instanceGame.FrameTimer.LastTimeSeconds);
	::gpk::SCoord2<uint32_t>					position				= instanceGame.TacticalDisplay.Screen.metrics() / 2;
	const uint32_t								messageLen				= (uint32_t)strlen(slowMessage.Message);
	memcpy(&instanceGame.TacticalDisplay.Screen[position.y][position.x - (messageLen + 1) / 2], slowMessage.Message, messageLen);
	if ( !bDonePrinting )
		return returnState;

	drawFireBackground(instanceGame.TacticalDisplay, instanceGame.FrameTimer.LastTimeSeconds);
	return drawUpgradeMenu(instanceGame, returnState);
}
