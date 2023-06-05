#include "tactical_gameplay.h"
#include "draw.h"
#include "Combat.h"

#include <algorithm>
#include <time.h>

static int32_t												getAgentsInTeamSight								(::klib::SAgentsReference& agentsInTeamSight, const ::klib::SAgentsReference& agentsInRange)								{
	int32_t agentsAdded = 0, agentCount = (uint32_t)agentsInRange.Count;
	while(agentsAdded < agentCount)
		agentsInTeamSight.Agents[agentsInTeamSight.Count++] = agentsInRange.Agents[agentsAdded++];

	return agentsAdded;
}

static int32_t												getAgentsInRange									(::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players, const ::gpk::n3<int32_t>& origin, double range, ::klib::SAgentsReference& agentsInRange)	{
	agentsInRange												= {};
	for(uint32_t iPlayer = 0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
		if(tacticalInfo.Setup.Players[iPlayer] == (::klib::PLAYER_INDEX)-1)
			continue;

		::klib::SGamePlayer												& currentPlayer										= players[tacticalInfo.Setup.Players[iPlayer]];
		for(uint32_t iAgent = 0, agentCount =  currentPlayer.Tactical.Squad.Size; iAgent < agentCount; ++iAgent) {
			if(currentPlayer.Tactical.Squad.Agents[iAgent] == -1)
				continue;

			const ::klib::CCharacter										& agent												= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[iAgent]];
			if(false == agent.IsAlive())
				continue;

			//const SEntityPoints	& playerAgentPoints = agent.FinalPoints;
			const ::klib::SEntityFlags										& playerAgentFlags									= agent.FinalFlags;

			const ::gpk::n3<int32_t>									& coordAgent										= agent.Position;
			const ::gpk::n3f32										distance											= (coordAgent-origin).Cast<float>();
			if(distance.Length() > range && ::gpk::bit_false(playerAgentFlags.Tech.AttackType, ::klib::ATTACK_TYPE_RANGED))
				continue;

			agentsInRange.Agents[agentsInRange.Count++]					= {{tacticalInfo.Setup.TeamPerPlayer[iPlayer], (int8_t)iPlayer, 0, (int8_t)iAgent}, {0, 0, 0}};
			if(agentsInRange.Count >= (int32_t)agentsInRange.Agents.size())
				break;
		}
	}
	return agentsInRange.Count;
}

void														recalculateAgentsInRangeAndSight					(::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players)																	{
	::memset(&tacticalInfo.AgentsInTeamSight[0], 0, sizeof(tacticalInfo.AgentsInTeamSight));
	for(uint32_t iPlayer=0; iPlayer < tacticalInfo.Setup.TotalPlayers; ++iPlayer) 	{
		if(tacticalInfo.Setup.Players[iPlayer] == (::klib::PLAYER_INDEX)-1)
			continue;

		::klib::SGamePlayer												& player											= players[tacticalInfo.Setup.Players[iPlayer]];
		uint16_t														squadSize											= tacticalInfo.Setup.SquadSize[iPlayer];
		for(uint32_t iAgent=0; iAgent < squadSize; ++iAgent) {
			if(player.Tactical.Squad.Agents[iAgent] == -1)
				continue;

			const ::klib::CCharacter										& agent												= *player.Tactical.Army[player.Tactical.Squad.Agents[iAgent]];
			if(false == agent.IsAlive())
				continue;

			double															finalRange											= ::klib::getFinalRange(agent, agent.FinalPoints);
			double															finalSight											= ::klib::getFinalSight(agent, agent.FinalPoints);
			::getAgentsInRange(tacticalInfo, players, agent.Position, finalRange, player.Tactical.Squad.AgentsInRange[iAgent]);
			::klib::getAgentsInSight(tacticalInfo, players, agent.Position, finalSight, player.Tactical.Squad.AgentsInSight[iAgent]);
			player.Tactical.Squad.AgentsInSquadSight.Count													= 0;
			::getAgentsInTeamSight(player.Tactical.Squad.AgentsInSquadSight, player.Tactical.Squad.AgentsInSight[iAgent]);
			::getAgentsInTeamSight(tacticalInfo.AgentsInTeamSight[tacticalInfo.Setup.TeamPerPlayer[iPlayer]], player.Tactical.Squad.AgentsInSquadSight);
		}
	}
}

