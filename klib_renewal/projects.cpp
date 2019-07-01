//#define NOMINMAX
#include "Game.h"
#include "projects.h"

#include "Accessory.h"
#include "Facility.h"
#include "Vehicle.h"
#include "Armor.h"
#include "Weapon.h"
#include "Profession.h"
#include "StageProp.h"

using namespace klib;

void addAccessory	(SPlayer& player, const SAccessory	& entity, std::string&	 messageSuccess) { ::klib::completeProduction(player.Goods.Inventory.Accessory	, entity, messageSuccess, definitionsAccessory	, modifiersAccessory	); }
void addArmor		(SPlayer& player, const SArmor		& entity, std::string&	 messageSuccess) { ::klib::completeProduction(player.Goods.Inventory.Armor		, entity, messageSuccess, definitionsArmor		, modifiersArmor		); }
void addWeapon		(SPlayer& player, const SWeapon		& entity, std::string&	 messageSuccess) { ::klib::completeProduction(player.Goods.Inventory.Weapon		, entity, messageSuccess, definitionsWeapon		, modifiersWeapon		); }
void addVehicle		(SPlayer& player, const SVehicle	& entity, std::string&	 messageSuccess) { ::klib::completeProduction(player.Goods.Inventory.Vehicle	, entity, messageSuccess, definitionsVehicle	, modifiersVehicle		); }
void addProfession	(SPlayer& player, const SProfession	& entity, std::string&	 messageSuccess) { ::klib::completeProduction(player.Goods.Inventory.Profession	, entity, messageSuccess, definitionsProfession	, modifiersProfession	); }
void addFacility	(SPlayer& player, const SFacility	& entity, std::string&	 messageSuccess) { ::klib::completeProduction(player.Goods.Inventory.Facility	, entity, messageSuccess, definitionsFacility	, modifiersFacility		); }
void addStageProp	(SPlayer& player, const SStageProp	& entity, std::string&	 messageSuccess) { ::klib::completeProduction(player.Goods.Inventory.StageProp	, entity, messageSuccess, definitionsStageProp	, modifiersStageProp	); }

