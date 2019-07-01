#include "gpk_enum.h"

#ifndef __PLAYERCONTROL_H__2937846982374698234__
#define __PLAYERCONTROL_H__2937846982374698234__

namespace klib
{
#pragma pack(push, 1)
	struct SPlayerSelection	{ 
		int16_t							PlayerSquad			;
		int16_t							PlayerUnit			;
		int16_t							TargetPlayer		;
		int16_t							TargetSquad			;
		int16_t							TargetUnit			;

		inline void						ClearTarget			()			noexcept	{ TargetPlayer = TargetSquad = TargetUnit = -1; }
	};

	GDEFINE_ENUM_TYPE(PLAYER_CONTROL, uint8_t);	// I added these so network messages can talk about the player members
	GDEFINE_ENUM_VALUE(PLAYER_CONTROL,	NONE				, 0);	// I added these so network messages can talk about the player members
	GDEFINE_ENUM_VALUE(PLAYER_CONTROL,	LOCAL				, 1);	// I added these so network messages can talk about the player members
	GDEFINE_ENUM_VALUE(PLAYER_CONTROL,	AI					, 2);	// I added these so network messages can talk about the player members
	GDEFINE_ENUM_VALUE(PLAYER_CONTROL,	REMOTE				, 4);	// I added these so network messages can talk about the player members

	GDEFINE_ENUM_TYPE(PLAYER_AI, uint8_t);	// I added these so network messages can talk about the player members
	GDEFINE_ENUM_VALUE(PLAYER_AI,		NEUTRAL				, 0x00);	// Neutral AI is passive AI which doesn't really care much about anything until she is attacked.
	GDEFINE_ENUM_VALUE(PLAYER_AI,		TEAMERS				, 0x01);	// Teamers AI emulates a human player for a human team. Her priority is to kill the opposite teams and defend her allies.
	GDEFINE_ENUM_VALUE(PLAYER_AI,		VIOLENT				, 0x02);	// Violent AI attacks every character in sight and breaks stuff.
	GDEFINE_ENUM_VALUE(PLAYER_AI,		RIOTERS				, 0x04);	// Rioters AI attack everything aggressive (not neutral, curious, fearful or assists).
	GDEFINE_ENUM_VALUE(PLAYER_AI,		REVENGE				, 0x08);	// Revenge AI attacks characters that hurt her.
	GDEFINE_ENUM_VALUE(PLAYER_AI,		CURIOUS				, 0x10);	// Curious AI open chests and walks into the action zone.
	GDEFINE_ENUM_VALUE(PLAYER_AI,		FEARFUL				, 0x20);	// Fearful AI tries to escape to places where no eye contact can be made with her.
	GDEFINE_ENUM_VALUE(PLAYER_AI,		ASSISTS				, 0x40);	// Assists AI aids characters which are in trouble by healing them or other support that the AI-controlled character can provide (probably depending on her current equipment?).

	GDEFINE_ENUM_TYPE(PLAYER_PROPERTY, uint8_t);	// I added these so network messages can talk about the player members.
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	DETAIL				, 0);
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	MONEY				, 1);
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	RESEARCH_COMPLETED	, 2);
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	RESEARCH_MAX		, 3);
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	INVENTORY			, 4);
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	SQUAD				, 5);
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	TEAM				, 6);
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	SELECTION			, 7);
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	ARMY				, 8);
	GDEFINE_ENUM_VALUE(PLAYER_PROPERTY,	NAME				, 9);

	struct SPlayerControl {	// can be AI or human.
		PLAYER_CONTROL					Type				;//	= PLAYER_CONTROL_AI;
		PLAYER_AI						AIMode				;//	= PLAYER_AI_NEUTRAL;
	};

	//struct SPlayerEconomy	{ 
	//	int32_t						Money			= 25000;
	//};
#pragma pack(pop)
}

#endif // __PLAYERCONTROL_H__2937846982374698234__