// Returns
bool														klib::moveStep										(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players, ::klib::STacticalPlayer& player, int8_t playerIndex, int32_t agentIndex, TEAM_TYPE teamId, STacticalBoard& board, ::gpk::n3<int32_t>& agentPosition_, ::klib::SGameMessages & messages) {
	if(agentPosition_ == player.Squad.TargetPositions[agentIndex])
		return player.Squad.ActionsLeft[agentIndex].Moves <= 0;	// I added this just in case but currently there is no situation in which this function is called when the agent is in the target position already.
	else if(::gpk::bit_false(player.Squad.AgentStates[agentIndex], ::klib::AGENT_STATE_MOVE))
		return player.Squad.ActionsLeft[agentIndex].Moves <= 0;	// I added this just in case but currently there is no situation in which this function is called when the agent is in the target position already.

	const ::gpk::n3<int32_t>									initialPosition										= agentPosition_;
	::gpk::n3<int32_t>											finalPosition										= agentPosition_;
	int8_t															movesLeft											= player.Squad.ActionsLeft[agentIndex].Moves;

	// This dice makes random the selection between moving left or moving forward first when both options are available.
	// Along the epilepsy shock we give to the characters when they get stuck this improves our rudimentary pathfinding for unknown and small obstacles.
	// (More formally "pathmaking" as we make our way through sometimes by passing through solid objects).
	if(rand() % 2) {
		if (movesLeft > 0) {
				 if (player.Squad.TargetPositions[agentIndex].z > finalPosition.z && board.Tiles.IsTileAvailable(finalPosition.x, finalPosition.z + 1)) { ++finalPosition.z; --movesLeft; }
			else if (player.Squad.TargetPositions[agentIndex].z < finalPosition.z && board.Tiles.IsTileAvailable(finalPosition.x, finalPosition.z - 1)) { --finalPosition.z; --movesLeft; }
		}
		if (movesLeft > 0) {
				 if (player.Squad.TargetPositions[agentIndex].x > finalPosition.x && board.Tiles.IsTileAvailable(finalPosition.x + 1, finalPosition.z)) { ++finalPosition.x; --movesLeft; }
			else if (player.Squad.TargetPositions[agentIndex].x < finalPosition.x && board.Tiles.IsTileAvailable(finalPosition.x - 1, finalPosition.z)) { --finalPosition.x; --movesLeft; }
		}
	}
	else {
		if (movesLeft > 0) {
				 if( player.Squad.TargetPositions[agentIndex].x > finalPosition.x && board.Tiles.IsTileAvailable( finalPosition.x + 1, finalPosition.z )) { ++finalPosition.x; --movesLeft; }
			else if( player.Squad.TargetPositions[agentIndex].x < finalPosition.x && board.Tiles.IsTileAvailable( finalPosition.x - 1, finalPosition.z )) { --finalPosition.x; --movesLeft; }
		}
		if (movesLeft > 0) {
				 if( player.Squad.TargetPositions[agentIndex].z > finalPosition.z && board.Tiles.IsTileAvailable( finalPosition.x, finalPosition.z + 1)) { ++finalPosition.z; --movesLeft; }
			else if( player.Squad.TargetPositions[agentIndex].z < finalPosition.z && board.Tiles.IsTileAvailable( finalPosition.x, finalPosition.z - 1)) { --finalPosition.z; --movesLeft; }
		}
	}
	player.Squad.ActionsLeft[agentIndex].Moves					= movesLeft;

	::gpk::view_grid<::klib::STileCharacter>						terrainAgents										= board.Tiles.Entities.Agents;
	bool															bArrived											= true;
	if( initialPosition == finalPosition ) {	// If we didn't move yet is because we're stuck.
		bArrived													= false;
		// The following noise gives epilepsy to the agents when they get stuck against a wall.
		int32_t															tryCount											= 0
			,															maxTry												= 9
			;
		do {
			if(::rand() % 2)
				finalPosition.x												+= rand() % 3 - 1;
			else
				finalPosition.z												+= rand() % 3 - 1;
		}
		while( (maxTry > ++tryCount)
			&& (finalPosition.x < 0 || finalPosition.x >= (int32_t)terrainAgents.metrics().x)
			|| (finalPosition.z < 0 || finalPosition.z >= (int32_t)terrainAgents.metrics().y)
		);
	}
	else {
		// Check if there's money here and pick it up.
		::klib::CCharacter												& playerAgent										= *player.Army[player.Squad.Agents[agentIndex]];
		playerAgent.Points.Coins									+= board.Tiles.Entities.Coins[finalPosition.z][finalPosition.x] >> 1;
		player.Money												+= board.Tiles.Entities.Coins[finalPosition.z][finalPosition.x] >> 1;
		board.Tiles.Entities.Coins[finalPosition.z][finalPosition.x]= 0;
		for(uint32_t iAOE = 0, countAOE = board.AreaOfEffect.AOE.size(); iAOE < countAOE; ++iAOE) {
			const ::klib::SAOE												& aoeInstance										= board.AreaOfEffect.AOE[iAOE];
			const ::gpk::n3<int32_t>									aoeCell												= aoeInstance.Position.Cell;
			::gpk::n3f32											aoePos												= aoeCell.Cast<float>();
			aoePos.x													+= aoeInstance.Position.Offset.x;
			aoePos.y													+= aoeInstance.Position.Offset.y;
			aoePos.z													+= aoeInstance.Position.Offset.z;
			::gpk::n3f32											currentTilePos										= playerAgent.Position.Cast<float>();
			if((aoePos - currentTilePos).Length() <= aoeInstance.RadiusOrHalfSize && aoeInstance.StatusInflict)
				::klib::applyAttackStatus(entityTables, messages, playerAgent, aoeInstance.StatusInflict, aoeInstance.Level, ::gpk::view_const_string{"Area of effect"});
		}
	}

	if( terrainAgents[initialPosition.z][initialPosition.x].AgentIndex  == agentIndex
	 && terrainAgents[initialPosition.z][initialPosition.x].PlayerIndex == playerIndex )
		terrainAgents[initialPosition.z][initialPosition.x]			= {teamId, -1, -1, -1};

	if( terrainAgents[finalPosition.z][finalPosition.x].AgentIndex  == -1
	 && terrainAgents[finalPosition.z][finalPosition.x].PlayerIndex == -1 )
		terrainAgents[finalPosition.z][finalPosition.x]				= {teamId, playerIndex, (int8_t)player.Selection.PlayerSquad, (int8_t)agentIndex};

	if(finalPosition != initialPosition) {
		agentPosition_												= finalPosition;
		//CCharacter													& agent																	= *player.Army[player.Squad.Agents[agentIndex]];
		::recalculateAgentsInRangeAndSight(tacticalInfo, players);
	}

	return (bArrived && finalPosition == player.Squad.TargetPositions[agentIndex]);
}

