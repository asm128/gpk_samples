#include "tactical_draw.h"
#include "Combat.h"

static	bool							isRelevantPlayer										(::klib::STacticalInfo& tacticalInfo, int32_t currentPlayer, int32_t otherPlayer)			{
	bool										bResult													= true;

	if( tacticalInfo.CurrentPlayer						== otherPlayer
	 || tacticalInfo.Setup.Players[otherPlayer]			== -1
	 || !::klib::isRelevantTeam(tacticalInfo.Setup.TeamPerPlayer[otherPlayer])
	 || tacticalInfo.Setup.TeamPerPlayer[otherPlayer]	== tacticalInfo.Setup.TeamPerPlayer[currentPlayer]
	)
		bResult													= false;

	return bResult;
};

static	void							selectAITarget											(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players)																{
	::klib::SGamePlayer							& currentPlayer											= players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];
	::klib::SPlayerControl						& playerControl											= currentPlayer.Tactical.Control;

	if(playerControl.AIMode == ::klib::PLAYER_AI_ASSISTS)
		return;
	else if( (!::gpk::bit_true(playerControl.AIMode, ::klib::PLAYER_AI_TEAMERS))		// Here we check if the AI has an aggressive flag. If it doesn't just cancel the turn.
		  && (!::gpk::bit_true(playerControl.AIMode, ::klib::PLAYER_AI_RIOTERS))
		  && (!::gpk::bit_true(playerControl.AIMode, ::klib::PLAYER_AI_VIOLENT))
		  && (!::gpk::bit_true(playerControl.AIMode, ::klib::PLAYER_AI_REVENGE))
		)
	{
		return;
	}

	//CCharacter								& playerCharacter										= *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];
	::klib::PLAYER_INDEX						targetPlayerIndex										= (::klib::PLAYER_INDEX)-1;
	::klib::SPlayerSelection					& playerSelection										= currentPlayer.Tactical.Selection;
	playerSelection.TargetPlayer			= -1;
	playerSelection.TargetUnit				= -1;
	playerSelection.TargetSquad				= -1;

	// Here we select a relevant target
	::klib::TEAM_TYPE							playerTeam												= tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer];
	for(uint32_t iAgentInSight=0, agentInSightCount = tacticalInfo.AgentsInTeamSight[playerTeam].Count; iAgentInSight < agentInSightCount; ++iAgentInSight) {
		::klib::STileCharacter						& agentInSight											=  tacticalInfo.AgentsInTeamSight[playerTeam].Agents[iAgentInSight].Agent;
		if(isRelevantPlayer(tacticalInfo, tacticalInfo.CurrentPlayer, (int32_t)agentInSight.PlayerIndex)) {
			playerSelection.TargetPlayer			= agentInSight.PlayerIndex;
			playerSelection.TargetUnit				= agentInSight.AgentIndex;
			playerSelection.TargetSquad				= agentInSight.SquadIndex;

			targetPlayerIndex						= tacticalInfo.Setup.Players[playerSelection.TargetPlayer];
			::klib::SGamePlayer							& targetPlayer											= players[targetPlayerIndex];
			const ::klib::CCharacter					& targetAgent											= *targetPlayer.Tactical.Army[targetPlayer.Tactical.Squad.Agents[playerSelection.TargetUnit]];
			if(false == targetAgent.IsAlive())
				continue;

			currentPlayer.Tactical.Squad.TargetPositions[playerSelection.PlayerUnit]	= targetAgent.Position;
			break;
		}
	}
}

static	void											selectAvailableTile										(const ::klib::STacticalBoard& tacticalBoard, ::gpk::n3<int32_t>& targetPosition)		{
	do {
		targetPosition.x										= (rand() % tacticalBoard.Tiles.Terrain.Geometry.metrics().x);
		targetPosition.z										= (rand() % tacticalBoard.Tiles.Terrain.Geometry.metrics().y);
	}
	while (	((targetPosition.x < 0	|| targetPosition.x >= (int32_t)tacticalBoard.Tiles.Terrain.Geometry.metrics().x)
		||	 (targetPosition.z < 0	|| targetPosition.z >= (int32_t)tacticalBoard.Tiles.Terrain.Geometry.metrics().y)
		)
		&&	!tacticalBoard.Tiles.IsTileAvailable(targetPosition.x, targetPosition.z)
	);
}

