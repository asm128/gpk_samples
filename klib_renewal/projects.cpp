//#define NOMINMAX
#include "Game.h"
#include "projects.h"

using namespace klib;

static inline	void addAccessory		(const ::klib::SEntityTables & entityTables, ::klib::SPlayer& player, const SAccessory	& entity, ::gpk::array_pod<char_t> &	 messageSuccess) { ::klib::completeProduction<::klib::SAccessory	>(player.Inventory.Accessory	, entity, messageSuccess, entityTables.Accessory	); }
static inline	void addArmor			(const ::klib::SEntityTables & entityTables, ::klib::SPlayer& player, const SArmor		& entity, ::gpk::array_pod<char_t> &	 messageSuccess) { ::klib::completeProduction<::klib::SArmor		>(player.Inventory.Armor		, entity, messageSuccess, entityTables.Armor		); }
static inline	void addWeapon			(const ::klib::SEntityTables & entityTables, ::klib::SPlayer& player, const SWeapon		& entity, ::gpk::array_pod<char_t> &	 messageSuccess) { ::klib::completeProduction<::klib::SWeapon		>(player.Inventory.Weapon		, entity, messageSuccess, entityTables.Weapon		); }
static inline	void addVehicle			(const ::klib::SEntityTables & entityTables, ::klib::SPlayer& player, const SVehicle	& entity, ::gpk::array_pod<char_t> &	 messageSuccess) { ::klib::completeProduction<::klib::SVehicle		>(player.Inventory.Vehicle		, entity, messageSuccess, entityTables.Vehicle		); }
static inline	void addProfession		(const ::klib::SEntityTables & entityTables, ::klib::SPlayer& player, const SProfession	& entity, ::gpk::array_pod<char_t> &	 messageSuccess) { ::klib::completeProduction<::klib::SProfession	>(player.Inventory.Profession	, entity, messageSuccess, entityTables.Profession	); }
static inline	void addFacility		(const ::klib::SEntityTables & entityTables, ::klib::SPlayer& player, const SFacility	& entity, ::gpk::array_pod<char_t> &	 messageSuccess) { ::klib::completeProduction<::klib::SFacility		>(player.Inventory.Facility		, entity, messageSuccess, entityTables.Facility		); }
static inline	void addStageProp		(const ::klib::SEntityTables & entityTables, ::klib::SPlayer& player, const SStageProp	& entity, ::gpk::array_pod<char_t> &	 messageSuccess) { ::klib::completeProduction<::klib::SStageProp	>(player.Inventory.StageProp	, entity, messageSuccess, entityTables.StageProp	); }

