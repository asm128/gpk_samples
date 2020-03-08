// #define NOMINMAX

#include "tactical_gameplay.h"
#include "draw.h"
#include "Combat.h"

#include <algorithm>
#include <time.h>

using namespace klib;

static int32_t												getAgentsInTeamSight								(SAgentsReference& agentsInTeamSight, const SAgentsReference& agentsInRange)								{
	int32_t agentsAdded = 0, agentCount = (uint32_t)agentsInRange.Count;
	while(agentsAdded < agentCount)
		agentsInTeamSight.Agents[agentsInTeamSight.Count++] = agentsInRange.Agents[agentsAdded++];

	return agentsAdded;
}


static int32_t												getAgentsInRange									(SGame& instanceGame, const ::gpk::SCoord3<int32_t>& origin, double range, SAgentsReference& agentsInRange)	{
	agentsInRange												= SAgentsReference();
	STacticalInfo													& tacticalInfo										= instanceGame.TacticalInfo;
	for(uint32_t iPlayer = 0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
		if(tacticalInfo.Setup.Players[iPlayer] == PLAYER_INDEX_INVALID)
			continue;

		SPlayer															& currentPlayer										= instanceGame.Players[tacticalInfo.Setup.Players[iPlayer]];
		for(uint32_t iAgent = 0, agentCount =  currentPlayer.Tactical.Squad.Size; iAgent < agentCount; ++iAgent) {
			if(currentPlayer.Tactical.Squad.Agents[iAgent] == -1)
				continue;

			const CCharacter												& agent												= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[iAgent]];
			if(false == agent.IsAlive())
				continue;

			//const SEntityPoints	& playerAgentPoints = agent.FinalPoints;
			const SEntityFlags												& playerAgentFlags									= agent.FinalFlags;

			const ::gpk::SCoord3<int32_t>									& coordAgent										= agent.Position;
			const ::gpk::SCoord3<float>									distance											= (coordAgent-origin).Cast<float>();
			if(distance.Length() > range && ::gpk::bit_false(playerAgentFlags.Tech.AttackType, ATTACK_TYPE_RANGED))
				continue;

			agentsInRange.Agents[agentsInRange.Count++]					= {{tacticalInfo.Setup.TeamPerPlayer[iPlayer], (int8_t)iPlayer, 0, (int8_t)iAgent}, {0, 0, 0}};
			if(agentsInRange.Count >= (int32_t)agentsInRange.Agents.size())
				break;
		}
	}
	return agentsInRange.Count;
}


void																					recalculateAgentsInRangeAndSight										(SGame& instanceGame)																	{
	STacticalInfo																				& tacticalInfo															= instanceGame.TacticalInfo;
	::memset(&tacticalInfo.AgentsInTeamSight[0], 0, sizeof(tacticalInfo.AgentsInTeamSight));
	for(uint32_t iPlayer=0; iPlayer < tacticalInfo.Setup.TotalPlayers; ++iPlayer) 	{
		if(tacticalInfo.Setup.Players[iPlayer] == PLAYER_INDEX_INVALID)
			continue;

		SPlayer																						& player																= instanceGame.Players[tacticalInfo.Setup.Players[iPlayer]];
		uint16_t																					squadSize																= tacticalInfo.Setup.SquadSize[iPlayer];
		for(uint32_t iAgent=0; iAgent < squadSize; ++iAgent) {
			if(player.Tactical.Squad.Agents[iAgent] == -1)
				continue;

			const CCharacter																			& agent																	= *player.Tactical.Army[player.Tactical.Squad.Agents[iAgent]];
			if(false == agent.IsAlive())
				continue;

			double																						finalRange																= getFinalRange(agent, agent.FinalPoints);
			double																						finalSight																= getFinalSight(agent, agent.FinalPoints);
			::getAgentsInRange(instanceGame, agent.Position, finalRange, player.Tactical.Squad.AgentsInRange[iAgent]);
			::getAgentsInSight(instanceGame, agent.Position, finalSight, player.Tactical.Squad.AgentsInSight[iAgent]);
			player.Tactical.Squad.AgentsInSquadSight.Count													= 0;
			::getAgentsInTeamSight(player.Tactical.Squad.AgentsInSquadSight, player.Tactical.Squad.AgentsInSight[iAgent]);
			::getAgentsInTeamSight(tacticalInfo.AgentsInTeamSight[tacticalInfo.Setup.TeamPerPlayer[iPlayer]], player.Tactical.Squad.AgentsInSquadSight);
		}
	}
}