void completeProduction(SPlayer& player, const SEntityResearch& product, std::string& messageSuccess) {
	switch(product.Type) {
	case ENTITY_TYPE_ACCESSORY	: ::addAccessory	(player, ( SAccessory	)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_ARMOR		: ::addArmor		(player, ( SArmor		)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_WEAPON		: ::addWeapon		(player, ( SWeapon		)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_VEHICLE	: ::addVehicle		(player, ( SVehicle		)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_PROFESSION	: ::addProfession	(player, ( SProfession	)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_FACILITY	: ::addFacility		(player, ( SFacility	)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_STAGE_PROP	: ::addStageProp	(player, ( SStageProp	)product.Entity, messageSuccess);  break;
	//case ENTITY_TYPE_ITEM		: 
	}
	messageSuccess += " Cost: " + ::std::to_string(product.PricePaid); 
}

void handleProductionStep(SGame& instanceGame) {
	SPlayer& player = instanceGame.Players[PLAYER_INDEX_USER];
	SPlayerProjects& playerProjects = player.Projects;

	int32_t budgetProduction		= playerProjects.BudgetProduction.bIsRatio ? int32_t(player.Money*(playerProjects.BudgetProduction.Money/100.0)) : ::gpk::min(playerProjects.BudgetProduction.Money, player.Money);
	int32_t actualCostProduction	= 0;
	for(uint32_t iProduct=0, productCount=playerProjects.QueuedProduction.size(); iProduct < productCount; ++iProduct) {
		if(actualCostProduction >= budgetProduction)
			break;

		SEntityResearch& product = playerProjects.QueuedProduction[iProduct];

		int32_t assignedFunds	= ::gpk::min(budgetProduction-actualCostProduction, product.PriceUnit-product.PricePaid);
		//product.PriceUnit		-= assignedFunds;
		product.PricePaid		+= assignedFunds;
		actualCostProduction	+= assignedFunds;
	}

	::gpk::array_obj<SEntityResearch> remainingProduction;
	for(uint32_t iProduct = 0, productCount = playerProjects.QueuedProduction.size(); iProduct < productCount; ++iProduct) {
		const SEntityResearch	& product = playerProjects.QueuedProduction[iProduct];
		if((product.PriceUnit-product.PricePaid) > 0) {
			remainingProduction.push_back(product);
			continue; // This product is still in production 
		}
		::completeProduction(player, product, instanceGame.UserSuccess);
		instanceGame.LogSuccess();
		++player.Score.CompletedProduction;
	}
	playerProjects.CostProduction	-= actualCostProduction;
	playerProjects.QueuedProduction	= remainingProduction;
	player.Money					-= actualCostProduction;
	player.Score.MoneySpent			+= actualCostProduction;
}

void completeResearch( const SEntityResearch& product, SCharacterResearch& playerResearch, std::string& successMessage ) {
	switch(product.Type) {
	case ENTITY_TYPE_ACCESSORY	: ::klib::completeResearch(product, playerResearch.Accessory	.MaxResearch, playerResearch.Accessory	, successMessage); break;
	case ENTITY_TYPE_ARMOR		: ::klib::completeResearch(product, playerResearch.Armor		.MaxResearch, playerResearch.Armor		, successMessage); break;
	case ENTITY_TYPE_WEAPON		: ::klib::completeResearch(product, playerResearch.Weapon		.MaxResearch, playerResearch.Weapon		, successMessage); break;
	case ENTITY_TYPE_VEHICLE	: ::klib::completeResearch(product, playerResearch.Vehicle		.MaxResearch, playerResearch.Vehicle	, successMessage); break;
	case ENTITY_TYPE_PROFESSION	: ::klib::completeResearch(product, playerResearch.Profession	.MaxResearch, playerResearch.Profession	, successMessage); break;
	case ENTITY_TYPE_FACILITY	: ::klib::completeResearch(product, playerResearch.Facility		.MaxResearch, playerResearch.Facility	, successMessage); break;
	case ENTITY_TYPE_STAGE_PROP	: ::klib::completeResearch(product, playerResearch.StageProp	.MaxResearch, playerResearch.StageProp	, successMessage); break;
	//case ENTITY_TYPE_ITEM		: 
	}

	successMessage += " Cost: " + ::std::to_string(product.PricePaid); 
}

void handleResearchStep(SGame& instanceGame) {
	SPlayer& playerUser = instanceGame.Players[PLAYER_INDEX_USER];
	SCharacterResearch	& playerResearch	= playerUser.Goods.CompletedResearch;
	SPlayerProjects		& playerProjects	= playerUser.Projects;

	int32_t budgetResearch		= playerProjects.BudgetResearch.bIsRatio ? int32_t(playerUser.Money*(playerProjects.BudgetResearch.Money/100.0)) : ::gpk::min(playerProjects.BudgetResearch.Money, playerUser.Money);
	int32_t actualCostResearch	= 0;
	for(uint32_t iProduct = 0, productCount = playerProjects.QueuedResearch.size(); iProduct < productCount; ++iProduct) {
		if(actualCostResearch >= budgetResearch)
			break;

		SEntityResearch				& product		= playerProjects.QueuedResearch[iProduct];
		int32_t						assignedFunds	= ::gpk::min(budgetResearch-actualCostResearch, product.PriceUnit-product.PricePaid);
		//product.PriceUnit		-= assignedFunds;
		product.PricePaid		+= assignedFunds;
		actualCostResearch		+= assignedFunds;
	}

	::gpk::array_obj<SEntityResearch> remainingResearch;
	for(uint32_t iProduct = 0, productCount = playerProjects.QueuedResearch.size(); iProduct < productCount; ++iProduct) {
		const SEntityResearch& product = playerProjects.QueuedResearch[iProduct];
		if((product.PriceUnit-product.PricePaid) > 0) {
			remainingResearch.push_back(product);
			continue; // This product is still in production 
		}
		::completeResearch(product, playerResearch, instanceGame.UserSuccess);
		instanceGame.LogSuccess(); 
		++playerUser.Score.CompletedResearch;
	}
	playerProjects.CostResearch		-=	actualCostResearch;
	playerProjects.QueuedResearch	=	remainingResearch;
	playerUser.Money				-=	actualCostResearch;
	playerUser.Score.MoneySpent		+=	actualCostResearch;
}