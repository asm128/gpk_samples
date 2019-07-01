#include "tactical_gameplay.h"
#include "tactical_draw.h"
#include "draw.h"
#include "Combat.h"
#include "Agent_helper.h"
#include "Item.h"

#include <time.h>

#if defined (GPK_WINDOWS)
#include <Mmsystem.h>	// currently we use this for WinAPI's PlaySound().
#endif

using namespace klib;

bool																	handleUserInput									(SGame& instanceGame, const SGameState& /*returnState*/)								{
	static ::klib::SAccumulator<double>											keyAccum										= {0.0, 0.6};

	STacticalInfo																& tacticalInfo									= instanceGame.TacticalInfo;
	//SPlayer																	& playerUser									= instanceGame.Players[PLAYER_INDEX_USER];
	SPlayer																		& currentPlayer									= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];
	SPlayerSelection															& playerSelection								= currentPlayer.Selection;
	const bool																	bDoneWaiting									= keyAccum.Accumulate(instanceGame.FrameTimer.LastTimeSeconds) > 0.0;
	if(instanceGame.FrameInput.Keys[VK_TAB] && bDoneWaiting) {
		//if(playerSelection.PlayerUnit != -1 && currentPlayer.Squad.Agents[playerSelection.PlayerUnit] != -1) {
		//	CCharacter																	& currentAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[playerSelection.PlayerUnit]];
		//	::gpk::SCoord3<int32_t>													& currentAgentPosition							= currentAgent.Position;
		//}
		if(instanceGame.FrameInput.Keys[VK_SHIFT]) {
			if(!currentPlayer.SelectPreviousAgent()) {
				keyAccum.Value															= 0.0;
				return true;
			}
		}
		else if(!currentPlayer.SelectNextAgent()) {
			keyAccum.Value															= 0.0;
			return true;
		}
		if(playerSelection.TargetPlayer != -1 && playerSelection.TargetUnit != -1) {
			SPlayer																		& playerTarget									= instanceGame.Players[tacticalInfo.Setup.Players[playerSelection.TargetPlayer]];
			if(playerTarget.Squad.Agents[playerSelection.TargetUnit] != -1)
				currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]			= playerTarget.Army[playerTarget.Squad.Agents[playerSelection.TargetUnit]]->Position;
			else
				playerSelection.ClearTarget();
		}
		keyAccum.Value															= 0.0;
	}
	else {
		int32_t																		mouseX											= instanceGame.FrameInput.Mouse.Deltas.x;
		int32_t																		mouseY											= instanceGame.FrameInput.Mouse.Deltas.y;

		::klib::SGlobalDisplay														& globalDisplay									= instanceGame.GlobalDisplay;
		::klib::STacticalDisplay													& tacticalDisplay								= instanceGame.TacticalDisplay;
		int32_t																		tacticalDisplayX								= (globalDisplay.Width>>1)	- (tacticalDisplay.Width>>1);
		//int32_t																		tacticalDisplayStop								= TACTICAL_DISPLAY_POSY		+ (tacticalDisplay.Depth);
		int32_t																		tacticalMouseX									= mouseX-tacticalDisplayX;
		int32_t																		tacticalMouseY									= mouseY-TACTICAL_DISPLAY_POSY;

		bool																		 bInArea										=  tacticalMouseX >= 0 && tacticalMouseX < STacticalBoard::Width
																																	&& tacticalMouseY >= 0 && tacticalMouseY < STacticalBoard::Depth
			;
		if(false == bInArea)
			return false;

		if(tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer] != PLAYER_INDEX_USER)
			return false;

		STacticalBoard																& tacticalBoard									= tacticalInfo.Board;

		if(playerSelection.PlayerUnit != -1 && currentPlayer.Squad.Agents[playerSelection.PlayerUnit] != -1) {
			CCharacter																	& currentAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[playerSelection.PlayerUnit]];
			const ::gpk::SCoord3<int32_t>												& currentAgentPosition							= currentAgent.Position;
			int32_t																		targetPlayerIndex								= tacticalBoard.Tiles.Entities.Agents[tacticalMouseY][tacticalMouseX].PlayerIndex;
			int32_t																		agentIndex										= tacticalBoard.Tiles.Entities.Agents[tacticalMouseY][tacticalMouseX].AgentIndex;
			if(0 != instanceGame.FrameInput.Mouse.Buttons[0]) {// || 0 != instanceGame.FrameInput.MouseButtons[4]) 
				currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]			= {tacticalMouseX, currentAgentPosition.y, tacticalMouseY};
				if(	targetPlayerIndex != -1 && agentIndex != -1 ) {
					if(targetPlayerIndex == tacticalInfo.CurrentPlayer && currentPlayer.Army[currentPlayer.Squad.Agents[agentIndex]]->IsAlive() && false == currentPlayer.Army[currentPlayer.Squad.Agents[agentIndex]]->DidLoseTurn() && (0 == instanceGame.FrameInput.Keys[VK_CONTROL] && 0 == instanceGame.FrameInput.Keys[VK_LCONTROL])) {
						playerSelection.PlayerUnit												= (int16_t)agentIndex;
						playerSelection.PlayerSquad												= 0;
						if(playerSelection.TargetPlayer != -1 && playerSelection.TargetUnit != -1) {
							SPlayer																		& playerTarget										= instanceGame.Players[tacticalInfo.Setup.Players[playerSelection.TargetPlayer]];
							if(playerTarget.Squad.Agents[playerSelection.TargetUnit] != -1)
								currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]			= playerTarget.Army[playerTarget.Squad.Agents[playerSelection.TargetUnit]]->Position;
							else
								playerSelection.ClearTarget();
						}
					}
					else {
						playerSelection.TargetPlayer											= (int16_t)targetPlayerIndex;
						playerSelection.TargetSquad												= 0;
						playerSelection.TargetUnit												= (int16_t)agentIndex;
						currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]			= {tacticalMouseX, currentAgentPosition.y, tacticalMouseY};
					}
				}
				else {
					playerSelection.ClearTarget();
					currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]			= {tacticalMouseX, currentAgentPosition.y, tacticalMouseY};
				}
			}
		}
	}

	return false;
};

