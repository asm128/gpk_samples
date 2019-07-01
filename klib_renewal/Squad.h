#include "gpk_coord.h"

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
	GDEFINE_ENUM_VALUE(AGENT_STATE,						IDLE	, 0x0000);
	GDEFINE_ENUM_VALUE(AGENT_STATE,						MOVE	, 0x0001);

#define MAX_AGENT_SQUAD_SLOTS	8
#define MAX_TACTICAL_PLAYERS	16
	struct SAgentReference {
				STileCharacter							Agent;
				::gpk::SCoord3<int32_t>					Position;
	};

#define MAX_POSSIBLE_AGENTS MAX_AGENT_SQUAD_SLOTS*MAX_TACTICAL_PLAYERS

	struct SAgentsReference {
				int32_t									Count										= 0;
				SAgentReference							Agents				[MAX_POSSIBLE_AGENTS]	= {};
	};

#define DEFAULT_SQUAD_SIZE		3
	// Squads should be indices to the army
	struct SSquad {
				int32_t									Size										= DEFAULT_SQUAD_SIZE; //MAX_AGENT_SQUAD_SLOTS;

				int16_t									Agents				[MAX_AGENT_SQUAD_SLOTS]	= {};
				STileCharacter							TargetAgents		[MAX_AGENT_SQUAD_SLOTS]	= {};
				::gpk::SCoord3<int32_t>					TargetPositions		[MAX_AGENT_SQUAD_SLOTS]	= {};
				AGENT_STATE								AgentStates			[MAX_AGENT_SQUAD_SLOTS]	= {};
				SAgentActions							ActionsLeft			[MAX_AGENT_SQUAD_SLOTS]	= {};
				SAgentsReference						AgentsInRange		[MAX_AGENT_SQUAD_SLOTS]	= {};
				SAgentsReference						AgentsInSight		[MAX_AGENT_SQUAD_SLOTS]	= {};
				SAgentsReference						AgentsInSquadSight							= {};
				int8_t									LockedAgent									= -1;

				bool									IsAgentAssigned								(int32_t indexAgent)	const	{
			bool										bAlreadySet									= false;
			for(int32_t iAgentOther = 0; iAgentOther < Size; ++iAgentOther)
				if(Agents[iAgentOther] == indexAgent) {
					bAlreadySet								= true;
					break;
				}

			return bAlreadySet;
		}

		inline	void								Clear										(int32_t index)					{
			if(index == -1) {
				::memset(Agents				, -1, sizeof(int16_t					)*::gpk::size(Agents			));
				::memset(TargetAgents		, -1, sizeof(STileCharacter				)*::gpk::size(TargetAgents		));
				::memset(TargetPositions	, -1, sizeof(::gpk::SCoord3<int32_t>	)*::gpk::size(TargetPositions	));
				::memset(AgentStates		,  0, sizeof(AGENT_STATE				)*::gpk::size(AgentStates		));
				::memset(ActionsLeft		, -1, sizeof(SAgentActions				)*::gpk::size(ActionsLeft		));
				::memset(AgentsInRange		,  0, sizeof(SAgentsReference			)*::gpk::size(AgentsInRange	));
				::memset(AgentsInSight		,  0, sizeof(SAgentsReference			)*::gpk::size(AgentsInSight	));
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