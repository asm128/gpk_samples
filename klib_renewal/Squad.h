#include "gpk_coord.h"
#include "gpk_array_static.h"

#include "CharacterTile.h"

#ifndef __SQUAD_H__9234729038472093472093847209384702938470293__
#define __SQUAD_H__9234729038472093472093847209384702938470293__

namespace klib
{
	struct SAgentActions {
				int8_t									Moves;
				int8_t									Actions;
	};


	GDEFINE_ENUM_TYPE(AGENT_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(AGENT_STATE,					IDLE	, 0x0000);
	GDEFINE_ENUM_VALUE(AGENT_STATE,					MOVE	, 0x0001);

	static constexpr const uint32_t					MAX_AGENT_SQUAD_SLOTS	= 8	;
	static constexpr const uint32_t					MAX_TACTICAL_PLAYERS	= 16;
	struct SAgentReference {
				STileCharacter							Agent;
				::gpk::SCoord3<int32_t>					Position;
	};

#define MAX_POSSIBLE_AGENTS MAX_AGENT_SQUAD_SLOTS*MAX_TACTICAL_PLAYERS

	struct SAgentsReference {
				int32_t														Count										= 0;
				::gpk::array_static<SAgentReference,MAX_POSSIBLE_AGENTS>	Agents					= {};
	};

#define DEFAULT_SQUAD_SIZE		3
	// Squads should be indices to the army
	struct SSquad {
				uint32_t								Size										= DEFAULT_SQUAD_SIZE; //MAX_AGENT_SQUAD_SLOTS;

				::gpk::array_static<int16_t					, MAX_AGENT_SQUAD_SLOTS>	Agents			= {-1, -1, -1, -1, -1, -1, -1, -1,};
				::gpk::array_static<STileCharacter			, MAX_AGENT_SQUAD_SLOTS>	TargetAgents	= {};
				::gpk::array_static<::gpk::SCoord3<int32_t>	, MAX_AGENT_SQUAD_SLOTS>	TargetPositions	= {};
				::gpk::array_static<AGENT_STATE				, MAX_AGENT_SQUAD_SLOTS>	AgentStates		= {};
				::gpk::array_static<SAgentActions			, MAX_AGENT_SQUAD_SLOTS>	ActionsLeft		= {};
				::gpk::array_static<SAgentsReference		, MAX_AGENT_SQUAD_SLOTS>	AgentsInRange	= {};
				::gpk::array_static<SAgentsReference		, MAX_AGENT_SQUAD_SLOTS>	AgentsInSight	= {};
				SAgentsReference						AgentsInSquadSight							= {};
				int8_t									LockedAgent									= -1;

				bool									IsAgentAssigned								(int32_t indexAgent)	const	{
			bool										bAlreadySet									= false;
			for(uint32_t iAgentOther = 0; iAgentOther < Size; ++iAgentOther)
				if(Agents[iAgentOther] == indexAgent) {
					bAlreadySet								= true;
					break;
				}

			return bAlreadySet;
		}

		inline	void								Clear										(int32_t index)					{
			if(index == -1) {
				::memset(Agents				.begin(), -1, sizeof(int16_t					)* Agents			.size());
				::memset(TargetAgents		.begin(), -1, sizeof(STileCharacter				)* TargetAgents		.size());
				::memset(TargetPositions	.begin(), -1, sizeof(::gpk::SCoord3<int32_t>	)* TargetPositions	.size());
				::memset(AgentStates		.begin(),  0, sizeof(AGENT_STATE				)* AgentStates		.size());
				::memset(ActionsLeft		.begin(), -1, sizeof(SAgentActions				)* ActionsLeft		.size());
				::memset(AgentsInRange		.begin(),  0, sizeof(SAgentsReference			)* AgentsInRange	.size());
				::memset(AgentsInSight		.begin(),  0, sizeof(SAgentsReference			)* AgentsInSight	.size());
				AgentsInSquadSight.Count				= 0;
				LockedAgent								= -1;
			}
			else {
				Agents					[index]			= -1;
				TargetAgents			[index]			= {TEAM_TYPE_SPECTATOR, -1, -1, -1};
				TargetPositions			[index]			= {};
				AgentStates				[index]			= AGENT_STATE_IDLE;
				ActionsLeft				[index]			= {0,0};
				AgentsInRange			[index].Count	= 0;
				AgentsInSight			[index].Count	= 0;
				if(LockedAgent == index)
					LockedAgent = -1;
			}
		}
	};
}

#endif // __SQUAD_H__9234729038472093472093847209384702938470293__
