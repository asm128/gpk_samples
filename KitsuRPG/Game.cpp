#include "Game.h"
#include "GameMenu.h"

#include "Item.h"
#include "Enemy.h"
#include "Profession.h"
#include "Armor.h"
#include "Weapon.h"
#include "Vehicle.h"
#include "Accessory.h"
#include "Facility.h"


#include "Menu.h"

// Looks into a research container for the requested technology and equips the entity in equippedEntity if the research is complete.
template<typename _EntityType, size_t _EntityContainerSize, size_t _DefinitionCount, size_t _ModifierCount>
bool																	equipEntityIfResearched			
	( uint32_t														slotIndex
	, ::klib::SEntityContainer<_EntityType, _EntityContainerSize>	& entityContainer
	, const ::klib::SResearchGroup<_EntityType>						& completedResearchPlayer
	, const ::klib::SResearchGroup<_EntityType>						& completedResearchAgent
	, const ::klib::SEntityRecord<_EntityType>						(&tableDefinitions	)[_DefinitionCount	]
	, const ::klib::SEntityRecord<_EntityType>						(&tableModifiers	)[_ModifierCount	]
	, _EntityType													& equippedEntity
	, const ::std::string											& cantAccessDefinitionError
	, const ::std::string											& cantAccessModifierError
	, const ::std::string											& storeOldWeaponMessage
	, const ::std::string											& equipNewWeaponMessage
	, const ::std::string											& researchTypeString
	)	
{	
	if(slotIndex >= entityContainer.Count) //invalid index do nothing.
		return false;
	
	bool																		bCancel											= false; 
	if( 0 < entityContainer[slotIndex].Entity.Definition && (-1) == completedResearchPlayer.Definitions.FindElement(entityContainer[slotIndex].Entity.Definition) && (-1) == completedResearchAgent.Definitions.FindElement(entityContainer[slotIndex].Entity.Definition)) { 
		bCancel																	= true;
		printf(cantAccessDefinitionError.c_str(), tableDefinitions[entityContainer[slotIndex].Entity.Definition].Name.begin());	//
	} 

	if( 0 < entityContainer[slotIndex].Entity.Modifier	&& (-1) == completedResearchPlayer.Modifiers.FindElement(entityContainer[slotIndex].Entity.Modifier) && (-1) == completedResearchAgent.Modifiers.FindElement(entityContainer[slotIndex].Entity.Modifier)) { 
		bCancel																	= true;
		static char																	itemText	[128]								= {}; 
		sprintf_s(itemText, tableModifiers[entityContainer[slotIndex].Entity.Modifier].Name.begin(), researchTypeString.c_str());
		printf(cantAccessModifierError.c_str(), itemText);
	}	

	if(bCancel)
		return false;

	printf(storeOldWeaponMessage.c_str(), getEntityName(equippedEntity, tableDefinitions, tableModifiers).c_str(), equippedEntity.Level); 
	unequipEntity(entityContainer, equippedEntity);
	equippedEntity															= entityContainer[slotIndex].Entity;	// Assign selected entity.
	entityContainer.DecreaseEntity(slotIndex);
	printf(equipNewWeaponMessage.c_str(), getEntityName(equippedEntity, tableDefinitions, tableModifiers).c_str(), equippedEntity.Level); 
	return true;
}

