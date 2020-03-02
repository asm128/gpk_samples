#include "tactical_draw.h"
#include "Combat.h"

using namespace klib;

//template <size_t _Size>
//int32_t getEnemyPlayers();

bool													isRelevantPlayer										(STacticalInfo& tacticalInfo, int32_t currentPlayer, int32_t otherPlayer)			{
	bool														bResult													= true;

	if( tacticalInfo.CurrentPlayer						== otherPlayer
	 || tacticalInfo.Setup.Players[otherPlayer]			== -1
	 || !isRelevantTeam(tacticalInfo.Setup.TeamPerPlayer[otherPlayer])
	 || tacticalInfo.Setup.TeamPerPlayer[otherPlayer]	== tacticalInfo.Setup.TeamPerPlayer[currentPlayer]
	)
		bResult													= false;

	return bResult;
};

void													selectAITarget											(SGame& instanceGame)																{
	STacticalInfo												& tacticalInfo											= instanceGame.TacticalInfo;
	SPlayer														& currentPlayer											= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];
	SPlayerControl												& playerControl											= currentPlayer.Tactical.Control;

	if(playerControl.AIMode == PLAYER_AI_ASSISTS)
		return;
	else if( (!::gpk::bit_true(playerControl.AIMode, PLAYER_AI_TEAMERS))		// Here we check if the AI has an aggressive flag. If it doesn't just cancel the turn.
		  && (!::gpk::bit_true(playerControl.AIMode, PLAYER_AI_RIOTERS))
		  && (!::gpk::bit_true(playerControl.AIMode, PLAYER_AI_VIOLENT))
		  && (!::gpk::bit_true(playerControl.AIMode, PLAYER_AI_REVENGE))
		)
	{
		return;
	}

	//CCharacter													& playerCharacter										= *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];
	PLAYER_INDEX												targetPlayerIndex										= PLAYER_INDEX_INVALID;
	currentPlayer.Tactical.Selection.TargetPlayer			= -1;
	currentPlayer.Tactical.Selection.TargetUnit				= -1;
	currentPlayer.Tactical.Selection.TargetSquad			= -1;

	// Here we select a relevant target
	TEAM_TYPE													playerTeam												= tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer];
	for(uint32_t iAgentInSight=0, agentInSightCount = tacticalInfo.AgentsInTeamSight[playerTeam].Count; iAgentInSight < agentInSightCount; ++iAgentInSight) {
		if(isRelevantPlayer(tacticalInfo, tacticalInfo.CurrentPlayer, (int32_t)tacticalInfo.AgentsInTeamSight[playerTeam].Agents[iAgentInSight].Agent.PlayerIndex)) {
			currentPlayer.Tactical.Selection.TargetPlayer					= tacticalInfo.AgentsInTeamSight[playerTeam].Agents[iAgentInSight].Agent.PlayerIndex;
			currentPlayer.Tactical.Selection.TargetUnit						= tacticalInfo.AgentsInTeamSight[playerTeam].Agents[iAgentInSight].Agent.AgentIndex;
			currentPlayer.Tactical.Selection.TargetSquad						= tacticalInfo.AgentsInTeamSight[playerTeam].Agents[iAgentInSight].Agent.SquadIndex;

			targetPlayerIndex										= tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer];
			SPlayer														& targetPlayer											= instanceGame.Players[targetPlayerIndex];
			const CCharacter											& targetAgent											= *targetPlayer.Tactical.Army[targetPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit]];
			if(false == targetAgent.IsAlive())
				continue;

			currentPlayer.Tactical.Squad.TargetPositions[currentPlayer.Tactical.Selection.PlayerUnit]	= targetAgent.Position;
			break;
		}
	}
}

void													selectAvailableTile										(const STacticalBoard& tacticalBoard, ::gpk::SCoord3<int32_t>& targetPosition)		{
	do {
		targetPosition.x										= (rand() % tacticalBoard.Tiles.Terrain.Geometry.metrics().x);
		targetPosition.z										= (rand() % tacticalBoard.Tiles.Terrain.Geometry.metrics().y);
	}
	while (		((targetPosition.x < 0	|| targetPosition.x >= (int32_t)tacticalBoard.Tiles.Terrain.Geometry.metrics().x)
			||   (targetPosition.z < 0	|| targetPosition.z >= (int32_t)tacticalBoard.Tiles.Terrain.Geometry.metrics().y)
		)
		&&	!tacticalBoard.Tiles.IsTileAvailable(targetPosition.x, targetPosition.z)
	);
}