void																	drawTileInfo
	(	const SGame						& instanceGame
	,	const ::klib::SInput			& frameInput
	,	const STacticalInfo				& tacticalInfo
	,	SGlobalDisplay					& globalDisplay	
	,	STacticalDisplay				& tacticalDisplay	
	)
{
	static	std::string															selectedTile									= "";
	static	char																messageSlow		[256]							= {'_',};
	static	uint16_t															messageColor									= COLOR_DARKGREEN;
	
	int32_t																		tacticalDisplayStop								= TACTICAL_DISPLAY_POSY + (tacticalDisplay.Depth);

	bool																		bDrawText										= false;
	int32_t																		mouseX											= frameInput.Mouse.Deltas.x;
	int32_t																		mouseY											= frameInput.Mouse.Deltas.y;
	int32_t																		tacticalDisplayX								= (globalDisplay.Width>>1)	- (tacticalDisplay.Width>>1);
	int32_t																		boardX											= mouseX-tacticalDisplayX;
	int32_t																		boardZ											= mouseY-TACTICAL_DISPLAY_POSY;

	bool																		bInTacticalMap									= false;
	if( mouseX >= tacticalDisplayX && mouseX < (int32_t)(tacticalDisplayX+tacticalDisplay.Width)
		 && mouseY >= TACTICAL_DISPLAY_POSY && mouseY < tacticalDisplayStop
		)
	{
		bInTacticalMap															= true;
		int32_t																		playerIndex										= tacticalInfo.Board.Tiles.Entities.Agents[boardZ][boardX].PlayerIndex;
		int32_t																		agentIndex										= tacticalInfo.Board.Tiles.Entities.Agents[boardZ][boardX].AgentIndex;
		int32_t																		terrainHeight									= tacticalInfo.Board.Tiles.Terrain.Topology[boardZ][boardX].Sharp+tacticalInfo.Board.Tiles.Terrain.Topology[boardZ][boardX].Smooth;

		if(playerIndex	!= -1 && agentIndex != -1 && tacticalInfo.Setup.Players[playerIndex] != -1) {
			const SPlayer																& boardPlayer									= instanceGame.Players[tacticalInfo.Setup.Players[playerIndex]];
			bool																		bDarken											= !	( boardPlayer.Control.Type == PLAYER_CONTROL_AI
																																		 && ( (boardPlayer.Control.AIMode == PLAYER_AI_ASSISTS)
																																		   || (boardPlayer.Control.AIMode == PLAYER_AI_RIOTERS)
																																		   || (boardPlayer.Control.AIMode == PLAYER_AI_VIOLENT)
																																			)
																																		);
			messageColor															= getPlayerColor(tacticalInfo, boardPlayer, (int8_t)playerIndex, PLAYER_INDEX_USER, bDarken);

			selectedTile															= boardPlayer.Army[boardPlayer.Squad.Agents[agentIndex]]->Name;
			bDrawText																= true;
		}
		else if(tacticalInfo.Board.Tiles.Entities.Coins[boardZ][boardX]) {
			selectedTile															= "Coins: " + std::to_string(tacticalInfo.Board.Tiles.Entities.Coins[boardZ][boardX]);
			messageColor															= COLOR_ORANGE;
			bDrawText																= true;
		}
		else if(tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Definition != -1) {
			::std::string																stagePropName									= getStagePropName(
				{	tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Definition			
				,	tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Modifier
				,	tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Level
				,	-1
				});
			selectedTile															= stagePropName;
			if(tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Level == -1)
				selectedTile															+= " (Destroyed)";
			messageColor															= COLOR_DARKGREY;
			bDrawText																= true;
		}
		else if ( terrainHeight 
			||	0 != tacticalInfo.Board.Tiles.Terrain.Geometry.Cells[boardZ][boardX].fHeight[0] 
			||	0 != tacticalInfo.Board.Tiles.Terrain.Geometry.Cells[boardZ][boardX].fHeight[1]
			||	0 != tacticalInfo.Board.Tiles.Terrain.Geometry.Cells[boardZ][boardX].fHeight[2]
			||	0 != tacticalInfo.Board.Tiles.Terrain.Geometry.Cells[boardZ][boardX].fHeight[3]
			)
		{
			messageColor															= COLOR_DARKGREY;
			char																		heightStr[128]									= {};
			sprintf_s(heightStr, "Tile heights: {%f,%f,%f,%f}"
				, tacticalInfo.Board.Tiles.Terrain.Geometry.Cells[boardZ][boardX].fHeight[0]
				, tacticalInfo.Board.Tiles.Terrain.Geometry.Cells[boardZ][boardX].fHeight[1]
				, tacticalInfo.Board.Tiles.Terrain.Geometry.Cells[boardZ][boardX].fHeight[2]
				, tacticalInfo.Board.Tiles.Terrain.Geometry.Cells[boardZ][boardX].fHeight[3]
			);
			selectedTile															= heightStr;
			bDrawText																= true;
			//if(terrainHeight >= FULL_COVER_HEIGHT) {
			//	selectedTile															= "Full cover terrain height: " + std::to_string(terrainHeight);
			//	bDrawText																= true;
			//}
			//else if(terrainHeight >= PARTIAL_COVER_HEIGHT) {
			//	selectedTile															= "Partial cover terrain height: " + std::to_string(terrainHeight);
			//	bDrawText																= true;
			//}
			//else if(terrainHeight > 0) {
			//	selectedTile															= "Non-blocking terrain height: " + std::to_string(terrainHeight);
			//	bDrawText																= true;
			//}
			//if(terrainHeight < -FULL_COVER_HEIGHT) {
			//	selectedTile															= "Water: " + std::to_string(terrainHeight);
			//	bDrawText																= true;
			//}
			//else if(terrainHeight <= -PARTIAL_COVER_HEIGHT) {
			//	selectedTile															= "Terrain depression: " + std::to_string(terrainHeight);
			//	bDrawText																= true;
			//}
			//else if(terrainHeight < 0) {
			//	selectedTile															= "Lower terrain: " + std::to_string(terrainHeight);
			//	bDrawText																= true;
			//}
		}
		::klib::getMessageSlow(messageSlow, selectedTile.c_str(), (uint32_t)selectedTile.size(), instanceGame.FrameTimer.LastTimeSeconds*4);
	}

	if(bDrawText)
		::klib::lineToGridColored(globalDisplay.Screen, globalDisplay.TextAttributes, messageColor, tacticalDisplayStop+3, 0, ::klib::SCREEN_CENTER, messageSlow);

	printfToGridColored(globalDisplay.Screen, globalDisplay.TextAttributes, bInTacticalMap ? messageColor : COLOR_DARKGREY, tacticalDisplayStop+3, tacticalDisplayX	+1, ::klib::SCREEN_LEFT, "%i, %i", mouseX-tacticalDisplayX, mouseY-TACTICAL_DISPLAY_POSY);

}

