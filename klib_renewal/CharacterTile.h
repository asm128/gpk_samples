#include "gpk_enum.h"

#ifndef __CHARACTER_TILE_H__2986498276489273469827346982734__
#define __CHARACTER_TILE_H__2986498276489273469827346982734__

namespace klib
{
#pragma pack(push, 1)

	GDEFINE_ENUM_TYPE(TEAM_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, SPECTATOR	,  0);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, CIVILIAN	,  1);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, ENEMY		,  2);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, ALLY		,  3);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, CUSTOM_0	,  4);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, CUSTOM_1	,  5);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, CUSTOM_2	,  6);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, CUSTOM_3	,  7);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, CUSTOM_4	,  8);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, CUSTOM_5	,  9);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, CUSTOM_6	, 10);
	GDEFINE_ENUM_VALUE(TEAM_TYPE, CUSTOM_7	, 11);
	
						bool				isRelevantTeam				(TEAM_TYPE teamId);

	// Stores team, player slot, squad and agent in squad.
	struct STileCharacter {
							TEAM_TYPE			TeamId		;
							int8_t				PlayerIndex	;
							int8_t				SquadIndex	;
							int8_t				AgentIndex	;

		inline constexpr	bool				operator ==				(const STileCharacter& other)	const	noexcept	{ return TeamId == other.TeamId && PlayerIndex == other.PlayerIndex && SquadIndex == other.SquadIndex && AgentIndex == other.AgentIndex; }
	};
#pragma pack(pop)
} // namespace

#endif // __CHARACTER_TILE_H__2986498276489273469827346982734__