// Returns
bool																					klib::moveStep															(SGame& instanceGame, SPlayer& player, int8_t playerIndex, int32_t agentIndex, TEAM_TYPE teamId, STacticalBoard& board, ::gpk::SCoord3<int32_t>& agentPosition_) {
	if(agentPosition_ == player.Tactical.Squad.TargetPositions[agentIndex])
		return player.Tactical.Squad.ActionsLeft[agentIndex].Moves <= 0;	// I added this just in case but currently there is no situation in which this function is called when the agent is in the target position already.
	else if(::gpk::bit_false(player.Tactical.Squad.AgentStates[agentIndex], AGENT_STATE_MOVE))
		return player.Tactical.Squad.ActionsLeft[agentIndex].Moves <= 0;	// I added this just in case but currently there is no situation in which this function is called when the agent is in the target position already.

	const ::gpk::SCoord3<int32_t>																initialPosition															= agentPosition_;
	::gpk::SCoord3<int32_t>																		finalPosition															= agentPosition_;
	int8_t																						movesLeft																= player.Tactical.Squad.ActionsLeft[agentIndex].Moves;

	// This dice makes random the selection between moving left or moving forward first when both options are available.
	// Along the epilepsy shock we give to the characters when they get stuck this improves our rudimentary pathfinding for unknown and small obstacles.
	// (More formally "pathmaking" as we make our way through sometimes by passing through solid objects).
	if(rand() % 2) {
		if (movesLeft > 0) {
				 if (player.Tactical.Squad.TargetPositions[agentIndex].z > finalPosition.z && board.Tiles.IsTileAvailable(finalPosition.x, finalPosition.z + 1)) { ++finalPosition.z; --movesLeft; }
			else if (player.Tactical.Squad.TargetPositions[agentIndex].z < finalPosition.z && board.Tiles.IsTileAvailable(finalPosition.x, finalPosition.z - 1)) { --finalPosition.z; --movesLeft; }
		}
		if (movesLeft > 0) {
				 if (player.Tactical.Squad.TargetPositions[agentIndex].x > finalPosition.x && board.Tiles.IsTileAvailable(finalPosition.x + 1, finalPosition.z)) { ++finalPosition.x; --movesLeft; }
			else if (player.Tactical.Squad.TargetPositions[agentIndex].x < finalPosition.x && board.Tiles.IsTileAvailable(finalPosition.x - 1, finalPosition.z)) { --finalPosition.x; --movesLeft; }
		}
	}
	else {
		if (movesLeft > 0) {
				 if( player.Tactical.Squad.TargetPositions[agentIndex].x > finalPosition.x && board.Tiles.IsTileAvailable( finalPosition.x + 1, finalPosition.z )) { ++finalPosition.x; --movesLeft; }
			else if( player.Tactical.Squad.TargetPositions[agentIndex].x < finalPosition.x && board.Tiles.IsTileAvailable( finalPosition.x - 1, finalPosition.z )) { --finalPosition.x; --movesLeft; }
		}
		if (movesLeft > 0) {
				 if( player.Tactical.Squad.TargetPositions[agentIndex].z > finalPosition.z && board.Tiles.IsTileAvailable( finalPosition.x, finalPosition.z + 1)) { ++finalPosition.z; --movesLeft; }
			else if( player.Tactical.Squad.TargetPositions[agentIndex].z < finalPosition.z && board.Tiles.IsTileAvailable( finalPosition.x, finalPosition.z - 1)) { --finalPosition.z; --movesLeft; }
		}
	}
	player.Tactical.Squad.ActionsLeft[agentIndex].Moves										= movesLeft;

	::gpk::view_grid<STileCharacter>															terrainAgents															= board.Tiles.Entities.Agents;
	bool																						bArrived																= true;
	if( initialPosition == finalPosition ) {	// If we didn't move yet is because we're stuck.
		bArrived																				= false;
		// The following noise gives epilepsy to the agents when they get stuck against a wall.
		int32_t																						tryCount																= 0
			,																						maxTry																	= 9
			;
		do {
			if(::rand() % 2)
				finalPosition.x																			+= rand() % 3 - 1;
			else
				finalPosition.z																			+= rand() % 3 - 1;
		}
		while( (maxTry > ++tryCount)
			&& (finalPosition.x < 0 || finalPosition.x >= (int32_t)terrainAgents.metrics().x)
			|| (finalPosition.z < 0 || finalPosition.z >= (int32_t)terrainAgents.metrics().y)
		);
	}
	else {
		// Check if there's money here and pick it up.
		CCharacter																					& playerAgent															= *player.Tactical.Army[player.Tactical.Squad.Agents[agentIndex]];
		playerAgent.Points.Coins																+= board.Tiles.Entities.Coins[finalPosition.z][finalPosition.x] >> 1;
		player.Tactical.Money																	+= board.Tiles.Entities.Coins[finalPosition.z][finalPosition.x] >> 1;
		board.Tiles.Entities.Coins[finalPosition.z][finalPosition.x]							= 0;
		for(uint32_t iAOE = 0, countAOE = board.AreaOfEffect.AOE.Slots.size(); iAOE < countAOE; ++iAOE) {
			const SAOE																					& aoeInstance															= board.AreaOfEffect.AOE[iAOE].Entity;
			const ::gpk::SCoord3<int32_t>																aoeCell																	= aoeInstance.Position.Cell;
			::gpk::SCoord3<float>																		aoePos																	= aoeCell.Cast<float>();
			aoePos.x																				+= aoeInstance.Position.Offset.x;
			aoePos.y																				+= aoeInstance.Position.Offset.y;
			aoePos.z																				+= aoeInstance.Position.Offset.z;
			::gpk::SCoord3<float>																		currentTilePos															= playerAgent.Position.Cast<float>();
			if((aoePos-currentTilePos).Length() <= aoeInstance.RadiusOrHalfSize && aoeInstance.StatusInflict)
				::applyAttackStatus(instanceGame.EntityTables, instanceGame.Messages, playerAgent, aoeInstance.StatusInflict, aoeInstance.Level, ::gpk::view_const_string{"Area of effect"});
		}
	}

	if( terrainAgents[initialPosition.z][initialPosition.x].AgentIndex  == agentIndex
	 && terrainAgents[initialPosition.z][initialPosition.x].PlayerIndex == playerIndex )
		terrainAgents[initialPosition.z][initialPosition.x]										= {teamId, -1, -1, -1};

	if( terrainAgents[finalPosition.z][finalPosition.x].AgentIndex  == -1
	 && terrainAgents[finalPosition.z][finalPosition.x].PlayerIndex == -1 )
		terrainAgents[finalPosition.z][finalPosition.x]											= {teamId, playerIndex, (int8_t)player.Tactical.Selection.PlayerSquad, (int8_t)agentIndex};

	if(finalPosition != initialPosition) {
		agentPosition_																			= finalPosition;
		//CCharacter																					& agent																	= *player.Army[player.Squad.Agents[agentIndex]];
		::recalculateAgentsInRangeAndSight(instanceGame);
	}

	return (bArrived && finalPosition == player.Tactical.Squad.TargetPositions[agentIndex]);
}