bool									klib::isTacticalValid			(::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players)	{
	bool										bResult							= false;
	for(uint32_t iPlayer=0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer<playerCount; ++iPlayer ) {
		if(tacticalInfo.Setup.Players[iPlayer] == -1)
			continue;

		const ::klib::STacticalPlayer				& currentPlayer					= players[tacticalInfo.Setup.Players[iPlayer]].Tactical;

		// Only team AI is considered for the victory as oppossed to other AI types like the ones for civilian characters .
		if(currentPlayer.Control.Type == ::klib::PLAYER_CONTROL_AI && currentPlayer.Control.AIMode != ::klib::PLAYER_AI_TEAMERS)
			continue;

		if(!currentPlayer.IsAlive())
			continue;

		// Compare this player against the others to make sure there are no other players of an opposite team.
		for(uint32_t iPlayerOther=iPlayer+1; iPlayerOther < playerCount; ++iPlayerOther) {
			if(tacticalInfo.Setup.Players[iPlayerOther] == -1)
				continue;

			const ::klib::STacticalPlayer				& playerOther = players[tacticalInfo.Setup.Players[iPlayerOther]].Tactical;

			if(playerOther.Control.Type == ::klib::PLAYER_CONTROL_AI && playerOther.Control.AIMode != ::klib::PLAYER_AI_TEAMERS)	// Only team AI is considered for the victory
				continue;

			if(tacticalInfo.Setup.TeamPerPlayer[iPlayer] == tacticalInfo.Setup.TeamPerPlayer[iPlayerOther])
				continue;

			if(!playerOther.IsAlive())
				continue;

			bResult																					= true;
			break;
		}

		if(bResult)
			break;
	}

	return bResult;
}

static	bool							fixAgentSelection			(::klib::STacticalPlayer&	currentPlayer)																{
	if(currentPlayer.Selection.PlayerUnit == -1 || currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit] == -1)
		return currentPlayer.SelectNextAgent();
	else {
		const ::klib::CCharacter					& agent						= *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];
		if(agent.DidLoseTurn() || !agent.IsAlive())
			return currentPlayer.SelectNextAgent();
	}
	return true;
}

uint32_t								resolveNextPlayer			(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players, ::klib::SGameMessages & messages)																	{
	::klib::STacticalSetup						& tacticalSetup				= tacticalInfo.Setup;
	int32_t										currentPlayerSlot			= -1;

	uint32_t									playerCountToCheck			= 0;

	bool										bNeedSkipCurrentPlayer		;
	bool										bCantMove					;

	do {
		++tacticalInfo.CurrentTeam;
		if(tacticalInfo.CurrentTeam >= (int32_t)tacticalSetup.TotalTeams)
			tacticalInfo.CurrentTeam				= 0;

		int32_t										totalPlayersForThisTeam		= 0;
		do {
			++tacticalInfo.CurrentPlayerPerTeam	[tacticalInfo.CurrentTeam];
			if(tacticalInfo.CurrentPlayerPerTeam	[tacticalInfo.CurrentTeam] >= (int32_t)tacticalSetup.PlayerCountPerTeam[tacticalInfo.CurrentTeam])
				tacticalInfo.CurrentPlayerPerTeam	[tacticalInfo.CurrentTeam]			= 0;

			currentPlayerSlot						= tacticalSetup.PlayersPerTeam[tacticalInfo.CurrentTeam][tacticalInfo.CurrentPlayerPerTeam[tacticalInfo.CurrentTeam]];
			::klib::STacticalPlayer						& player					= players[tacticalSetup.Players[currentPlayerSlot]].Tactical;
			bCantMove								= false == player.CanMove();
			if(bCantMove) {
				for(uint32_t iAgent=0, agentCount = tacticalSetup.SquadSize[currentPlayerSlot]; iAgent<agentCount; ++iAgent) {
					if(player.Squad.Agents[iAgent] == -1)
						continue;
					::klib::CCharacter																				& agent																	= *player.Army[player.Squad.Agents[iAgent]];
					if(!agent.IsAlive())
						continue;
					::klib::applyRoundStatusAndBonusesAndSkipRound(entityTables, messages, agent);
					if(!agent.IsAlive())
						::klib::handleAgentDeath(tacticalInfo, players, agent, tacticalInfo.Setup.TeamPerPlayer[currentPlayerSlot], messages);
				}
			}

			++playerCountToCheck;
		}
		while(bCantMove && (++totalPlayersForThisTeam) < tacticalSetup.PlayerCountPerTeam[tacticalInfo.CurrentTeam] && (playerCountToCheck) < (tacticalSetup.TotalPlayers));

		bNeedSkipCurrentPlayer																	= bCantMove;

		if (bNeedSkipCurrentPlayer) {
			info_printf("Why skip? - bCantMove ? %s", bCantMove ? "true" : "false");
		}
	}
	while(::klib::isTacticalValid(tacticalInfo, players) && bNeedSkipCurrentPlayer && (playerCountToCheck) < (tacticalSetup.TotalPlayers));

	if(bNeedSkipCurrentPlayer)
		info_printf("Why skip? - bCantMove ? %s", bCantMove ? "true" : "false");
	return currentPlayerSlot;
}

