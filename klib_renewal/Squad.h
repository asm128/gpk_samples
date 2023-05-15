#include "gpk_coord.h"
#include "gpk_ptr.h"
#include "gpk_array_static.h"

#include "CharacterTile.h"

#ifndef __SQUAD_H__9234729038472093472093847209384702938470293__
#define __SQUAD_H__9234729038472093472093847209384702938470293__

namespace klib
{
#pragma pack(push, 1)
	struct SAgentActions {
		int8_t									Moves;
		int8_t									Actions;
	};


	GDEFINE_ENUM_TYPE(AGENT_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(AGENT_STATE,					IDLE	, 0x0000);
	GDEFINE_ENUM_VALUE(AGENT_STATE,					MOVE	, 0x0001);

	struct SAgentReference {
		::klib::STileCharacter					Agent;
		::gpk::n3<int32_t>					Position;
	};

	static constexpr const uint32_t					MAX_AGENT_SQUAD_SLOTS	= 3;
	static constexpr const uint32_t					MAX_TACTICAL_PLAYERS	= 16;
	static constexpr const uint32_t					MAX_POSSIBLE_AGENTS		= MAX_AGENT_SQUAD_SLOTS * MAX_TACTICAL_PLAYERS;

	struct SAgentsReference {
		uint32_t													Count					= 0;
		::gpk::array_static<SAgentReference,MAX_POSSIBLE_AGENTS>	Agents					= {};

		operator							::gpk::view_array<::klib::SAgentReference>		()				noexcept	{ return {Agents.begin(), Count}; }
		operator							::gpk::view_array<const ::klib::SAgentReference>()		const	noexcept	{ return {Agents.begin(), Count}; }
	};

	static constexpr const uint32_t					DEFAULT_SQUAD_SIZE		= 3;
	// Squads should be indices to the army
	struct SSquad {
		uint32_t										Size										= ::klib::DEFAULT_SQUAD_SIZE; //MAX_AGENT_SQUAD_SLOTS;

		::gpk::array_static<int16_t						, MAX_AGENT_SQUAD_SLOTS>	Agents			= {-1, -1, -1};//, -1, -1, -1, -1, -1,};
		::gpk::array_static<::klib::STileCharacter		, MAX_AGENT_SQUAD_SLOTS>	TargetAgents	= {};
		::gpk::array_static<::gpk::n3<int32_t>		, MAX_AGENT_SQUAD_SLOTS>	TargetPositions	= {};
		::gpk::array_static<::klib::AGENT_STATE			, MAX_AGENT_SQUAD_SLOTS>	AgentStates		= {};
		::gpk::array_static<::klib::SAgentActions		, MAX_AGENT_SQUAD_SLOTS>	ActionsLeft		= {};
		::gpk::array_static<::klib::SAgentsReference	, MAX_AGENT_SQUAD_SLOTS>	AgentsInRange	= {};
		::gpk::array_static<::klib::SAgentsReference	, MAX_AGENT_SQUAD_SLOTS>	AgentsInSight	= {};
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
				::memset(TargetPositions	.begin(), -1, sizeof(::gpk::n3<int32_t>	) * TargetPositions	.size());
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
		::gpk::array_pod<::klib::SSquad>				Squads;

		int32_t											Create				()										{ int32_t newIndex = Squads.push_back({}); return newIndex; }
		int32_t											Delete				(int32_t idSquad)						{ return Squads.remove(idSquad); }
		int32_t											SquadSize			(int32_t idSquad)						{ return Squads[idSquad].Size; }
		int32_t											SquadResize			(int32_t idSquad, uint32_t newSize)		{ gthrow_if(newSize > Squads[idSquad].Agents.size(), "Cannot resize squad to %i. Squad too large.", ); return Squads[idSquad].Size = newSize; }

		::gpk::view_array<int16_t					>	Agents				(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.Agents			.begin(), squad.Size}; }
		::gpk::view_array<::klib::STileCharacter	>	TargetAgents		(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.TargetAgents		.begin(), squad.Size}; }
		::gpk::view_array<::gpk::n3<int32_t>	>	TargetPositions		(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.TargetPositions	.begin(), squad.Size}; }
		::gpk::view_array<::klib::AGENT_STATE		>	AgentStates			(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.AgentStates		.begin(), squad.Size}; }
		::gpk::view_array<::klib::SAgentActions		>	ActionsLeft			(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.ActionsLeft		.begin(), squad.Size}; }
		::gpk::view_array<::klib::SAgentsReference	>	AgentsInRange		(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.AgentsInRange		.begin(), squad.Size}; }
		::gpk::view_array<::klib::SAgentsReference	>	AgentsInSight		(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.AgentsInSight		.begin(), squad.Size}; }
		::gpk::view_array<::klib::SAgentReference	>	AgentsInSquadSight	(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return {squad.AgentsInSquadSight.Agents.begin(), squad.AgentsInSquadSight.Count}; }
		int8_t											LockedAgent			(int32_t idSquad)						{ ::klib::SSquad & squad = Squads[idSquad]; return squad.LockedAgent; }

	};
}

#endif // __SQUAD_H__9234729038472093472093847209384702938470293__