static	void											getValidCoordForAgentDestination						(::gpk::n3<int32_t>& targetPositionAgent, const ::klib::STacticalBoard& tacticalBoard)	{
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

void													selectAIDestination										(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players)	{
	::klib::SGamePlayer											& currentPlayer											= players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];

	::gpk::n3<int32_t>										& targetPositionAgent									= currentPlayer.Tactical.Squad.TargetPositions[currentPlayer.Tactical.Selection.PlayerUnit];

	if(currentPlayer.Tactical.Control.AIMode == ::klib::PLAYER_AI_ASSISTS) {
		::selectAvailableTile(tacticalInfo.Board, targetPositionAgent);
		return;
	}
	else if( (!::gpk::bit_true(currentPlayer.Tactical.Control.AIMode, ::klib::PLAYER_AI_TEAMERS))
		  && (!::gpk::bit_true(currentPlayer.Tactical.Control.AIMode, ::klib::PLAYER_AI_RIOTERS))
		  && (!::gpk::bit_true(currentPlayer.Tactical.Control.AIMode, ::klib::PLAYER_AI_VIOLENT))
		  && (!::gpk::bit_true(currentPlayer.Tactical.Control.AIMode, ::klib::PLAYER_AI_REVENGE))
		) {
		::selectAvailableTile(tacticalInfo.Board, targetPositionAgent);
		return;
	}

	if(currentPlayer.Tactical.Selection.TargetPlayer == -1 || tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer] == -1) {
		::getValidCoordForAgentDestination(targetPositionAgent, tacticalInfo.Board);
		return;
	}

	::klib::SGamePlayer											& playerTarget											= players[tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer]];

	if(currentPlayer.Tactical.Selection.TargetUnit == -1 || playerTarget.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit] == -1) {
		::getValidCoordForAgentDestination(targetPositionAgent, tacticalInfo.Board);
		return;
	}


	const ::klib::SEntityFlags									& playerAgentFlags										= currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]]->FinalFlags;
	targetPositionAgent										= playerTarget.Tactical.Army[playerTarget.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit]]->Position;
	::gpk::n3<int32_t>										vectorToAgent											= targetPositionAgent-currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]]->Position;
	::gpk::n3<float>										vec														= vectorToAgent.Cast<float>();
	if(::gpk::bit_false(playerAgentFlags.Tech.AttackType, ::klib::ATTACK_TYPE_MELEE) && vec.Length() <= 18) {
		::getValidCoordForAgentDestination(targetPositionAgent, tacticalInfo.Board);
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

::klib::TURN_ACTION										selectAIAction											(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players)	{
	::klib::TURN_ACTION											result													= ::klib::TURN_ACTION_CONTINUE;
	::klib::SGamePlayer											& currentPlayer											= players[::klib::getCurrentPlayerIndex(tacticalInfo)];
	::klib::CCharacter											& currentAgent											= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]];
	if(0 < currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves
		&& currentPlayer.Tactical.Squad.TargetPositions[currentPlayer.Tactical.Selection.PlayerUnit] == currentAgent.Position)
	{
		selectAITarget		(tacticalInfo, players);
		selectAIDestination	(tacticalInfo, players);
	}
	else if(0 >= currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves && 0 < currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Actions) {
		if(currentPlayer.Tactical.Control.AIMode == ::klib::PLAYER_AI_ASSISTS)
			result													= ::klib::TURN_ACTION_CANCEL;
		else if( (::gpk::bit_false(currentPlayer.Tactical.Control.AIMode, ::klib::PLAYER_AI_TEAMERS))		// Here we check if the AI has an aggressive flag. If it doesn't just cancel the turn.
			  && (::gpk::bit_false(currentPlayer.Tactical.Control.AIMode, ::klib::PLAYER_AI_RIOTERS))
			  && (::gpk::bit_false(currentPlayer.Tactical.Control.AIMode, ::klib::PLAYER_AI_VIOLENT))
			  && (::gpk::bit_false(currentPlayer.Tactical.Control.AIMode, ::klib::PLAYER_AI_REVENGE))
			)
			result													= ::klib::TURN_ACTION_CANCEL;
		else {
			selectAITarget(tacticalInfo, players);
			if(currentPlayer.Tactical.Selection.TargetPlayer != -1 && tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer] != -1) {
				const ::klib::SGamePlayer									& targetPlayer											= players[tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer]];
				if((currentAgent.ActiveBonus.Status.Status & ::klib::COMBAT_STATUS_BLACKOUT) && (currentAgent.FinalFlags.Tech.Tech & ::klib::ENTITY_TECHNOLOGY_DIGITAL))
					result													= ::klib::TURN_ACTION_CANCEL;	// currently there is no better handling for this situation.
				else if(currentPlayer.Tactical.Selection.TargetUnit != -1 && targetPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit] != -1) {
					const ::klib::CCharacter									& targetCharacter										= *targetPlayer.Tactical.Army[targetPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit]];
					if(targetCharacter.IsAlive())  {
						::klib::CCharacter											& playerAgent											= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]];

						const ::gpk::n3<int32_t>								& coordPlayer											= playerAgent.Position;
						const ::gpk::n3<int32_t>								& coordTarget											= targetCharacter.Position;
						const ::gpk::n3<float>									distance												= (coordTarget-coordPlayer).Cast<float>();

						const ::klib::SEntityPoints									& playerAgentPoints										= playerAgent.FinalPoints;
						const ::klib::SEntityFlags									& playerAgentFlags										= playerAgent.FinalFlags;

						if(::gpk::bit_true(playerAgentFlags.Tech.AttackType, ::klib::ATTACK_TYPE_RANGED)) {
							bool														bInSight												= false;
							::klib::STacticalSetup										& tacticalSetup											= tacticalInfo.Setup;

							for(uint32_t iAgentInSight=0, agentsInSightCount = tacticalInfo.AgentsInTeamSight[tacticalSetup.TeamPerPlayer[tacticalInfo.CurrentPlayer]].Count;
								iAgentInSight < agentsInSightCount; ++iAgentInSight
								)
							{
								const ::klib::SAgentReference								& targetAgentTile										= tacticalInfo.AgentsInTeamSight[tacticalSetup.TeamPerPlayer[tacticalInfo.CurrentPlayer]].Agents[iAgentInSight];
								if( targetAgentTile.Agent.TeamId == tacticalSetup.TeamPerPlayer[tacticalInfo.CurrentPlayer] )
									continue;

								if( false == ::klib::isRelevantTeam(targetAgentTile.Agent.TeamId) || false == ::isRelevantPlayer(tacticalInfo, tacticalInfo.CurrentPlayer, targetAgentTile.Agent.PlayerIndex))
									continue;

								bInSight												= true;
								if(bInSight)
									break;
							}
							result													= bInSight ? ::klib::TURN_ACTION_ATTACK : ::klib::TURN_ACTION_CANCEL;
						}
						else {
							double														finalSight												= getFinalSight(playerAgent, playerAgentPoints);
							double														finalRange												= getFinalRange(playerAgent, playerAgentPoints);
							result													= (distance.Length() > finalRange || distance.Length() > finalSight)
								? ::klib::TURN_ACTION_CANCEL
								: ::klib::TURN_ACTION_ATTACK
								;
						}	// if (ranged)
					}	// if(targetAgentSelected)
				}// if(targetPlayerSelected)
			}
			else { //
				if(0 >= currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves)
					result													= ::klib::TURN_ACTION_CANCEL;
			}
		}
	}
	else if(0 < currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves)
		::gpk::bit_set(currentPlayer.Tactical.Squad.AgentStates[currentPlayer.Tactical.Selection.PlayerUnit], ::klib::AGENT_STATE_MOVE);

	return result;
}