// Ending the turn resets action and movement counters and executes minimal AI for selecting another unit.
// This function changes the value of STacticalInfo::CurrentPlayer.
void									klib::endTurn															(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players, ::klib::SGameMessages & messages)		{
	for(uint32_t iPlayer=0, playerCount=tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
		if(-1 == tacticalInfo.Setup.Players[iPlayer])
			continue;

		::klib::STacticalPlayer						& playerToClear															= players[tacticalInfo.Setup.Players[iPlayer]].Tactical;
		if(!playerToClear.IsAlive())
			continue;

		playerToClear.Selection.TargetPlayer	= -1;
		playerToClear.Selection.TargetSquad		= -1;
		playerToClear.Selection.TargetUnit		= -1;

		for(uint32_t iAgent = 0, agentCount = tacticalInfo.Setup.SquadSize[iPlayer]; iAgent < agentCount; ++iAgent) {
			if(-1 == playerToClear.Squad.Agents[iAgent])
				continue;

			::klib::CCharacter							& character																= *playerToClear.Army[playerToClear.Squad.Agents[iAgent]];
			if(0 >= character.Points.LifeCurrent.Health)
				continue;

			//character.Recalculate();
			const ::klib::SEntityPoints																& agentFinalPoints														= character.FinalPoints;
			playerToClear.Squad.ActionsLeft[iAgent].Moves								= (int8_t)agentFinalPoints.Fitness.Movement;
			playerToClear.Squad.ActionsLeft[iAgent].Actions							= 1;
			playerToClear.Squad.TargetPositions[iAgent]								= character.Position;

			::gpk::bit_clear(playerToClear.Squad.AgentStates[iAgent], ::klib::AGENT_STATE_MOVE);

			::klib::applyTurnStatusAndBonusesAndSkipTurn(entityTables, messages, character);
			++character.Score.TurnsPlayed;

			if(!character.IsAlive())
				::klib::handleAgentDeath(tacticalInfo, players, character, tacticalInfo.Setup.TeamPerPlayer[iPlayer], messages);
			else if(((int8_t)iPlayer) == tacticalInfo.CurrentPlayer) {
				::klib::applyRoundStatusAndBonusesAndSkipRound(entityTables, messages, character);
				if(!character.IsAlive())
					::klib::handleAgentDeath(tacticalInfo, players, character, tacticalInfo.Setup.TeamPerPlayer[iPlayer], messages);
			}
			else if(tacticalInfo.Setup.TeamPerPlayer[iPlayer] != tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer] ) {
				::klib::applyEnemyTurnStatusAndBonusesAndSkipTurn(entityTables, messages, character);
				if(!character.IsAlive())
					::klib::handleAgentDeath(tacticalInfo, players, character, tacticalInfo.Setup.TeamPerPlayer[iPlayer], messages);
			}
		}

		// Change character selections at the end of the turn because selected agents may have died in between turns.
		if(playerToClear.Control.Type == ::klib::PLAYER_CONTROL_AI) {
			while(::rand() % tacticalInfo.Setup.SquadSize[iPlayer])
				playerToClear.SelectNextAgent();
		}
		::fixAgentSelection(playerToClear);	// Make sure a valid agent is selected.
		playerToClear.Squad.LockedAgent															= -1;
	}

	uint32_t																					iAOE																	= 0;
	while(iAOE < tacticalInfo.Board.AreaOfEffect.AOE.size()) {
		::klib::SAOE																				& aoeInstance															= tacticalInfo.Board.AreaOfEffect.AOE[iAOE];
		if(0 == --aoeInstance.TurnsLeft) {
			tacticalInfo.Board.AreaOfEffect.Coords	.remove_unordered(iAOE);
			tacticalInfo.Board.AreaOfEffect.AOE		.remove_unordered(iAOE);
		}
		else
			++iAOE;
	}

	tacticalInfo.CurrentPlayer																= (int8_t)resolveNextPlayer(entityTables, tacticalInfo, players, messages);	// Change current player.
}

void																					selectAIDestination														(::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players);
bool																					klib::updateCurrentPlayer												(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players, ::klib::SGameMessages & messages)																	{
	::klib::STacticalPlayer																		& currentPlayer															= players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]].Tactical;	// the current player is only valid in this scope. After this code the current player can change

	if( !::fixAgentSelection(currentPlayer) )
		return false;

	if( currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]]->Points.LifeCurrent.Health <= 0 )
		return false;

	::gpk::n3<int32_t>																		& currentAgentPosition													= currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]]->Position;

	bool																						bHasArrived																= true;
	if( currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit] != currentAgentPosition && (0 < currentPlayer.Squad.ActionsLeft[currentPlayer.Selection.PlayerUnit].Moves) ) {
		bHasArrived																				= ::klib::moveStep(entityTables, tacticalInfo, players, currentPlayer, tacticalInfo.CurrentPlayer, currentPlayer.Selection.PlayerUnit, tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer], tacticalInfo.Board, currentAgentPosition, messages);
		if(bHasArrived)
			::gpk::bit_clear(currentPlayer.Squad.AgentStates[currentPlayer.Selection.PlayerUnit], ::klib::AGENT_STATE_MOVE);
	}

	if(currentPlayer.Control.Type != ::klib::PLAYER_CONTROL_AI)
		return true;
	if( 0 == currentPlayer.Squad.ActionsLeft[currentPlayer.Selection.PlayerUnit].Moves
	 && 0 == currentPlayer.Squad.ActionsLeft[currentPlayer.Selection.PlayerUnit].Actions
	 )
		return false;
	if( bHasArrived && (0 < currentPlayer.Squad.ActionsLeft[currentPlayer.Selection.PlayerUnit].Moves) ) {
		::selectAIDestination(tacticalInfo, players);
		::gpk::bit_set(currentPlayer.Squad.AgentStates[currentPlayer.Selection.PlayerUnit], ::klib::AGENT_STATE_MOVE);
	}
	return true;
}

