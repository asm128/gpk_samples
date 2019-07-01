#include "Character.h"
#include "Combat.h"
#include "GameMenu.h"

#ifndef __GAME_H__99823740927349023649827346982734__
#define __GAME_H__99823740927349023649827346982734__

void																tavern										(klib::CCharacter				& adventurer);	// Main loop of the game. From there the player can choose to go shopping, fighting or take a nap to recover life points.
void																mercenaryJob								(klib::CCharacter				& adventurer);	// Displays the combat difficulty menu from where the player can start combat or go back to the tavern.
void																bar											(klib::CCharacter				& adventurer);	// Displays the available items for buying along with the player money.
void																displayArmor								(const klib::CCharacter			& adventurer);	// 
void																displayWeapon								(const klib::CCharacter			& adventurer);	// 
void																displayAccessory							(const klib::CCharacter			& adventurer);	// 
void																displayVehicle								(const klib::CCharacter			& adventurer);	// 
void																displayProfession							(const klib::CCharacter			& adventurer);	// 
void																displayFacility								(const klib::CCharacter			& adventurer);	// 
void																displayScore								(const klib::SCharacterScore	& adventurer);	// Displays the player's character points and statistics.

template <size_t _InventorySize>
void																displayInventory							(const klib::SEntityContainer<klib::SItem, _InventorySize>& inventory, const ::std::string& characterName)												{
	printf("\n-- %s's inventory --\n", characterName.c_str());
	if(inventory.Count) {
		printf("You look at the remaining supplies...\n");
		for (unsigned int i = 0; i < inventory.Count; i++)
			printf("%u: x%.2u %s.\n", i + 1, inventory[i].Count, klib::getItemName(inventory[i].Entity).c_str());
	}
	printf("\n");
}

template <size_t _Size1, size_t _Size2>
int32_t																displayInventoryMenu						(klib::CCharacter& adventurer, const char (&menuTitle)[_Size1], const char (&exitOption)[_Size2], bool bPrintPrice=false, bool bSellPrice=true)			{
	::klib::SMenuItem<int32_t>												itemOptions	[MAX_INVENTORY_SLOTS+1]			= {};
	char																	itemOption	[128]							= {};
	for(uint32_t i=0; i<adventurer.Goods.Inventory.Items.Count; ++i) {
		const klib::SItem														& itemEntity								= adventurer.Goods.Inventory.Items[i].Entity;
		::std::string															itemName									= klib::getItemName(itemEntity);
		int32_t																	finalPrice									= klib::getItemPrice(itemEntity, bSellPrice);

		if(bPrintPrice)
			sprintf_s(itemOption, "%i coins each - x%.2u %s", finalPrice, adventurer.Goods.Inventory.Items[i].Count, itemName.c_str());
		else
			sprintf_s(itemOption, "- x%.2u %s", adventurer.Goods.Inventory.Items[i].Count, itemName.c_str());
		
		itemOptions[i].ReturnValue											= i;
 		itemOptions[i].Text													= itemOption;
	}
	itemOptions[adventurer.Goods.Inventory.Items.Count].ReturnValue		= adventurer.Goods.Inventory.Items.Count;
	itemOptions[adventurer.Goods.Inventory.Items.Count].Text			= exitOption;

	return displayMenu(adventurer.Goods.Inventory.Items.Count+1, menuTitle, itemOptions);
}

// Combat is executed from the mercenary job menu and executes the battle turns until one of the combatants is dead.
void																combat										(klib::CCharacter& adventurer, int32_t enemyType);	

#endif // __GAME_H__99823740927349023649827346982734__