#include "EntityHelper.h"
#include "EntityImpl.h"

#ifndef __CHARACTERPOINTS_H__0298734092864380972346234__
#define __CHARACTERPOINTS_H__0298734092864380972346234__

namespace klib
{
#pragma pack(push, 1)
	// This struct holds a counter for every action that we want to record mostly for score purposes.
	struct SCharacterScore {
		uint64_t					MoneyEarned				= 0;
		uint64_t					MoneySpent				= 0;

		uint64_t					DamageDealt				= 0;
		uint64_t					DamageTaken				= 0;

		uint32_t					TurnsPlayed				= 0;

		uint32_t					BattlesWon				= 0;
		uint32_t					BattlesLost				= 0;

		uint32_t					EscapesSucceeded		= 0;
		uint32_t					EscapesFailed			= 0;

		uint32_t					EnemiesKilled			= 0;
		uint32_t					AttacksHit				= 0;
		uint32_t					AttacksMissed			= 0;
		uint32_t					AttacksReceived			= 0;
		uint32_t					AttacksAvoided			= 0;

		uint32_t					PotionsUsed				= 0;
		uint32_t					GrenadesUsed			= 0;

		uint32_t					CompletedResearch		= 0;
		uint32_t					CompletedProduction		= 0;
		uint32_t					CompletedUpgrade		= 0;
	};


	// This structure holds the max and current values for a given point gauge such as health or shield which can't reach either a lower level than 0 or a higher level than the max value
	struct SPointGauge {
		int32_t						Max		;
		int32_t						Current	;
		// -------------------------
		SPointGauge&				operator =				(int32_t valueToClamp)								{ Current  = (valueToClamp < Max) ? valueToClamp : Max; Current = (Current < 0) ? 0 : Current; return *this; };
		SPointGauge&				operator-=				(int32_t valueToClamp)								{ Current -= valueToClamp; operator=(Current);	return *this; };
		SPointGauge&				operator+=				(int32_t valueToClamp)								{ Current += valueToClamp; operator=(Current);	return *this; };

		SPointGauge&				operator =				(const SPointGauge& other)				noexcept	{ Max  = other.Max; Current  = other.Current;	return *this; }
		SPointGauge&				operator-=				(const SPointGauge& other)				noexcept	{ Max -= other.Max; Current -= other.Current;	return *this; }
		SPointGauge&				operator+=				(const SPointGauge& other)				noexcept	{ Max += other.Max; Current += other.Current;	return *this; }
	};

	struct SEntityGauges {
		SPointGauge					Health	;
		SPointGauge					Mana	;
		SPointGauge					Shield	;

		void						Print					()								const				{
			char							formatted[64]			= {};
			::sprintf_s(formatted, "%i/%i", Health	.Current, Health	.Max); ::printf("Health .....: %s.\n", formatted);
			::sprintf_s(formatted, "%i/%i", Mana	.Current, Mana		.Max); ::printf("Mana -------: %s.\n", formatted);
			::sprintf_s(formatted, "%i/%i", Shield	.Current, Shield	.Max); ::printf("Shield .....: %s.\n", formatted);
		}
	};

#pragma pack(pop)
}

#endif __CHARACTERPOINTS_H__0298734092864380972346234__