template<typename _EntityType, size_t _EntityContainerSize, size_t _DefinitionCount, size_t _ModifierCount>
void																	equipEntityMenu
	( ::klib::SEntityContainer<_EntityType, _EntityContainerSize>	& characterInventory
	, const ::klib::SResearchGroup<_EntityType>						& completedResearch 
	, const ::klib::SEntityRecord<_EntityType>						(&tableDefinitions	)[_DefinitionCount	]
	, const ::klib::SEntityRecord<_EntityType>						(&tableModifiers	)[_ModifierCount	]
	, _EntityType													& currentEntity
	, const ::std::string											& noWeaponMessage
	, const ::std::string											& currentlyCarryingMessage
	, const ::std::string											& selectYourChoiceMessage
	, const ::std::string											& cantAccessDefinitionError
	, const ::std::string											& cantAccessModifierError
	, const ::std::string											& storeOldWeaponMessage
	, const ::std::string											& equipNewWeaponMessage
	, const ::std::string											& researchTypeString
	) 
{ 
	if( 0 == characterInventory.Count )	{ 
		printf("%s", noWeaponMessage.c_str()); 
		return; 
	}
	
	static char																	menuItemText[256]							= {};	
	static ::klib::SMenuItem<int16_t>											menuItems	[256]							= {}; 
	int32_t																		menuItemCount								= 0; 

	for(int32_t i=0, count = characterInventory.Count; i<count; ++i) { 
		sprintf_s(menuItemText, "x%.02i: %s level %u", characterInventory[i].Count, ::klib::getEntityName(characterInventory[i].Entity, tableDefinitions, tableModifiers).c_str(), characterInventory[i].Entity.Level);
		menuItems[menuItemCount++]												= {(int16_t)i, menuItemText}; 
	}	

	printf(currentlyCarryingMessage.c_str(), klib::getEntityName(currentEntity, tableDefinitions, tableModifiers).c_str(), currentEntity.Level);
	menuItems[menuItemCount++]												= {256, "Exit this menu"}; 
	int32_t																		selectedValue								= displayMenu(selectYourChoiceMessage.c_str(), menuItems, menuItemCount);	
	if(selectedValue == 256) 
		return; 

	equipEntityIfResearched(selectedValue, characterInventory, completedResearch, completedResearch, tableDefinitions, tableModifiers, currentEntity, cantAccessDefinitionError, cantAccessModifierError, storeOldWeaponMessage, equipNewWeaponMessage, researchTypeString);
}

void																	equipWeaponMenu								(klib::CCharacter& adventurer)						{ 
	equipEntityMenu
		( adventurer.Goods.Inventory			.Weapon
		, adventurer.Goods.CompletedResearch	.Weapon
		, ::klib::definitionsWeapon
		, ::klib::modifiersWeapon
		, adventurer.CurrentEquip				.Weapon
		, "You don't have any other weapons yet!\n"
		, "You're currently carrying a %s level %u.\n"
		, "Select your weapon of choice"
		, "You can't access to %s weapons until you have researched them!\n"	
		, "You can't access to %s weapons until you have researched them!\n"	
		, "You store %s level %u in your arsenal.\n"
		, "You equip %s level %u.\n"
		, "Science"
		);
}

void																	equipAccessoryMenu							(klib::CCharacter& adventurer)						{ 
	equipEntityMenu
		( adventurer.Goods.Inventory			.Accessory
		, adventurer.Goods.CompletedResearch	.Accessory
		, klib::definitionsAccessory
		, klib::modifiersAccessory
		, adventurer.CurrentEquip				.Accessory
		, "You don't have any other accessories yet!\n"
		, "You're currently wearing a %s level %u.\n"
		, "Select an accessory to wear"
		, "You can't access to %s without researching it first!\n"	
		, "You can't access to %s without researching it first!\n"	
		, "You take out your %s level %u.\n"
		, "You put on your %s level %u.\n"
		, "Crafting"
		);
}

void																	equipArmorMenu								(klib::CCharacter& adventurer)						{
	equipEntityMenu
		( adventurer.Goods.Inventory			.Armor
		, adventurer.Goods.CompletedResearch	.Armor
		, klib::definitionsArmor
		, klib::modifiersArmor
		, adventurer.CurrentEquip				.Armor
		, "You don't have any other armors yet!\n"
		, "You're currently wearing a %s level %u.\n"
		, "Select your armor of choice"
		, "You can't access to %s armors until you have researched them!\n"	
		, "You can't access to %s armors until you have researched them!\n"	
		, "You store %s level %u in your wardrobe.\n"
		, "You equip %s level %u.\n"
		, "Technology"
		);
}

void																	equipProfessionMenu							(klib::CCharacter& adventurer)						{ 
	equipEntityMenu
		( adventurer.Goods.Inventory			.Profession
		, adventurer.Goods.CompletedResearch	.Profession
		, klib::definitionsProfession
		, klib::modifiersProfession
		, adventurer.CurrentEquip				.Profession
		, "You don't have any other professions yet!\n"
		, "You're currently assigned as a %s level %u.\n"
		, "Select your favorite agent for the next mission"
		, "You can't access to %s agents until you have researched them!\n"	
		, "You can't access to %s until you have researched it!\n"	
		, "You give some free days to your %s level %u.\n"
		, "You call your %s level %u for notifying about the next assignment.\n"
		, "Rank"
		);
};