void																	drawPlayerInfo									(SGame& instanceGame)																{
	static bool																	bSwap											= false;
	static ::klib::STimer														animationTimer;
	static ::klib::SAccumulator<double>											animationAccum									= {0.0, 0.1};

	animationTimer.Frame();
	if( animationAccum.Accumulate(animationTimer.LastTimeSeconds) ) {
		bSwap																	= !bSwap;
		animationAccum.Value													= 0;
	}
	SGlobalDisplay																& globalDisplay									= instanceGame.GlobalDisplay;
	STacticalDisplay															& tacticalDisplay								= instanceGame.TacticalDisplay;
	const int32_t																tacticalDisplayStop								= TACTICAL_DISPLAY_POSY		+ (tacticalDisplay.Depth);
	const int32_t																tacticalDisplayX								= (globalDisplay.Width>>1)	- (tacticalDisplay.Width>>1);
	const STacticalInfo															& tacticalInfo									= instanceGame.TacticalInfo;

	const PLAYER_INDEX															currentPlayerIndex								= getCurrentPlayerIndex(tacticalInfo);
	if(currentPlayerIndex == PLAYER_INDEX_INVALID)
		return;

#define PLAYER_INFO_POSY 1

	uint16_t																	messageColor									= COLOR_DARKGREEN;
	::std::string																selectionText									= "";
	SPlayer&																	currentPlayer									= instanceGame.Players[currentPlayerIndex];
	if( currentPlayer.Selection.PlayerUnit != -1 
	 && currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit] != -1 
	 && currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]]->IsAlive()
	 ) 
	{
		bool																		bDarken											= !	( currentPlayer.Control.Type == PLAYER_CONTROL_AI
																																		 && ( (currentPlayer.Control.AIMode == PLAYER_AI_ASSISTS)
																																		   || (currentPlayer.Control.AIMode == PLAYER_AI_RIOTERS)
																																		   || (currentPlayer.Control.AIMode == PLAYER_AI_VIOLENT)
																																			)
																																		);
		messageColor															= getPlayerColor(tacticalInfo, currentPlayer, tacticalInfo.CurrentPlayer, PLAYER_INDEX_USER, bDarken);
		if(currentPlayer.Selection.PlayerUnit != -1 && currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit] != -1) {
			CCharacter																	& playerAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];
			displayDetailedAgentSlot(globalDisplay, PLAYER_INFO_POSY, 4, playerAgent, messageColor);
			displayStatusEffectsAndTechs(globalDisplay, PLAYER_INFO_POSY + 9, 4 + 32, playerAgent);
		}

		selectionText															= "Player name: "	+ ::std::string(currentPlayer.Name.begin());	
		lineToGridColored(globalDisplay.Screen, globalDisplay.TextAttributes, messageColor, 1, tacticalDisplayX+1, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());
		selectionText															= "Agent name: "	+ currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]]->Name;
		lineToGridColored(globalDisplay.Screen, globalDisplay.TextAttributes, messageColor, 2, tacticalDisplayX+1, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());
		selectionText															= "Moves left: " + std::to_string(currentPlayer.Squad.ActionsLeft[currentPlayer.Selection.PlayerUnit].Moves) + ". Actions left: " + std::to_string(currentPlayer.Squad.ActionsLeft[currentPlayer.Selection.PlayerUnit].Actions) + ".";
		lineToGridColored(globalDisplay.Screen, globalDisplay.TextAttributes, messageColor, 3, tacticalDisplayX+1, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());
		
		int32_t																		selectionX										= currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].x;
		int32_t																		selectionZ										= currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].z;
		selectionText															= "Target Position: " + std::to_string(selectionX) + ", " + std::to_string(selectionZ);
		lineToGridColored(globalDisplay.Screen, globalDisplay.TextAttributes, messageColor, 4, tacticalDisplayX+1, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());

		if(bSwap && currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit] != currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]]->Position) {
			selectionText															= "-";
			if(selectionX-1 >= 0) 
				lineToGridColored(tacticalDisplay.Screen	, tacticalDisplay.TextAttributes	, messageColor, selectionZ, selectionX-1, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());
			if(selectionX+1 < tacticalDisplay.Width) 
				lineToGridColored(tacticalDisplay.Screen	, tacticalDisplay.TextAttributes	, messageColor, selectionZ, selectionX+1, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());

			selectionText															= "|";
			if(selectionZ-1 >= 0) 
				lineToGridColored(tacticalDisplay.Screen	, tacticalDisplay.TextAttributes	, messageColor, selectionZ-1, selectionX, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());
			if(selectionZ+1 < tacticalDisplay.Depth) 
				lineToGridColored(tacticalDisplay.Screen	, tacticalDisplay.TextAttributes	, messageColor, selectionZ+1, selectionX, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());
		}
	}

	if(currentPlayer.Selection.TargetPlayer == -1 || tacticalInfo.Setup.Players[currentPlayer.Selection.TargetPlayer] == -1)
		return;

	SPlayer																		& currentTarget									= instanceGame.Players[tacticalInfo.Setup.Players[currentPlayer.Selection.TargetPlayer]];
	bool																		bDarken											= !	( currentTarget.Control.Type == PLAYER_CONTROL_AI
																																	&&	(	(currentTarget.Control.AIMode == PLAYER_AI_ASSISTS)
																																		||	(currentTarget.Control.AIMode == PLAYER_AI_RIOTERS)
																																		||	(currentTarget.Control.AIMode == PLAYER_AI_VIOLENT)
																																		)
																																	);

	messageColor															= getPlayerColor(tacticalInfo, currentTarget, (int8_t)currentPlayer.Selection.TargetPlayer, PLAYER_INDEX_USER, bDarken);

	selectionText															= std::string("Target player: ") + std::string(currentTarget.Name.begin());
	lineToGridColored(globalDisplay.Screen, globalDisplay.TextAttributes, messageColor, 2, tacticalDisplayX+1, ::klib::SCREEN_RIGHT, selectionText.c_str());

	if( currentPlayer.Selection.TargetUnit == -1 || currentTarget.Squad.Agents[currentPlayer.Selection.TargetUnit] == -1 )
		return;

	CCharacter																	& targetAgent									= *currentTarget.Army[currentTarget.Squad.Agents[currentPlayer.Selection.TargetUnit]];
	
	int32_t																		agentX											= targetAgent.Position.x;
	int32_t																		agentZ											= targetAgent.Position.z;
	if(bSwap) {
		selectionText															= "-";
		if(agentX-1 >= 0) 
			lineToGridColored(tacticalDisplay.Screen	, tacticalDisplay.TextAttributes	, messageColor, agentZ, agentX-1, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());
		if(agentX+1 < tacticalDisplay.Width) 
			lineToGridColored(tacticalDisplay.Screen	, tacticalDisplay.TextAttributes	, messageColor, agentZ, agentX+1, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());

		selectionText															= "|";
		if(agentZ-1 >= 0) 
			lineToGridColored(tacticalDisplay.Screen	, tacticalDisplay.TextAttributes	, messageColor, agentZ-1, agentX, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());
		if(agentZ+1 < tacticalDisplay.Depth) 
			lineToGridColored(tacticalDisplay.Screen	, tacticalDisplay.TextAttributes	, messageColor, agentZ+1, agentX, ::klib::SCREEN_LEFT, selectionText.c_str(), (uint32_t)selectionText.size());
	}

	if( !targetAgent.IsAlive() )
		return;

	int32_t																		xOffset											= tacticalDisplayX + tacticalDisplay.Width + 4;
	displayDetailedAgentSlot(globalDisplay, PLAYER_INFO_POSY, xOffset, targetAgent, messageColor);
	displayStatusEffectsAndTechs(globalDisplay, PLAYER_INFO_POSY+9, xOffset+32, targetAgent);
	selectionText															= "Target: " + targetAgent.Name;
	lineToGridColored(globalDisplay.Screen, globalDisplay.TextAttributes, messageColor, 3, tacticalDisplayX+1, ::klib::SCREEN_RIGHT, selectionText.c_str());
}

