#include "Player.h"
#include "klib_grid.h"
#include "menus.h"
#include "TacticalInfo.h"
#include "klib_timer.h"

#include "gpk_sync.h"

#include <time.h>

#ifndef __GAME_H__91827309126391263192312312354__
#define __GAME_H__91827309126391263192312312354__

namespace klib
{
	static				double										getFinalSight			(double initialSight, const CCharacter& playerAgent)						{
		double																finalSight				= initialSight;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_SHOCK	))	finalSight *= 1.5	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_BURN	))	finalSight *= 1.3	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_RAGE	))	finalSight *= 1.2	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_BERSERK	))	finalSight *= 1.1	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_CHARMED	))	finalSight *= 0.85	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_FROZEN	))	finalSight *= 0.75	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_DRUNK	))	finalSight *= 0.65	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_BLIND	))	finalSight *= 0.3	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_SLEEP	))	finalSight *= 0.2	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_STUN	))	finalSight *= 0.1	;
		return finalSight;
	}

#define SIGHT_OFFSET	1.7
#define RANGE_OFFSET	1.7

	static inline		double						getFinalSight			(const CCharacter& playerAgent, const SEntityPoints& playerAgentPoints)						{	return getFinalSight(playerAgentPoints.Fitness	.Sight + SIGHT_OFFSET, playerAgent);	}
	static inline		double						getFinalRange			(const CCharacter& playerAgent, const SEntityPoints& playerAgentPoints)						{	return getFinalSight(playerAgentPoints.Attack	.Range + RANGE_OFFSET, playerAgent);	}

	struct SWeightedDisplay {
							::gpk::SRenderTarget<char, uint16_t>	Screen					= {};
							::gpk::SImage<float	>					DisplayWeights			= {};
							::gpk::SImage<float	>					Speed					= {};
							::gpk::SImage<float	>					SpeedTarget				= {};

		inline				::gpk::error_t							Resize					(::gpk::SCoord2<uint32_t> newSize)															{
			Screen			.resize(newSize, ' ', (uint16_t)::klib::ASCII_COLOR_INDEX_WHITE);
			DisplayWeights	.resize(newSize, 0.0f);
			Speed			.resize(newSize, 0.0f);
			SpeedTarget		.resize(newSize, 0.0f);
			return 0;
		}
		inline				void									Clear					()																							{
			::klib::clearGrid(Screen.Color			.View, ' ');
			::klib::clearGrid(Screen.DepthStencil	.View, (uint16_t)::klib::ASCII_COLOR_INDEX_WHITE);
			::klib::clearGrid(DisplayWeights		.View, 0.0f);
			::klib::clearGrid(Speed					.View, 0.0f);
			::klib::clearGrid(SpeedTarget			.View, 0.0f);
		}
	};

	// Game Mode talks about the tactical mode mostly.
	GDEFINE_ENUM_TYPE(GAME_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(GAME_MODE, CAMPAIGN	, 0);
	GDEFINE_ENUM_VALUE(GAME_MODE, SKIRMISH	, 1);
	GDEFINE_ENUM_VALUE(GAME_MODE, LAN		, 2);
	GDEFINE_ENUM_VALUE(GAME_MODE, ONLINE	, 3);

	// Game Flags tell us about the current state of the application.
	GDEFINE_ENUM_TYPE(GAME_FLAGS, uint16_t);
	GDEFINE_ENUM_VALUE(GAME_FLAGS, RUNNING			, 0x0001);
	GDEFINE_ENUM_VALUE(GAME_FLAGS, STARTED			, 0x0002);
	GDEFINE_ENUM_VALUE(GAME_FLAGS, CONNECTED		, 0x0004);
	GDEFINE_ENUM_VALUE(GAME_FLAGS, NETWORK_ENABLED	, 0x0008);
	GDEFINE_ENUM_VALUE(GAME_FLAGS, TACTICAL			, 0x0010);
	GDEFINE_ENUM_VALUE(GAME_FLAGS, TURN_BUSY		, 0x0020);
	GDEFINE_ENUM_VALUE(GAME_FLAGS, TACTICAL_REMOTE	, 0x0040);
	GDEFINE_ENUM_VALUE(GAME_FLAGS, CHEATSON			, 0x0080);
	GDEFINE_ENUM_VALUE(GAME_FLAGS, HELPON			, 0x0100);

	// Game Flags tell us about the current state of the application.
	GDEFINE_ENUM_TYPE(GAME_EFFECT, uint8_t);
	GDEFINE_ENUM_VALUE(GAME_EFFECT, FOGOFWAR	, 0x01		);
	GDEFINE_ENUM_VALUE(GAME_EFFECT, TEAMONLY	, 0x02		);
	GDEFINE_ENUM_VALUE(GAME_EFFECT, AOEFFECT	, 0x04		);
	GDEFINE_ENUM_VALUE(GAME_EFFECT, CHEATSON	, 0x08		);

	// Game cheats.
	GDEFINE_ENUM_TYPE(GAME_CHEAT, uint16_t);
	GDEFINE_ENUM_VALUE(GAME_CHEAT, IMMORTAL	, 0x0001);	// Player's agents can't be killed.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, LONGRUNS	, 0x0002);	// Give 127 movement points to the current character.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, RESEARCH	, 0x0004);	// Have everything researched in the game.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, BUILDALL	, 0x0008);	// Build 99 of everything as long as there is room for it in your inventory. For free.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, BUYWORLD	, 0x0010);	// Buy 99 of everything as long as there is room for it in your inventory. For free.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, SUPERMAN	, 0x0020);	// Add extra 50% of the base character points to all characters.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, UNABOMBA	, 0x0040);	// Every shot is explosive.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, GIVEMEHP	, 0x0080);	// Gives Health	to an agent for the amount of max Health for that character.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, GIVEMEMP	, 0x0100);	// Gives Shield	to an agent for the amount of max Shield for that character.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, GIVEMESP	, 0x0200);	// Gives Mana	to an agent for the amount of max Mana	 for that character.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, TAKEMEHP	, 0x0400);	// Removes Health	from an agent for 10% of the amount of max Health for that character.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, TAKEMEMP	, 0x0800);	// Removes Shield	from an agent for 10% of the amount of max Shield for that character.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, TAKEMESP	, 0x1000);	// Removes Mana		from an agent for 10% of the amount of max Mana   for that character.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, UNLIMITD	, 0x2000);	// Nothing gets discounted or removed on use.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, MAGNATES	, 0x4000);	// Get 1,000,000,000 coins in funds and another 1,000,000,000 to be split between your army.
	GDEFINE_ENUM_VALUE(GAME_CHEAT, IMWINNER	, 0x8000);	// Win the tactical game.

#define MAX_PLAYER_TYPES 16
	struct SGameMenus {
	};
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (GAME_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(GAME_EVENT, NOOP		, 0x00);
	GDEFINE_ENUM_VALUE(GAME_EVENT, CONFIRM	, 0x01);
	GDEFINE_ENUM_VALUE(GAME_EVENT, CANCEL	, 0x02);

	struct SGameEvent {
							::klib::GAME_EVENT						Event;
							::klib::SGameState						GameState;
							//::klib::ENTITY_TYPE						EntityType;
							//::klib::ENTITY_GRADE					EntityGrade;
							//::klib::SEntity							Entity;
							int64_t									Value;
	};
#pragma pack(pop)
	static constexpr const uint32_t		SHOP_EXIT_VALUE				= 0x7FFF;

	struct SBuyable {
		int16_t							Definition;
		int16_t							Grade;
		int64_t							Price;
		int64_t							MaintenanceCost;
		::gpk::array_pod<char_t>		Name;
	};

	struct SShopMenus {
		::klib::SMenuItem<::klib::SBuyable>		MenuItemsAccessory	[256]	= {};
		::klib::SMenuItem<::klib::SBuyable>		MenuItemsStageProp	[256]	= {};
		::klib::SMenuItem<::klib::SBuyable>		MenuItemsFacility	[256]	= {};
		::klib::SMenuItem<::klib::SBuyable>		MenuItemsVehicle	[256]	= {};
		::klib::SMenuItem<::klib::SBuyable>		MenuItemsProfession	[256]	= {};
		::klib::SMenuItem<::klib::SBuyable>		MenuItemsWeapon		[256]	= {};
		::klib::SMenuItem<::klib::SBuyable>		MenuItemsArmor		[256]	= {};
		::klib::SMenuItem<::klib::SBuyable>		MenuItemsItem		[256]	= {};
		::klib::SMenuItem<::klib::SBuyable>		MenuItemsAgent		[256]	= {};

		::klib::SMenuHeader<::klib::SBuyable>	MenuAccessory				= {{::klib::SHOP_EXIT_VALUE},	::gpk::view_const_string{"Accessory"	" a la carte"}, 48};
		::klib::SMenuHeader<::klib::SBuyable>	MenuStageProp				= {{::klib::SHOP_EXIT_VALUE},	::gpk::view_const_string{"Stage Prop"	" a la carte"}, 48};
		::klib::SMenuHeader<::klib::SBuyable>	MenuFacility				= {{::klib::SHOP_EXIT_VALUE},	::gpk::view_const_string{"Facility"		" a la carte"}, 48};
		::klib::SMenuHeader<::klib::SBuyable>	MenuVehicle					= {{::klib::SHOP_EXIT_VALUE},	::gpk::view_const_string{"Vehicle"		" a la carte"}, 48};
		::klib::SMenuHeader<::klib::SBuyable>	MenuProfession				= {{::klib::SHOP_EXIT_VALUE},	::gpk::view_const_string{"Job License"	" a la carte"}, 48};
		::klib::SMenuHeader<::klib::SBuyable>	MenuWeapon					= {{::klib::SHOP_EXIT_VALUE},	::gpk::view_const_string{"Weapon"		" a la carte"}, 48};
		::klib::SMenuHeader<::klib::SBuyable>	MenuArmor					= {{::klib::SHOP_EXIT_VALUE},	::gpk::view_const_string{"Armor"		" a la carte"}, 48};
		::klib::SMenuHeader<::klib::SBuyable>	MenuItem					= {{::klib::SHOP_EXIT_VALUE},	::gpk::view_const_string{"Item"			" a la carte"}, 48};
		::klib::SMenuHeader<::klib::SBuyable>	MenuAgent					= {{::klib::SHOP_EXIT_VALUE},	::gpk::view_const_string{"Agent"		" a la carte"}, 48};

		::gpk::array_pod<char_t>				NamesAccessory		[256]	= {};
		::gpk::array_pod<char_t>				NamesStageProp		[256]	= {};
		::gpk::array_pod<char_t>				NamesFacility		[256]	= {};
		::gpk::array_pod<char_t>				NamesVehicle		[256]	= {};
		::gpk::array_pod<char_t>				NamesProfession		[256]	= {};
		::gpk::array_pod<char_t>				NamesWeapon			[256]	= {};
		::gpk::array_pod<char_t>				NamesArmor			[256]	= {};
		::gpk::array_pod<char_t>				NamesItem			[256]	= {};
		::gpk::array_pod<char_t>				NamesAgent			[256]	= {};
	};

	::gpk::error_t												initBuyMenus		(const ::klib::SEntityTables & entityTables, ::klib::SShopMenus& menus);
	::gpk::error_t												reinitBuyMenus		(const ::klib::SEntityTables & entityTables, ::klib::SCharacterInventory & playerInventory, ::klib::SShopMenus& menus);

	//----------------------------------------------------------------------------------------------------------------------------------------------
	struct SGame {
							// Game Flags tell us about the			current state of the application.
							::klib::GAME_FLAGS						Flags							= (::klib::GAME_FLAGS)(::klib::GAME_FLAGS_NETWORK_ENABLED | ::klib::GAME_FLAGS_TURN_BUSY);
							::klib::GAME_MODE						Mode							= ::klib::GAME_MODE_CAMPAIGN;	// This is the default because it's the only available mode at the moment
							::klib::SGameState						State							= {::klib::GAME_STATE_MENU_MAIN,};
							::klib::SGameState						PreviousState					= {::klib::GAME_STATE_MENU_MAIN,};
							uint64_t								ServerTime						= 0;
							int64_t									Seed							= 0;

							::klib::SGamePlayer						Players[MAX_PLAYER_TYPES]		= {};

							::klib::SInput							FrameInput						= {};
							::klib::STimer							FrameTimer						= {};

							// Tactical board.
							::klib::STacticalInfo					TacticalInfo					= {};

							// Displays.
							::klib::SWeightedDisplay				TacticalDisplay					= {};
							::klib::SWeightedDisplay				GlobalDisplay					= {};

							// Feedback messages.
							::klib::SGameMessages					Messages;
							::gpk::array_pod<::klib::SGameEvent>	Events;

							// For the special effect
							::klib::SEntityTables					EntityTables					= {};

							::std::mutex							PlayerMutex						= {};
							::std::mutex							ServerTimeMutex					= {};

							::klib::SShopMenus						ShopMenus;

							void									ClearDisplays					()																						{
			TacticalDisplay	.Clear();
			GlobalDisplay	.Clear();
		}

		inline				void									LogAuxStateMessage				()	{ Messages.LogAuxStateMessage	(); }
		inline				void									LogAuxMessage					()	{ Messages.LogAuxMessage		(); }
		inline				void									LogAuxSuccess					()	{ Messages.LogAuxSuccess		(); }
		inline				void									LogAuxError						()	{ Messages.LogAuxError			(); }
		inline				void									LogAuxMiss						()	{ Messages.LogAuxMiss			(); }

		inline				void									LogStateMessage					()	{ Messages.LogStateMessage	(); }
		inline				void									LogMessage						()	{ Messages.LogMessage		(); }
		inline				void									LogSuccess						()	{ Messages.LogSuccess		(); }
		inline				void									LogError						()	{ Messages.LogError			(); }
		inline				void									LogMiss							()	{ Messages.LogMiss			(); }
		inline				void									ClearMessages					()	{ Messages.ClearMessages	(); }
	};	// struct

	//----------------------------------------------------------------------------------------------------------------------------------------------
	// functions
						::gpk::error_t							initGame						(::klib::SGame & instanceGame);
						::gpk::error_t							resetGame						(::klib::SGame & instanceGame);
						::gpk::error_t							eventProcess					(::klib::SGame & instanceGame);
						::gpk::error_t							initTacticalMap					(::klib::SGame & instanceGame);
						::gpk::error_t							handleMissionEnd				(::klib::SGame & instanceGame);
						::gpk::error_t							showMenu						(::klib::SGame & instanceGame);
	static inline		::klib::PLAYER_INDEX					getCurrentPlayerIndex			(const ::klib::STacticalInfo& tacticalInfo)				{ return ( tacticalInfo.CurrentPlayer == -1) ? (::klib::PLAYER_INDEX)-1 : tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]; }
						int64_t									missionCost						(const ::klib::SGamePlayer& player, const SSquad& squadSetup, uint32_t maxAgents = ::klib::MAX_AGENT_SQUAD_SLOTS);
} // namespace

#endif // __GAME_H__91827309126391263192312312354__