static inline	void completeProduction	(const ::klib::SEntityTables & entityTables, ::klib::SPlayer& player, const SEntityResearch& product, ::gpk::array_pod<char_t>& messageSuccess) {
	switch(product.Type) {
	case ENTITY_TYPE_ACCESSORY	: ::addAccessory	(entityTables, player, ( SAccessory	)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_ARMOR		: ::addArmor		(entityTables, player, ( SArmor		)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_WEAPON		: ::addWeapon		(entityTables, player, ( SWeapon	)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_VEHICLE	: ::addVehicle		(entityTables, player, ( SVehicle	)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_PROFESSION	: ::addProfession	(entityTables, player, ( SProfession)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_FACILITY	: ::addFacility		(entityTables, player, ( SFacility	)product.Entity, messageSuccess);  break;
	case ENTITY_TYPE_STAGE_PROP	: ::addStageProp	(entityTables, player, ( SStageProp	)product.Entity, messageSuccess);  break;
	//case ENTITY_TYPE_ITEM		:
	}
	char	price[64];
	sprintf_s(price, "%lli", product.PricePaid);
	messageSuccess.append_string(" Cost: ");
	messageSuccess.append_string(price);
}

void						handleProductionStep		(SGame& instanceGame) {
	SPlayer							& player					= instanceGame.Players[PLAYER_INDEX_USER];
	SPlayerProjects					& playerProjects			= player.Projects;

	int64_t							budgetProduction			= playerProjects.BudgetProduction.bIsRatio ? int32_t(player.Tactical.Money*(playerProjects.BudgetProduction.Money/100.0)) : ::gpk::min(playerProjects.BudgetProduction.Money, player.Tactical.Money);
	int64_t							actualCostProduction		= 0;
	for(uint32_t iProduct=0, productCount = playerProjects.QueuedProduction.size(); iProduct < productCount; ++iProduct) {
		if(actualCostProduction >= budgetProduction)
			break;

		SEntityResearch				& product					= playerProjects.QueuedProduction[iProduct];

		int64_t						assignedFunds				= ::gpk::min(budgetProduction-actualCostProduction, product.PriceUnit-product.PricePaid);
		//product.PriceUnit		-= assignedFunds;
		product.PricePaid		+= assignedFunds;
		actualCostProduction	+= assignedFunds;
	}

	::gpk::array_obj<SEntityResearch> remainingProduction;
	for(uint32_t iProduct = 0, productCount = playerProjects.QueuedProduction.size(); iProduct < productCount; ++iProduct) {
		const SEntityResearch						& product				= playerProjects.QueuedProduction[iProduct];
		if((product.PriceUnit-product.PricePaid) > 0) {
			remainingProduction.push_back(product);
			continue; // This product is still in production
		}
		::completeProduction(instanceGame.EntityTables, player, product, instanceGame.Messages.UserSuccess);
		instanceGame.LogSuccess();
		++player.Tactical.Score.CompletedProduction;
	}
	playerProjects.CostProduction			-= actualCostProduction;
	playerProjects.QueuedProduction			= remainingProduction;
	player.Tactical.Money					-= actualCostProduction;
	player.Tactical.Score.MoneySpent		+= actualCostProduction;
}

void							completeResearch				(const SEntityResearch& product, SCharacterResearch& playerResearch, ::gpk::array_pod<char_t> & successMessage) {
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

	successMessage.append_string(" Cost: ");
	char price [64];
	sprintf_s(price, "%lli", product.PricePaid);
	successMessage.append_string(price);
}

void							handleResearchStep				(SGame& instanceGame) {
	::klib::SPlayer						& playerUser					= instanceGame.Players[PLAYER_INDEX_USER];
	::klib::SCharacterResearch			& playerResearch				= playerUser.Tactical.Research;
	::klib::SPlayerProjects				& playerProjects				= playerUser.Projects;

	int64_t								budgetResearch					= playerProjects.BudgetResearch.bIsRatio ? int64_t(playerUser.Tactical.Money * (playerProjects.BudgetResearch.Money/100.0)) : ::gpk::min(playerProjects.BudgetResearch.Money, playerUser.Tactical.Money);
	int64_t								actualCostResearch				= 0;
	for(uint32_t iProduct = 0, productCount = playerProjects.QueuedResearch.size(); iProduct < productCount; ++iProduct) {
		if(actualCostResearch >= budgetResearch)
			break;

		SEntityResearch						& product		= playerProjects.QueuedResearch[iProduct];
		int64_t								assignedFunds	= ::gpk::min(budgetResearch-actualCostResearch, product.PriceUnit-product.PricePaid);
		//product.PriceUnit				-= assignedFunds;
		product.PricePaid				+= assignedFunds;
		actualCostResearch				+= assignedFunds;
	}

	::gpk::array_obj<SEntityResearch>	remainingResearch;
	for(uint32_t iProduct = 0, productCount = playerProjects.QueuedResearch.size(); iProduct < productCount; ++iProduct) {
		const SEntityResearch& product = playerProjects.QueuedResearch[iProduct];
		if((product.PriceUnit-product.PricePaid) > 0) {
			remainingResearch.push_back(product);
			continue; // This product is still in production
		}
		::completeResearch(product, playerResearch, instanceGame.Messages.UserSuccess);
		instanceGame.LogSuccess();
		++playerUser.Tactical.Score.CompletedResearch;
	}
	playerProjects.CostResearch				-=	actualCostResearch;
	playerProjects.QueuedResearch			=	remainingResearch;
	playerUser.Tactical.Money				-=	actualCostResearch;
	playerUser.Tactical.Score.MoneySpent	+=	actualCostResearch;
}