bool																					klib::isTacticalValid													(SGame& instanceGame)																	{
	STacticalInfo																				& tacticalInfo															= instanceGame.TacticalInfo;
	bool																						bResult																	= false;
	for(uint32_t iPlayer=0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer<playerCount; ++iPlayer ) {
		if(tacticalInfo.Setup.Players[iPlayer] == -1)
			continue;

		const SPlayer& currentPlayer = instanceGame.Players[tacticalInfo.Setup.Players[iPlayer]];

		// Only team AI is considered for the victory as oppossed to other AI types like the ones for civilian characters .
		if(currentPlayer.Tactical.Control.Type == PLAYER_CONTROL_AI && currentPlayer.Tactical.Control.AIMode != PLAYER_AI_TEAMERS)
			continue;

		if(!currentPlayer.IsAlive())
			continue;

		// Compare this player against the others to make sure there are no other players of an opposite team.
		for(uint32_t iPlayerOther=iPlayer+1; iPlayerOther < playerCount; ++iPlayerOther) {
			if(tacticalInfo.Setup.Players[iPlayerOther] == -1)
				continue;

			const SPlayer& playerOther = instanceGame.Players[tacticalInfo.Setup.Players[iPlayerOther]];

			if(playerOther.Tactical.Control.Type == PLAYER_CONTROL_AI && playerOther.Tactical.Control.AIMode != PLAYER_AI_TEAMERS)	// Only team AI is considered for the victory
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

bool																					fixAgentSelection														(SPlayer&	currentPlayer)																{
	if(currentPlayer.Tactical.Selection.PlayerUnit == -1 || currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit] == -1)
		return currentPlayer.SelectNextAgent();
	else {
		const CCharacter& agent = *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]];
		if(agent.DidLoseTurn() || !agent.IsAlive())
			return currentPlayer.SelectNextAgent();
	}

	return true;
}