void																	equipVehicleMenu							(klib::CCharacter& adventurer)						{ 
	equipEntityMenu
		( adventurer.Goods.Inventory			.Vehicle
		, adventurer.Goods.CompletedResearch	.Vehicle
		, klib::definitionsVehicle
		, klib::modifiersVehicle
		, adventurer.CurrentEquip				.Vehicle
		, "You don't have any other vehicles yet!\n"
		, "You're currently piloting a %s level %u.\n"
		, "Select your vehicle of choice"
		, "You can't access to %s vehicles until you have researched them!\n"	
		, "You can't access to %s vehicles until you have researched them!\n"	
		, "You leave your %s level %u.\n"
		, "You get ready to pilot your %s level %u.\n"
		, "Mechanics"
		);
};

void																	equipFacilityMenu							(klib::CCharacter& adventurer)						{ 
	equipEntityMenu
		( adventurer.Goods.Inventory			.Facility
		, adventurer.Goods.CompletedResearch	.Facility
		, klib::definitionsFacility
		, klib::modifiersFacility 
		, adventurer.CurrentEquip				.Facility
		, "You don't have any other building yet!\n"
		, "You're currently inside a %s level %u.\n"
		, "Select your building of choice"
		, "You can't access to %s buildings until you have researched them!\n"	
		, "You can't access to %s buildings until you have researched them!\n"	
		, "You leave your %s level %u.\n"
		, "You get into your %s level %u.\n"
		, "Construction"
		);
};

void																	researchWeaponDefinition					(klib::CCharacter& adventurer);
void																	researchWeaponModifier						(klib::CCharacter& adventurer);
void																	researchArmorDefinition						(klib::CCharacter& adventurer);
void																	researchArmorModifier						(klib::CCharacter& adventurer);
void																	researchAccessoryDefinition					(klib::CCharacter& adventurer);
void																	researchAccessoryModifier					(klib::CCharacter& adventurer);
void																	researchProfessionDefinition				(klib::CCharacter& adventurer);
void																	researchProfessionModifier					(klib::CCharacter& adventurer);
void																	researchVehicleDefinition					(klib::CCharacter& adventurer);
void																	researchVehicleModifier						(klib::CCharacter& adventurer);
void																	researchFacilityDefinition					(klib::CCharacter& adventurer);
void																	researchFacilityModifier					(klib::CCharacter& adventurer);

static const char														optionNotSupported[]						= "Option not supported yet. Please select a valid option.\n";

void																	labs										(klib::CCharacter& adventurer)						{
	// This is the main loop of the game and queries for user input until the exit option is selected.
	static const klib::SMenuItem<int>											tavernOptions[]								=
	{ { 0, "Research new weapons"						}
	, { 1, "Research new sciences"						}
	, { 2, "Research new accessories"					}
	, { 3, "Research new techniques"					}
	, { 4, "Research new armors"						}
	, { 5, "Research new improvements"					}
	, { 6, "Learn new professions"						}
	, { 7, "Train for new ranks"						}
	, { 8, "Research new vehicles"						}
	, { 9, "Research new advancements"					}
	, { 10, "Research new buildings"					}
	, { 11, "Research new construction technologies"	}
	, { 12, "Back to main menu"							}
	};

	while (true) { // Wait for exit request
		int																			tavernChoice								= displayMenu("You take a look at your collected samples..", tavernOptions);

			 if( 0	== tavernChoice ) {	researchWeaponDefinition		(adventurer);	}	// 
		else if( 1	== tavernChoice ) {	researchWeaponModifier			(adventurer);	}	// 
		else if( 2	== tavernChoice ) {	researchAccessoryDefinition		(adventurer);	}	// 
		else if( 3	== tavernChoice ) {	researchAccessoryModifier		(adventurer);	}	// 
		else if( 4	== tavernChoice ) {	researchArmorDefinition			(adventurer);	}	// 
		else if( 5	== tavernChoice ) {	researchArmorModifier			(adventurer);	}	// 
		else if( 6	== tavernChoice ) {	researchProfessionDefinition	(adventurer);	}	// 
		else if( 7	== tavernChoice ) {	researchProfessionModifier		(adventurer);	}	// 
		else if( 8	== tavernChoice ) {	researchVehicleDefinition		(adventurer);	}	// 
		else if( 9	== tavernChoice ) {	researchVehicleModifier			(adventurer);	}	// 
		else if( 10	== tavernChoice ) {	researchFacilityDefinition		(adventurer);	}	// 
		else if( 11	== tavernChoice ) {	researchFacilityModifier		(adventurer);	}	// 
		else if( 12	== tavernChoice ) {	break;											}	// 
		else {	
			printf(optionNotSupported);
		}
	}
}