void													getValidCoordForAgentDestination						(::gpk::SCoord3<int32_t>& targetPositionAgent, const STacticalBoard& tacticalBoard)	{
	do {
			 if( rand()%2 )	targetPositionAgent.x = (rand()%tacticalBoard.Tiles.Terrain.Geometry.metrics().x);
		else if( rand()%2 )	targetPositionAgent.z = (rand()%tacticalBoard.Tiles.Terrain.Geometry.metrics().y);
		else {
			targetPositionAgent.x = (rand()%tacticalBoard.Tiles.Terrain.Geometry.metrics().x);
			targetPositionAgent.z = (rand()%tacticalBoard.Tiles.Terrain.Geometry.metrics().y);
		}
	}
	while(!tacticalBoard.Tiles.IsTileAvailable(targetPositionAgent.x, targetPositionAgent.z));
	return;
}

void													selectAIDestination										(SGame& instanceGame)																{
	STacticalInfo												& tacticalInfo											= instanceGame.TacticalInfo;
	SPlayer														& currentPlayer											= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];

	::gpk::SCoord3<int32_t>									& targetPositionAgent									= currentPlayer.Tactical.Squad.TargetPositions[currentPlayer.Tactical.Selection.PlayerUnit];

	if(currentPlayer.Tactical.Control.AIMode == PLAYER_AI_ASSISTS) {
		selectAvailableTile(tacticalInfo.Board, targetPositionAgent);
		return;
	}
	else if( (!::gpk::bit_true(currentPlayer.Tactical.Control.AIMode, PLAYER_AI_TEAMERS))
		  && (!::gpk::bit_true(currentPlayer.Tactical.Control.AIMode, PLAYER_AI_RIOTERS))
		  && (!::gpk::bit_true(currentPlayer.Tactical.Control.AIMode, PLAYER_AI_VIOLENT))
		  && (!::gpk::bit_true(currentPlayer.Tactical.Control.AIMode, PLAYER_AI_REVENGE))
		)
	{
		selectAvailableTile(tacticalInfo.Board, targetPositionAgent);
		return;
	}

	if(currentPlayer.Tactical.Selection.TargetPlayer == -1 || tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer] == -1) {
		getValidCoordForAgentDestination(targetPositionAgent, tacticalInfo.Board);
		return;
	}

	SPlayer														& playerTarget											= instanceGame.Players[tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer]];

	if(currentPlayer.Tactical.Selection.TargetUnit == -1 || playerTarget.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit] == -1) {
		getValidCoordForAgentDestination(targetPositionAgent, tacticalInfo.Board);
		return;
	}


	const SEntityFlags											& playerAgentFlags										= currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]]->FinalFlags;
	targetPositionAgent										= playerTarget.Tactical.Army[playerTarget.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit]]->Position;
	::gpk::SCoord3<int32_t>									vectorToAgent											= targetPositionAgent-currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]]->Position;
	::gpk::SCoord3<float>										vec														= vectorToAgent.Cast<float>();
	if(::gpk::bit_false(playerAgentFlags.Tech.AttackType, ATTACK_TYPE_MELEE) && vec.Length() <= 18) {
		getValidCoordForAgentDestination(targetPositionAgent, tacticalInfo.Board);
		return;
	}

	while(!tacticalInfo.Board.Tiles.IsTileAvailable(targetPositionAgent.x, targetPositionAgent.z)) {
		do {
			if(rand() % 2)	targetPositionAgent.x					+= (rand()%2) ? 1 : -1;
			else			targetPositionAgent.z					+= (rand()%2) ? 1 : -1;
		}
		while( (targetPositionAgent.x < 0 || targetPositionAgent.x >= (int32_t)tacticalInfo.Board.Tiles.Terrain.Geometry.metrics().x)
			|| (targetPositionAgent.z < 0 || targetPositionAgent.z >= (int32_t)tacticalInfo.Board.Tiles.Terrain.Geometry.metrics().y)
		);
	}
};

