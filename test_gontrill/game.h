// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "weapon.h"
#include "ship.h"

#include "gpk_coord.h"
#include "gpk_view_bit.h"
#include "gpk_array_static.h"

#ifndef GAME_H_982374982374
#define GAME_H_982374982374

#pragma pack(push, 1)

enum PLAYER_TYPE : int8_t
	{ PLAYER_TYPE_PLAYER				= 0
	, PLAYER_TYPE_ENEMY
	, PLAYER_TYPE_COUNT
	, PLAYER_TYPE_INVALID				= -1
	};

enum HEALTH_TYPE : int8_t
	{ HEALTH_TYPE_HEALTH				= 0
	, HEALTH_TYPE_SHIELD
	, HEALTH_TYPE_LIFE
	, HEALTH_TYPE_WEAPON_LEVEL
	, HEALTH_TYPE_SHIP_LEVEL
	, HEALTH_TYPE_COUNT
	, HEALTH_TYPE_INVALID				= -1
	};

enum BUFF_TYPE : int8_t
	{ BUFF_TYPE_POINTS					= 0
	, BUFF_TYPE_FORCE_FIELD
	, BUFF_TYPE_FIRE_RATIO
	, BUFF_TYPE_COUNT
	, BUFF_TYPE_INVALID					= -1
	};

enum POWERUP_FAMILY : int8_t
	{ POWERUP_FAMILY_WEAPON				= 0
	, POWERUP_FAMILY_HEALTH
	, POWERUP_FAMILY_BUFF
	, POWERUP_FAMILY_BOMB
	, POWERUP_FAMILY_COUNT
	, POWERUP_FAMILY_INVALID			= -1
	};

enum GAME_ELEMENT : int8_t
	{ GAME_ELEMENT_PLAYER				= 0
	, GAME_ELEMENT_ENEMY
	, GAME_ELEMENT_POWERUP
	, GAME_ELEMENT_SHOT
	, GAME_ELEMENT_PROP
	, GAME_ELEMENT_COUNT
	, GAME_ELEMENT_INVALID				= -1
	};

struct SPowerup {
						BUFF_TYPE																TypeBuff															= BUFF_TYPE_INVALID;
						HEALTH_TYPE																TypeHealth															= HEALTH_TYPE_INVALID;
						WEAPON_TYPE																TypeWeapon															= WEAPON_TYPE_INVALID;
};

struct SWeapon {
						int32_t																	IndexProperties;
};

struct SProjectile {
						float																	Speed;
						double																	TimeLived;
						WEAPON_TYPE																TypeWeapon;
						PLAYER_TYPE																TypePlayer;
						int32_t																	ShipIndex;
};

static constexpr	const int32_t															MAX_PLAYERS															= 4;
static constexpr	const int32_t															MAX_POWERUP															= 64;
static constexpr	const int32_t															MAX_ENEMIES															= 128;
static constexpr	const int32_t															MAX_PROJECTILES														= 512;

template<size_t _sizeArray>
struct SArrayElementState : public ::gpk::view_bit<uint64_t> {
public:
	typedef				uint64_t																TStorage;
						TStorage																Data				[(_sizeArray / (8 * sizeof(TStorage))) + 1]		= {};

																								SArrayElementState													()						: view_bit(Data, _sizeArray) {}
};
#pragma pack(pop)

template<size_t _sizeArray>
					::gpk::error_t															firstUnused															(const ::SArrayElementState<_sizeArray>& container)			{
	for(uint32_t iObject = 0; iObject < _sizeArray; ++iObject)
		if(0 == container[iObject])
			return iObject;
	return -1;
}

static constexpr	const uint32_t															MAGIC_NUMBER														= 1397704771;

template<uint32_t _sizeArray>
struct SPropertiesPowerup		{
						::SArrayElementState						< _sizeArray>				Alive																= {};
						::gpk::array_static<::gpk::n2<float>	, _sizeArray>				Position															= {};
						::gpk::array_static<POWERUP_FAMILY			, _sizeArray>				Family																= {};
						::gpk::array_static<::SPowerup				, _sizeArray>				Type																= {};
};

template<uint32_t _sizeArray>
struct SPropertiesProjectile	{
						::SArrayElementState						< _sizeArray>				Alive																= {};
						::gpk::array_static<::SProjectile			, _sizeArray>				Projectiles															= {};
						::gpk::array_static<::SHealthPoints			, _sizeArray>				Health																= {};
};

template<uint32_t _sizeArray>
struct SPropertiesEnemy			{
						::SArrayElementState						< _sizeArray>				Alive																= {};
						::gpk::array_static<::SShipState			, _sizeArray>				States																= {};
						::gpk::array_static<::gpk::n2<float>	, _sizeArray>				Position															= {};
						::gpk::array_static<::gpk::n2<float>	, _sizeArray>				Direction															= {};
						::gpk::array_static<::SHealthPoints			, _sizeArray>				Health																= {};
						::gpk::array_static<double					, _sizeArray>				TimeLived															= {};
						::gpk::array_static<double					, _sizeArray>				SkillTimer															= {};
						::gpk::array_static<int8_t					, _sizeArray>				Target																= {};
						::gpk::array_static<::SWeapon				, _sizeArray>				Weapon																= {};
						::gpk::array_static<double					, _sizeArray>				WeaponDelay															= {};
						::gpk::array_static<uint32_t				, _sizeArray>				PathStep															= {};
						::gpk::array_static<float					, _sizeArray>				TimerPath															= {};
};

template<uint32_t _sizeArray>
struct SPropertiesShip			{
						::SArrayElementState						< _sizeArray>				Alive																= {};
						::gpk::array_static<::SShipState			, _sizeArray>				States																= {};
						::gpk::array_static<::SHealthPoints			, _sizeArray>				Health																= {};
						::gpk::array_static<::SWeapon				, _sizeArray>				Weapon																= {};
						::gpk::array_static<::gpk::n2<float>	, _sizeArray>				Position															= {};
						::gpk::array_static<::gpk::n2<float>	, _sizeArray>				Direction															= {};
						::gpk::array_static<bool					, _sizeArray>				LineOfFire															= {};
						::gpk::array_static<double					, _sizeArray>				WeaponDelay															= {};
};

struct SGame {
						uint32_t																ShipsPlaying														= 2;
						::SPropertiesShip		<MAX_PLAYERS>									Ships																= {};
						::SPropertiesEnemy		<MAX_ENEMIES>									Enemies																= {};
						::SPropertiesProjectile	<MAX_PROJECTILES>								Projectiles															= {};
						::SPropertiesPowerup	<MAX_POWERUP>									Powerups															= {};
						::gpk::array_static<::gpk::n2<float>, MAX_PLAYERS>					PositionCrosshair													= {};
						uint32_t																CountEnemies														= 0;
						uint32_t																CountProjectiles													= 0;
						uint32_t																CountPowerups														= 0;

						float																	HalfWidthShip														= 5;
						float																	HalfWidthCrosshair													= 5;
						float																	HalfWidthEnemy														= 5;
						float																	HalfWidthPowerup													= 5;

						double																	GhostTimer															= 0;
						uint32_t																PathStep															= 0;
						::gpk::n2<float>													PathEnemy					[10]									=
							{ { 10.f,  10.f}
							, {320.f, 180.f}
							, { 50.f, 200.f}
							, {480.f,  10.f}
							, {150.f,  20.f}
							, {180.f, 320.f}
							, {200.f,  50.f}
							, { 10.f, 480.f}
							, { 50.f,  50.f}
							, { 20.f, 120.f}
							};
};

#endif // GAME_H_982374982374