void																	inspect										(klib::CCharacter& adventurer)						{
	static const klib::SMenuItem<int>											tavernOptions[]								=
	{ { 0,	"Inspect equipped weapon"		}
	, { 1,	"Inspect equipped accessory"	}
	, { 2,	"Inspect equipped armor"		}
	, { 3,	"Inspect active agent"			}
	, { 4,	"Inspect current vehicle"		}
	, { 5,	"Inspect current facility"		}
	, { 6,	"Show inventory"				}
	, { 7,	"Back to main menu"				}
	};	

	while (true) { // Wait for exit request
		int																			tavernChoice								= displayMenu("You wonder about what to do next..", tavernOptions);

		// Interpret user input.
			 if( 0 == tavernChoice ) {	displayWeapon		(adventurer);	}	// 
		else if( 1 == tavernChoice ) {	displayAccessory	(adventurer);	}	// 
		else if( 2 == tavernChoice ) {	displayArmor		(adventurer);	}	// 
		else if( 3 == tavernChoice ) {	displayProfession	(adventurer);	}	// 
		else if( 4 == tavernChoice ) {	displayVehicle		(adventurer);	}	// 
		else if( 5 == tavernChoice ) {	displayFacility		(adventurer);	}	// 
		else if( 6 == tavernChoice ) {	displayInventory	(adventurer.Goods.Inventory.Items, adventurer.Name.c_str());	}	// 
		else if( 7 == tavernChoice ) {	break;	}	// 
		else {	
			printf(optionNotSupported);
		}
	}
}	
	
void																	arsenal										(klib::CCharacter& adventurer)						{
	// This is the main loop of the game and queries for user input until the exit option is selected.
	static const klib::SMenuItem<int>											tavernOptions[]								=
		{ {  0,	"Select agent for the next mission"		}
		, {  1,	"Equip weapon"							}
		, {  2,	"Equip accessory"						}
		, {  3,	"Equip armor"							}
		, {  4,	"Select vehicle"						}
		, {  5,	"Get into building"						}
		, {  6,	"Back to main menu"						}
		};

	while (true) { // Wait for exit request
		int																			tavernChoice								= displayMenu("You wonder about what to do next..", tavernOptions);

		// Interpret user input.
			 if( 0 == tavernChoice ) {	equipProfessionMenu	(adventurer);	}	// 
		else if( 1 == tavernChoice ) {	equipWeaponMenu		(adventurer);	}	// 
		else if( 2 == tavernChoice ) {	equipAccessoryMenu	(adventurer);	}	// 
		else if( 3 == tavernChoice ) {	equipArmorMenu		(adventurer);	}	// 
		else if( 4 == tavernChoice ) {	equipVehicleMenu	(adventurer);	}	// 
		else if( 5 == tavernChoice ) {	equipFacilityMenu	(adventurer);	}	// 
		else if( 6 == tavernChoice ) {	break;	}	// 
		else {	
			printf(optionNotSupported);
		}
	}
}

