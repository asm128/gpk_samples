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
template<typename _tEntity>
bool																	equipEntityIfResearched
	( uint32_t									slotIndex
	, ::klib::SEntityContainer		<_tEntity>	& entityContainer
	, const ::klib::SResearchGroup	<_tEntity>	& completedResearchPlayer
	, const ::klib::SResearchGroup	<_tEntity>	& completedResearchAgent
	, const ::klib::SEntityTable	<_tEntity>	& table
	, _tEntity									& equippedEntity
	, const ::gpk::view_const_char				& cantAccessDefinitionError
	, const ::gpk::view_const_char				& cantAccessModifierError
	, const ::gpk::view_const_char				& storeOldWeaponMessage
	, const ::gpk::view_const_char				& equipNewWeaponMessage
	, const ::gpk::view_const_char				& researchTypeString
	)
{
	if(slotIndex >= entityContainer.size()) //invalid index do nothing.
		return false;

	bool																		bCancel											= false;
	if( 0 < entityContainer[slotIndex].Entity.Definition && (-1) == completedResearchPlayer.Definitions.FindElement(entityContainer[slotIndex].Entity.Definition) && (-1) == completedResearchAgent.Definitions.FindElement(entityContainer[slotIndex].Entity.Definition)) {
		bCancel																	= true;
		printf(cantAccessDefinitionError.begin(), table.Definitions[entityContainer[slotIndex].Entity.Definition].Name.begin());	//
	}

	if( 0 < entityContainer[slotIndex].Entity.Modifier	&& (-1) == completedResearchPlayer.Modifiers.FindElement(entityContainer[slotIndex].Entity.Modifier) && (-1) == completedResearchAgent.Modifiers.FindElement(entityContainer[slotIndex].Entity.Modifier)) {
		bCancel																	= true;
		static char																	itemText	[128]								= {};
		sprintf_s(itemText, table.Modifiers[entityContainer[slotIndex].Entity.Modifier].Name.begin(), researchTypeString.begin());
		printf(cantAccessModifierError.begin(), itemText);
	}

	if(bCancel)
		return false;

	printf(storeOldWeaponMessage.begin(), ::klib::getEntityName(table, equippedEntity).begin(), equippedEntity.Level);
	unequipEntity(entityContainer, equippedEntity);
	equippedEntity																= entityContainer[slotIndex].Entity;	// Assign selected entity.
	entityContainer.DecreaseEntity(slotIndex);
	printf(equipNewWeaponMessage.begin(), ::klib::getEntityName(table, equippedEntity).begin(), equippedEntity.Level);
	return true;
}

template<typename _tEntity>
void																	equipEntityMenu
	( ::klib::SEntityContainer		<_tEntity>	& characterInventory
	, const ::klib::SResearchGroup	<_tEntity>	& completedResearch
	, const ::klib::SEntityTable	<_tEntity>	& table
	, _tEntity									& currentEntity
	, const ::gpk::view_const_char				& noWeaponMessage
	, const ::gpk::view_const_char				& currentlyCarryingMessage
	, const ::gpk::view_const_char				& selectYourChoiceMessage
	, const ::gpk::view_const_char				& cantAccessDefinitionError
	, const ::gpk::view_const_char				& cantAccessModifierError
	, const ::gpk::view_const_char				& storeOldWeaponMessage
	, const ::gpk::view_const_char				& equipNewWeaponMessage
	, const ::gpk::view_const_char				& researchTypeString
	)
{
	if( 0 == characterInventory.size() )	{
		printf("%s", noWeaponMessage.begin());
		return;
	}

	static char																	menuItemText[256]							= {};
	static ::klib::SMenuItem<int16_t>											menuItems	[256]							= {};
	int32_t																		menuItemCount								= 0;

	for(int32_t i=0, count = characterInventory.size(); i<count; ++i) {
		sprintf_s(menuItemText, "x%.02i: %s level %u", characterInventory[i].Count, ::klib::getEntityName(table, characterInventory[i].Entity).begin(), characterInventory[i].Entity.Level);
		menuItems[menuItemCount++]												= {(int16_t)i, menuItemText};
	}

	printf(currentlyCarryingMessage.begin(), ::klib::getEntityName(table, currentEntity).begin(), currentEntity.Level);
	menuItems[menuItemCount++]												= {256, "Exit this menu"};
	int32_t																		selectedValue								= displayMenu(selectYourChoiceMessage, ::gpk::view_array<const ::klib::SMenuItem<int16_t>>{menuItems}, menuItemCount);
	if(selectedValue == 256)
		return;

	equipEntityIfResearched(selectedValue, characterInventory, completedResearch, completedResearch, table, currentEntity, cantAccessDefinitionError, cantAccessModifierError, storeOldWeaponMessage, equipNewWeaponMessage, researchTypeString);
}