bool																	shoot											(SGame& instanceGame, int32_t tacticalPlayer, int32_t squadAgent)					{
	//STacticalDisplay															& tacticalDisplay								= instanceGame.TacticalDisplay;
	STacticalInfo																& tacticalInfo									= instanceGame.TacticalInfo;
	SPlayer																		& playerShooter									= instanceGame.Players[tacticalInfo.Setup.Players[tacticalPlayer]];
	CCharacter																	& agentShooter									= *playerShooter.Army[playerShooter.Squad.Agents[squadAgent]];

	const ::gpk::SCoord3<int32_t>												& targetTile									= playerShooter.Squad.TargetPositions[squadAgent];
	if(targetTile == agentShooter.Position)
		return false;

	int32_t																		totalBullets									= ::gpk::bit_true(agentShooter.FinalFlags.Tech.AttackType, ATTACK_TYPE_SPLASH) ? 10 : 1;
	if(::gpk::bit_true(agentShooter.FinalFlags.Tech.AttackType, ATTACK_TYPE_BURST))
		totalBullets															*= 3;
	
	//int																			finalChance										= agentShooter.FinalPoints.Attack.Hit;
	bool																		bIsBlind										= true_if(agentShooter.ActiveBonus.Status.Status & COMBAT_STATUS_BLIND);
	if(bIsBlind)
		printf("Blindness causes %s to have %u hit chance for this turn.\n", agentShooter.Name.c_str(), agentShooter.FinalPoints.Attack.Hit >>= 1);

	//if ((rand() % 100) < finalChance ) {
		 if(::gpk::bit_true(agentShooter.FinalFlags.Tech.AttackType			, ATTACK_TYPE_SPLASH		)) PlaySound("..\\gpk_data\\sounds\\Shotgun_Blast-Jim_Rogers-1914772763.wav"		, 0, SND_ASYNC | SND_FILENAME);
	else if(::gpk::bit_true(agentShooter.FinalFlags.Tech.ProjectileClass	, PROJECTILE_CLASS_ROCKET	)) PlaySound("..\\gpk_data\\sounds\\Missle_Launch-Kibblesbob-2118796725.wav"		, 0, SND_ASYNC | SND_FILENAME);
	else if(::gpk::bit_true(agentShooter.FinalFlags.Tech.ProjectileClass	, PROJECTILE_CLASS_GRENADE	)) PlaySound("..\\gpk_data\\sounds\\grenade-launcher-daniel_simon.wav"			, 0, SND_ASYNC | SND_FILENAME);
	else if(::gpk::bit_true(agentShooter.FinalFlags.Tech.ProjectileClass	, PROJECTILE_CLASS_BULLET	)) PlaySound("..\\gpk_data\\sounds\\Anti Aircraft Gun-Mike_Koenig-1303768514.wav"	, 0, SND_ASYNC | SND_FILENAME);
	else if(::gpk::bit_true(agentShooter.FinalFlags.Tech.ProjectileClass	, PROJECTILE_CLASS_RAY		)) PlaySound("..\\gpk_data\\sounds\\Gun_Shot-Marvin-1140816320.wav"				, 0, SND_ASYNC | SND_FILENAME);
	
	for(int32_t iBullet=0; iBullet<totalBullets; ++iBullet) {
		::gpk::bit_clear(playerShooter.Squad.AgentStates[squadAgent], AGENT_STATE_MOVE);
		SBullet																		newBullet;
		newBullet.Position.Cell													= {agentShooter.Position.x, agentShooter.Position.y, agentShooter.Position.z};
		newBullet.Position.Offset												= {.5f, .75f, .5f};
		newBullet.Direction														= {(float)(targetTile.x-newBullet.Position.Cell.x), (float)(targetTile.y-newBullet.Position.Cell.y), (float)(targetTile.z-newBullet.Position.Cell.z)};
		if(::gpk::bit_true(agentShooter.FinalFlags.Tech.AttackType, ATTACK_TYPE_SPLASH)) {
			double																		angleOffset										= 1.0/20.0*(totalBullets/2);
			double																		angle											= iBullet/20.0;
			newBullet.Direction.RotateY(angle-angleOffset);
		}
		newBullet.Direction.Normalize();
		newBullet.Position.Offset												+= newBullet.Direction*float(.5/totalBullets*iBullet);
		newBullet.Shooter.PlayerIndex											= (int8_t)tacticalPlayer;
		newBullet.Shooter.TeamId												= tacticalInfo.Setup.TeamPerPlayer[tacticalPlayer];
		newBullet.Shooter.AgentIndex											= (int8_t)squadAgent;
		newBullet.Shooter.SquadIndex											= 0;
		newBullet.Points.Damage													= agentShooter.FinalPoints.Attack.Damage		/ std::max(1, (totalBullets/2));
		newBullet.Points.DirectDamage											= agentShooter.FinalPoints.Attack.DirectDamage	;
		newBullet.Points.Effect													= agentShooter.FinalFlags.Effect.Attack			;
		newBullet.Points.StatusInflict											= agentShooter.FinalFlags.Status.Inflict		;
		newBullet.Points.Tech													= ENTITY_GRADE_STRUCT_ALL						;
		newBullet.Points.Tech.AttackType										= agentShooter.FinalFlags.Tech.AttackType		;
		newBullet.Points.Tech.ProjectileClass									= agentShooter.FinalFlags.Tech.ProjectileClass	;
		newBullet.Points.Tech.AmmoEffect										= agentShooter.FinalFlags.Tech.AmmoEffect		;
		newBullet.Points.Level													= agentShooter.CurrentEquip.Weapon.Level		;

		tacticalInfo.AddBullet(newBullet);

	}
	playerShooter.Squad.TargetPositions[squadAgent]							= agentShooter.Position;
	return true;
}

		TURN_ACTION														selectAIAction									(SGame& instanceGame);
inline	TURN_ACTION														selectRemoteAction								(SGame& instanceGame)																{ return selectAIAction(instanceGame); }
		bool															characterTurn									(SGame& instanceGame, TURN_ACTION combatOption)										{
	STacticalInfo																& tacticalInfo									= instanceGame.TacticalInfo;
	SGlobalDisplay																& globalDisplay									= instanceGame.GlobalDisplay;
	SPlayer																		& currentPlayer									= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];
	bool																		bNotCanceled									= true;

	if(combatOption == TURN_ACTION_ATTACK && currentPlayer.Squad.ActionsLeft[currentPlayer.Selection.PlayerUnit].Actions > 0) {
		CCharacter																	& currentAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];
		CCharacter																	& playerAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];
		instanceGame.ClearMessages();
		if(::gpk::bit_true(currentAgent.FinalFlags.Tech.AttackType, ATTACK_TYPE_RANGED)) {
			if(true_if(playerAgent.ActiveBonus.Status.Status & COMBAT_STATUS_BLACKOUT) && (getWeaponFlags(playerAgent.CurrentEquip.Weapon).Tech.Tech & ENTITY_TECHNOLOGY_DIGITAL)) {
				instanceGame.UserMessage												= "This weapon was disabled by an electromagnetic pulse.";
				instanceGame.LogMessage();
			}
			else if(shoot(instanceGame, tacticalInfo.CurrentPlayer, currentPlayer.Selection.PlayerUnit)) {
				instanceGame.UserSuccess												= playerAgent.Name + " shoots!"; 
				instanceGame.LogSuccess();
				--currentPlayer.Squad.ActionsLeft[currentPlayer.Selection.PlayerUnit].Actions;
			}
			else {
				instanceGame.UserMiss													= "You need to select a valid target in order to attack!"; 
				instanceGame.LogMiss();
			}
		}
		else if(::gpk::bit_true(currentAgent.FinalFlags.Tech.AttackType, ATTACK_TYPE_MELEE)) {
			if(currentPlayer.Selection.TargetPlayer == -1 || tacticalInfo.Setup.Players[currentPlayer.Selection.TargetPlayer] == PLAYER_INDEX_INVALID || currentPlayer.Selection.TargetUnit == -1) {
				instanceGame.UserMiss													= "You need to select a valid target in order to attack!"; 
				instanceGame.LogMiss();
			}
			else {
				SPlayer																		& targetPlayer									= instanceGame.Players[tacticalInfo.Setup.Players[currentPlayer.Selection.TargetPlayer]];
				CCharacter																	& targetAgent									= *targetPlayer	.Army[targetPlayer.Squad.Agents[currentPlayer.Selection.TargetUnit]];
				//CCharacter																	& playerAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];

				::gpk::SCoord3<int32_t>													coordPlayer										= playerAgent.Position; 
				::gpk::SCoord3<int32_t>													coordTarget										= targetAgent.Position;
				::gpk::SCoord3<float>														distance										= (coordTarget-coordPlayer).Cast<float>();

				const SEntityPoints															& playerAgentPoints								= playerAgent.FinalPoints;
				const SEntityFlags															& playerAgentFlags								= playerAgent.FinalFlags;

				double																		finalSight										= getFinalRange(playerAgent, playerAgentPoints);
				double																		finalRange										= getFinalSight(playerAgent, playerAgentPoints);
				if( (distance.Length() > finalSight || distance.Length() > finalRange) 
				 && ::gpk::bit_false(playerAgentFlags.Tech.AttackType, ATTACK_TYPE_RANGED)
				 )
				{
					instanceGame.UserMiss													= "You can't attack " + targetAgent.Name + " from that distance."; 
					instanceGame.LogMiss();
				}
				else {
					char																		formatCoords[256]								= {};
					sprintf_s(formatCoords, "{x=%i, y=%i, z=%i}", coordTarget.x, coordTarget.y, coordTarget.z);
					::std::string																targetType										= "Target Coords: " + std::string(formatCoords) + ". Target player index: "	+ std::to_string(currentPlayer.Selection.TargetPlayer)	+ ". Target player unit: "	+ std::to_string(currentPlayer.Selection.TargetUnit) + ". Target player name: "		+ std::string(targetPlayer.Name.begin());
					sprintf_s(formatCoords, "{x=%i, y=%i, z=%i}", coordPlayer.x, coordPlayer.y, coordPlayer.z);
					::std::string																userType										= "Origin Coords: " + std::string(formatCoords) + ". Current player index: "	+ std::to_string(tacticalInfo.CurrentPlayer)			+ ". Current player unit: "	+ std::to_string(currentPlayer.Selection.PlayerUnit) + ". Current player name: "	+ std::string(currentPlayer.Name.begin());
			
					static const HANDLE															hConsoleOut										= GetStdHandle( STD_OUTPUT_HANDLE );
					COORD																		cursorPos										= {0, ((SHORT)globalDisplay.Depth>>1)+7};
					SetConsoleCursorPosition( hConsoleOut, cursorPos );
					printf("%s.\n%s.\n\n", targetType.c_str(), userType.c_str());

					klib::attack(playerAgent, targetAgent);

					if(!targetAgent.IsAlive())	
						handleAgentDeath(instanceGame, targetAgent, playerAgent, tacticalInfo.Setup.TeamPerPlayer[currentPlayer.Selection.TargetPlayer]);
					if(!playerAgent.IsAlive()) { 
						handleAgentDeath(instanceGame, playerAgent, targetAgent, tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer]);
						bNotCanceled															= false;
						endTurn(instanceGame);
					}

					--currentPlayer.Squad.ActionsLeft[currentPlayer.Selection.PlayerUnit].Actions;
					getchar();
				}		
			}
		}// if(melee)

	} // if(TURN_ACTION_ATTACK)
	else if(combatOption == TURN_ACTION_INVENTORY) {
		instanceGame.State.Substate												= GAME_SUBSTATE_CHARACTER;
	}
	else if(combatOption == TURN_ACTION_CANCEL) {
		instanceGame.UserMessage												= std::string(currentPlayer.Name.begin()) + " canceled the turn.";
		instanceGame.LogMessage();
		bNotCanceled															= false;
		endTurn(instanceGame);
	}
	else if(combatOption == TURN_ACTION_MOVE) {
		char																		buffer[128]										= {};
		CCharacter																	& playerAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];
		//bool																		bSuccess										= false;
		if(tacticalInfo.Board.Tiles.IsTileAvailable(currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].x, currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].z)) {
			currentPlayer.Squad.AgentStates[currentPlayer.Selection.PlayerUnit]		= AGENT_STATE_MOVE;
			sprintf_s(buffer, "Player %s moves %s to {%i, %i, %i}."
				, currentPlayer.Name.begin()
				, playerAgent.Name.c_str()
				, currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].x
				, currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].y
				, currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].z
			);
			instanceGame.UserSuccess												= buffer;
			instanceGame.LogSuccess();
		}
		else {
			sprintf_s(buffer, "Cannot move to {%i, %i, %i}. Terrain occupied or unreachable."
				, currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].x
				, currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].y
				, currentPlayer.Squad.TargetPositions[currentPlayer.Selection.PlayerUnit].z
			);
			instanceGame.UserError													= buffer;
			instanceGame.LogError();
		}
	}
	else if(combatOption != TURN_ACTION_CONTINUE) {
		instanceGame.UserError													= "This function isn't available!";
		instanceGame.LogError();
	}

	return bNotCanceled;
}