template<typename _TEntity>
void																					pickupEntities
	(	::klib::SEntityContainer<_TEntity>	& playerEntities
	,	::gpk::array_pod<_TEntity>			& mapEntities
	,	int32_t								maxCount				= 4096
	)
{
	int32_t																						currentCount															= 0;
	while(mapEntities.size() && currentCount < maxCount) {
		mapEntities[0].Owner																	= -1;
		playerEntities.AddElement(mapEntities[0]);
		mapEntities.remove_unordered(0);
		++currentCount;
	}
}

void																					distributeDropsForVictoriousTeam										(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players, ::klib::TEAM_TYPE teamVictorious, ::klib::SGameMessages & messages)											{
	uint32_t																					totalWinners															= 0;
	::gpk::array_static<int32_t, ::klib::MAX_TACTICAL_PLAYERS>									indexWinners															= {};
	::memset(&indexWinners[0], -1, sizeof(::klib::PLAYER_INDEX) * indexWinners.size());

	for(uint32_t iPlayer = 0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
		if(tacticalInfo.Setup.Players[iPlayer] == -1 || tacticalInfo.Setup.TeamPerPlayer[iPlayer] != teamVictorious)
			continue;

		indexWinners[totalWinners++]															= iPlayer;
	}

	int64_t																						totalMapMoney															= 0;
	for(uint32_t z = 0, depth = tacticalInfo.Board.Tiles.Terrain.Geometry.metrics().y; z < depth; ++z)
	for(uint32_t x = 0, width = tacticalInfo.Board.Tiles.Terrain.Geometry.metrics().x; x < width; ++x)
		totalMapMoney																			+= tacticalInfo.Board.Tiles.Entities.Coins[z][x];

	if(totalWinners == 0) {
		messages.UserMiss																		= ::gpk::view_const_string{"No winners?"};
		messages.LogMiss();
	}
	else if(totalWinners == 1) {
		::klib::SGamePlayer																			& winnerPlayer															= players[tacticalInfo.Setup.Players[indexWinners[0]]];
		::pickupEntities(winnerPlayer.Inventory.Profession	, tacticalInfo.Drops.Profession	);
		::pickupEntities(winnerPlayer.Inventory.Accessory	, tacticalInfo.Drops.Accessory	);
		::pickupEntities(winnerPlayer.Inventory.Armor		, tacticalInfo.Drops.Armor		);
		::pickupEntities(winnerPlayer.Inventory.Weapon		, tacticalInfo.Drops.Weapon		);
		::pickupEntities(winnerPlayer.Inventory.Vehicle		, tacticalInfo.Drops.Vehicle	);
		::pickupEntities(winnerPlayer.Inventory.Facility	, tacticalInfo.Drops.Facility	);
		::pickupEntities(winnerPlayer.Inventory.StageProp	, tacticalInfo.Drops.StageProp	);
		::pickupEntities(winnerPlayer.Inventory.Items		, tacticalInfo.Drops.Items		);
		for(uint32_t iAgent = 0; iAgent < winnerPlayer.Tactical.Squad.Agents.size(); ++iAgent) {
			int16_t currentAgent = winnerPlayer.Tactical.Squad.Agents[iAgent];
			if(-1 != currentAgent) {
				winnerPlayer.Tactical.Army[currentAgent]->Recalculate(entityTables);
				int64_t																					agentReward				= 0;
				if(winnerPlayer.Tactical.Army[currentAgent]->FinalPoints.LifeCurrent.Health > 0)
					agentReward																				= int64_t(winnerPlayer.Tactical.Army[currentAgent]->Points.Coins / 4.0 * 3);
				else
					agentReward																				= winnerPlayer.Tactical.Army[currentAgent]->Points.Coins;
				if(agentReward) {
					winnerPlayer.Tactical.Money																		+= agentReward;
					winnerPlayer.Tactical.Score.MoneyEarned															+= agentReward;
					winnerPlayer.Tactical.Army[currentAgent]->Points.Coins											-= agentReward;
					sprintf_s(messages.Aux, "Money from agent %i: %llu.", iAgent, agentReward);
					messages.LogAuxSuccess();
				}
			}
		}
		winnerPlayer.Tactical.Money																+= totalMapMoney;
		winnerPlayer.Tactical.Score.MoneyEarned													+= totalMapMoney;
		winnerPlayer.Tactical.Score.BattlesWon													+= 1;

		sprintf_s(messages.Aux, "Money from map: %lli.", totalMapMoney);
		messages.LogAuxSuccess();
	}
	else {
		int32_t																						totalProfession															= tacticalInfo.Drops.Profession	.size();
		int32_t																						totalAccessory															= tacticalInfo.Drops.Accessory	.size();
		int32_t																						totalArmor																= tacticalInfo.Drops.Armor		.size();
		int32_t																						totalWeapon																= tacticalInfo.Drops.Weapon		.size();
		int32_t																						totalVehicle															= tacticalInfo.Drops.Vehicle	.size();
		int32_t																						totalFacility															= tacticalInfo.Drops.Facility	.size();
		int32_t																						totalStageProp															= tacticalInfo.Drops.StageProp	.size();
		int32_t																						totalItems																= tacticalInfo.Drops.Items		.size();
		for(uint32_t iWinner = 0; iWinner < totalWinners; ++iWinner) {
			::klib::SGamePlayer																				& winnerPlayer															= players[tacticalInfo.Setup.Players[indexWinners[iWinner]]];
			::pickupEntities(winnerPlayer.Inventory.Profession	, tacticalInfo.Drops.Profession	, totalProfession	/ totalWinners);
			::pickupEntities(winnerPlayer.Inventory.Accessory	, tacticalInfo.Drops.Accessory	, totalAccessory	/ totalWinners);
			::pickupEntities(winnerPlayer.Inventory.Armor		, tacticalInfo.Drops.Armor		, totalArmor		/ totalWinners);
			::pickupEntities(winnerPlayer.Inventory.Weapon		, tacticalInfo.Drops.Weapon		, totalWeapon		/ totalWinners);
			::pickupEntities(winnerPlayer.Inventory.Vehicle		, tacticalInfo.Drops.Vehicle	, totalVehicle		/ totalWinners);
			::pickupEntities(winnerPlayer.Inventory.Facility	, tacticalInfo.Drops.Facility	, totalFacility		/ totalWinners);
			::pickupEntities(winnerPlayer.Inventory.StageProp	, tacticalInfo.Drops.StageProp	, totalStageProp	/ totalWinners);
			::pickupEntities(winnerPlayer.Inventory.Items		, tacticalInfo.Drops.Items		, totalItems		/ totalWinners);
			winnerPlayer.Tactical.Score.BattlesWon															+= 1;
		}

		for(uint32_t iWinner = 0; iWinner < totalWinners; ++iWinner) {
			::klib::SGamePlayer																				& winnerPlayer															= players[tacticalInfo.Setup.Players[indexWinners[iWinner]]];
			for(uint32_t iAgent = 0, agentCount = tacticalInfo.Setup.SquadSize[indexWinners[iWinner]]; iAgent < agentCount; ++iAgent) {
				if(winnerPlayer.Tactical.Squad.Agents[iAgent] == -1)
					continue;

				if(false == winnerPlayer.Tactical.Army[winnerPlayer.Tactical.Squad.Agents[iAgent]]->IsAlive())
					continue;

				++winnerPlayer.Tactical.Army[winnerPlayer.Tactical.Squad.Agents[iAgent]]->Score.BattlesWon;
			}
		}

		const int32_t														playerIndices [] =
			{ tacticalInfo.Setup.Players[indexWinners[::rand() % totalWinners]]
			, tacticalInfo.Setup.Players[indexWinners[::rand() % totalWinners]]
			, tacticalInfo.Setup.Players[indexWinners[::rand() % totalWinners]]
			, tacticalInfo.Setup.Players[indexWinners[::rand() % totalWinners]]
			, tacticalInfo.Setup.Players[indexWinners[::rand() % totalWinners]]
			, tacticalInfo.Setup.Players[indexWinners[::rand() % totalWinners]]
			, tacticalInfo.Setup.Players[indexWinners[::rand() % totalWinners]]
			, tacticalInfo.Setup.Players[indexWinners[::rand() % totalWinners]]
			};

		// Give the remaining drops to random winners
		if(	tacticalInfo.Drops.Profession	.size()) ::pickupEntities(players[playerIndices[0]].Inventory.Profession	, tacticalInfo.Drops.Profession	);
		if(	tacticalInfo.Drops.Accessory	.size()) ::pickupEntities(players[playerIndices[1]].Inventory.Accessory		, tacticalInfo.Drops.Accessory	);
		if(	tacticalInfo.Drops.Armor		.size()) ::pickupEntities(players[playerIndices[2]].Inventory.Armor			, tacticalInfo.Drops.Armor		);
		if(	tacticalInfo.Drops.Weapon		.size()) ::pickupEntities(players[playerIndices[3]].Inventory.Weapon		, tacticalInfo.Drops.Weapon		);
		if(	tacticalInfo.Drops.Vehicle		.size()) ::pickupEntities(players[playerIndices[4]].Inventory.Vehicle		, tacticalInfo.Drops.Vehicle	);
		if(	tacticalInfo.Drops.Facility		.size()) ::pickupEntities(players[playerIndices[5]].Inventory.Facility		, tacticalInfo.Drops.Facility	);
		if(	tacticalInfo.Drops.StageProp	.size()) ::pickupEntities(players[playerIndices[6]].Inventory.StageProp		, tacticalInfo.Drops.StageProp	);
		if(	tacticalInfo.Drops.Items		.size()) ::pickupEntities(players[playerIndices[7]].Inventory.Items			, tacticalInfo.Drops.Items		);

		int64_t																					reward																	= totalMapMoney / totalWinners;
		for(uint32_t iWinner=0; iWinner<totalWinners; ++iWinner) {
			::klib::SGamePlayer																			& winnerPlayer															= players[tacticalInfo.Setup.Players[indexWinners[iWinner]]];
			winnerPlayer.Tactical.Money															+= reward;
			totalMapMoney																		-= reward;
			winnerPlayer.Tactical.Score.MoneyEarned												+= reward;
			sprintf_s(messages.Aux, "Money from map for player %u: %lli.", indexWinners[iWinner], reward);
			messages.LogAuxSuccess();

			for(uint32_t iAgent = 0; iAgent < winnerPlayer.Tactical.Squad.Size; ++iAgent) {
				int16_t				currentAgent = winnerPlayer.Tactical.Squad.Agents[iAgent];
				if(-1 != currentAgent) {
					winnerPlayer.Tactical.Army[currentAgent]->Recalculate(entityTables);
					int64_t																					agentReward				= 0;
					if(winnerPlayer.Tactical.Army[currentAgent]->FinalPoints.LifeCurrent.Health > 0)
						agentReward																			= int64_t(winnerPlayer.Tactical.Army[currentAgent]->Points.Coins / 4.0 * 3);
					else
						agentReward																			= winnerPlayer.Tactical.Army[currentAgent]->Points.Coins;
					if(agentReward) {
						winnerPlayer.Tactical.Money																		+= agentReward;
						winnerPlayer.Tactical.Score.MoneyEarned															+= agentReward;
						winnerPlayer.Tactical.Army[currentAgent]->Points.Coins											-= agentReward;
						sprintf_s(messages.Aux, "Money from agent %i for player %u: %lli.", iAgent, indexWinners[iWinner], agentReward);
						messages.LogAuxSuccess();
					}
				}
			}
		}
		if(totalMapMoney)
			players[tacticalInfo.Setup.Players[indexWinners[::rand()%totalWinners]]].Tactical.Money	+= totalMapMoney;
	}

	for(uint32_t iPlayer = 0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
		if(tacticalInfo.Setup.Players[iPlayer] == -1)
			continue;

		bool																					bIsWinner																= false;
		for(uint32_t iWinner = 0; iWinner < totalWinners; ++iWinner) {
			if( indexWinners[iWinner] == (int32_t)iPlayer ) {
				bIsWinner																			= true;
				break;
			}
		}

		if(bIsWinner)
			continue;

		players[tacticalInfo.Setup.Players[iPlayer]].Tactical.Score.BattlesLost			+= 1;
	}
}