void																	menuEquipWeapon								(const ::klib::SEntityTable<::klib::SWeapon> & table, ::klib::CCharacter& adventurer)						{
	equipEntityMenu
		( adventurer.Goods.Inventory			.Weapon
		, adventurer.Goods.CompletedResearch	.Weapon
		, table
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

void																	menuEquipAccessory							(const ::klib::SEntityTable<::klib::SAccessory> & table, ::klib::CCharacter& adventurer)						{
	equipEntityMenu
		( adventurer.Goods.Inventory			.Accessory
		, adventurer.Goods.CompletedResearch	.Accessory
		, table
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

void																	menuEquipArmor								(const ::klib::SEntityTable<::klib::SArmor> & table, ::klib::CCharacter& adventurer)						{
	equipEntityMenu
		( adventurer.Goods.Inventory			.Armor
		, adventurer.Goods.CompletedResearch	.Armor
		, table
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

void																	menuEquipProfession							(const ::klib::SEntityTable<::klib::SProfession> & table, ::klib::CCharacter& adventurer)						{
	equipEntityMenu
		( adventurer.Goods.Inventory			.Profession
		, adventurer.Goods.CompletedResearch	.Profession
		, table
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

void																	menuEquipVehicle							(const ::klib::SEntityTable<::klib::SVehicle> & table, ::klib::CCharacter& adventurer)						{
	equipEntityMenu
		( adventurer.Goods.Inventory			.Vehicle
		, adventurer.Goods.CompletedResearch	.Vehicle
		, table
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

void																	menuEquipFacility							(const ::klib::SEntityTable<::klib::SFacility> & table, ::klib::CCharacter& adventurer)						{
	equipEntityMenu
		( adventurer.Goods.Inventory			.Facility
		, adventurer.Goods.CompletedResearch	.Facility
		, table
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

void																	researchWeaponDefinition					(const ::klib::SEntityTable<::klib::SWeapon		> & table, ::klib::CCharacter& adventurer);
void																	researchWeaponModifier						(const ::klib::SEntityTable<::klib::SWeapon		> & table, ::klib::CCharacter& adventurer);
void																	researchArmorDefinition						(const ::klib::SEntityTable<::klib::SArmor		> & table, ::klib::CCharacter& adventurer);
void																	researchArmorModifier						(const ::klib::SEntityTable<::klib::SArmor		> & table, ::klib::CCharacter& adventurer);
void																	researchAccessoryDefinition					(const ::klib::SEntityTable<::klib::SAccessory	> & table, ::klib::CCharacter& adventurer);
void																	researchAccessoryModifier					(const ::klib::SEntityTable<::klib::SAccessory	> & table, ::klib::CCharacter& adventurer);
void																	researchProfessionDefinition				(const ::klib::SEntityTable<::klib::SProfession	> & table, ::klib::CCharacter& adventurer);
void																	researchProfessionModifier					(const ::klib::SEntityTable<::klib::SProfession	> & table, ::klib::CCharacter& adventurer);
void																	researchVehicleDefinition					(const ::klib::SEntityTable<::klib::SVehicle	> & table, ::klib::CCharacter& adventurer);
void																	researchVehicleModifier						(const ::klib::SEntityTable<::klib::SVehicle	> & table, ::klib::CCharacter& adventurer);
void																	researchFacilityDefinition					(const ::klib::SEntityTable<::klib::SFacility	> & table, ::klib::CCharacter& adventurer);
void																	researchFacilityModifier					(const ::klib::SEntityTable<::klib::SFacility	> & table, ::klib::CCharacter& adventurer);

static const char														optionNotSupported[]						= "Option not supported yet. Please select a valid option.\n";

void																	labs										(const ::klib::SEntityTables & tables, ::klib::CCharacter& adventurer)						{
	// This is the main loop of the game and queries for user input until the exit option is selected.
	static const ::klib::SMenuItem<int>											tavernOptions[]								=
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
		int																			tavernChoice								= displayMenu("You take a look at your collected samples..", ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{tavernOptions});

			 if( 0	== tavernChoice ) {	researchWeaponDefinition		(tables.Weapon		, adventurer);	}	//
		else if( 1	== tavernChoice ) {	researchWeaponModifier			(tables.Weapon		, adventurer);	}	//
		else if( 2	== tavernChoice ) {	researchAccessoryDefinition		(tables.Accessory	, adventurer);	}	//
		else if( 3	== tavernChoice ) {	researchAccessoryModifier		(tables.Accessory	, adventurer);	}	//
		else if( 4	== tavernChoice ) {	researchArmorDefinition			(tables.Armor		, adventurer);	}	//
		else if( 5	== tavernChoice ) {	researchArmorModifier			(tables.Armor		, adventurer);	}	//
		else if( 6	== tavernChoice ) {	researchProfessionDefinition	(tables.Profession	, adventurer);	}	//
		else if( 7	== tavernChoice ) {	researchProfessionModifier		(tables.Profession	, adventurer);	}	//
		else if( 8	== tavernChoice ) {	researchVehicleDefinition		(tables.Vehicle		, adventurer);	}	//
		else if( 9	== tavernChoice ) {	researchVehicleModifier			(tables.Vehicle		, adventurer);	}	//
		else if( 10	== tavernChoice ) {	researchFacilityDefinition		(tables.Facility	, adventurer);	}	//
		else if( 11	== tavernChoice ) {	researchFacilityModifier		(tables.Facility	, adventurer);	}	//
		else if( 12	== tavernChoice ) {	break;											}	//
		else {
			printf(optionNotSupported);
		}
	}
}

void																	inspect										(const ::klib::SEntityTables & tables, ::klib::CCharacter& adventurer)						{
	static const ::klib::SMenuItem<int>											tavernOptions[]								=
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
		int																			tavernChoice								= displayMenu("You wonder about what to do next..", ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{tavernOptions});

		// Interpret user input.
			 if( 0 == tavernChoice ) {	displayWeapon		(tables, adventurer);	}	//
		else if( 1 == tavernChoice ) {	displayAccessory	(tables, adventurer);	}	//
		else if( 2 == tavernChoice ) {	displayArmor		(tables, adventurer);	}	//
		else if( 3 == tavernChoice ) {	displayProfession	(tables, adventurer);	}	//
		else if( 4 == tavernChoice ) {	displayVehicle		(tables, adventurer);	}	//
		else if( 5 == tavernChoice ) {	displayFacility		(tables, adventurer);	}	//
		else if( 6 == tavernChoice ) {	displayInventory	(adventurer.Goods.Inventory.Items, adventurer.Name);	}	//
		else if( 7 == tavernChoice ) {	break;	}	//
		else {
			printf(optionNotSupported);
		}
	}
}

void																	arsenal										(const ::klib::SEntityTables & entityTables, ::klib::CCharacter& adventurer)						{
	// This is the main loop of the game and queries for user input until the exit option is selected.
	static const ::klib::SMenuItem<int>											tavernOptions[]								=
		{ {  0,	"Select agent for the next mission"		}
		, {  1,	"Equip weapon"							}
		, {  2,	"Equip accessory"						}
		, {  3,	"Equip armor"							}
		, {  4,	"Select vehicle"						}
		, {  5,	"Get into building"						}
		, {  6,	"Back to main menu"						}
		};

	while (true) { // Wait for exit request
		int																			tavernChoice								= displayMenu("You wonder about what to do next..", ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{tavernOptions});

		// Interpret user input.
			 if( 0 == tavernChoice ) {	menuEquipProfession	(entityTables.Profession, adventurer);	}	//
		else if( 1 == tavernChoice ) {	menuEquipWeapon		(entityTables.Weapon	, adventurer);	}	//
		else if( 2 == tavernChoice ) {	menuEquipAccessory	(entityTables.Accessory	, adventurer);	}	//
		else if( 3 == tavernChoice ) {	menuEquipArmor		(entityTables.Armor		, adventurer);	}	//
		else if( 4 == tavernChoice ) {	menuEquipVehicle	(entityTables.Vehicle	, adventurer);	}	//
		else if( 5 == tavernChoice ) {	menuEquipFacility	(entityTables.Facility	, adventurer);	}	//
		else if( 6 == tavernChoice ) {	break;	}	//
		else {
			printf(optionNotSupported);
		}
	}
}

void																	sell										(::klib::CCharacter& adventurer)						{
	while (true) {	// break the loop to leave the shop
		int32_t indexInventory = displayInventoryMenu(adventurer, "Select an item to sell", "Back to tavern");
		if(indexInventory == (int32_t)adventurer.Goods.Inventory.Items.size()) {	// exit option
			indexInventory														= adventurer.Goods.Inventory.Items.size();	// Exit menu
			break;
		}
		else {
			const ::klib::SItem& itemEntity = adventurer.Goods.Inventory.Items[indexInventory].Entity;
			int32_t itemPrice =  getItemPrice(itemEntity, true);
			adventurer.Points.Coins += itemPrice;
			adventurer.Goods.Inventory.Items.DecreaseEntity(indexInventory);
			printf("You sold %s and got paid %i coins for it.\n", getItemName(itemEntity).begin(), itemPrice);
		}
	}
}

void																	rest										(const ::klib::SEntityTables & tables, ::klib::SCharacter& character)						{
	klib::rest(tables, character);
	printf("\nYou decide to get some rest.\n");
	character.Points.LifeCurrent.Print();
}

void																	tavern										(const ::klib::SEntityTables & entityTables, ::klib::CCharacter& adventurer)						{
	// This is the main loop of the game and queries for user input until the exit option is selected.
	static const ::klib::SMenuItem<int>											tavernOptions[]								=
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
		int																		tavernChoice								= displayMenu("You wonder about what to do next..", ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{tavernOptions});

			 if( 0 == tavernChoice ) {	::rest				(entityTables, adventurer);	}
		else if( 1 == tavernChoice ) {	mercenaryJob		(entityTables, adventurer);	}
		else if( 2 == tavernChoice ) {	arsenal				(entityTables, adventurer);	}
		else if( 3 == tavernChoice ) {	labs				(entityTables, adventurer);	}
		else if( 4 == tavernChoice ) {	inspect				(entityTables, adventurer);	}
		else if( 5 == tavernChoice ) {	bar					(adventurer);	}
		else if( 6 == tavernChoice ) {	sell				(adventurer);	}
		else if( 7 == tavernChoice ) {	displayScore		(adventurer.Score);	}
		else if( 8 == tavernChoice ) {	break;	}	//
		else {
			printf(optionNotSupported);
		}
	}
}

void																	mercenaryJob								(const ::klib::SEntityTables & tables, ::klib::CCharacter& adventurer)						{
	// Build menu items.
	static const size_t															enemyCount									= ::gpk::size(klib::enemyDefinitions);
	char																		t		[64];
	klib::SMenuItem<int32_t>		jobOptions[enemyCount];
	for(int32_t i=0, count = enemyCount-1; i<count; ++i) {
		jobOptions[i].ReturnValue												= i+1;
		sprintf_s(t, "Level %i", i+1);
		jobOptions[i].Text														= ::gpk::view_const_string{t};
	}
	jobOptions[enemyCount-1].ReturnValue									= enemyCount;
	jobOptions[enemyCount-1].Text											= "Back to tavern";

	const int32_t																enemyType									= displayMenu("You decide to enroll for a mercenary job", ::gpk::view_array<const ::klib::SMenuItem<int32_t>>{jobOptions});

	if(enemyCount == enemyType)	// This option cancels the loop which causes to exit to the tavern.
		printf("Welcome back, %s.\n", adventurer.Name.begin());
	else {
		printf("You challenge a %s.\n", ::klib::enemyDefinitions[enemyType].Name.begin());
		::klib::SGameMessages	messages;
		combat(tables, messages, adventurer, enemyType);
	}
}


template<size_t _Size>
static int																initializeItemMenu							(::klib::SMenuItem<klib::SItem>(&menuItems)[_Size])	{
	char																		itemOption[128]								= {};
	static const size_t															descriptionCount							= ::gpk::size(klib::itemDescriptions);
	static const size_t															gradeCount									= ::klib::itemGrades.size();
	for(int32_t indexItem = 0, itemCount = descriptionCount - 1; indexItem < itemCount; ++indexItem) {
		const int32_t																indexDefinition								= (int32_t)indexItem + 1;
		for(int32_t grade = 0; grade < gradeCount; ++grade) {
			const int32_t																finalMenuItemIndex							= indexItem * gradeCount + grade;
			menuItems[finalMenuItemIndex].ReturnValue								= { (int16_t)indexDefinition, 0, (int16_t)grade };
			const ::gpk::array_pod<char_t>												itemName									= ::klib::getItemName( menuItems[finalMenuItemIndex].ReturnValue );
			sprintf_s(itemOption, "- $%.2u Coins - %s", (int32_t)klib::getItemPrice(menuItems[finalMenuItemIndex].ReturnValue), itemName.begin());
			menuItems[finalMenuItemIndex].Text										= itemOption;
		}
	}
	menuItems[klib::getFinalItemCount()].ReturnValue						= { (int16_t)klib::getFinalItemCount(), 0, 0 };
	menuItems[klib::getFinalItemCount()].Text								= "Leave the bar";
	return 0;
}

void																	bar											(::klib::CCharacter& adventurer)						{
	printf("\nDo you want to buy some drinks?\n\n");

	static const size_t															menuItemCount								= klib::getFinalItemCount()+1;
	static klib::SMenuItem<klib::SItem>											itemOptions[menuItemCount];
	static const int															initialized									= ::initializeItemMenu(itemOptions);

	char																		menuTitle[128]								= {};
	while (true) {	// break the loop in order to leave the shop
		sprintf_s(menuTitle, "You have %lli coins", adventurer.Points.Coins);
		const ::klib::SItem															selectedItem								= ::displayMenu(menuTitle, ::gpk::view_array<const ::klib::SMenuItem<::klib::SItem>>{itemOptions});
		if( selectedItem.Definition == klib::getFinalItemCount() ) {
			printf("You leave the bar.\n");
			break;
		}
		else {
			int																		itemPrice									= klib::getItemPrice(selectedItem);	// Get a copy of this value because we use it very often.
			const ::gpk::array_pod<char_t>											itemName									= klib::getItemName(selectedItem);

			// Check first for conditions that prevent from acquiring the item
			if(adventurer.Points.Coins < itemPrice)
				printf("You can't afford to buy %s! Choose something else...\n", itemName.begin());
			else if(adventurer.Goods.Inventory.Items.AddElement(selectedItem)) {	// addItem() returns false if the inventory is full.
				printf("You spend %u coins buying %s.\n", itemPrice, itemName.begin());
				adventurer.Points.Coins												-= itemPrice;
				adventurer.Score.MoneySpent											+= itemPrice;
			}
			else
				printf("Not enough space in inventory!\n");
		}
	}
	displayInventory(adventurer.Goods.Inventory.Items, adventurer.Name);
}

// messageFormat requires to support 2 strings at the beginning and an integer at the end: "\n-- %s is carrying %s level %u:\n"
template<typename _tEntity>
void																	displayCharacterEquip
	( const _tEntity														& equip
	, const ::gpk::view_const_char											& messageFormat
	, const ::gpk::view_const_char											& adventurerName
	, const ::gpk::view_const_char											& weaponName
	, const ::klib::SEntityTable<_tEntity>									& table
	)
{
	const ::klib::SEntityPoints													entityPoints								= ::klib::getEntityPoints	(table, equip);
	const ::klib::SEntityFlags													entityFlags									= ::klib::getEntityFlags	(table, equip);

	printf(messageFormat.begin(), adventurerName.begin(), weaponName.begin(), equip.Level);
	entityPoints.Print();
	entityFlags	.Print();
}

void																	displayWeapon								(const ::klib::SEntityTables & tables, const klib::CCharacter& adventurer)				{ const ::klib::SWeapon		& current = adventurer.CurrentEquip.Weapon		; displayCharacterEquip(current, "\n-- %s is carrying %s level %u:\n"		, adventurer.Name, klib::getEntityName(tables.Weapon	, current), tables.Weapon		); }
void																	displayAccessory							(const ::klib::SEntityTables & tables, const klib::CCharacter& adventurer)				{ const ::klib::SAccessory	& current = adventurer.CurrentEquip.Accessory	; displayCharacterEquip(current, "\n-- %s is wearing %s level %u:\n"		, adventurer.Name, klib::getEntityName(tables.Accessory	, current), tables.Accessory	); }
void																	displayArmor								(const ::klib::SEntityTables & tables, const klib::CCharacter& adventurer)				{ const ::klib::SArmor		& current = adventurer.CurrentEquip.Armor		; displayCharacterEquip(current, "\n-- %s is wearing %s level %u:\n"		, adventurer.Name, klib::getEntityName(tables.Armor		, current), tables.Armor		); }
void																	displayProfession							(const ::klib::SEntityTables & tables, const klib::CCharacter& adventurer)				{ const ::klib::SProfession	& current = adventurer.CurrentEquip.Profession	; displayCharacterEquip(current, "\n-- %s is commanding a %s level %u:\n"	, adventurer.Name, klib::getEntityName(tables.Profession, current), tables.Profession	); }
void																	displayVehicle								(const ::klib::SEntityTables & tables, const klib::CCharacter& adventurer)				{ const ::klib::SVehicle	& current = adventurer.CurrentEquip.Vehicle		; displayCharacterEquip(current, "\n-- %s is piloting %s level %u:\n"		, adventurer.Name, klib::getEntityName(tables.Vehicle	, current), tables.Vehicle		); }
void																	displayFacility								(const ::klib::SEntityTables & tables, const klib::CCharacter& adventurer)				{ const ::klib::SFacility	& current = adventurer.CurrentEquip.Facility	; displayCharacterEquip(current, "\n-- %s is in a %s level %u:\n"			, adventurer.Name, klib::getEntityName(tables.Facility	, current), tables.Facility		); }
void																	displayResume								(::klib::CCharacter& adventurer)						{
	const ::klib::SEntityPoints													& basePoints		= adventurer.Points;
	const ::klib::SEntityPoints													& finalPoints		= adventurer.FinalPoints;
	const ::klib::SEntityFlags													& finalFlags		= adventurer.FinalFlags	;

	printf("\n-- %s final points:\n", adventurer.Name.begin());
	printf("- Max Life:\n");
	finalPoints.LifeMax.Print();
	printf("- Current Life:\n");
	basePoints.LifeCurrent.Print();
	printf("- Attack:\n");
	finalPoints.Attack.Print();
	printf("- Coins: %lli.\n", basePoints.Coins);
	printf("- Bonus Coins per turn: %lli.\n", finalPoints.Coins);
	finalFlags.Print();

	printf("\n-- %s base character points:\n", adventurer.Name.begin());
	printf("- Max Life:\n");
	basePoints.LifeMax.Print();
	printf("- Attack:\n");
	basePoints.Attack.Print();
	finalFlags.Print();
}

void																	displayInventory							(const ::klib::SEntityContainer<klib::SItem>& inventory, const ::gpk::view_const_char& characterName)												{
	printf("\n-- %s's inventory --\n", characterName.begin());
	if(inventory.size()) {
		printf("You look at the remaining supplies...\n");
		for (unsigned int i = 0; i < inventory.size(); i++)
			printf("%u: x%.2u %s.\n", i + 1, inventory[i].Count, ::klib::getItemName(inventory[i].Entity).begin());
	}
	printf("\n");
}

void																	displayScore								(const ::klib::SCharacterScore& score)				{
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

		, gameCounters.UsedPotions
		, gameCounters.UsedGrenades
	);
}