uint32_t																				resolveNextPlayer														(SGame& instanceGame)																	{
	STacticalInfo																				& tacticalInfo															= instanceGame.TacticalInfo;
	STacticalSetup																				& tacticalSetup															= tacticalInfo.Setup;
	int32_t																						currentPlayerSlot														= -1;

	uint32_t																					playerCountToCheck														= 0;

	bool																						bNeedSkipCurrentPlayer													;
	bool																						bCantMove																;

	do {
		++tacticalInfo.CurrentTeam;
		if(tacticalInfo.CurrentTeam >= (int32_t)tacticalSetup.TotalTeams)
			tacticalInfo.CurrentTeam																= 0;

		int32_t																							totalPlayersForThisTeam													= 0;
		do {
			++tacticalInfo.CurrentPlayerPerTeam	[tacticalInfo.CurrentTeam];
			if(tacticalInfo.CurrentPlayerPerTeam	[tacticalInfo.CurrentTeam] >= (int32_t)tacticalSetup.PlayerCountPerTeam[tacticalInfo.CurrentTeam])
				tacticalInfo.CurrentPlayerPerTeam	[tacticalInfo.CurrentTeam]								= 0;

			currentPlayerSlot																			= tacticalSetup.PlayersPerTeam[tacticalInfo.CurrentTeam][tacticalInfo.CurrentPlayerPerTeam[tacticalInfo.CurrentTeam]];
			SPlayer																							& player																= instanceGame.Players[tacticalSetup.Players[currentPlayerSlot]];
			bCantMove																					= false == player.CanMove();
			if(bCantMove) {
				for(uint32_t iAgent=0, agentCount = tacticalSetup.SquadSize[currentPlayerSlot]; iAgent<agentCount; ++iAgent) {
					if(player.Tactical.Squad.Agents[iAgent] == -1)
						continue;
					CCharacter																						& agent																	= *player.Tactical.Army[player.Tactical.Squad.Agents[iAgent]];
					if(!agent.IsAlive())
						continue;
					::klib::applyRoundStatusAndBonusesAndSkipRound(instanceGame.EntityTables, instanceGame.Messages, agent);
					if(!agent.IsAlive())
						::handleAgentDeath(instanceGame, agent, tacticalInfo.Setup.TeamPerPlayer[currentPlayerSlot]);
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
	while(::isTacticalValid(instanceGame) && bNeedSkipCurrentPlayer && (playerCountToCheck) < (tacticalSetup.TotalPlayers));

	if(bNeedSkipCurrentPlayer)
		info_printf("Why skip? - bCantMove ? %s", bCantMove ? "true" : "false");
	return currentPlayerSlot;
}

// Ending the turn resets action and movement counters and executes minimal AI for selecting another unit.
// This function changes the value of STacticalInfo::CurrentPlayer.
void																					klib::endTurn															(SGame& instanceGame)																	{
	STacticalInfo																				& tacticalInfo															= instanceGame.TacticalInfo;
	for(uint32_t iPlayer=0, playerCount=tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
		if(-1 == tacticalInfo.Setup.Players[iPlayer])
			continue;

		SPlayer																					& playerToClear															= instanceGame.Players[tacticalInfo.Setup.Players[iPlayer]];
		if(!playerToClear.IsAlive())
			continue;

		playerToClear.Tactical.Selection.TargetPlayer												= -1;
		playerToClear.Tactical.Selection.TargetSquad													= -1;
		playerToClear.Tactical.Selection.TargetUnit													= -1;

		for(uint32_t iAgent=0, agentCount = (uint32_t)tacticalInfo.Setup.SquadSize[iPlayer]; iAgent<agentCount; iAgent++) {
			if(-1 == playerToClear.Tactical.Squad.Agents[iAgent])
				continue;

			CCharacter																				& character																= *playerToClear.Tactical.Army[playerToClear.Tactical.Squad.Agents[iAgent]];
			if(0 >= character.Points.LifeCurrent.Health)
				continue;

			//character.Recalculate();
			const SEntityPoints																		& agentFinalPoints														= character.FinalPoints;
			playerToClear.Tactical.Squad.ActionsLeft[iAgent].Moves										= (int8_t)agentFinalPoints.Fitness.Movement;
			playerToClear.Tactical.Squad.ActionsLeft[iAgent].Actions										= 1;
			playerToClear.Tactical.Squad.TargetPositions[iAgent]											= character.Position;

			::gpk::bit_clear(playerToClear.Tactical.Squad.AgentStates[iAgent], AGENT_STATE_MOVE);

			::klib::applyTurnStatusAndBonusesAndSkipTurn(instanceGame.EntityTables, instanceGame.Messages, character);
			++character.Score.TurnsPlayed;

			if(!character.IsAlive())
				::handleAgentDeath(instanceGame, character, tacticalInfo.Setup.TeamPerPlayer[iPlayer]);
			else if(((int8_t)iPlayer) == tacticalInfo.CurrentPlayer) {
				::klib::applyRoundStatusAndBonusesAndSkipRound(instanceGame.EntityTables, instanceGame.Messages, character);
				if(!character.IsAlive())
					::handleAgentDeath(instanceGame, character, tacticalInfo.Setup.TeamPerPlayer[iPlayer]);
			}
			else if(tacticalInfo.Setup.TeamPerPlayer[iPlayer] != tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer] ) {
				::klib::applyEnemyTurnStatusAndBonusesAndSkipTurn(instanceGame.EntityTables, instanceGame.Messages, character);
				if(!character.IsAlive())
					::handleAgentDeath(instanceGame, character, tacticalInfo.Setup.TeamPerPlayer[iPlayer]);
			}
		}

		// Change character selections at the end of the turn because selected agents may have died in between turns.
		if(playerToClear.Tactical.Control.Type == PLAYER_CONTROL_AI) {
			while(::rand() % tacticalInfo.Setup.SquadSize[iPlayer])
				playerToClear.SelectNextAgent();
		}
		::fixAgentSelection(playerToClear);	// Make sure a valid agent is selected.
		playerToClear.Tactical.Squad.LockedAgent															= -1;
	}

	uint32_t																					iAOE																	= 0;
	while(iAOE < tacticalInfo.Board.AreaOfEffect.AOE.Slots.size()) {
		SAOE																						& aoeInstance															= tacticalInfo.Board.AreaOfEffect.AOE[iAOE].Entity;
		if(0 == --aoeInstance.TurnsLeft) {
			tacticalInfo.Board.AreaOfEffect.Coords.DecreaseEntity(tacticalInfo.Board.AreaOfEffect.Coords.FindElement(aoeInstance.Position.Cell));
			tacticalInfo.Board.AreaOfEffect.AOE.DecreaseEntity(iAOE);
		}
		else
			++iAOE;
	}

	tacticalInfo.CurrentPlayer																= (int8_t)resolveNextPlayer(instanceGame);	// Change current player.

	static const HANDLE																			hConsoleOut																= GetStdHandle( STD_OUTPUT_HANDLE );
	COORD																						cursorPos																= {0, ((SHORT)instanceGame.GlobalDisplay.Screen.metrics().y >> 1) + 10};
	::SetConsoleCursorPosition( hConsoleOut, cursorPos );
}

void																					selectAIDestination														(SGame& instanceGame);
bool																					klib::updateCurrentPlayer												(SGame& instanceGame)																	{
	STacticalInfo																				& tacticalInfo															= instanceGame.TacticalInfo;
	SPlayer																						& currentPlayer															= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];	// the current player is only valid in this scope. After this code the current player can change

	if( !::fixAgentSelection(currentPlayer) )
		return false;

	if( currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]]->Points.LifeCurrent.Health <= 0 )
		return false;

	::gpk::SCoord3<int32_t>																		& currentAgentPosition													= currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.PlayerUnit]]->Position;

	bool																						bHasArrived																= true;
	if( currentPlayer.Tactical.Squad.TargetPositions[currentPlayer.Tactical.Selection.PlayerUnit] != currentAgentPosition && (0 < currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves) ) {
		bHasArrived																				= ::moveStep(instanceGame, currentPlayer, tacticalInfo.CurrentPlayer, currentPlayer.Tactical.Selection.PlayerUnit, tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer], tacticalInfo.Board, currentAgentPosition);
		if(bHasArrived)
			::gpk::bit_clear(currentPlayer.Tactical.Squad.AgentStates[currentPlayer.Tactical.Selection.PlayerUnit], AGENT_STATE_MOVE);
	}

	if(currentPlayer.Tactical.Control.Type != PLAYER_CONTROL_AI)
		return true;
	if( 0 == currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves
	 && 0 == currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Actions
	 )
		return false;
	if( bHasArrived && (0 < currentPlayer.Tactical.Squad.ActionsLeft[currentPlayer.Tactical.Selection.PlayerUnit].Moves) ) {
		::selectAIDestination(instanceGame);
		::gpk::bit_set(currentPlayer.Tactical.Squad.AgentStates[currentPlayer.Tactical.Selection.PlayerUnit], AGENT_STATE_MOVE);
	}
	return true;
}