template <size_t _Size1>
int32_t																	initInventoryMenu								(klib::CCharacter& adventurer, klib::SMenuItem<int32_t> (&itemOptions)[_Size1], bool bPrintPrice=false, bool bSellPrice=true)	{
	char																		itemOption[128]									= {};
	
	for(uint32_t i=0; i<adventurer.Goods.Inventory.Items.Count; ++i) {
		const klib::SItem															& itemEntity									= adventurer.Goods.Inventory.Items[i].Entity;
		std::string																	itemName										= getItemName(itemEntity);

		if(bPrintPrice) {
			int32_t																		finalPrice										= getItemPrice(itemEntity, bSellPrice);
			sprintf_s(itemOption, "%i coins each - x%.2u %s", finalPrice, adventurer.Goods.Inventory.Items[i].Count, itemName.c_str());
		}
		else
			sprintf_s(itemOption, "- x%.2u %s", adventurer.Goods.Inventory.Items[i].Count, itemName.c_str());
		
		itemOptions[i].ReturnValue												= i;
 		itemOptions[i].Text														= itemOption;
	}

	return 0;
}

int32_t																	drawInventoryMenu								(SGame& instanceGame, SPlayer& player, klib::CCharacter& adventurer, const std::string& menuTitle)								{
	::klib::SMenuItem<int32_t>													itemOptions[MAX_INVENTORY_SLOTS+1]				= {};
	SGlobalDisplay&																globalDisplay									= instanceGame.GlobalDisplay;
	SCharacterInventory&														characterInventory								= adventurer.Goods.Inventory;
	int32_t																		initMenu										= initInventoryMenu(adventurer, itemOptions, false, false);
	return drawMenu(instanceGame.GlobalDisplay.Screen, &globalDisplay.TextAttributes.Cells[0][0], characterInventory.Items.Count, menuTitle, itemOptions, instanceGame.FrameInput, (int32_t)adventurer.Goods.Inventory.Items.Count, -1, 50);
		
};

TURN_ACTION																useItems										(SGame& instanceGame, SPlayer& player, klib::CCharacter& user, const std::string& menuTitle, bool bIsAIControlled) {
	int32_t																		indexInventory									= ~0U;
	SCharacterInventory															& characterInventory							= user.Goods.Inventory;
	bool																		bUsedItem										= false;
	if(!bIsAIControlled)  {
		indexInventory															= drawInventoryMenu(instanceGame, player, user, menuTitle);
		if(indexInventory < (int32_t)user.Goods.Inventory.Items.Count)
			bUsedItem																= true;
	}
	else { // not a player so execute choice by AI
		//indexInventory = selectItemsAI(user, target);
		indexInventory															= user.Goods.Inventory.Items.Count;
		if(indexInventory < (int32_t)user.Goods.Inventory.Items.Count)
			bUsedItem																= true;
	}

	if(indexInventory == (int32_t)user.Goods.Inventory.Items.Count)
		return TURN_ACTION_MAIN;
	else if(indexInventory == -1)
		return TURN_ACTION_CONTINUE;

	if(bUsedItem) {
		const klib::SItem															& entityItem									= user.Goods.Inventory.Items[indexInventory].Entity;
		const klib::CItem															& itemDescription								= klib::itemDescriptions[entityItem.Definition];
		const klib::SEntityPoints													& userFinalPoints								= user.FinalPoints;
		// Only use potions if we have less than 60% HP
		if( klib::ITEM_TYPE_POTION == itemDescription.Type 
			&&  (	(klib::PROPERTY_TYPE_HEALTH	== itemDescription.Property && user.Points.LifeCurrent.Health	>= userFinalPoints.LifeMax.Health	)
				||	(klib::PROPERTY_TYPE_SHIELD	== itemDescription.Property && user.Points.LifeCurrent.Shield	>= userFinalPoints.LifeMax.Shield	)
				||	(klib::PROPERTY_TYPE_MANA	== itemDescription.Property && user.Points.LifeCurrent.Mana		>= userFinalPoints.LifeMax.Mana		)
					)
			)
		{
			std::string																	itemName										= getItemName(entityItem);
			instanceGame.UserMessage												= "You don't need to use " + itemName + "!";
			instanceGame.LogMessage();
			bUsedItem																= false;
		}
		else
			bUsedItem																= true;
	}

	if(bUsedItem) {
		const klib::SItem															& entityItem									= user.Goods.Inventory.Items[indexInventory].Entity;
		const klib::CItem															& itemDescription								= klib::itemDescriptions[entityItem.Definition];
		if( klib::ITEM_TYPE_POTION == itemDescription.Type )
			bUsedItem																= klib::executeItem(indexInventory, user, user);
		else {
		//if( klib::ITEM_TYPE_GRENADE == itemDescription.Type )
		//{
		//
		//}
		//else if( klib::ITEM_TYPE_POTION == itemDescription.Type )
			if(player.Selection.TargetPlayer != -1 && player.Selection.TargetUnit != -1) {
				SPlayer																		& targetPlayer									= instanceGame.Players[instanceGame.TacticalInfo.Setup.Players[player.Selection.TargetPlayer]];
				if(targetPlayer.Squad.Agents[player.Selection.TargetUnit] != -1) {
					CCharacter																	& targetAgent									= *targetPlayer.Army[targetPlayer.Squad.Agents[player.Selection.TargetUnit]];
					bUsedItem																= klib::executeItem(indexInventory, user, targetAgent);
				}
				else {
					bUsedItem																= false;
					std::string																	itemName										= getItemName(entityItem);
					instanceGame.UserMessage												= "You need to select a valid target in order to use " + itemName + "!";
					instanceGame.LogMessage();
				}
			}
			else {
				bUsedItem																= false;
				std::string																	itemName										= getItemName(entityItem);
				instanceGame.UserMessage = "You need to select a valid target in order to use " + itemName + "!";
				instanceGame.LogMessage();
			}
		}
	}
	
	if(bUsedItem && 0 >= player.Squad.ActionsLeft[player.Selection.PlayerUnit].Moves)
		return TURN_ACTION_CANCEL;
	
	return TURN_ACTION_CONTINUE;
}