void																	sell										(klib::CCharacter& adventurer)						{
	while (true) {	// break the loop to leave the shop
		int32_t indexInventory = displayInventoryMenu(adventurer, "Select an item to sell", "Back to tavern");
		if(indexInventory == (int32_t)adventurer.Goods.Inventory.Items.Count) {	// exit option
			indexInventory														= adventurer.Goods.Inventory.Items.Count;	// Exit menu
			break;
		}
		else {
			const klib::SItem& itemEntity = adventurer.Goods.Inventory.Items[indexInventory].Entity;
			int32_t itemPrice =  getItemPrice(itemEntity, true);
			adventurer.Points.Coins += itemPrice;
			adventurer.Goods.Inventory.Items.DecreaseEntity(indexInventory);
			printf("You sold %s and got paid %i coins for it.\n", getItemName(itemEntity).c_str(), itemPrice); 
		}
	}
}

void																	rest										(klib::SCharacter& character)						{
	klib::rest(character);
	printf("\nYou decide to get some rest.\n");
	character.Points.LifeCurrent.Print();
}

void																	tavern										(klib::CCharacter& adventurer)						{
	// This is the main loop of the game and queries for user input until the exit option is selected.
	static const klib::SMenuItem<int>											tavernOptions[]								=
		{ {	0, "Rest"						}
		, {	1, "Look for a mercenary job"	}
		, {	2, "Go to arsenal"				}
		, {	3, "Visit research labs"		}
		, {	4, "Inspect current equipment"	}
		, {	5, "Go for a drink"				}
		, {	6, "Sell stuff you don't need"	}
		, {	7, "Display score"				}
		, {	8, "Exit game"					}
		};

	while (true) { // Wait for exit request
		int																		tavernChoice								= displayMenu("You wonder about what to do next..", tavernOptions);

			 if( 0 == tavernChoice ) {	::rest				(adventurer);	}
		else if( 1 == tavernChoice ) {	mercenaryJob		(adventurer);	}
		else if( 2 == tavernChoice ) {	arsenal				(adventurer);	}
		else if( 3 == tavernChoice ) {	labs				(adventurer);	}
		else if( 4 == tavernChoice ) {	inspect				(adventurer);	}
		else if( 5 == tavernChoice ) {	bar					(adventurer);	}
		else if( 6 == tavernChoice ) {	sell				(adventurer);	}
		else if( 7 == tavernChoice ) {	displayScore		(adventurer.Score);	}
		else if( 8 == tavernChoice ) {	break;	}	// 
		else {	
			printf(optionNotSupported);
		}
	}
}

void																	mercenaryJob								(klib::CCharacter& adventurer)						{
	// Build menu items.
	static const size_t															enemyCount									= ::gpk::size(klib::enemyDefinitions);
	::std::string t;
	klib::SMenuItem<int32_t> jobOptions[enemyCount];
	for(int32_t i=0, count = enemyCount-1; i<count; ++i) {
		jobOptions[i].ReturnValue												= i+1;
		t	= "Level " + std::to_string(i+1);;
		jobOptions[i].Text														= {t.data(), (uint32_t)t.size()};
	}
	jobOptions[enemyCount-1].ReturnValue									= enemyCount;
	jobOptions[enemyCount-1].Text											= "Back to tavern";

	const int32_t																enemyType									= displayMenu("You decide to enroll for a mercenary job", jobOptions);

	if(enemyCount == enemyType)	// This option cancels the loop which causes to exit to the tavern.
		printf("Welcome back, %s.\n", adventurer.Name.c_str());
	else {
		printf("You challenge a %s.\n", klib::enemyDefinitions[enemyType].Name.c_str()); 
		combat(adventurer, enemyType);
	}
}


template<size_t _Size>
static int																initializeItemMenu							(klib::SMenuItem<klib::SItem>(&menuItems)[_Size])	{
	char																		itemOption[128]								= {};
	static const size_t															descriptionCount							= ::gpk::size(klib::itemDescriptions);
	static const size_t															gradeCount									= ::gpk::size(klib::itemGrades);
	for(int32_t indexItem = 0, itemCount = descriptionCount - 1; indexItem < itemCount; ++indexItem) {
		const int32_t																indexDefinition								= (int32_t)indexItem + 1;
		for(int32_t grade = 0; grade < gradeCount; ++grade) {
			const int32_t																finalMenuItemIndex							= indexItem * gradeCount + grade;
			menuItems[finalMenuItemIndex].ReturnValue								= { (int16_t)indexDefinition, 0, (int16_t)grade };
			std::string itemName = klib::getItemName( menuItems[finalMenuItemIndex].ReturnValue );
			sprintf_s(itemOption, "- $%.2u Coins - %s", (int32_t)klib::getItemPrice(menuItems[finalMenuItemIndex].ReturnValue), itemName.c_str());
			menuItems[finalMenuItemIndex].Text										= itemOption;
		}
	}
	menuItems[klib::getFinalItemCount()].ReturnValue						= { (int16_t)klib::getFinalItemCount(), 0, 0 };
	menuItems[klib::getFinalItemCount()].Text								= "Leave the bar";
	return 0;
}