template<typename _TEntity>
void																					pickupEntities
	(	SEntityContainer<_TEntity>	& playerEntities
	,	SEntityContainer<_TEntity>	& mapEntities
	,	int32_t						maxCount				= 4096
	)
{
	int32_t																						currentCount															= 0;
	while(mapEntities.Slots.size() && currentCount < maxCount) {
		mapEntities[0].Entity.Owner																= -1;
		playerEntities.AddElement(mapEntities[0].Entity);
		mapEntities.DecreaseEntity(0);
		++currentCount;
	}
}

void																					distributeDropsForVictoriousTeam										(SGame& instanceGame, TEAM_TYPE teamVictorious)											{
	STacticalInfo																				& tacticalInfo															= instanceGame.TacticalInfo;
	uint32_t																					totalWinners															= 0;
	int32_t																						indexWinners[MAX_TACTICAL_PLAYERS]										= {};
	::memset(&indexWinners[0], -1, sizeof(PLAYER_INDEX) * ::gpk::size(indexWinners));

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
		instanceGame.Messages.UserMiss															= ::gpk::view_const_string{"No winners?"};
		instanceGame.LogMiss();
	}
	else if(totalWinners == 1) {
		SPlayer																						& winnerPlayer															= instanceGame.Players[tacticalInfo.Setup.Players[indexWinners[0]]];
		pickupEntities(winnerPlayer.Inventory.Profession, tacticalInfo.Drops.Profession	);
		pickupEntities(winnerPlayer.Inventory.Accessory	, tacticalInfo.Drops.Accessory	);
		pickupEntities(winnerPlayer.Inventory.Armor		, tacticalInfo.Drops.Armor		);
		pickupEntities(winnerPlayer.Inventory.Weapon	, tacticalInfo.Drops.Weapon		);
		pickupEntities(winnerPlayer.Inventory.Vehicle	, tacticalInfo.Drops.Vehicle	);
		pickupEntities(winnerPlayer.Inventory.Facility	, tacticalInfo.Drops.Facility	);
		pickupEntities(winnerPlayer.Inventory.StageProp	, tacticalInfo.Drops.StageProp	);
		pickupEntities(winnerPlayer.Inventory.Items		, tacticalInfo.Drops.Items		);
		for(uint32_t iAgent = 0; iAgent < winnerPlayer.Tactical.Squad.Agents.size(); ++iAgent) {
			int16_t currentAgent = winnerPlayer.Tactical.Squad.Agents[iAgent];
			if(-1 != currentAgent) {
				winnerPlayer.Tactical.Army[currentAgent]->Recalculate(instanceGame.EntityTables);
				int64_t																					agentReward				= 0;
				if(winnerPlayer.Tactical.Army[currentAgent]->FinalPoints.LifeCurrent.Health > 0)
					agentReward																				= int64_t(winnerPlayer.Tactical.Army[currentAgent]->Points.Coins / 4.0 * 3);
				else
					agentReward																				= winnerPlayer.Tactical.Army[currentAgent]->Points.Coins;
				if(agentReward) {
					winnerPlayer.Tactical.Money																		+= agentReward;
					winnerPlayer.Tactical.Score.MoneyEarned															+= agentReward;
					winnerPlayer.Tactical.Army[currentAgent]->Points.Coins											-= agentReward;
					sprintf_s(instanceGame.Messages.Aux, "Money from agent %i: %llu.", iAgent, agentReward);
					instanceGame.Messages.LogAuxSuccess();
				}
			}
		}
		winnerPlayer.Tactical.Money																+= totalMapMoney;
		winnerPlayer.Tactical.Score.MoneyEarned													+= totalMapMoney;
		winnerPlayer.Tactical.Score.BattlesWon													+= 1;

		sprintf_s(instanceGame.Messages.Aux, "Money from map: %lli.", totalMapMoney);
		instanceGame.Messages.LogAuxSuccess();
	}
	else {
		int32_t																						totalProfession															= 0;
		int32_t																						totalAccessory															= 0;
		int32_t																						totalArmor																= 0;
		int32_t																						totalWeapon																= 0;
		int32_t																						totalVehicle															= 0;
		int32_t																						totalFacility															= 0;
		int32_t																						totalStageProp															= 0;
		int32_t																						totalItems																= 0;
		for(uint32_t i = 0; i < tacticalInfo.Drops.Profession	.Slots.size(); ++i) totalProfession	+= tacticalInfo.Drops.Profession	[i].Count;
		for(uint32_t i = 0; i < tacticalInfo.Drops.Accessory	.Slots.size(); ++i) totalAccessory	+= tacticalInfo.Drops.Accessory		[i].Count;
		for(uint32_t i = 0; i < tacticalInfo.Drops.Armor		.Slots.size(); ++i) totalArmor		+= tacticalInfo.Drops.Armor			[i].Count;
		for(uint32_t i = 0; i < tacticalInfo.Drops.Weapon		.Slots.size(); ++i) totalWeapon		+= tacticalInfo.Drops.Weapon		[i].Count;
		for(uint32_t i = 0; i < tacticalInfo.Drops.Vehicle		.Slots.size(); ++i) totalVehicle	+= tacticalInfo.Drops.Vehicle		[i].Count;
		for(uint32_t i = 0; i < tacticalInfo.Drops.Facility		.Slots.size(); ++i) totalFacility	+= tacticalInfo.Drops.Facility		[i].Count;
		for(uint32_t i = 0; i < tacticalInfo.Drops.StageProp	.Slots.size(); ++i) totalStageProp	+= tacticalInfo.Drops.StageProp		[i].Count;
		for(uint32_t i = 0; i < tacticalInfo.Drops.Items		.Slots.size(); ++i) totalItems		+= tacticalInfo.Drops.Items			[i].Count;

		for(uint32_t iWinner = 0; iWinner < totalWinners; ++iWinner) {
			SPlayer																						& winnerPlayer															= instanceGame.Players[tacticalInfo.Setup.Players[indexWinners[iWinner]]];
			pickupEntities(winnerPlayer.Inventory.Profession, tacticalInfo.Drops.Profession	, totalProfession	/ totalWinners);
			pickupEntities(winnerPlayer.Inventory.Accessory	, tacticalInfo.Drops.Accessory	, totalAccessory	/ totalWinners);
			pickupEntities(winnerPlayer.Inventory.Armor		, tacticalInfo.Drops.Armor		, totalArmor		/ totalWinners);
			pickupEntities(winnerPlayer.Inventory.Weapon	, tacticalInfo.Drops.Weapon		, totalWeapon		/ totalWinners);
			pickupEntities(winnerPlayer.Inventory.Vehicle	, tacticalInfo.Drops.Vehicle	, totalVehicle		/ totalWinners);
			pickupEntities(winnerPlayer.Inventory.Facility	, tacticalInfo.Drops.Facility	, totalFacility		/ totalWinners);
			pickupEntities(winnerPlayer.Inventory.StageProp	, tacticalInfo.Drops.StageProp	, totalStageProp	/ totalWinners);
			pickupEntities(winnerPlayer.Inventory.Items		, tacticalInfo.Drops.Items		, totalItems		/ totalWinners);
			winnerPlayer.Tactical.Score.BattlesWon															+= 1;
		}

		for(uint32_t iWinner = 0; iWinner < totalWinners; ++iWinner) {
			SPlayer																						& winnerPlayer															= instanceGame.Players[tacticalInfo.Setup.Players[indexWinners[iWinner]]];
			for(uint32_t iAgent = 0, agentCount = tacticalInfo.Setup.SquadSize[indexWinners[iWinner]]; iAgent < agentCount; ++iAgent) {
				if(winnerPlayer.Tactical.Squad.Agents[iAgent] == -1)
					continue;

				if(false == winnerPlayer.Tactical.Army[winnerPlayer.Tactical.Squad.Agents[iAgent]]->IsAlive())
					continue;

				++winnerPlayer.Tactical.Army[winnerPlayer.Tactical.Squad.Agents[iAgent]]->Score.BattlesWon;
			}
		}

		const int32_t playerIndices [] =
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
		if(	tacticalInfo.Drops.Profession	.Slots.size()) pickupEntities(instanceGame.Players[playerIndices[0]].Inventory.Profession	, tacticalInfo.Drops.Profession	);
		if(	tacticalInfo.Drops.Accessory	.Slots.size()) pickupEntities(instanceGame.Players[playerIndices[1]].Inventory.Accessory	, tacticalInfo.Drops.Accessory	);
		if(	tacticalInfo.Drops.Armor		.Slots.size()) pickupEntities(instanceGame.Players[playerIndices[2]].Inventory.Armor		, tacticalInfo.Drops.Armor		);
		if(	tacticalInfo.Drops.Weapon		.Slots.size()) pickupEntities(instanceGame.Players[playerIndices[3]].Inventory.Weapon		, tacticalInfo.Drops.Weapon		);
		if(	tacticalInfo.Drops.Vehicle		.Slots.size()) pickupEntities(instanceGame.Players[playerIndices[4]].Inventory.Vehicle		, tacticalInfo.Drops.Vehicle	);
		if(	tacticalInfo.Drops.Facility		.Slots.size()) pickupEntities(instanceGame.Players[playerIndices[5]].Inventory.Facility		, tacticalInfo.Drops.Facility	);
		if(	tacticalInfo.Drops.StageProp	.Slots.size()) pickupEntities(instanceGame.Players[playerIndices[6]].Inventory.StageProp	, tacticalInfo.Drops.StageProp	);
		if(	tacticalInfo.Drops.Items		.Slots.size()) pickupEntities(instanceGame.Players[playerIndices[7]].Inventory.Items		, tacticalInfo.Drops.Items		);

		int64_t																					reward																	= totalMapMoney / totalWinners;
		for(uint32_t iWinner=0; iWinner<totalWinners; ++iWinner) {
			SPlayer																					& winnerPlayer															= instanceGame.Players[tacticalInfo.Setup.Players[indexWinners[iWinner]]];
			winnerPlayer.Tactical.Money															+= reward;
			totalMapMoney																		-= reward;
			winnerPlayer.Tactical.Score.MoneyEarned												+= reward;
			sprintf_s(instanceGame.Messages.Aux, "Money from map for player %u: %lli.", indexWinners[iWinner], reward);
			instanceGame.Messages.LogAuxSuccess();

			for(uint32_t iAgent = 0; iAgent < winnerPlayer.Tactical.Squad.Size; ++iAgent) {
				int16_t				currentAgent = winnerPlayer.Tactical.Squad.Agents[iAgent];
				if(-1 != currentAgent) {
					winnerPlayer.Tactical.Army[currentAgent]->Recalculate(instanceGame.EntityTables);
					int64_t																					agentReward				= 0;
					if(winnerPlayer.Tactical.Army[currentAgent]->FinalPoints.LifeCurrent.Health > 0)
						agentReward																			= int64_t(winnerPlayer.Tactical.Army[currentAgent]->Points.Coins / 4.0 * 3);
					else
						agentReward																			= winnerPlayer.Tactical.Army[currentAgent]->Points.Coins;
					if(agentReward) {
						winnerPlayer.Tactical.Money																		+= agentReward;
						winnerPlayer.Tactical.Score.MoneyEarned															+= agentReward;
						winnerPlayer.Tactical.Army[currentAgent]->Points.Coins											-= agentReward;
						sprintf_s(instanceGame.Messages.Aux, "Money from agent %i for player %u: %lli.", iAgent, indexWinners[iWinner], agentReward);
						instanceGame.Messages.LogAuxSuccess();
					}
				}
			}
		}
		if(totalMapMoney)
			instanceGame.Players[tacticalInfo.Setup.Players[indexWinners[::rand()%totalWinners]]].Tactical.Money	+= totalMapMoney;
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

		instanceGame.Players[tacticalInfo.Setup.Players[iPlayer]].Tactical.Score.BattlesLost			+= 1;
	}
}