SGameState																endMission										(SGame& instanceGame)									{
	klib::determineOutcome(instanceGame);						// Determine outcome before exiting tactical mode.
	::gpk::bit_clear(instanceGame.Flags, GAME_FLAGS_TACTICAL);	// Tell the system that the tactical mode is over.
	return {GAME_STATE_WELCOME_COMMANDER};
}


void																	updateBullets									(SGame& instanceGame)									{
	STacticalInfo																& tacticalInfo									= instanceGame.TacticalInfo;

	for(uint32_t iBullet=0;iBullet<tacticalInfo.Board.Shots.Bullet.Count; ++iBullet) {
		for(uint32_t iShot=0; iBullet < tacticalInfo.Board.Shots.Bullet.Count && iShot < tacticalInfo.Board.Shots.Bullet[iBullet].Count; ++iShot) {
			double																		fSpeed											= 10.0;
			double																		fActualSpeed									= instanceGame.FrameTimer.LastTimeSeconds*fSpeed;
			//double fActualSpeed = 1.0*fSpeed;
			if(fActualSpeed >= 0.25)
				fActualSpeed															= 0.25;
			STacticalCoord																& bulletPos										= tacticalInfo.Board.Shots.Bullet[iBullet].Entity.Position;
			::gpk::SCoord3<float>														& bulletDir										= tacticalInfo.Board.Shots.Bullet[iBullet].Entity.Direction;
			bulletPos.Offset.x														+= (float)(bulletDir.x*fActualSpeed);
			bulletPos.Offset.y														+= (float)(bulletDir.y*fActualSpeed);
			bulletPos.Offset.z														+= (float)(bulletDir.z*fActualSpeed);
			SBullet																		newBullet										= tacticalInfo.Board.Shots.Bullet[iBullet].Entity;
			STacticalCoord																& newBulletPos									= newBullet.Position;
			STacticalCoord																oldBulletPos									= newBullet.Position;

			bool																		bImpact											= false;
			if( newBulletPos.Offset.x < 0 || newBulletPos.Offset.x >= 1.0 
			 || newBulletPos.Offset.y < 0 || newBulletPos.Offset.y >= 1.0 
			 || newBulletPos.Offset.z < 0 || newBulletPos.Offset.z >= 1.0 
			)
			{
				int32_t																		coordDiffX										= (int32_t)newBulletPos.Offset.x;
				int32_t																		coordDiffY										= (int32_t)newBulletPos.Offset.y;
				int32_t																		coordDiffZ										= (int32_t)newBulletPos.Offset.z;

				newBulletPos.Cell.x														+= coordDiffX;
				newBulletPos.Cell.y														+= coordDiffY;
				newBulletPos.Cell.z														+= coordDiffZ;

				newBulletPos.Offset.x													-= coordDiffX;
				newBulletPos.Offset.y													-= coordDiffY;
				newBulletPos.Offset.z													-= coordDiffZ;

				if(newBulletPos.Offset.x < 0) { newBulletPos.Cell.x	-= 1; newBulletPos.Offset.x += 1.0f; }
				if(newBulletPos.Offset.y < 0) { newBulletPos.Cell.y	-= 1; newBulletPos.Offset.y += 1.0f; }
				if(newBulletPos.Offset.z < 0) { newBulletPos.Cell.z	-= 1; newBulletPos.Offset.z += 1.0f; }

				if( newBulletPos.Cell.x >= 0 && newBulletPos.Cell.x < tacticalInfo.Board.Width
				 && newBulletPos.Cell.z >= 0 && newBulletPos.Cell.z < tacticalInfo.Board.Depth
				)
				{
					int32_t																		newx											= newBulletPos.Cell.x;
					int32_t																		newz											= newBulletPos.Cell.z;
					if(tacticalInfo.Board.Tiles.Entities.Agents	[newz][newx].AgentIndex	!= -1	&& tacticalInfo.Board.Tiles.Entities.Agents	[newz][newx].PlayerIndex != -1 ) {
						bImpact																	= true;
						if(tacticalInfo.Setup.Players[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].PlayerIndex] != PLAYER_INDEX_INVALID) {
							SPlayer																		& playerHit										= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].PlayerIndex]];
							SPlayer																		& playerShooter									= instanceGame.Players[tacticalInfo.Setup.Players[newBullet.Shooter.PlayerIndex]];
							if( playerHit.Squad.Agents[tacticalInfo.Setup.Players[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].AgentIndex]] != -1 ) {
								CCharacter																	& agentHit										= *playerHit.Army[playerHit.Squad.Agents[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].AgentIndex]];
								if(agentHit.IsAlive()) {
									CCharacter																	& agentShooter									= *playerShooter	.Army[playerShooter.Squad.Agents[newBullet.Shooter.AgentIndex]];
									char																		agentName[128]									= {};
									sprintf_s(agentName, "Bullet shot by %s.", agentShooter.Name.c_str());
 									applySuccessfulWeaponHit(agentShooter, agentHit, newBullet.Points.Damage, agentName);
									agentShooter	.Score.DamageDealt										+= newBullet.Points.Damage;
									++agentShooter	.Score.AttacksHit;
									agentHit		.Score.DamageTaken										+= newBullet.Points.Damage;
									++agentHit		.Score.AttacksReceived;
									agentShooter	.Recalculate();
									agentHit		.Recalculate();
									if(0 >= agentShooter	.Points.LifeCurrent.Health)	handleAgentDeath(instanceGame, agentShooter	, agentHit		, tacticalInfo.Setup.TeamPerPlayer[newBullet.Shooter.PlayerIndex]);
									if(0 >= agentHit		.Points.LifeCurrent.Health)	handleAgentDeath(instanceGame, agentHit		, agentShooter	, tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].PlayerIndex]);
								}
								else
									tacticalInfo.Board.Tiles.Entities.Agents[newz][newx]					= {TEAM_TYPE_INVALID, -1, -1, -1};
							}
						}
					}
					else if(tacticalInfo.Board.Tiles.Entities.Props	[newz][newx].Level != -1 ) {
						tacticalInfo.Board.Tiles.Entities.Props[newz][newx].Level				= -1/* = {-1, -1, -1, -1}*/; 
						bImpact																	= true; 
					}
					else {	
						bool																		bKeepBulleting									= true;
						//if(tacticalInfo.Board.Tiles.Terrain.Topology	[newz][newx].Smooth		>= PARTIAL_COVER_HEIGHT) {tacticalInfo.Board.Tiles.Terrain.Topology	[newz][newx].Smooth		--	; bKeepBulleting = false; bImpact = true; }
						//if(tacticalInfo.Board.Tiles.Terrain.Topology	[newz][newx].Sharp		>= PARTIAL_COVER_HEIGHT) {tacticalInfo.Board.Tiles.Terrain.Topology	[newz][newx].Sharp		--	; bKeepBulleting = false; bImpact = true; }
						//if(tacticalInfo.Board.Tiles.Terrain.Topology	[newz][newx].Collision	>= PARTIAL_COVER_HEIGHT) {tacticalInfo.Board.Tiles.Terrain.Topology	[newz][newx].Collision	--	; bKeepBulleting = false; bImpact = true; }
						if(bKeepBulleting) {
							if(tacticalInfo.Board.Tiles.IsTileAvailable(newBulletPos.Cell.x, newBulletPos.Cell.z)) {									  
								bool																		bAdded1											= tacticalInfo.Board.Shots.Bullet.AddElement(newBullet);
								if(bAdded1)
									bool																		bAdded2											= tacticalInfo.Board.Shots.Coords.AddElement(newBulletPos.Cell);
							}
						}
					}
				}
				else 
					instanceGame.UserMessage.clear();

				tacticalInfo.Board.Shots.Bullet.DecreaseEntity(iBullet);
				tacticalInfo.Board.Shots.Coords.DecreaseEntity(tacticalInfo.Board.Shots.Coords.FindElement(oldBulletPos.Cell));

				if(bImpact 
					&&	(	::gpk::bit_true(newBullet.Points.Tech.ProjectileClass, PROJECTILE_CLASS_ROCKET	) 
						||	::gpk::bit_true(newBullet.Points.Tech.AmmoEffect, AMMO_EFFECT_EXPLOSIVE			) 
						||	::gpk::bit_true(newBullet.Points.Tech.AmmoEffect, AMMO_EFFECT_BLAST				) 
						)
					)
				{
					PlaySound("..\\gpk_data\\sounds\\Explosion_Ultra_Bass-Mark_DiAngelo-1810420658.wav", 0, SND_ASYNC | SND_FILENAME);
					SAOE																		newAOE										= {};
					newAOE.Position															= newBullet.Position;
					newAOE.RadiusOrHalfSize													= newBullet.Points.Level;
					newAOE.Caster															= newBullet.Shooter;
					newAOE.Flags.Effect.Attack												= newBullet.Points.Effect;
					newAOE.Flags.Tech														= newBullet.Points.Tech;
					newAOE.StatusInflict													= newBullet.Points.StatusInflict;
					newAOE.Level															= newBullet.Points.Level;
					newAOE.TurnsLeft														= newBullet.Points.Level;
					tacticalInfo.AddAOE(newAOE);
					if(::gpk::bit_true(newBullet.Points.Tech.AmmoEffect, AMMO_EFFECT_EXPLOSIVE)) {
						int32_t y=0;
						for(int32_t z = (int32_t)(newAOE.Position.Cell.z-newAOE.RadiusOrHalfSize), maxz=int32_t(newAOE.Position.Cell.z+newAOE.RadiusOrHalfSize); z < maxz; ++z) {
							if(z < 0 || z >= tacticalInfo.Board.Depth)
								continue;
							for(int32_t x = (int32_t)(newAOE.Position.Cell.x-newAOE.RadiusOrHalfSize), maxx=int32_t(newAOE.Position.Cell.x+newAOE.RadiusOrHalfSize); x < maxx; ++x) {
								if(x < 0 || x >= tacticalInfo.Board.Width)
									continue;

								const ::gpk::SCoord3<int32_t>	currentCoord	= {x, y, z};
								const ::gpk::SCoord3<float>	distance		= (currentCoord-newAOE.Position.Cell).Cast<float>();
								double length = distance.Length();
								if((length+1.0000000000001) > newAOE.RadiusOrHalfSize)
									continue;

								if(tacticalInfo.Board.Tiles.Entities.Props[z][x].Level != -1 )	
									tacticalInfo.Board.Tiles.Entities.Props[z][x].Level = -1; 
								else {
									double proportion = length/newAOE.RadiusOrHalfSize;
									tacticalInfo.Board.Tiles.Terrain.Topology[z][x].Smooth -= (int8_t)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

									STileGeometry& tileGeometry = tacticalInfo.Board.Tiles.Terrain.Geometry[z][x];

									if(proportion > 1.0)
										continue;
									tileGeometry.fHeight[0]	-= (float)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

									length = ::gpk::SCoord3<float>{distance.x+1, distance.y, distance.z}.Length();
									proportion = length/newAOE.RadiusOrHalfSize;
									if(proportion <= 1.0)
										tileGeometry.fHeight[1]	-= (float)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

									length = ::gpk::SCoord3<float>{distance.x, distance.y, distance.z+1}.Length();
									proportion = length/newAOE.RadiusOrHalfSize;
									if(proportion <= 1.0)
										tileGeometry.fHeight[2]	-= (float)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

									length = ::gpk::SCoord3<float>{distance.x+1, distance.y, distance.z+1}.Length();
									proportion = length/newAOE.RadiusOrHalfSize;
									if(proportion <= 1.0)
										tileGeometry.fHeight[3]	-= (float)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

									if(false)
									{
										tileGeometry.fHeight[0] = (float)(int32_t)(tileGeometry.fHeight[0]);
										tileGeometry.fHeight[1] = (float)(int32_t)(tileGeometry.fHeight[1]);
										tileGeometry.fHeight[2] = (float)(int32_t)(tileGeometry.fHeight[2]);
										tileGeometry.fHeight[3] = (float)(int32_t)(tileGeometry.fHeight[3]);
									}
								}
							}
						}

						SAgentsReference					agentsInRange;
						getAgentsInSight(instanceGame, newAOE.Position.Cell, newAOE.RadiusOrHalfSize, agentsInRange);
						SPlayer								& playerShooter		= instanceGame.Players[tacticalInfo.Setup.Players[newAOE.Caster.PlayerIndex]];
						CCharacter							& shooter			= *playerShooter.Army[playerShooter.Squad.Agents[newAOE.Caster.AgentIndex]];
						TEAM_TYPE							teamShooter			= tacticalInfo.Setup.TeamPerPlayer[newAOE.Caster.PlayerIndex];
						for(int32_t iAgentInRange=0; iAgentInRange < agentsInRange.Count; ++iAgentInRange) {
							SPlayer&							playerVictim			= instanceGame.Players[tacticalInfo.Setup.Players[agentsInRange.Agents[iAgentInRange].Agent.PlayerIndex]];
							TEAM_TYPE							teamVictim				= tacticalInfo.Setup.TeamPerPlayer[agentsInRange.Agents[iAgentInRange].Agent.PlayerIndex];
							CCharacter&							agentVictim				= *playerVictim.Army[playerVictim.Squad.Agents[agentsInRange.Agents[iAgentInRange].Agent.AgentIndex]];
							//::gpk::SCoord3<int32_t> distance = agentVictim.Position-newAOE.Position.Cell;
							const ::gpk::SCoord3<int32_t>		& coordAgent			= agentVictim.Position;
							const ::gpk::SCoord3<float>		distance				= (coordAgent-newAOE.Position.Cell).Cast<float>();
							double								length					= distance.Length();
							if(length > newAOE.RadiusOrHalfSize) 
								continue;

							double proportion = length/newAOE.RadiusOrHalfSize;
							if(proportion >= 1.0)
								continue;
							const std::string weaponName = getWeaponName(shooter.CurrentEquip.Weapon);
							applySuccessfulWeaponHit(shooter, agentVictim, (int32_t)(newBullet.Points.Damage*(1.0-proportion)), weaponName.c_str());
							if(false == agentVictim.IsAlive())
								handleAgentDeath(instanceGame, agentVictim, shooter, teamVictim);
							if(false == shooter.IsAlive())
								handleAgentDeath(instanceGame, shooter, agentVictim, teamShooter);
						}
					}
				}
				else if(bImpact) 
				{

				}
			}

		}
	}
}

