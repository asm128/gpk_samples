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

SGameState drawUpgradeMenu(SGame& instanceGame, const SGameState& returnState)
{
	SPlayer										& player				= instanceGame.Players[PLAYER_INDEX_USER];
	SCharacterGoods								& playerCompany			= player.Goods;
	SCharacterInventory							& playerInventory		= playerCompany.Inventory;
	klib::SCharacterResearch					& researchCompleted		= playerCompany.CompletedResearch;
	
	klib::SCharacterResearch					researchedItems			= {};

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)					\
	generateResearchedList(researchedItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_);	\
	for(iAgent=0; iAgent<armySize; ++iAgent) {																			\
		if( 0 == player.Army[iAgent] )																					\
			continue;																									\
																														\
		const CCharacter							& playerAgent			= *player.Army[iAgent];						\
		generateResearchedListFromAgent( researchedItems.EntityToken_ 													\
			, playerAgent.CurrentEquip.EntityToken_ 																	\
			, playerAgent.Goods.Inventory.EntityToken_ 																	\
			, researchCompleted.EntityToken_																			\
		);																												\
	}																													\
	researchedDefinitions					+= researchedItems.EntityToken_.Definitions.Count;							\
	researchedModifiers						+= researchedItems.EntityToken_.Modifiers.Count;

#define GET_AVAILABLE_RESEARCH_FOR_ENTITY_NO_EQUIP(EntityToken_, ProgressiveDefinitions_, ProgressiveModifiers_)		\
	generateResearchedList(researchedItems.EntityToken_, playerInventory.EntityToken_, researchCompleted.EntityToken_);	\
	for(iAgent=0; iAgent<armySize; ++iAgent) {																			\
		if( 0 == player.Army[iAgent] )																					\
			continue;																									\
																														\
		const CCharacter							& playerAgent					= *player.Army[iAgent];				\
		generateResearchedListFromAgentNoEquip( researchedItems.EntityToken_ 											\
			, playerAgent.Goods.Inventory.EntityToken_ 																	\
			, researchCompleted.EntityToken_																			\
		);																												\
	}																													\
	researchedDefinitions					+= researchedItems.EntityToken_.Definitions.Count;							\
	researchedModifiers						+= researchedItems.EntityToken_.Modifiers.Count;

	int32_t										iAgent;
	const int32_t								armySize						= (int32_t)player.Army.size();
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

#define MAX_RESEARCH_ITEMS					\
	 ::gpk::size(definitionsAccessory)		\
	+::gpk::size(definitionsWeapon)			\
	+::gpk::size(definitionsArmor)			\
	+::gpk::size(definitionsProfession)		\
	+::gpk::size(definitionsVehicle)			\
	+::gpk::size(definitionsFacility)		\
	+::gpk::size(definitionsStageProp)		\
		//+::gpk::size(modifiersAccessory)		\\
		//+::gpk::size(modifiersWeapon)			\\
		//+::gpk::size(modifiersArmor)			\\
		//+::gpk::size(modifiersProfession)		\\
		//+::gpk::size(modifiersVehicle)			\\
		//+::gpk::size(modifiersFacility)		\\
		//+::gpk::size(modifiersStageProp)

	typedef ::klib::SMenuItem<SEntityResearch>	TMenuItem;
	static	TMenuItem							menuItems[MAX_RESEARCH_ITEMS]	= {};

	uint32_t									researchedCount					= 0;
	char										composite	[256]				= {};
	const char									* labelEntityType				= nullptr;

#define ADD_RESEARCH_DEFINITIONS(place, type, records)																		\
	labelEntityType							= ::gpk::get_value_label(type).begin();										\
	for(uint32_t i=0, count=place.Definitions.Count; i<count; ++i) {														\
		TMenuItem									& menuItem						= menuItems[researchedCount];			\
		int16_t										idEntity						= place.Definitions[i].Entity;			\
		menuItem.ReturnValue.ResearchIndex		= i;																		\
		menuItem.ReturnValue.IsModifier			= false;																	\
		int32_t										priceUnit						= records[idEntity].Points.PriceBuy/2;	\
		menuItem.ReturnValue.PriceUnit			= priceUnit;																\
		menuItem.ReturnValue.PricePaid			= 0;																		\
		menuItem.ReturnValue.Entity				= {idEntity, 0, 1, -1};														\
		sprintf_s(composite, "%s: %s", labelEntityType, records[idEntity].Name.begin());									\
		menuItem.ReturnValue.Name				= composite;																\
		sprintf_s(composite, "%-40.40s $%i", menuItem.ReturnValue.Name.c_str(), priceUnit);									\
		menuItem.Text							= composite;																\
		menuItem.ReturnValue.Type				= type;																		\
		++researchedCount;																									\
	}