void																					klib::determineOutcome													(SGame& instanceGame, bool aborted)																	{
	TEAM_TYPE																					teamVictorious															= TEAM_TYPE_SPECTATOR;
	STacticalInfo																				& tacticalInfo															= instanceGame.TacticalInfo;
	if(aborted)  {
		instanceGame.Messages.UserMessage	= ::gpk::view_const_string{"Mission aborted."};
		instanceGame.LogMessage();
		return;
	}
	for(uint32_t iPlayer = 0, playerCount = tacticalInfo.Setup.TotalPlayers; iPlayer < playerCount; ++iPlayer) {
		if(tacticalInfo.Setup.Players[iPlayer] == -1)
			continue;

		SPlayer																						& currentPlayer															= instanceGame.Players[tacticalInfo.Setup.Players[iPlayer]];

		if(currentPlayer.Tactical.Control.Type == PLAYER_CONTROL_AI && currentPlayer.Tactical.Control.AIMode != PLAYER_AI_TEAMERS)	// Only team AI is considered for the victory
			continue;

		for(uint32_t iAgent = 0, agentCount = tacticalInfo.Setup.SquadSize[iPlayer]; iAgent < agentCount; ++iAgent) {
			if(currentPlayer.Tactical.Squad.Agents[iAgent] == -1)
				continue;

			if(0 == currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[iAgent]])
				continue;

			const CCharacter																			& deadCharacter															= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[iAgent]];
			if(deadCharacter.IsAlive()) {
				CCharacter																					& liveCharacter															= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[iAgent]];
				liveCharacter.CurrentEquip.Accessory	.Level											+= 1;
				liveCharacter.CurrentEquip.Weapon		.Level											+= 1;
				liveCharacter.CurrentEquip.Armor		.Level											+= 1;
				liveCharacter.CurrentEquip.Profession	.Level											+= 1;
				//liveCharacter.CurrentEquip.Vehicle		.Level += 1;
				//liveCharacter.CurrentEquip.Facility		.Level += 1;
				//liveCharacter.CurrentEquip.StageProp	.Level += 1;
				liveCharacter.Recalculate(instanceGame.EntityTables);
				continue;
			}

			int32_t																						oldAgentIndex															= currentPlayer.Tactical.Squad.Agents[iAgent];
			currentPlayer.Tactical.Squad.Agents[iAgent]														= -1;
			currentPlayer.Memorial.push_back(deadCharacter);
			if(oldAgentIndex == (int32_t)(currentPlayer.Tactical.Army.size()-1))
				currentPlayer.Tactical.Army.pop_back(0);
			else
				currentPlayer.Tactical.Army[oldAgentIndex]														= {};
		}

		if(!currentPlayer.IsAlive())
			continue;
		teamVictorious																			= tacticalInfo.Setup.TeamPerPlayer[iPlayer];
		break;
	}

	if(teamVictorious == TEAM_TYPE_SPECTATOR) {
		instanceGame.Messages.UserMessage	= ::gpk::view_const_string{"No winners."};
		instanceGame.LogMessage();
	} else {
		::sprintf_s(instanceGame.Messages.Aux, "Team %s won the match.", ::gpk::get_value_label(teamVictorious).begin());
		if(teamVictorious == TEAM_TYPE_ALLY)
			instanceGame.Messages.LogAuxSuccess();
		else
			instanceGame.Messages.LogAuxMiss();
	}
	if(teamVictorious != TEAM_TYPE_SPECTATOR) // only give rewards if somebody won.
		::distributeDropsForVictoriousTeam(instanceGame, teamVictorious);

	instanceGame.TacticalInfo.Clear();
}