bool																	initTacticalGame								(SGame& instanceGame);
SGameState																drawTacticalScreen								(SGame& instanceGame, const SGameState& returnState)				{
	SGameState																	exitState										= returnState;
	if(false == ::gpk::bit_true(instanceGame.Flags, GAME_FLAGS_TACTICAL)) {
		if(!initTacticalGame(instanceGame)) {
			::gpk::bit_clear(instanceGame.Flags, GAME_FLAGS_TACTICAL);
			return {GAME_STATE_WELCOME_COMMANDER};
		}
		//int32_t finalCost = ::klib::missionCost(instanceGame.Players[PLAYER_INDEX_USER], instanceGame.Players[PLAYER_INDEX_USER].Squad, instanceGame.Players[PLAYER_INDEX_USER].Squad.Size);
		PlaySound("..\\gpk_data\\sounds\\Intro_Wind-Mark_DiAngelo-844491759.wav", 0, SND_ASYNC | SND_FILENAME);
		//instanceGame.Players[PLAYER_INDEX_USER].Money -= finalCost;
	}
	
	STacticalInfo&		tacticalInfo	= instanceGame.TacticalInfo;
	if(tacticalInfo.Board.Shots.Bullet.Count <= 0) {
		handleUserInput(instanceGame, returnState);
		if(!updateCurrentPlayer(instanceGame)) {
			endTurn(instanceGame);

			 // If players have agents still alive we just continue in the tactical screen. Otherwise go back to main screen. 
			if(isTacticalValid(instanceGame))
				return exitState;

			return endMission(instanceGame);
		}
	}

	STacticalDisplay&	tacticalDisplay	= instanceGame.TacticalDisplay;
	SPlayer&			currentPlayer	= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];
	drawTacticalBoard(instanceGame, tacticalInfo, tacticalDisplay, tacticalInfo.CurrentPlayer, tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer], currentPlayer.Selection, true);

	SGlobalDisplay&		globalDisplay	= instanceGame.GlobalDisplay;
	clearGrid(globalDisplay.Screen, ' ');
	drawTileInfo(instanceGame, instanceGame.FrameInput, tacticalInfo, globalDisplay, tacticalDisplay);
	drawPlayerInfo(instanceGame);

	TURN_ACTION			selectedAction	= TURN_ACTION_CONTINUE;

	if(tacticalInfo.Board.Shots.Bullet.Count <= 0) {
		// Need to construct menu title
		std::string menuTitle = "Mission Over";
		if( currentPlayer.Selection.PlayerUnit != -1 && currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit] != -1 && GAME_SUBSTATE_CHARACTER != instanceGame.State.Substate)
			menuTitle = "Agent #" + std::to_string(currentPlayer.Selection.PlayerUnit+1) + ": " + currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]]->Name;
		else if(currentPlayer.Selection.PlayerUnit != -1)
			menuTitle = "Agent #" + std::to_string(currentPlayer.Selection.PlayerUnit+1);

		if(currentPlayer.Control.Type == PLAYER_CONTROL_LOCAL)  {
 			if(instanceGame.State.Substate == GAME_SUBSTATE_INVENTORY) {
				menuTitle += " - Inventory";
				CCharacter& currentAgent = *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];
				selectedAction = useItems(instanceGame, currentPlayer, currentAgent, menuTitle, false);
			}
 			else if(instanceGame.State.Substate == GAME_SUBSTATE_EQUIPMENT) {
				menuTitle += " - Equipment";
				exitState = drawMenu(instanceGame.GlobalDisplay.Screen, &globalDisplay.TextAttributes.Cells[0][0], menuTitle, optionsCombatTurnEquip, instanceGame.FrameInput, {GAME_STATE_TACTICAL_CONTROL, GAME_SUBSTATE_MAIN}, exitState, 10);
				selectedAction = TURN_ACTION_CONTINUE;
			}
			else {
				if(instanceGame.State.Substate != GAME_SUBSTATE_MAIN) {
					instanceGame.ClearMessages();
					instanceGame.UserError = "Unrecognized game substate!";
					instanceGame.LogError();
				}
				selectedAction = drawMenu(instanceGame.GlobalDisplay.Screen, &globalDisplay.TextAttributes.Cells[0][0], menuTitle, optionsCombatTurn, instanceGame.FrameInput, TURN_ACTION_MENUS, TURN_ACTION_CONTINUE, 10);
			}
		}
		else if(currentPlayer.Control.Type == PLAYER_CONTROL_AI)  {
			selectedAction = selectAIAction(instanceGame);
#if !(defined(NWOL_DEBUG_ENABLED))
			Sleep(70);	
#endif
		}
		else if(currentPlayer.Control.Type == PLAYER_CONTROL_REMOTE) 
			selectedAction = selectRemoteAction(instanceGame);
	}
	else {
		char					bulletInfo[256];
		::gpk::SCoord3<float>	bulletPos		= {0.0f,0.0f,0.0f};
		const SBullet&			bulletToPrint	= tacticalInfo.Board.Shots.Bullet[0].Entity;
		bulletPos.x = bulletToPrint.Position.Cell.x + bulletToPrint.Position.Offset.x; 
		bulletPos.y = bulletToPrint.Position.Cell.y + bulletToPrint.Position.Offset.y;
		bulletPos.z = bulletToPrint.Position.Cell.z + bulletToPrint.Position.Offset.z;
		sprintf_s(bulletInfo, "Bullet at {%f, %f, %f} with direction {%f, %f, %f}", bulletPos.x, bulletPos.y, bulletPos.z, bulletToPrint.Direction.x, bulletToPrint.Direction.y, bulletToPrint.Direction.z);
		instanceGame.UserMessage = bulletInfo;
	}

	bool	bNotCanceled	= true;
		 if(selectedAction == TURN_ACTION_MENUS)			return {GAME_STATE_WELCOME_COMMANDER};
	else if(selectedAction == TURN_ACTION_INVENTORY)		return {exitState.State, GAME_SUBSTATE_INVENTORY};
	else if(selectedAction == TURN_ACTION_EQUIPMENT)		return {exitState.State, GAME_SUBSTATE_EQUIPMENT};
	else if(selectedAction == TURN_ACTION_MAIN)				return {exitState.State, GAME_SUBSTATE_MAIN};
	else if(selectedAction == TURN_ACTION_ABORT_MISSION) {
		currentPlayer.Squad.Clear(-1);
		return endMission(instanceGame);
	}
	else if(selectedAction == TURN_ACTION_AUTOPLAY) {
		currentPlayer.Control.Type = PLAYER_CONTROL_AI;
		tacticalInfo.Setup.Controls[tacticalInfo.CurrentPlayer].Type = PLAYER_CONTROL_AI;
		return exitState;
	}
	else 
		bNotCanceled = characterTurn(instanceGame, selectedAction);

	bool movesRemaining = false;

	for(int32_t i = 0; i < currentPlayer.Squad.Size; i++) {
		if(currentPlayer.Squad.ActionsLeft[i].Actions != 0 || currentPlayer.Squad.ActionsLeft[i].Moves != 0){
			movesRemaining = true;
		break;
		}
	}

	if( (tacticalInfo.Board.Shots.Bullet.Count <= 0) && bNotCanceled && !movesRemaining) 
		endTurn(instanceGame);

	 // If players have agents still alive we just continue in the tactical screen. Otherwise go back to main screen. 
	if(isTacticalValid(instanceGame)) {
		updateBullets(instanceGame);
		return exitState;
	}

	return endMission(instanceGame);
}