TURN_ACTION												selectAIAction											(SGame& instanceGame)																{
	TURN_ACTION													result													= TURN_ACTION_CONTINUE;
	STacticalInfo												& tacticalInfo											= instanceGame.TacticalInfo;
	SPlayer														& currentPlayer											= instanceGame.Players[getCurrentPlayerIndex(tacticalInfo)];
	CCharacter													& currentAgent											= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]];
	if(0 < currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves
		&& currentPlayer.Tactical.Squad.TargetPositions[currentPlayer.Tactical.Selection.PlayerUnit] == currentAgent.Position)
	{
		selectAITarget		(instanceGame);
		selectAIDestination	(instanceGame);
	}
	else if(0 >= currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves && 0 < currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Actions) {
		if(currentPlayer.Tactical.Control.AIMode == PLAYER_AI_ASSISTS)
			result													= TURN_ACTION_CANCEL;
		else if( (::gpk::bit_false(currentPlayer.Tactical.Control.AIMode, PLAYER_AI_TEAMERS))		// Here we check if the AI has an aggressive flag. If it doesn't just cancel the turn.
			  && (::gpk::bit_false(currentPlayer.Tactical.Control.AIMode, PLAYER_AI_RIOTERS))
			  && (::gpk::bit_false(currentPlayer.Tactical.Control.AIMode, PLAYER_AI_VIOLENT))
			  && (::gpk::bit_false(currentPlayer.Tactical.Control.AIMode, PLAYER_AI_REVENGE))
			)
			result													= TURN_ACTION_CANCEL;
		else {
			selectAITarget(instanceGame);
			if(currentPlayer.Tactical.Selection.TargetPlayer != -1 && tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer] != -1) {
				const SPlayer												& targetPlayer											= instanceGame.Players[tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer]];
				if((currentAgent.ActiveBonus.Status.Status & COMBAT_STATUS_BLACKOUT) && (currentAgent.FinalFlags.Tech.Tech & ENTITY_TECHNOLOGY_DIGITAL))
					result													= TURN_ACTION_CANCEL;	// currently there is no better handling for this situation.
				else if(currentPlayer.Tactical.Selection.TargetUnit != -1 && targetPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit] != -1) {
					const CCharacter											& targetCharacter										= *targetPlayer.Tactical.Army[targetPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit]];
					if(targetCharacter.IsAlive())  {
						CCharacter													& playerAgent											= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]];

						const ::gpk::SCoord3<int32_t>								& coordPlayer											= playerAgent.Position;
						const ::gpk::SCoord3<int32_t>								& coordTarget											= targetCharacter.Position;
						const ::gpk::SCoord3<float>								distance												= (coordTarget-coordPlayer).Cast<float>();

						const SEntityPoints											& playerAgentPoints										= playerAgent.FinalPoints;
						const SEntityFlags											& playerAgentFlags										= playerAgent.FinalFlags;

						if(::gpk::bit_true(playerAgentFlags.Tech.AttackType, ATTACK_TYPE_RANGED)) {
							bool														bInSight												= false;
							STacticalSetup												& tacticalSetup											= tacticalInfo.Setup;

							for(uint32_t iAgentInSight=0, agentsInSightCount = tacticalInfo.AgentsInTeamSight[tacticalSetup.TeamPerPlayer[tacticalInfo.CurrentPlayer]].Count;
								iAgentInSight < agentsInSightCount; ++iAgentInSight
								)
							{
								const SAgentReference										& targetAgentTile										= tacticalInfo.AgentsInTeamSight[tacticalSetup.TeamPerPlayer[tacticalInfo.CurrentPlayer]].Agents[iAgentInSight];
								if( targetAgentTile.Agent.TeamId == tacticalSetup.TeamPerPlayer[tacticalInfo.CurrentPlayer] )
									continue;

								if( false == isRelevantTeam(targetAgentTile.Agent.TeamId) || false == isRelevantPlayer(tacticalInfo, tacticalInfo.CurrentPlayer, targetAgentTile.Agent.PlayerIndex))
									continue;

								bInSight												= true;
								if(bInSight)
									break;
							}

							if(bInSight)
								result													= TURN_ACTION_ATTACK;
							else
								result													= TURN_ACTION_CANCEL;
						}
						else {
							double														finalSight												= getFinalSight(playerAgent, playerAgentPoints);
							double														finalRange												= getFinalRange(playerAgent, playerAgentPoints);
							if(distance.Length() > finalRange || distance.Length() > finalSight)
								result													= TURN_ACTION_CANCEL;
							else
								result													= TURN_ACTION_ATTACK;
						}	// if (ranged)
					}	// if(targetAgentSelected)
				}// if(targetPlayerSelected)
			}
			else { //
				if(0 >= currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves)
					result													= TURN_ACTION_CANCEL;
			}
		}
	}
	else if(0 < currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves)
		::gpk::bit_set(currentPlayer.Tactical.Squad.AgentStates[currentPlayer.Tactical.Selection.PlayerUnit], AGENT_STATE_MOVE);

	return result;
}