void																					klib::determineOutcome													(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players, ::klib::SGameMessages & messages, bool aborted)																	{
	::klib::TEAM_TYPE																			teamVictorious															= ::klib::TEAM_TYPE_SPECTATOR;
	if(aborted)  {
		messages.UserMessage																	= ::gpk::view_const_string{"Mission aborted."};
		messages.LogMessage();
		return;
	}
	for(uint32_t iPlayer = 0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
		if(tacticalInfo.Setup.Players[iPlayer] == -1)
			continue;

		::klib::SGamePlayer																			& currentPlayer															= players[tacticalInfo.Setup.Players[iPlayer]];

		if(currentPlayer.Tactical.Control.Type == ::klib::PLAYER_CONTROL_AI && currentPlayer.Tactical.Control.AIMode != ::klib::PLAYER_AI_TEAMERS)	// Only team AI is considered for the victory
			continue;

		for(uint32_t iAgent = 0, agentCount = tacticalInfo.Setup.SquadSize[iPlayer]; iAgent < agentCount; ++iAgent) {
			if(currentPlayer.Tactical.Squad.Agents[iAgent] == -1)
				continue;

			if(0 == currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[iAgent]].get_ref())
				continue;

			const ::klib::CCharacter																	& deadCharacter															= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[iAgent]];
			if(deadCharacter.IsAlive()) {
				::klib::CCharacter																			& liveCharacter															= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[iAgent]];
				liveCharacter.CurrentEquip.Accessory	.Level											+= 1;
				liveCharacter.CurrentEquip.Weapon		.Level											+= 1;
				liveCharacter.CurrentEquip.Armor		.Level											+= 1;
				liveCharacter.CurrentEquip.Profession	.Level											+= 1;
				liveCharacter.Recalculate(entityTables);
				continue;
			}

			int32_t																						oldAgentIndex															= currentPlayer.Tactical.Squad.Agents[iAgent];
			currentPlayer.Tactical.Squad.Agents[iAgent]												= -1;
			currentPlayer.Memorial.push_back(deadCharacter);
			if(oldAgentIndex == (int32_t)(currentPlayer.Tactical.Army.size()-1))
				currentPlayer.Tactical.Army.pop_back();
			else
				currentPlayer.Tactical.Army[oldAgentIndex]												= {};
		}
		if(!currentPlayer.Tactical.IsAlive())
			continue;
		teamVictorious																			= tacticalInfo.Setup.TeamPerPlayer[iPlayer];
		break;
	}

	if(teamVictorious == ::klib::TEAM_TYPE_SPECTATOR) {
		messages.UserMessage																	= ::gpk::view_const_string{"No winners."};
		messages.LogMessage();
	} else {
		::sprintf_s(messages.Aux, "Team %s won the match.", ::gpk::get_value_label(teamVictorious).begin());
		if(teamVictorious == ::klib::TEAM_TYPE_ALLY)
			messages.LogAuxSuccess();
		else
			messages.LogAuxMiss();
	}
	if(teamVictorious != ::klib::TEAM_TYPE_SPECTATOR) // only give rewards if somebody won.
		::distributeDropsForVictoriousTeam(entityTables, tacticalInfo, players, teamVictorious, messages);

	tacticalInfo.Clear();
}

