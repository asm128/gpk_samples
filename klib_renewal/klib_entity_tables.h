#include "gpk_view_array.h"
#include "EntityImpl.h"

#ifndef KLIB_TABLES_H_09283
#define KLIB_TABLES_H_09283

namespace klib
{

	struct SEntityTables {
		::klib::SEntityTable<::klib::SProfession>		Profession			= {};
		::klib::SEntityTable<::klib::SWeapon	>		Weapon				= {};
		::klib::SEntityTable<::klib::SArmor		>		Armor				= {};
		::klib::SEntityTable<::klib::SAccessory	>		Accessory			= {};
		::klib::SEntityTable<::klib::SVehicle	>		Vehicle				= {};
		::klib::SEntityTable<::klib::SFacility	>		Facility			= {};
		::klib::SEntityTable<::klib::SStageProp	>		StageProp			= {};
		::klib::SEntityTable<::klib::STile		>		Tile				= {};
		::klib::SEntityTable<::klib::SItem		>		Item				= {};
	};

	static inline	SEntityPoints	getArmorPoints		(const ::klib::SEntityTables & entityTables, const SArmor& entity) { return ::klib::getEntityPoints	(entityTables.Armor, entity); }
	static inline	SEntityFlags	getArmorFlags		(const ::klib::SEntityTables & entityTables, const SArmor& entity) { return ::klib::getEntityFlags	(entityTables.Armor, entity); }

	static inline	SEntityPoints	getAccessoryPoints	(const ::klib::SEntityTables & entityTables, const SAccessory& entity) { return ::klib::getEntityPoints	(entityTables.Accessory, entity); }
	static inline	SEntityFlags	getAccessoryFlags	(const ::klib::SEntityTables & entityTables, const SAccessory& entity) { return ::klib::getEntityFlags	(entityTables.Accessory, entity); }

	static inline	SEntityPoints	getFacilityPoints	(const ::klib::SEntityTables & entityTables, const SFacility& entity) { return ::klib::getEntityPoints	(entityTables.Facility, entity); }
	static inline	SEntityFlags	getFacilityFlags	(const ::klib::SEntityTables & entityTables, const SFacility& entity) { return ::klib::getEntityFlags	(entityTables.Facility, entity); }

	static inline	SEntityPoints	getProfessionPoints	(const ::klib::SEntityTables & entityTables, const SProfession& entity) { return ::klib::getEntityPoints(entityTables.Profession, entity); }
	static inline	SEntityFlags	getProfessionFlags	(const ::klib::SEntityTables & entityTables, const SProfession& entity) { return ::klib::getEntityFlags	(entityTables.Profession, entity); }

	static inline	SEntityPoints	getStagePropPoints	(const ::klib::SEntityTables & entityTables, const SStageProp& entity) { return ::klib::getEntityPoints	(entityTables.StageProp, entity); }
	static inline	SEntityFlags	getStagePropFlags	(const ::klib::SEntityTables & entityTables, const SStageProp& entity) { return ::klib::getEntityFlags	(entityTables.StageProp, entity); }

	static inline	SEntityPoints	getVehiclePoints	(const ::klib::SEntityTables & entityTables, const SVehicle& entity) { return ::klib::getEntityPoints	(entityTables.Vehicle, entity); }
	static inline	SEntityFlags	getVehicleFlags		(const ::klib::SEntityTables & entityTables, const SVehicle& entity) { return ::klib::getEntityFlags	(entityTables.Vehicle, entity); }

	static inline	SEntityPoints	getWeaponPoints	(const ::klib::SEntityTables & entityTables, const SWeapon& entity) { return ::klib::getEntityPoints(entityTables.Weapon, entity); }
	static inline	SEntityFlags	getWeaponFlags	(const ::klib::SEntityTables & entityTables, const SWeapon& entity) { return ::klib::getEntityFlags	(entityTables.Weapon, entity); }
} // namespace

#endif // KLIB_TABLES_H_09283