template<typename _TEntity>
	void																				dropEntities
	(	SEntityContainer<_TEntity>					& mapEntities
	,	SEntityContainer<_TEntity>					& deadTargetEntities
	,	SEntityContainer<::gpk::SCoord3<int32_t>>	& entityCoords
	,	const ::gpk::SCoord3<int32_t>&	deadTargetCoords
	)
{
	while(deadTargetEntities.Slots.size()) {
		if(deadTargetEntities[0].Entity.Definition != -1) {
			if(mapEntities	.AddElement(deadTargetEntities[0].Entity))
				entityCoords.AddElement(deadTargetCoords);
		}
		deadTargetEntities	.DecreaseEntity(0);
	}
}

void																					klib::handleAgentDeath													(SGame& instanceGame, CCharacter& deadTarget, TEAM_TYPE teamId)							{
	STacticalInfo																				& tacticalInfo															= instanceGame.TacticalInfo;
	SCharacterInventory																			& targetInventory														= deadTarget.Goods.Inventory;
	SCharacterEquip																				& targetEquip															= deadTarget.CurrentEquip;
	SMapInventory																				& mapDrops																= tacticalInfo.Drops;

	dropEntities(mapDrops.	Profession	, targetInventory.	Profession	, mapDrops.	CoordsProfession	, deadTarget.Position);
	dropEntities(mapDrops.	Accessory	, targetInventory.	Accessory	, mapDrops.	CoordsAccessory		, deadTarget.Position);
	dropEntities(mapDrops.	Armor		, targetInventory.	Armor		, mapDrops.	CoordsArmor			, deadTarget.Position);
	dropEntities(mapDrops.	Weapon		, targetInventory.	Weapon		, mapDrops.	CoordsWeapon		, deadTarget.Position);
	dropEntities(mapDrops.	Vehicle		, targetInventory.	Vehicle		, mapDrops.	CoordsVehicle		, deadTarget.Position);
	dropEntities(mapDrops.	Facility	, targetInventory.	Facility	, mapDrops.	CoordsFacility		, deadTarget.Position);
	dropEntities(mapDrops.	StageProp	, targetInventory.	StageProp	, mapDrops.	CoordsStageProp		, deadTarget.Position);
	dropEntities(mapDrops.	Items		, targetInventory.	Items		, mapDrops.	CoordsItems			, deadTarget.Position);

	targetEquip.Profession	.Owner = -1; mapDrops.	Profession	.AddElement(targetEquip.	Profession	);	targetEquip.	Profession	=	SProfession	();	mapDrops.	CoordsProfession	.AddElement(deadTarget.Position);
	targetEquip.Accessory	.Owner = -1; mapDrops.	Accessory	.AddElement(targetEquip.	Accessory	);	targetEquip.	Accessory	=	SAccessory	();	mapDrops.	CoordsAccessory		.AddElement(deadTarget.Position);
	targetEquip.Armor		.Owner = -1; mapDrops.	Armor		.AddElement(targetEquip.	Armor		);	targetEquip.	Armor		=	SArmor		();	mapDrops.	CoordsArmor			.AddElement(deadTarget.Position);
	targetEquip.Weapon		.Owner = -1; mapDrops.	Weapon		.AddElement(targetEquip.	Weapon		);	targetEquip.	Weapon		=	SWeapon		();	mapDrops.	CoordsWeapon		.AddElement(deadTarget.Position);
	targetEquip.Vehicle		.Owner = -1; mapDrops.	Vehicle		.AddElement(targetEquip.	Vehicle		);	targetEquip.	Vehicle		=	SVehicle	();	mapDrops.	CoordsVehicle		.AddElement(deadTarget.Position);
	targetEquip.Facility	.Owner = -1; mapDrops.	Facility	.AddElement(targetEquip.	Facility	);	targetEquip.	Facility	=	SFacility	();	mapDrops.	CoordsFacility		.AddElement(deadTarget.Position);
	targetEquip.StageProp	.Owner = -1; mapDrops.	StageProp	.AddElement(targetEquip.	StageProp	);	targetEquip.	StageProp	=	SStageProp	();	mapDrops.	CoordsStageProp		.AddElement(deadTarget.Position);
	//targetEquip.Items		.Owner = -1; mapDrops.	Items		.AddElement(targetEquip.	Items		);	targetEquip.	Items		=	SItems		();	mapDrops.	CoordsItems			.AddElement(deadTarget.Position);

	tacticalInfo.Board.Tiles.Entities.Coins[deadTarget.Position.z][deadTarget.Position.x] += deadTarget.Points.Coins;
	deadTarget.Points.Coins																	= 0;

	::gpk::array_pod<char_t>																	ripText																	= deadTarget.Name;
	ripText.append_string(" has died.");
	if(teamId == tacticalInfo.Setup.TeamPerPlayer[PLAYER_INDEX_USER]) {
		instanceGame.Messages.UserMiss																	= ripText;
		instanceGame.LogMiss();
	}
	else {
		instanceGame.Messages.UserSuccess																= ripText;
		instanceGame.LogSuccess();
	}
	::recalculateAgentsInRangeAndSight(instanceGame);
}

void																					klib::handleAgentDeath													(SGame& instanceGame, CCharacter& deadTarget, CCharacter& attacker, TEAM_TYPE teamId)	{
	::handleAgentDeath(instanceGame, deadTarget, teamId);
	++attacker.Score.EnemiesKilled;
}