#define ADD_RESEARCH_MODIFIERS(place, type, records, text)																	\
	labelEntityType							= ::gpk::get_value_label(type).begin();										\
	for(uint32_t i=0, count=place.Modifiers.Count; i<count; ++i) {															\
		TMenuItem									& menuItem						= menuItems[researchedCount];			\
		int16_t										idEntity						= place.Modifiers[i].Entity;			\
		menuItem.ReturnValue.ResearchIndex		= i;																		\
		menuItem.ReturnValue.IsModifier			= true;																		\
		int32_t										priceUnit						= records[idEntity].Points.PriceBuy/2;	\
		menuItem.ReturnValue.PriceUnit			= priceUnit;																\
		menuItem.ReturnValue.PricePaid			= 0;																		\
		menuItem.ReturnValue.Entity				= {0, idEntity, 1, -1};														\
		sprintf_s(precompose, records[idEntity].Name.c_str(), text);														\
		sprintf_s(composite, "%s: %s", labelEntityType, precompose);														\
		menuItem.ReturnValue.Name				= composite;																\
		sprintf_s(composite, "%-40.40s $%i", menuItem.ReturnValue.Name.c_str(), priceUnit);									\
		menuItem.Text							= composite;																\
		menuItem.ReturnValue.Type				= type;																		\
		++researchedCount;																									\
	}

	ADD_RESEARCH_DEFINITIONS(researchedItems.Profession	, ENTITY_TYPE_PROFESSION	, definitionsProfession	);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Weapon		, ENTITY_TYPE_WEAPON		, definitionsWeapon		);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Armor		, ENTITY_TYPE_ARMOR			, definitionsArmor		);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Accessory	, ENTITY_TYPE_ACCESSORY		, definitionsAccessory	);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Vehicle	, ENTITY_TYPE_VEHICLE		, definitionsVehicle	);
	ADD_RESEARCH_DEFINITIONS(researchedItems.Facility	, ENTITY_TYPE_FACILITY		, definitionsFacility	);
	ADD_RESEARCH_DEFINITIONS(researchedItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, definitionsStageProp	);

//	char precompose[256] ={};
//	ADD_RESEARCH_MODIFIERS(researchedItems.Profession	, ENTITY_TYPE_PROFESSION	, modifiersProfession	, "Rank"				);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Weapon		, ENTITY_TYPE_WEAPON		, modifiersWeapon		, "Science"				);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Armor		, ENTITY_TYPE_ARMOR			, modifiersArmor		, "Technology"			);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Accessory	, ENTITY_TYPE_ACCESSORY		, modifiersAccessory	, "Crafting Technique"	);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Vehicle		, ENTITY_TYPE_VEHICLE		, modifiersVehicle		, "Transportation"		);
//	ADD_RESEARCH_MODIFIERS(researchedItems.Facility		, ENTITY_TYPE_FACILITY		, modifiersFacility		, "Construction"		);
//	ADD_RESEARCH_MODIFIERS(researchedItems.StageProp	, ENTITY_TYPE_STAGE_PROP	, modifiersStageProp	, "Enhacement"			);

	SEntityResearch								selectedChoice					= drawMenu
		(	instanceGame.GlobalDisplay.Screen
		,	&instanceGame.GlobalDisplay.TextAttributes.Cells[0][0]
		,	(size_t)researchedCount
		,	"Available Production"
		,	menuItems
		,	instanceGame.FrameInput
		,	SEntityResearch{"Exit upgrade menu"	, {(int32_t)researchedCount}}
		,	SEntityResearch{"No action selected", -1}
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
		acknowledgeProduction(selectedChoice, player.Projects, instanceGame.UserSuccess);	
		instanceGame.LogSuccess(); 
		break;
	default:
		break;
	}

	return returnState;
}

SGameState drawUpgrade(SGame& instanceGame, const SGameState& returnState)
{
	static const ::gpk::label textToPrint = "Upgrade.";

	bool bDonePrinting = ::klib::getMessageSlow(instanceGame.SlowMessage, textToPrint.begin(), textToPrint.size(), instanceGame.FrameTimer.LastTimeSeconds);
	memcpy(&instanceGame.PostEffectDisplay.Screen.Cells[instanceGame.PostEffectDisplay.Depth>>1][instanceGame.PostEffectDisplay.Width/2-(strlen(instanceGame.SlowMessage)+1)/2], instanceGame.SlowMessage, strlen(instanceGame.SlowMessage));
	if ( !bDonePrinting ) 
		return returnState;

	drawFireBackground(instanceGame.PostEffectDisplay, instanceGame.FrameTimer.LastTimeSeconds);
	return drawUpgradeMenu(instanceGame, returnState);
}
