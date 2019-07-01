#include "Player.h"
#include "klib_grid.h"
#include "menus.h"
#include "TacticalInfo.h"

#include "gpk_sync.h"

#include <time.h>

#ifndef __GAME_H__91827309126391263192312312354__
#define __GAME_H__91827309126391263192312312354__

namespace klib
{
	static				double										getFinalSight			(double initialSight, const CCharacter& playerAgent)						{
		double																finalSight				= initialSight;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_SHOCK		))	finalSight *= 1.5	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_BURN		))	finalSight *= 1.3	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_RAGE		))	finalSight *= 1.2	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_BERSERK	))	finalSight *= 1.1	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_CHARMED	))	finalSight *= 0.85	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_FROZEN		))	finalSight *= 0.75	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_DRUNK		))	finalSight *= 0.65	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_BLIND		))	finalSight *= 0.3	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_SLEEP		))	finalSight *= 0.2	;
		if(::gpk::bit_true(playerAgent.ActiveBonus.Status.Status, COMBAT_STATUS_STUN		))	finalSight *= 0.1	;
		return finalSight;
	}

#define SIGHT_OFFSET	1.7
#define RANGE_OFFSET	1.7

	static inline		double										getFinalSight			(const CCharacter& playerAgent, const SEntityPoints& playerAgentPoints)						{	return getFinalSight(playerAgentPoints.Fitness	.Sight + SIGHT_OFFSET, playerAgent);	}
	static inline		double										getFinalRange			(const CCharacter& playerAgent, const SEntityPoints& playerAgentPoints)						{	return getFinalSight(playerAgentPoints.Attack	.Range + RANGE_OFFSET, playerAgent);	}

	template<size_t _Width, size_t _Depth>
	struct SWeightedDisplay {
		static constexpr	const uint32_t								Width					= (uint32_t)_Width;
		static constexpr	const uint32_t								Depth					= (uint32_t)_Depth;

							::klib::SGrid<char		, _Width, _Depth>	Screen					= {};
							::klib::SGrid<uint16_t	, _Width, _Depth>	TextAttributes			= {};
							::klib::SGrid<float		, _Width, _Depth>	DisplayWeights			= {};
							::klib::SGrid<float		, _Width, _Depth>	Speed					= {};
							::klib::SGrid<float		, _Width, _Depth>	SpeedTarget				= {};

		inline				void										Clear					()																							{
			::klib::clearGrid(Screen			, ' ');
			::klib::clearGrid(TextAttributes	, (uint16_t)COLOR_WHITE);
			::klib::clearGrid(DisplayWeights	, 0.0f);
			::klib::clearGrid(Speed			, 0.0f);
			::klib::clearGrid(SpeedTarget	, 0.0f);
		}
	};


#define DEFAULT_ASCII_DISPLAY_HEIGHT	83
#define DEFAULT_ASCII_DISPLAY_WIDTH		((uint32_t)(DEFAULT_ASCII_SCREEN_HEIGHT*2.666666f))
	typedef	SWeightedDisplay<DEFAULT_ASCII_DISPLAY_WIDTH, DEFAULT_ASCII_DISPLAY_HEIGHT>	SGlobalDisplay		;
	typedef SWeightedDisplay<STacticalBoard::Width, STacticalBoard::Depth>				STacticalDisplay	;
	typedef SWeightedDisplay<STacticalBoard::Width, STacticalBoard::Depth>				SPostEffectDisplay	;
	typedef ::klib::SGrid<char, SGlobalDisplay::Width, SGlobalDisplay::Depth>			SMenuDisplay		;

	//
	struct SFrameInfo {
							::klib::SInput								Input			= {};
							::klib::STimer								Timer			= {};
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

	//----------------------------------------------------------------------------------------------------------------------------------------------
	struct SGame
	{
		struct SLogLine {
								uint16_t								Color;
								const std::string						Message;
		};

							// Game Flags tell us about the			current state of the application.
							GAME_FLAGS								Flags							= (GAME_FLAGS)(GAME_FLAGS_NETWORK_ENABLED | GAME_FLAGS_TURN_BUSY);
							GAME_MODE								Mode							= GAME_MODE_CAMPAIGN;	// This is the default because it's the only available mode at the moment
							SGameState								State							= {GAME_STATE_MENU_MAIN,};
							SGameState								PreviousState					= {GAME_STATE_MENU_MAIN,};
							uint64_t								ServerTime						= 0;
							int64_t									Seed							= 0;

							SPlayer									Players[MAX_PLAYER_TYPES]		= {};

							::klib::SInput							FrameInput						= {};
							::klib::STimer							FrameTimer						= {};

							// Tactical board.
							STacticalInfo							TacticalInfo					= {};

							// Displays.
							STacticalDisplay						TacticalDisplay					= {};
							SPostEffectDisplay						PostEffectDisplay				= {};
							SGlobalDisplay							GlobalDisplay					= {};
							SMenuDisplay							MenuDisplay						= {};

							// Feedback messages.
							::std::string							StateMessage					= "";
							::std::string							UserMessage						= "";
							::std::string							UserSuccess						= "";
							::std::string							UserMiss						= "";
							::std::string							UserError						= "";
							::gpk::array_obj<SLogLine>				UserLog							= {};

							// For the special effect
							char									SlowMessage[256]				= {'_',};

							::std::mutex							PlayerMutex						= {};
							::std::mutex							ServerTimeMutex					= {};

							void									ClearDisplays					()																						{
			TacticalDisplay		.Clear();	
			GlobalDisplay		.Clear();	
			PostEffectDisplay	.Clear();	
			clearGrid(MenuDisplay);
		}

							void									ClearMessages					()
		{
			UserError	= 
			UserMessage	= 
			UserMiss	= 
			UserSuccess	= "";
		}

		inline				void									LogMessage						()																						{ UserLog.push_back({COLOR_YELLOW	, UserMessage	}); }
		inline				void									LogSuccess						()																						{ UserLog.push_back({COLOR_CYAN		, UserSuccess	}); }
		inline				void									LogError						()																						{ UserLog.push_back({COLOR_RED		, UserError		}); }
		inline				void									LogMiss							()																						{ UserLog.push_back({COLOR_RED		, UserMiss		}); }
		inline				void									LogStateMessage					()																						{ UserLog.push_back({COLOR_CYAN		, StateMessage	}); }
	};	// struct

	//----------------------------------------------------------------------------------------------------------------------------------------------
	// functions
						void									initGame						(SGame& instanceGame);
						void									resetGame						(SGame& instanceGame);
						void									showMenu						(SGame& instanceGame);
						void									initTacticalMap					(SGame& instanceGame);
	static inline		PLAYER_INDEX							getCurrentPlayerIndex			(const STacticalInfo& tacticalInfo)														{ return ( tacticalInfo.CurrentPlayer == -1) ? PLAYER_INDEX_INVALID : tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]; }
						uint32_t								missionCost						(SPlayer& player, const SSquad& squadSetup, uint32_t maxAgents=MAX_AGENT_SQUAD_SLOTS);
} // namespace 

#endif // __GAME_H__91827309126391263192312312354__