template<typename _TEntity>
	void																				dropEntities
	(	::gpk::array_pod<_TEntity>					& mapEntities
	,	::klib::SEntityContainer<_TEntity>			& deadTargetEntities
	,	::gpk::array_pod<::gpk::n3<int32_t>>	& entityCoords
	,	const ::gpk::n3<int32_t>				& deadTargetCoords
	) {
	while(deadTargetEntities.Slots.size()) {
		if(deadTargetEntities[0].Entity.Definition != -1) {
			mapEntities.push_back(deadTargetEntities[0].Entity);
			entityCoords.push_back(deadTargetCoords);
		}
		deadTargetEntities	.DecreaseEntity(0);
	}
}

void																					klib::handleAgentDeath													(::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players, ::klib::CCharacter& deadTarget, ::klib::TEAM_TYPE teamId, ::klib::SGameMessages& messages)	{
	::klib::SCharacterInventory																	& targetInventory														= deadTarget.Goods.Inventory;
	::klib::SCharacterEquip																		& targetEquip															= deadTarget.CurrentEquip;
	::klib::SMapInventory																		& mapDrops																= tacticalInfo.Drops;

	::dropEntities(mapDrops.Profession	, targetInventory.Profession	, mapDrops.CoordsProfession	, deadTarget.Position);
	::dropEntities(mapDrops.Accessory	, targetInventory.Accessory		, mapDrops.CoordsAccessory	, deadTarget.Position);
	::dropEntities(mapDrops.Armor		, targetInventory.Armor			, mapDrops.CoordsArmor		, deadTarget.Position);
	::dropEntities(mapDrops.Weapon		, targetInventory.Weapon		, mapDrops.CoordsWeapon		, deadTarget.Position);
	::dropEntities(mapDrops.Vehicle		, targetInventory.Vehicle		, mapDrops.CoordsVehicle	, deadTarget.Position);
	::dropEntities(mapDrops.Facility	, targetInventory.Facility		, mapDrops.CoordsFacility	, deadTarget.Position);
	::dropEntities(mapDrops.StageProp	, targetInventory.StageProp		, mapDrops.CoordsStageProp	, deadTarget.Position);
	::dropEntities(mapDrops.Items		, targetInventory.Items			, mapDrops.CoordsItems		, deadTarget.Position);

	/*targetEquip.Profession.Owner = -1;*/	mapDrops.Profession	.push_back(targetEquip.Profession	); targetEquip.Profession	= {}; mapDrops.CoordsProfession	.push_back(deadTarget.Position);
	/*targetEquip.Accessory	.Owner = -1;*/	mapDrops.Accessory	.push_back(targetEquip.Accessory	); targetEquip.Accessory	= {}; mapDrops.CoordsAccessory	.push_back(deadTarget.Position);
	/*targetEquip.Armor		.Owner = -1;*/	mapDrops.Armor		.push_back(targetEquip.Armor		); targetEquip.Armor		= {}; mapDrops.CoordsArmor		.push_back(deadTarget.Position);
	/*targetEquip.Weapon	.Owner = -1;*/	mapDrops.Weapon		.push_back(targetEquip.Weapon		); targetEquip.Weapon		= {}; mapDrops.CoordsWeapon		.push_back(deadTarget.Position);
	/*targetEquip.Vehicle	.Owner = -1;*/	mapDrops.Vehicle	.push_back(targetEquip.Vehicle		); targetEquip.Vehicle		= {}; mapDrops.CoordsVehicle	.push_back(deadTarget.Position);
	/*targetEquip.Facility	.Owner = -1;*/	mapDrops.Facility	.push_back(targetEquip.Facility		); targetEquip.Facility		= {}; mapDrops.CoordsFacility	.push_back(deadTarget.Position);
	/*targetEquip.StageProp	.Owner = -1;*/	mapDrops.StageProp	.push_back(targetEquip.StageProp	); targetEquip.StageProp	= {}; mapDrops.CoordsStageProp	.push_back(deadTarget.Position);
	///*targetEquip.Items	.Owner = -1;*/	mapDrops.Items		.push_back(targetEquip.Items		); targetEquip.Item			= {}; mapDrops.CoordsItems		.push_back(deadTarget.Position);

	tacticalInfo.Board.Tiles.Entities.Coins[deadTarget.Position.z][deadTarget.Position.x] += deadTarget.Points.Coins;
	deadTarget.Points.Coins																	= 0;

	::gpk::apod<char>																	ripText																	= deadTarget.Name;
	ripText.append_string(" has died.");
	if(teamId == tacticalInfo.Setup.TeamPerPlayer[::klib::PLAYER_INDEX_USER]) {
		messages.UserMiss																		= ripText;
		messages.LogMiss();
	}
	else {
		messages.UserSuccess																	= ripText;
		messages.LogSuccess();
	}
	::recalculateAgentsInRangeAndSight(tacticalInfo, players);
}

void																					klib::handleAgentDeath													(::klib::STacticalInfo & tacticalInfo, ::gpk::view<::klib::SGamePlayer> players, ::klib::CCharacter& deadTarget, ::klib::CCharacter& attacker, ::klib::TEAM_TYPE teamId, ::klib::SGameMessages & messages)	{
	::klib::handleAgentDeath(tacticalInfo, players, deadTarget, teamId, messages);
	++attacker.Score.EnemiesKilled;
}