void																	bar											(klib::CCharacter& adventurer)						{
	printf("\nDo you want to buy some drinks?\n\n");

	static const size_t															menuItemCount								= klib::getFinalItemCount()+1;
	static klib::SMenuItem<klib::SItem>											itemOptions[menuItemCount];
	static const int															initialized									= initializeItemMenu(itemOptions);

	char																		menuTitle[128]								= {};
	while (true) {	// break the loop in order to leave the shop
		sprintf_s(menuTitle, "You have %u coins", adventurer.Points.Coins);
		const klib::SItem															selectedItem								= displayMenu(menuTitle, itemOptions);
		if( selectedItem.Definition == klib::getFinalItemCount() ) {
			printf("You leave the bar.\n");
			break;
		}
		else {
			int																		itemPrice									= klib::getItemPrice(selectedItem);	// Get a copy of this value because we use it very often.
			const ::std::string														itemName									= klib::getItemName(selectedItem);

			// Check first for conditions that prevent from acquiring the item
			if(adventurer.Points.Coins < itemPrice)
				printf("You can't afford to buy %s! Choose something else...\n", itemName.c_str());
			else if(adventurer.Goods.Inventory.Items.AddElement(selectedItem)) {	// addItem() returns false if the inventory is full.
				printf("You spend %u coins buying %s.\n", itemPrice, itemName.c_str());
				adventurer.Points.Coins												-= itemPrice;
				adventurer.Score.MoneySpent											+= itemPrice;
			}
			else
				printf("Not enough space in inventory!\n");
		}
	}
	displayInventory(adventurer.Goods.Inventory.Items, adventurer.Name.c_str());
}

// messageFormat requires to support 2 strings at the beginning and an integer at the end: "\n-- %s is carrying %s level %u:\n"
template<typename _EntityType, size_t _definitionCount, size_t _modifierCount>
void																	displayCharacterEquip
	(	const _EntityType									& equip
	,	const std::string									& messageFormat
	,	const std::string									& adventurerName
	,	const std::string									& weaponName
	,	const klib::SEntityRecord<_EntityType>				(&tableDefinitions	)[_definitionCount	]
	,	const klib::SEntityRecord<_EntityType>				(&tableModifiers	)[_modifierCount	]
	)
{
	const klib::SEntityPoints													entityPoints								= klib::getEntityPoints	(equip, tableDefinitions, tableModifiers);
	const klib::SEntityFlags													entityFlags									= klib::getEntityFlags	(equip, tableDefinitions, tableModifiers);

	printf(messageFormat.c_str(), adventurerName.c_str(), weaponName.c_str(), equip.Level);
	entityPoints.Print();
	entityFlags	.Print();
}

