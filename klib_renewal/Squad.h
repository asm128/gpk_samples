#include "gpk_n3.h"
#include "gpk_array_ptr.h"
#include "gpk_array_static.h"

#include "CharacterTile.h"

#ifndef __SQUAD_H__9234729038472093472093847209384702938470293__
#define __SQUAD_H__9234729038472093472093847209384702938470293__

namespace klib
{
#pragma pack(push, 1)
	struct SAgentActions {
		int8_t					Moves;
		int8_t					Actions;
	};


	GDEFINE_ENUM_TYPE(AGENT_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(AGENT_STATE, IDLE, 0x0000);
	GDEFINE_ENUM_VALUE(AGENT_STATE, MOVE, 0x0001);

	struct SAgentReference {
		::klib::STileCharacter	Agent;
		::gpk::n3i32			Position;
	};

	stacxpr const uint32_t					MAX_AGENT_SQUAD_SLOTS	= 3;
	stacxpr const uint32_t					MAX_TACTICAL_PLAYERS	= 16;
	stacxpr const uint32_t					MAX_POSSIBLE_AGENTS		= MAX_AGENT_SQUAD_SLOTS * MAX_TACTICAL_PLAYERS;

	struct SAgentsReference {
		uint32_t												Count					= 0;
		::gpk::astatic<SAgentReference, MAX_POSSIBLE_AGENTS>	Agents					= {};

		operator	::gpk::view<::klib::SAgentReference>		()				noexcept	{ return {Agents.begin(), Count}; }
		operator	::gpk::view<const ::klib::SAgentReference>	()		const	noexcept	{ return {Agents.begin(), Count}; }
	};

	stacxpr const uint32_t					DEFAULT_SQUAD_SIZE		= 3;
	// Squads should be indices to the army
	struct SSquad {
		uint32_t										Size										= ::klib::DEFAULT_SQUAD_SIZE; //MAX_AGENT_SQUAD_SLOTS;

		::gpk::astatic<int16_t						, MAX_AGENT_SQUAD_SLOTS>	Agents			= {-1, -1, -1};//, -1, -1, -1, -1, -1,};
		::gpk::astatic<::klib::STileCharacter		, MAX_AGENT_SQUAD_SLOTS>	TargetAgents	= {};
		::gpk::astatic<::gpk::n3i32			, MAX_AGENT_SQUAD_SLOTS>	TargetPositions	= {};
		::gpk::astatic<::klib::AGENT_STATE			, MAX_AGENT_SQUAD_SLOTS>	AgentStates		= {};
		::gpk::astatic<::klib::SAgentActions		, MAX_AGENT_SQUAD_SLOTS>	ActionsLeft		= {};
		::gpk::astatic<::klib::SAgentsReference		, MAX_AGENT_SQUAD_SLOTS>	AgentsInRange	= {};
		::gpk::astatic<::klib::SAgentsReference		, MAX_AGENT_SQUAD_SLOTS>	AgentsInSight	= {};
		::klib::SAgentsReference						AgentsInSquadSight							= {};
		int8_t											LockedAgent									= -1;

		bool											IsAgentAssigned								(int32_t indexAgent)	const	{
			bool												bAlreadySet									= false;
			for(uint32_t iAgentOther = 0; iAgentOther < Size; ++iAgentOther)
				if(Agents[iAgentOther] == indexAgent) {
					bAlreadySet								= true;
					break;
				}

			return bAlreadySet;
		}

		void											Clear										(int32_t index)					{
			if(index == -1) {
				::memset(Agents				.begin(), -1, sizeof(int16_t					) * Agents			.size());
				::memset(TargetAgents		.begin(), -1, sizeof(::klib::STileCharacter		) * TargetAgents	.size());
				::memset(TargetPositions	.begin(), -1, sizeof(::gpk::n3i32	) * TargetPositions	.size());
				::memset(AgentStates		.begin(),  0, sizeof(::klib::AGENT_STATE		) * AgentStates		.size());
				::memset(ActionsLeft		.begin(), -1, sizeof(::klib::SAgentActions		) * ActionsLeft		.size());
				::memset(AgentsInRange		.begin(),  0, sizeof(::klib::SAgentsReference	) * AgentsInRange	.size());
				::memset(AgentsInSight		.begin(),  0, sizeof(::klib::SAgentsReference	) * AgentsInSight	.size());
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

#pragma pack(pop)

	//
	struct SSquadManager {
		::gpk::apod<::klib::SSquad>					Squads;

		int32_t										Create				()										{ int32_t newIndex = Squads.push_back({}); return newIndex; }
		int32_t										Delete				(int32_t idSquad)						{ return Squads.remove(idSquad); }
		int32_t										SquadSize			(int32_t idSquad)						{ return Squads[idSquad].Size; }
		int32_t										SquadResize			(int32_t idSquad, uint32_t newSize)		{ gthrow_if(newSize > Squads[idSquad].Agents.size(), "Cannot resize squad to %i. Squad too large.", ); return Squads[idSquad].Size = newSize; }

		::gpk::view<int16_t						>	Agents				(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.Agents			.begin(), squad.Size}; }
		::gpk::view<::klib::STileCharacter		>	TargetAgents		(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.TargetAgents		.begin(), squad.Size}; }
		::gpk::view<::gpk::n3i32				>	TargetPositions		(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.TargetPositions	.begin(), squad.Size}; }
		::gpk::view<::klib::AGENT_STATE			>	AgentStates			(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.AgentStates		.begin(), squad.Size}; }
		::gpk::view<::klib::SAgentActions		>	ActionsLeft			(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.ActionsLeft		.begin(), squad.Size}; }
		::gpk::view<::klib::SAgentsReference	>	AgentsInRange		(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.AgentsInRange		.begin(), squad.Size}; }
		::gpk::view<::klib::SAgentsReference	>	AgentsInSight		(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.AgentsInSight		.begin(), squad.Size}; }
		::gpk::view<::klib::SAgentReference		>	AgentsInSquadSight	(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.AgentsInSquadSight.Agents.begin(), squad.AgentsInSquadSight.Count}; }
		int8_t										LockedAgent			(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return squad.LockedAgent; }

	};
}

#endif // __SQUAD_H__9234729038472093472093847209384702938470293__
