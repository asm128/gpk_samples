#include "Character.h"
#include "Combat.h"
#include "Item.h"
#include "GameMenu.h"

#ifndef __GAME_H__99823740927349023649827346982734__
#define __GAME_H__99823740927349023649827346982734__

void																tavern										(const ::klib::SEntityTables & entityTables, ::klib::CCharacter				& adventurer);	// Main loop of the game. From there the player can choose to go shopping, fighting or take a nap to recover life points.
void																mercenaryJob								(const ::klib::SEntityTables & tables, ::klib::CCharacter				& adventurer);	// Displays the combat difficulty menu from where the player can start combat or go back to the tavern.
void																bar											(::klib::CCharacter				& adventurer);	// Displays the available items for buying along with the player money.
void																displayArmor								(const ::klib::SEntityTables & tables, const klib::CCharacter			& adventurer);	//
void																displayWeapon								(const ::klib::SEntityTables & tables, const klib::CCharacter			& adventurer);	//
void																displayAccessory							(const ::klib::SEntityTables & tables, const klib::CCharacter			& adventurer);	//
void																displayVehicle								(const ::klib::SEntityTables & tables, const klib::CCharacter			& adventurer);	//
void																displayProfession							(const ::klib::SEntityTables & tables, const klib::CCharacter			& adventurer);	//
void																displayFacility								(const ::klib::SEntityTables & tables, const klib::CCharacter			& adventurer);	//
void																displayScore								(const ::klib::SCharacterScore	& adventurer);	// Displays the player's character points and statistics.
void																displayInventory							(const ::klib::SEntityContainer<klib::SItem>& inventory, const ::gpk::vcc& characterName);
template <size_t _Size1, size_t _Size2>
int32_t																displayInventoryMenu						(klib::CCharacter& adventurer, const char (&menuTitle)[_Size1], const char (&exitOption)[_Size2], bool bPrintPrice=false, bool bSellPrice=true)			{
	::klib::SMenuItem<int32_t>												itemOptions	[4096+1]						= {};
	char																	itemOption	[128]							= {};
	for(uint32_t i=0; i<adventurer.Goods.Inventory.Items.Slots.size(); ++i) {
		const ::klib::SItem														& itemEntity								= adventurer.Goods.Inventory.Items[i].Entity;
		const ::gpk::array_pod<char>											itemName									= ::klib::getItemName(itemEntity);
		int32_t																	finalPrice									= ::klib::getItemPrice(itemEntity, bSellPrice);

		if(bPrintPrice)
			sprintf_s(itemOption, "%i coins each - x%.2u %s", finalPrice, adventurer.Goods.Inventory.Items[i].Count, itemName.begin());
		else
			sprintf_s(itemOption, "- x%.2u %s", adventurer.Goods.Inventory.Items[i].Count, itemName.begin());

		itemOptions[i].ReturnValue											= i;
 		itemOptions[i].Text													= itemOption;
	}
	itemOptions[adventurer.Goods.Inventory.Items.Slots.size()].ReturnValue	= adventurer.Goods.Inventory.Items.Slots.size();
	itemOptions[adventurer.Goods.Inventory.Items.Slots.size()].Text			= exitOption;

	return displayMenu(adventurer.Goods.Inventory.Items.Slots.size() + 1, menuTitle, ::gpk::view<const ::klib::SMenuItem<int32_t>>{itemOptions});
}

// Combat is executed from the mercenary job menu and executes the battle turns until one of the combatants is dead.
void																combat										(const ::klib::SEntityTables & tables, struct klib::SGameMessages & messages, klib::CCharacter& adventurer, int32_t enemyType);

#endif // __GAME_H__99823740927349023649827346982734__
