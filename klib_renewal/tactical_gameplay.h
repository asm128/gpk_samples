#include "Game.h"

#ifndef __TACTICAL_GAMEPLAY_H__9823490273460921364098237432__
#define __TACTICAL_GAMEPLAY_H__9823490273460921364098237432__

namespace klib
{
	bool moveStep				(SGame& instanceGame, SPlayer& player, int8_t playerIndex, int32_t agentIndex, TEAM_TYPE teamId, STacticalBoard& board, ::gpk::SCoord3<int32_t>& agentPosition);
	bool isTacticalValid		(SGame& instanceGame);
	void endTurn				(SGame& instanceGame);
	bool updateCurrentPlayer	(SGame& instanceGame);
	void determineOutcome		(SGame& instanceGame);
	void handleAgentDeath		(SGame& instanceGame, CCharacter& deadTarget, TEAM_TYPE teamId);
	void handleAgentDeath		(SGame& instanceGame, CCharacter& deadTarget, CCharacter& attacker, TEAM_TYPE teamId);
	bool isTacticalValid		(SGame& instanceGame);



	static int32_t												getAgentsInSight									(SGame& instanceGame, const ::gpk::SCoord3<int32_t>& origin, double range, SAgentsReference& agentsInRange)	{
		agentsInRange												= SAgentsReference();
		STacticalInfo													& tacticalInfo										= instanceGame.TacticalInfo;
		for(uint32_t iPlayer = 0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
			if(tacticalInfo.Setup.Players[iPlayer] == PLAYER_INDEX_INVALID)
				continue;

			SPlayer															& currentPlayer										= instanceGame.Players[tacticalInfo.Setup.Players[iPlayer]];
			for(uint32_t iAgent = 0, agentCount =  currentPlayer.Squad.Size; iAgent < agentCount; ++iAgent) {
				if(currentPlayer.Squad.Agents[iAgent] == -1)
					continue;

				const CCharacter												& agent												= *currentPlayer.Army[currentPlayer.Squad.Agents[iAgent]];
				if(false == agent.IsAlive())
					continue;

				const ::gpk::SCoord3<int32_t>									& coordAgent										= agent.Position;
				const ::gpk::SCoord3<float>									distance											= (coordAgent-origin).Cast<float>();
				if(distance.Length() > range) 
					continue;

				agentsInRange.Agents[agentsInRange.Count++]					= {tacticalInfo.Setup.TeamPerPlayer[iPlayer], (int8_t)iPlayer, 0, (int8_t)iAgent};
				if(agentsInRange.Count >= (int32_t)::gpk::size(agentsInRange.Agents))
					break;
			}
		}
		return agentsInRange.Count;
	}


}; // 

#endif // __TACTICAL_GAMEPLAY_H__9823490273460921364098237432__
