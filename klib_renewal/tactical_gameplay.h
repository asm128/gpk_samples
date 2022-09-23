#include "Game.h"

#ifndef __TACTICAL_GAMEPLAY_H__9823490273460921364098237432__
#define __TACTICAL_GAMEPLAY_H__9823490273460921364098237432__

namespace klib
{
	bool moveStep				(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players, STacticalPlayer& player, int8_t playerIndex, int32_t agentIndex, TEAM_TYPE teamId, STacticalBoard& board, ::gpk::SCoord3<int32_t>& agentPosition, ::klib::SGameMessages & messages);
	void endTurn				(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players, ::klib::SGameMessages & messages);
	bool updateCurrentPlayer	(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players, ::klib::SGameMessages & messages);
	void determineOutcome		(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players, ::klib::SGameMessages & messages, bool aborted);
	bool isTacticalValid		(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players);
	void handleAgentDeath		(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players, ::klib::CCharacter& deadTarget, ::klib::TEAM_TYPE teamId, ::klib::SGameMessages& messages);
	void handleAgentDeath		(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players, ::klib::CCharacter& deadTarget, ::klib::CCharacter& attacker, TEAM_TYPE teamId, ::klib::SGameMessages & messages);
	bool isTacticalValid		(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players);

	static int32_t												getAgentsInSight									(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<const ::klib::SGamePlayer> players, const ::gpk::SCoord3<int32_t>& origin, double range, SAgentsReference& agentsInRange)	{
		agentsInRange												= {};
		for(uint32_t iPlayer = 0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
			if(tacticalInfo.Setup.Players[iPlayer] == (::klib::PLAYER_INDEX)-1)
				continue;

			const ::klib::SGamePlayer											& currentPlayer										= players[tacticalInfo.Setup.Players[iPlayer]];
			for(uint32_t iAgent = 0, agentCount =  currentPlayer.Tactical.Squad.Size; iAgent < agentCount; ++iAgent) {
				if(currentPlayer.Tactical.Squad.Agents[iAgent] == -1)
					continue;

				const ::klib::CCharacter										& agent												= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[iAgent]];
				if(false == agent.IsAlive())
					continue;

				const ::gpk::SCoord3<int32_t>									& coordAgent										= agent.Position;
				const ::gpk::SCoord3<float>										distance											= (coordAgent-origin).Cast<float>();
				if(distance.Length() > range)
					continue;

				agentsInRange.Agents[agentsInRange.Count++]					= {tacticalInfo.Setup.TeamPerPlayer[iPlayer], (int8_t)iPlayer, 0, (int8_t)iAgent};
				if(agentsInRange.Count >= (int32_t)agentsInRange.Agents.size())
					break;
			}
		}
		return agentsInRange.Count;
	}


}; //

#endif // __TACTICAL_GAMEPLAY_H__9823490273460921364098237432__