void																	displayWeapon								(const klib::CCharacter& adventurer)				{ const klib::SWeapon		& current = adventurer.CurrentEquip.Weapon		; displayCharacterEquip(current, "\n-- %s is carrying %s level %u:\n"		, adventurer.Name, klib::getWeaponName		(current), klib::definitionsWeapon		, klib::modifiersWeapon		); }
void																	displayAccessory							(const klib::CCharacter& adventurer)				{ const klib::SAccessory	& current = adventurer.CurrentEquip.Accessory	; displayCharacterEquip(current, "\n-- %s is wearing %s level %u:\n"		, adventurer.Name, klib::getAccessoryName	(current), klib::definitionsAccessory	, klib::modifiersAccessory	); }
void																	displayArmor								(const klib::CCharacter& adventurer)				{ const klib::SArmor		& current = adventurer.CurrentEquip.Armor		; displayCharacterEquip(current, "\n-- %s is wearing %s level %u:\n"		, adventurer.Name, klib::getArmorName		(current), klib::definitionsArmor		, klib::modifiersArmor		); }
void																	displayProfession							(const klib::CCharacter& adventurer)				{ const klib::SProfession	& current = adventurer.CurrentEquip.Profession	; displayCharacterEquip(current, "\n-- %s is commanding a %s level %u:\n"	, adventurer.Name, klib::getProfessionName	(current), klib::definitionsProfession	, klib::modifiersProfession	); }
void																	displayVehicle								(const klib::CCharacter& adventurer)				{ const klib::SVehicle		& current = adventurer.CurrentEquip.Vehicle		; displayCharacterEquip(current, "\n-- %s is piloting %s level %u:\n"		, adventurer.Name, klib::getVehicleName		(current), klib::definitionsVehicle		, klib::modifiersVehicle	); }
void																	displayFacility								(const klib::CCharacter& adventurer)				{ const klib::SFacility		& current = adventurer.CurrentEquip.Facility	; displayCharacterEquip(current, "\n-- %s is in a %s level %u:\n"			, adventurer.Name, klib::getFacilityName	(current), klib::definitionsFacility	, klib::modifiersFacility	); }
void																	displayResume								(klib::CCharacter& adventurer)						{
	const klib::SEntityPoints													& basePoints		= adventurer.Points;
	const klib::SEntityPoints													& finalPoints		= adventurer.FinalPoints;
	const klib::SEntityFlags													& finalFlags		= adventurer.FinalFlags	;

	printf("\n-- %s final points:\n", adventurer.Name.c_str());
	printf("- Max Life:\n");
	finalPoints.LifeMax.Print();
	printf("- Current Life:\n");
	basePoints.LifeCurrent.Print();
	printf("- Attack:\n");
	finalPoints.Attack.Print();
	printf("- Coins: %i.\n", basePoints.Coins);
	printf("- Bonus Coins per turn: %i.\n", finalPoints.Coins);
	finalFlags.Print();

	printf("\n-- %s base character points:\n", adventurer.Name.c_str());
	printf("- Max Life:\n");
	basePoints.LifeMax.Print();
	printf("- Attack:\n");
	basePoints.Attack.Print();
	finalFlags.Print();
}


//
//void displayEquip(const klib::CCharacter& adventurer) 
//{
//	displayResume		(adventurer);
//	displayProfession	(adventurer);
//	displayWeapon		(adventurer);
//	displayArmor		(adventurer);
//	displayVehicle		(adventurer);
//}

void																	displayScore								(const klib::SCharacterScore& score)				{
	const ::klib::SCharacterScore												& gameCounters								= score;
	printf("\n-- Player statistics:\n\n"
		"Battles Won         : %u\n"
		"Battles Lost        : %u\n"
		"Turns Played        : %u\n"
		"--------------------\n"
		"Enemies Killed      : %u\n"
		"Damage Dealt        : %llu\n"
		"Damage Taken        : %llu\n"
		"--------------------\n"
		"Escapes Succeeded   : %u\n"
		"Escapes Failed      : %u\n"
		"--------------------\n"
		"Money Earned        : %llu\n"
		"Money Spent         : %llu\n"
		"--------------------\n"
		"Attacks Hit         : %u\n"
		"Attacks Missed      : %u\n"
		"Attacks Received    : %u\n"
		"Attacks Avoided     : %u\n"
		"--------------------\n"
		"Potions Used        : %u\n"
		"Grenades Used       : %u\n\n"
		, gameCounters.BattlesWon			
		, gameCounters.BattlesLost
		, gameCounters.TurnsPlayed			
	
		, gameCounters.EnemiesKilled		
		, gameCounters.DamageDealt			
		, gameCounters.DamageTaken			
	
		, gameCounters.EscapesSucceeded	
		, gameCounters.EscapesFailed		
	
		, gameCounters.MoneyEarned			
		, gameCounters.MoneySpent			

		, gameCounters.AttacksHit			
		, gameCounters.AttacksMissed		
		, gameCounters.AttacksReceived		
		, gameCounters.AttacksAvoided		

		, gameCounters.PotionsUsed			
		, gameCounters.GrenadesUsed		
	);
}
