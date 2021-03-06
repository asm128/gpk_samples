#include "Game.h"
#include "GameResearch.h"

#include "Menu.h"

#include <algorithm>

void				researchWeaponDefinition			(const ::klib::SEntityTable<::klib::SWeapon> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Weapon, adventurer.Goods.CompletedResearch.Weapon.Definitions, table.Definitions, adventurer.Goods.CompletedResearch.Weapon.MaxResearch, false	, true
		, "%s Blueprint"
		, "You've designed all the blueprints available! (%u)\n"
		, "You don't have any weapon sample available to research yet!\n"
		, "Select weapon to research"
		, "You start researching %s.\n"
		, "You have designed a %s blueprint and user manual!\n"
	);
}

void				researchWeaponModifier				(const ::klib::SEntityTable<::klib::SWeapon> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Weapon, adventurer.Goods.CompletedResearch.Weapon.Modifiers, table.Modifiers, adventurer.Goods.CompletedResearch.Weapon.MaxResearch, true	, true
		, "Science"
		, "You've researched all the sciences available! (%u)\n"
		, "You don't have any theory available to experiment yet!\n"
		, "Select science to master"
		, "You start experimenting with %s.\n"
		, "You have developed a novel method for %s implementation and everyone in the world loves you for it!\n"
	);
}


void				researchArmorDefinition				(const ::klib::SEntityTable<::klib::SArmor> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Armor, adventurer.Goods.CompletedResearch.Armor.Definitions, table.Definitions, adventurer.Goods.CompletedResearch.Armor.MaxResearch, false	, true
		, "%s Design"
		, "You've designed all the armors available! (%u)\n"
		, "You don't have any idea available to design yet!\n"
		, "Select armor to design"
		, "You start designing %s.\n"
		, "You have finished your high-end %s design with the common combat enhacements that identify your work!\n"
	);
}

void				researchArmorModifier				(const ::klib::SEntityTable<::klib::SArmor> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Armor, adventurer.Goods.CompletedResearch.Armor.Modifiers, table.Modifiers, adventurer.Goods.CompletedResearch.Armor.MaxResearch, true	, true
		, "Technology"
		, "You've researched all the armor technologies available! (%u)\n"
		, "You don't have any armor sample to study from yet!\n"
		, "Select technology to study"
		, "You start experimenting %s.\n"
		, "You have developed a stylized and expensive %s technology and everyone in the world loves you for it!\n"
	);
}

void				researchAccessoryDefinition			(const ::klib::SEntityTable<::klib::SAccessory> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Accessory, adventurer.Goods.CompletedResearch.Accessory.Definitions, table.Definitions, adventurer.Goods.CompletedResearch.Accessory.MaxResearch, false	, true
		, "%s Accessory"
		, "You've researched all the accessories available! (%u)\n"
		, "You don't have any accessories available to research yet!\n"
		, "Select accessory to research"
		, "You start researching %s.\n"
		, "You got to win the beauty contest by showing your new %s!\n"
	);
}

void				researchAccessoryModifier			(const ::klib::SEntityTable<::klib::SAccessory> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Accessory, adventurer.Goods.CompletedResearch.Accessory.Modifiers, table.Modifiers, adventurer.Goods.CompletedResearch.Accessory.MaxResearch, true	, true
		, "Crafting"
		, "You've researched all the crafting techniques available! (%u)\n"
		, "You don't have any crafting techniques to learn about yet!\n"
		, "Select technique to research"
		, "You start practicing %s.\n"
		, "You become known as the inventor of %s.\n"
	);
}

void				researchProfessionDefinition		(const ::klib::SEntityTable<::klib::SProfession> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Profession, adventurer.Goods.CompletedResearch.Profession.Definitions, table.Definitions, adventurer.Goods.CompletedResearch.Profession.MaxResearch, false	, true
		, "%s Mastery"
		, "You've mastered all the professions available! (%u)\n"
		, "You don't have any profession notes available to learn from yet!\n"
		, "Select profession to learn"
		, "You start learning %s.\n"
		, "You have learned all you could about being a %s!\n"
	);
}

void				researchProfessionModifier			(const ::klib::SEntityTable<::klib::SProfession> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Profession, adventurer.Goods.CompletedResearch.Profession.Modifiers, table.Modifiers, adventurer.Goods.CompletedResearch.Profession.MaxResearch, true	, true
		, "Rank"
		, "You've achieved all the ranks available! (%u)\n"
		, "You don't have any rank notes available to learn from yet!\n"
		, "Select rank to train"
		, "You start training for %s.\n"
		, "You have achieved a superior military rank for %s and everyone in the world loves you for it!\n"
	);
}

void				researchVehicleDefinition			(const ::klib::SEntityTable<::klib::SVehicle> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Vehicle, adventurer.Goods.CompletedResearch.Vehicle.Definitions, table.Definitions, adventurer.Goods.CompletedResearch.Vehicle.MaxResearch, false	, true
		, "%s Prototype"
		, "You've designed all the vehicles available! (%u)\n"
		, "You don't have any transportation technologies available to research yet!\n"
		, "Select vehicle to research"
		, "You start researching %s.\n"
		, "You have engineered a %s! Simulator sold separately...\n"
	);
}

void				researchVehicleModifier				(const ::klib::SEntityTable<::klib::SVehicle> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Vehicle, adventurer.Goods.CompletedResearch.Vehicle.Modifiers, table.Modifiers, adventurer.Goods.CompletedResearch.Vehicle.MaxResearch, true	, true
		, "Mechanics"
		, "You've engineered all the vehicle mechanics available! (%u)\n"
		, "You don't have any mechanics available to experiment with yet!\n"
		, "Select mechanics to engineer"
		, "You start engineering %s.\n"
		, "You have mounted an advanced combat vehicle production line for %s and you get a Nobel Peace Prize!\n"
	);
}

void				researchFacilityDefinition			(const ::klib::SEntityTable<::klib::SFacility> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Facility, adventurer.Goods.CompletedResearch.Facility.Definitions, table.Definitions, adventurer.Goods.CompletedResearch.Facility.MaxResearch, false	, true
		, "%s Architecture"
		, "You've researched all the architectures available! (%u)\n"
		, "You don't have any architectures available to research yet!\n"
		, "Select building to research"
		, "You start research %s.\n"
		, "Your new %s has been approved and your neighbors will have a new reason to enjoy the view!\n"
	);
}

void				researchFacilityModifier			(const ::klib::SEntityTable<::klib::SFacility> & table, ::klib::CCharacter& adventurer)										{
	::research
	(adventurer.Goods.Inventory.Facility, adventurer.Goods.CompletedResearch.Facility.Modifiers, table.Modifiers, adventurer.Goods.CompletedResearch.Facility.MaxResearch, true	, true
		, "Construction"
		, "You've researched all the construction technology available! (%u)\n"
		, "You don't have any construction technology to learn about yet!\n"
		, "Select technology to research"
		, "You start experimenting %s.\n"
		, "You reinvented %s and earned an army of followers!\n"
	);
}
