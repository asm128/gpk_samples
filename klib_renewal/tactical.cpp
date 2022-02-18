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

bool																	handleUserInput									(::klib::SGame& instanceGame, const ::klib::SGameState& /*returnState*/)								{
	static ::klib::SAccumulator<double>											keyAccum										= {0.0, 0.6};

	::klib::STacticalInfo														& tacticalInfo									= instanceGame.TacticalInfo;
	//::klib::STacticalPlayer													& playerUser									= instanceGame.Players[PLAYER_INDEX_USER].Tactical;
	::klib::STacticalPlayer														& currentPlayer									= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]].Tactical;
	::klib::SPlayerSelection													& playerSelection								= currentPlayer.Selection;
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
			::klib::STacticalPlayer															& playerTarget									= instanceGame.Players[tacticalInfo.Setup.Players[playerSelection.TargetPlayer]].Tactical;
			if(playerTarget.Squad.Agents[playerSelection.TargetUnit] != -1)
				currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]	= playerTarget.Army[playerTarget.Squad.Agents[playerSelection.TargetUnit]]->Position;
			else
				playerSelection.ClearTarget();
		}
		keyAccum.Value															= 0.0;
	}
	else {
		int32_t																		mouseX											= instanceGame.FrameInput.Mouse.Deltas.x;
		int32_t																		mouseY											= instanceGame.FrameInput.Mouse.Deltas.y;

		const ::gpk::SCoord2<uint32_t>												& metricsDisplayGlobal							= instanceGame.GlobalDisplay	.Screen.metrics();
		const ::gpk::SCoord2<uint32_t>												& metricsDisplayTactical						= instanceGame.TacticalDisplay	.Screen.metrics();
		int32_t																		tacticalDisplayX								= (metricsDisplayGlobal.x >> 1)	- (metricsDisplayTactical.x >> 1);
		//int32_t																		tacticalDisplayStop								= TACTICAL_DISPLAY_POSY		+ (tacticalDisplay.Depth);
		int32_t																		tacticalMouseX									= mouseX-tacticalDisplayX;
		int32_t																		tacticalMouseY									= mouseY - TACTICAL_DISPLAY_POSY;

		bool																		 bInArea
			=  tacticalMouseX >= 0 && tacticalMouseX < (int32_t)metricsDisplayTactical.x
			&& tacticalMouseY >= 0 && tacticalMouseY < (int32_t)metricsDisplayTactical.y
			;
		if(false == bInArea)
			return false;

		if(tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer] != ::klib::PLAYER_INDEX_USER)
			return false;

		::klib::STacticalBoard														& tacticalBoard									= tacticalInfo.Board;

		if(playerSelection.PlayerUnit != -1 && currentPlayer.Squad.Agents[playerSelection.PlayerUnit] != -1) {
			::klib::CCharacter															& currentAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[playerSelection.PlayerUnit]];
			const ::gpk::SCoord3<int32_t>												& currentAgentPosition							= currentAgent.Position;
			int32_t																		targetPlayerIndex								= tacticalBoard.Tiles.Entities.Agents[tacticalMouseY][tacticalMouseX].PlayerIndex;
			int32_t																		agentIndex										= tacticalBoard.Tiles.Entities.Agents[tacticalMouseY][tacticalMouseX].AgentIndex;
			if(0 != instanceGame.FrameInput.Mouse.Buttons[0]) {// || 0 != instanceGame.FrameInput.MouseButtons[4])
				currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]	= {tacticalMouseX, currentAgentPosition.y, tacticalMouseY};
				if(	targetPlayerIndex != -1 && agentIndex != -1 ) {
					if(targetPlayerIndex == tacticalInfo.CurrentPlayer && currentPlayer.Army[currentPlayer.Squad.Agents[agentIndex]]->IsAlive() && false == currentPlayer.Army[currentPlayer.Squad.Agents[agentIndex]]->DidLoseTurn() && (0 == instanceGame.FrameInput.Keys[VK_CONTROL] && 0 == instanceGame.FrameInput.Keys[VK_LCONTROL])) {
						playerSelection.PlayerUnit												= (int16_t)agentIndex;
						playerSelection.PlayerSquad												= 0;
						if(playerSelection.TargetPlayer != -1 && playerSelection.TargetUnit != -1) {
							::klib::STacticalPlayer														& playerTarget										= instanceGame.Players[tacticalInfo.Setup.Players[playerSelection.TargetPlayer]].Tactical;
							if(playerTarget.Squad.Agents[playerSelection.TargetUnit] != -1)
								currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]	= playerTarget.Army[playerTarget.Squad.Agents[playerSelection.TargetUnit]]->Position;
							else
								playerSelection.ClearTarget();
						}
					}
					else {
						playerSelection.TargetPlayer												= (int16_t)targetPlayerIndex;
						playerSelection.TargetSquad													= 0;
						playerSelection.TargetUnit													= (int16_t)agentIndex;
						currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]	= {tacticalMouseX, currentAgentPosition.y, tacticalMouseY};
					}
				}
				else {
					playerSelection.ClearTarget();
					currentPlayer.Squad.TargetPositions[playerSelection.PlayerUnit]	= {tacticalMouseX, currentAgentPosition.y, tacticalMouseY};
				}
			}
		}
	}

	return false;
};

void																	drawTileInfo
	(	const ::klib::SGame			& instanceGame
	,	const ::klib::SInput		& frameInput
	,	const ::klib::STacticalInfo	& tacticalInfo
	,	::gpk::SRenderTarget<char, uint16_t>	& globalDisplay
	,	::gpk::SRenderTarget<char, uint16_t>	& tacticalDisplay
	)
{
	static	::gpk::array_pod<char_t>											selectedTile									= "";
	static	::klib::SMessageSlow												messageSlow										= {};
	static	uint16_t															messageColor									= ::klib::ASCII_COLOR_INDEX_DARKGREEN;

	int32_t																		tacticalDisplayStop								= TACTICAL_DISPLAY_POSY + (tacticalDisplay.metrics().y);

	bool																		bDrawText										= false;
	int32_t																		mouseX											= frameInput.Mouse.Deltas.x;
	int32_t																		mouseY											= frameInput.Mouse.Deltas.y;
	int32_t																		tacticalDisplayX								= (globalDisplay.metrics().x >> 1)	- (tacticalDisplay.metrics().x >> 1);
	int32_t																		boardX											= mouseX-tacticalDisplayX;
	int32_t																		boardZ											= mouseY-TACTICAL_DISPLAY_POSY;

	bool																		bInTacticalMap									= false;
	if( mouseX >= tacticalDisplayX && mouseX < (int32_t)(tacticalDisplayX+tacticalDisplay.metrics().x)
		 && mouseY >= TACTICAL_DISPLAY_POSY && mouseY < tacticalDisplayStop
		)
	{
		bInTacticalMap															= true;
		int32_t																		playerIndex										= tacticalInfo.Board.Tiles.Entities.Agents[boardZ][boardX].PlayerIndex;
		int32_t																		agentIndex										= tacticalInfo.Board.Tiles.Entities.Agents[boardZ][boardX].AgentIndex;
		int32_t																		terrainHeight									= tacticalInfo.Board.Tiles.Terrain.Topology[boardZ][boardX].Sharp+tacticalInfo.Board.Tiles.Terrain.Topology[boardZ][boardX].Smooth;

		if(playerIndex	!= -1 && agentIndex != -1 && tacticalInfo.Setup.Players[playerIndex] != -1) {
			const ::klib::STacticalPlayer												& boardPlayer									= instanceGame.Players[tacticalInfo.Setup.Players[playerIndex]].Tactical;
			bool																		bDarken											= !	( boardPlayer.Control.Type == ::klib::PLAYER_CONTROL_AI
																																		 && ( (boardPlayer.Control.AIMode == ::klib::PLAYER_AI_ASSISTS)
																																		   || (boardPlayer.Control.AIMode == ::klib::PLAYER_AI_RIOTERS)
																																		   || (boardPlayer.Control.AIMode == ::klib::PLAYER_AI_VIOLENT)
																																			)
																																		);
			messageColor															= ::klib::getPlayerColor(tacticalInfo, boardPlayer, (int8_t)playerIndex, ::klib::PLAYER_INDEX_USER, bDarken);

			selectedTile															= boardPlayer.Army[boardPlayer.Squad.Agents[agentIndex]]->Name;
			bDrawText																= true;
		}
		else if(tacticalInfo.Board.Tiles.Entities.Coins[boardZ][boardX]) {
			selectedTile = ::gpk::view_const_string{"Coins: "};
			char																		tileString	[64];
			sprintf_s(tileString, "%lli", tacticalInfo.Board.Tiles.Entities.Coins[boardZ][boardX]);
			selectedTile.append_string(tileString);
			messageColor															= ::klib::ASCII_COLOR_INDEX_ORANGE;
			bDrawText																= true;
		}
		else if(tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Definition != -1) {
			::gpk::array_pod<char_t>													stagePropName									= ::klib::getEntityName(instanceGame.EntityTables.StageProp,
				{	tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Definition
				,	tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Modifier
				,	tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Level
				,	-1
				});
			selectedTile															= stagePropName;
			if(tacticalInfo.Board.Tiles.Entities.Props[boardZ][boardX].Level == -1)
				selectedTile.append_string(" (Destroyed)");
			messageColor															= ::klib::ASCII_COLOR_INDEX_DARKGREY;
			bDrawText																= true;
		}
		else if ( terrainHeight
			||	0 != tacticalInfo.Board.Tiles.Terrain.Geometry[boardZ][boardX].fHeight[0]
			||	0 != tacticalInfo.Board.Tiles.Terrain.Geometry[boardZ][boardX].fHeight[1]
			||	0 != tacticalInfo.Board.Tiles.Terrain.Geometry[boardZ][boardX].fHeight[2]
			||	0 != tacticalInfo.Board.Tiles.Terrain.Geometry[boardZ][boardX].fHeight[3]
			)
		{
			messageColor															= ::klib::ASCII_COLOR_INDEX_DARKGREY;
			char																		heightStr[128]									= {};
			sprintf_s(heightStr, "Tile heights: {%f,%f,%f,%f}"
				, tacticalInfo.Board.Tiles.Terrain.Geometry[boardZ][boardX].fHeight[0]
				, tacticalInfo.Board.Tiles.Terrain.Geometry[boardZ][boardX].fHeight[1]
				, tacticalInfo.Board.Tiles.Terrain.Geometry[boardZ][boardX].fHeight[2]
				, tacticalInfo.Board.Tiles.Terrain.Geometry[boardZ][boardX].fHeight[3]
			);
			selectedTile															= ::gpk::view_const_string{heightStr};
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
		if(selectedTile.size())
			::klib::getMessageSlow(messageSlow, selectedTile, instanceGame.FrameTimer.LastTimeSeconds * 4);
	}

	if(bDrawText)
		::klib::lineToGridColored(globalDisplay.Color, globalDisplay.DepthStencil, messageColor, tacticalDisplayStop + 1, 0, ::klib::SCREEN_CENTER, messageSlow.Message);

	::klib::printfToGridColored(globalDisplay.Color.View, globalDisplay.DepthStencil, bInTacticalMap ? messageColor : ::klib::ASCII_COLOR_INDEX_DARKGREY, tacticalDisplayStop + 1, tacticalDisplayX	+ 1, ::klib::SCREEN_LEFT, "%i, %i", mouseX-tacticalDisplayX, mouseY-TACTICAL_DISPLAY_POSY);
}

void																drawPlayerInfo									(::klib::SGame& instanceGame)																{
	static bool																	bSwap											= false;
	static ::klib::STimer														animationTimer;
	static ::klib::SAccumulator<double>											animationAccum									= {0.0, 0.1};

	animationTimer.Frame();
	if( animationAccum.Accumulate(animationTimer.LastTimeSeconds) ) {
		bSwap																	= !bSwap;
		animationAccum.Value													= 0;
	}
	::gpk::SRenderTarget<char, uint16_t>										& displayGlobal									= instanceGame.GlobalDisplay	.Screen;
	::gpk::SRenderTarget<char, uint16_t>										& displayTactical								= instanceGame.TacticalDisplay	.Screen;

	const ::gpk::SCoord2<uint32_t>												& metricsDisplayGlobal							= displayGlobal  .metrics();
	const ::gpk::SCoord2<uint32_t>												& metricsDisplayTactical						= displayTactical.metrics();
	//const int32_t																tacticalDisplayStop								= TACTICAL_DISPLAY_POSY			+ (metricsDisplayTactical.y);
	const int32_t																tacticalDisplayX								= (metricsDisplayGlobal.x >> 1)	- (metricsDisplayTactical.x >> 1);
	const ::klib::STacticalInfo													& tacticalInfo									= instanceGame.TacticalInfo;

	const ::klib::PLAYER_INDEX													currentPlayerIndex								= ::klib::getCurrentPlayerIndex(tacticalInfo);
	if(currentPlayerIndex == ::klib::PLAYER_INDEX_INVALID)
		return;

#define PLAYER_INFO_POSY 1

	uint16_t																	messageColor									= ::klib::ASCII_COLOR_INDEX_DARKGREEN;
	::gpk::array_pod<char_t>													selectionText									= {};
	::klib::STacticalPlayer														& currentPlayer									= instanceGame.Players[currentPlayerIndex].Tactical;
	const int32_t																selectionPlayerUnit								= currentPlayer.Selection.PlayerUnit;
	if( selectionPlayerUnit != -1
	 && currentPlayer.Squad.Agents[selectionPlayerUnit] != -1
	 && currentPlayer.Army[currentPlayer.Squad.Agents[selectionPlayerUnit]]->IsAlive()
	 )
	{
		bool																		bDarken											= !	( currentPlayer.Control.Type == ::klib::PLAYER_CONTROL_AI
																																	 && ( (currentPlayer.Control.AIMode == ::klib::PLAYER_AI_ASSISTS)
																																	   || (currentPlayer.Control.AIMode == ::klib::PLAYER_AI_RIOTERS)
																																	   || (currentPlayer.Control.AIMode == ::klib::PLAYER_AI_VIOLENT)
																																		)
																																	);
		messageColor															= ::klib::getPlayerColor(tacticalInfo, currentPlayer, tacticalInfo.CurrentPlayer, ::klib::PLAYER_INDEX_USER, bDarken);
		if(selectionPlayerUnit != -1 && currentPlayer.Squad.Agents[selectionPlayerUnit] != -1) {
			::klib::CCharacter															& playerAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[selectionPlayerUnit]];
			::klib::displayDetailedAgentSlot	(instanceGame.EntityTables, displayGlobal.Color, displayGlobal.DepthStencil, PLAYER_INFO_POSY, 4, playerAgent, messageColor);
			::klib::displayStatusEffectsAndTechs(displayGlobal.Color, displayGlobal.DepthStencil, PLAYER_INFO_POSY + 36, 4, playerAgent);
		}

		selectionText	= ::gpk::view_const_string{"Player name: "};	selectionText.append(currentPlayer.Name);
		::klib::lineToGridColored(displayGlobal.Color, displayGlobal.DepthStencil, messageColor, 1, tacticalDisplayX+1, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());
		selectionText	= ::gpk::view_const_string{"Agent name: "};		selectionText.append(currentPlayer.Army[currentPlayer.Squad.Agents[selectionPlayerUnit]]->Name);
		::klib::lineToGridColored(displayGlobal.Color, displayGlobal.DepthStencil, messageColor, 2, tacticalDisplayX+1, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());
		selectionText															= ::gpk::view_const_string{"Moves left: "};
		char				number[65];
		sprintf_s(number, "%i", currentPlayer.Squad.ActionsLeft[selectionPlayerUnit].Moves);
		selectionText.append_string(number);
		selectionText.append_string(". Actions left: ");
		sprintf_s(number, "%i", currentPlayer.Squad.ActionsLeft[selectionPlayerUnit].Actions);
		selectionText.append_string(number);
		selectionText.append_string(".");
		::klib::lineToGridColored(displayGlobal.Color, displayGlobal.DepthStencil, messageColor, 3, tacticalDisplayX + 1, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());

		int32_t																		selectionX										= currentPlayer.Squad.TargetPositions[selectionPlayerUnit].x;
		int32_t																		selectionZ										= currentPlayer.Squad.TargetPositions[selectionPlayerUnit].z;
		selectionText															= ::gpk::view_const_string{"Target Position: "};
		sprintf_s(number, "%i", selectionX);
		selectionText.append_string(number);
		selectionText.append_string(", ");
		sprintf_s(number, "%i", selectionZ);
		selectionText.append_string(number);
		lineToGridColored(displayGlobal.Color, displayGlobal.DepthStencil, messageColor, 4, tacticalDisplayX+1, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());

		if(bSwap && currentPlayer.Squad.TargetPositions[selectionPlayerUnit] != currentPlayer.Army[currentPlayer.Squad.Agents[selectionPlayerUnit]]->Position) {
			selectionText															= ::gpk::view_const_string{"-"};
			if(selectionX-1 >= 0)
				lineToGridColored(displayTactical.Color	, displayTactical.DepthStencil, messageColor, selectionZ, selectionX-1, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());
			if(selectionX + 1 < (int32_t)displayTactical.metrics().x)
				lineToGridColored(displayTactical.Color	, displayTactical.DepthStencil, messageColor, selectionZ, selectionX+1, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());

			selectionText															= ::gpk::view_const_string{"|"};
			if(selectionZ-1 >= 0)
				lineToGridColored(displayTactical.Color	, displayTactical.DepthStencil, messageColor, selectionZ-1, selectionX, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());
			if(selectionZ+1 < (int32_t)displayTactical.metrics().y)
				lineToGridColored(displayTactical.Color	, displayTactical.DepthStencil, messageColor, selectionZ+1, selectionX, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());
		}
	}

	if(currentPlayer.Selection.TargetPlayer == -1 || tacticalInfo.Setup.Players[currentPlayer.Selection.TargetPlayer] == -1)
		return;

	::klib::STacticalPlayer															& currentTarget									= instanceGame.Players[tacticalInfo.Setup.Players[currentPlayer.Selection.TargetPlayer]].Tactical;
	bool																		bDarken											= !	( currentTarget.Control.Type == ::klib::PLAYER_CONTROL_AI
																																&&	(	(currentTarget.Control.AIMode == ::klib::PLAYER_AI_ASSISTS)
																																	||	(currentTarget.Control.AIMode == ::klib::PLAYER_AI_RIOTERS)
																																	||	(currentTarget.Control.AIMode == ::klib::PLAYER_AI_VIOLENT)
																																	)
																																);

	messageColor															= ::klib::getPlayerColor(tacticalInfo, currentTarget, (int8_t)currentPlayer.Selection.TargetPlayer, ::klib::PLAYER_INDEX_USER, bDarken);

	selectionText															= ::gpk::view_const_string{"Target player: "};
	selectionText.append(currentTarget.Name);
	lineToGridColored(displayGlobal.Color, displayGlobal.DepthStencil, messageColor, 2, tacticalDisplayX+1, ::klib::SCREEN_RIGHT, selectionText.begin());

	if( currentPlayer.Selection.TargetUnit == -1 || currentTarget.Squad.Agents[currentPlayer.Selection.TargetUnit] == -1 )
		return;

	::klib::CCharacter															& targetAgent									= *currentTarget.Army[currentTarget.Squad.Agents[currentPlayer.Selection.TargetUnit]];

	int32_t																		agentX											= targetAgent.Position.x;
	int32_t																		agentZ											= targetAgent.Position.z;
	if(bSwap) {
		selectionText															= "-";
		if(agentX-1 >= 0)
			::klib::lineToGridColored(displayTactical.Color, displayTactical.DepthStencil	, messageColor, agentZ, agentX-1, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());
		if(agentX+1 < (int32_t)displayTactical.metrics().x)
			::klib::lineToGridColored(displayTactical.Color, displayTactical.DepthStencil	, messageColor, agentZ, agentX+1, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());

		selectionText															= "|";
		if(agentZ-1 >= 0)
			::klib::lineToGridColored(displayTactical.Color, displayTactical.DepthStencil	, messageColor, agentZ-1, agentX, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());
		if(agentZ+1 < (int32_t)displayTactical.metrics().y)
			::klib::lineToGridColored(displayTactical.Color, displayTactical.DepthStencil	, messageColor, agentZ+1, agentX, ::klib::SCREEN_LEFT, selectionText.begin(), (uint32_t)selectionText.size());
	}

	if( !targetAgent.IsAlive() )
		return;

	int32_t																		xOffset											= tacticalDisplayX + displayTactical.metrics().x + 4;
	::klib::displayDetailedAgentSlot	(instanceGame.EntityTables, displayGlobal.Color, displayGlobal.DepthStencil, PLAYER_INFO_POSY, xOffset, targetAgent, messageColor);
	::klib::displayStatusEffectsAndTechs(displayGlobal.Color, displayGlobal.DepthStencil, PLAYER_INFO_POSY + 36, xOffset, targetAgent);
	selectionText															= ::gpk::view_const_string{"Target: "};
	selectionText.append(targetAgent.Name);
	::klib::lineToGridColored(displayGlobal.Color, displayGlobal.DepthStencil, messageColor, 3, tacticalDisplayX+1, ::klib::SCREEN_RIGHT, selectionText.begin());
}

bool																	shoot											(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players, int32_t tacticalPlayer, int32_t squadAgent, ::klib::SGameMessages & messages)					{
	::klib::SGamePlayer																& playerShooter									= players[tacticalInfo.Setup.Players[tacticalPlayer]];
	::klib::CCharacter															& agentShooter									= *playerShooter.Tactical.Army[playerShooter.Tactical.Squad.Agents[squadAgent]];

	const ::gpk::SCoord3<int32_t>												& targetTile									= playerShooter.Tactical.Squad.TargetPositions[squadAgent];
	if(targetTile == agentShooter.Position)
		return false;

	int32_t																		totalBullets									= ::gpk::bit_true(agentShooter.FinalFlags.Tech.AttackType, ::klib::ATTACK_TYPE_SPLASH) ? 10 : 1;
	if(::gpk::bit_true(agentShooter.FinalFlags.Tech.AttackType, ::klib::ATTACK_TYPE_BURST))
		totalBullets															*= 3;

	//int																			finalChance										= agentShooter.FinalPoints.Attack.Hit;
	bool																		bIsBlind										= true_if(agentShooter.ActiveBonus.Status.Status & ::klib::COMBAT_STATUS_BLIND);
	if(bIsBlind) {
		sprintf_s(messages.Aux, "Blindness causes %s to have %u hit chance for this turn.", agentShooter.Name.begin(), agentShooter.FinalPoints.Attack.Hit >>= 1);
		messages.LogAuxMessage();
	}

	//if ((rand() % 100) < finalChance ) {
		 if(::gpk::bit_true(agentShooter.FinalFlags.Tech.AttackType			, ::klib::ATTACK_TYPE_SPLASH		)) PlaySound("..\\gpk_data\\sounds\\Shotgun_Blast-Jim_Rogers-1914772763.wav"		, 0, SND_ASYNC | SND_FILENAME);
	else if(::gpk::bit_true(agentShooter.FinalFlags.Tech.ProjectileClass	, ::klib::PROJECTILE_CLASS_ROCKET	)) PlaySound("..\\gpk_data\\sounds\\Missle_Launch-Kibblesbob-2118796725.wav"		, 0, SND_ASYNC | SND_FILENAME);
	else if(::gpk::bit_true(agentShooter.FinalFlags.Tech.ProjectileClass	, ::klib::PROJECTILE_CLASS_GRENADE	)) PlaySound("..\\gpk_data\\sounds\\grenade-launcher-daniel_simon.wav"				, 0, SND_ASYNC | SND_FILENAME);
	else if(::gpk::bit_true(agentShooter.FinalFlags.Tech.ProjectileClass	, ::klib::PROJECTILE_CLASS_BULLET	)) PlaySound("..\\gpk_data\\sounds\\Anti Aircraft Gun-Mike_Koenig-1303768514.wav"	, 0, SND_ASYNC | SND_FILENAME);
	else if(::gpk::bit_true(agentShooter.FinalFlags.Tech.ProjectileClass	, ::klib::PROJECTILE_CLASS_RAY		)) PlaySound("..\\gpk_data\\sounds\\Gun_Shot-Marvin-1140816320.wav"					, 0, SND_ASYNC | SND_FILENAME);

	for(int32_t iBullet=0; iBullet<totalBullets; ++iBullet) {
		::gpk::bit_clear(playerShooter.Tactical.Squad.AgentStates[squadAgent], ::klib::AGENT_STATE_MOVE);
		::klib::SBullet																newBullet;
		newBullet.Position.Cell													= {agentShooter.Position.x, agentShooter.Position.y, agentShooter.Position.z};
		newBullet.Position.Offset												= {.5f, .75f, .5f};
		newBullet.Direction														= {(float)(targetTile.x-newBullet.Position.Cell.x), (float)(targetTile.y-newBullet.Position.Cell.y), (float)(targetTile.z-newBullet.Position.Cell.z)};
		if(::gpk::bit_true(agentShooter.FinalFlags.Tech.AttackType, ::klib::ATTACK_TYPE_SPLASH)) {
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
		newBullet.Points.Damage													= agentShooter.FinalPoints.Attack.Damage		/ ::gpk::max(1, (totalBullets/2));
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
	playerShooter.Tactical.Squad.TargetPositions[squadAgent]							= agentShooter.Position;
	return true;
}

		::klib::TURN_ACTION												selectAIAction									(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players);
inline	::klib::TURN_ACTION												selectRemoteAction								(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players)	{
	return selectAIAction(tacticalInfo, players);
}

enum CHARACTER_TURN_ACTION
	{ CHARACTER_TURN_ACTION_CANCELED
	, CHARACTER_TURN_ACTION_CONTINUE
	, CHARACTER_TURN_ACTION_INVENTORY
	};

static CHARACTER_TURN_ACTION											characterTurn									(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players, ::klib::TURN_ACTION combatOption, ::klib::SGameMessages & messages)	{
	::klib::SGamePlayer																& currentPlayer									= players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];
	//bool																		bNotCanceled									= true;
	CHARACTER_TURN_ACTION														result											= CHARACTER_TURN_ACTION_CONTINUE;
	const int32_t																selectionPlayerUnit								= currentPlayer.Tactical.Selection.PlayerUnit;
	if(combatOption == ::klib::TURN_ACTION_ATTACK && currentPlayer.Tactical.Squad.ActionsLeft[selectionPlayerUnit].Actions > 0) {
		::klib::CCharacter															& currentAgent									= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[selectionPlayerUnit]];
		::klib::CCharacter															& playerAgent									= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[selectionPlayerUnit]];
		messages.ClearMessages();
		if(::gpk::bit_true(currentAgent.FinalFlags.Tech.AttackType, ::klib::ATTACK_TYPE_RANGED)) {
			if(true_if(playerAgent.ActiveBonus.Status.Status & ::klib::COMBAT_STATUS_BLACKOUT) && (::klib::getEntityFlags(entityTables.Weapon, playerAgent.CurrentEquip.Weapon).Tech.Tech & ::klib::ENTITY_TECHNOLOGY_DIGITAL)) {
				messages.UserMessage												= ::gpk::view_const_string{"This weapon was disabled by an electromagnetic pulse."};
				messages.LogMessage();
			}
			else if(::shoot(tacticalInfo, players, tacticalInfo.CurrentPlayer, selectionPlayerUnit, messages)) {
				messages.UserSuccess												= playerAgent.Name;
				messages.UserSuccess.append_string(" shoots!");
				messages.LogSuccess();
				--currentPlayer.Tactical.Squad.ActionsLeft[selectionPlayerUnit].Actions;
			}
			else {
				messages.UserMiss													= ::gpk::view_const_string{"You need to select a valid target in order to attack!"};
				messages.LogMiss();
			}
		}
		else if(::gpk::bit_true(currentAgent.FinalFlags.Tech.AttackType, ::klib::ATTACK_TYPE_MELEE)) {
			if(currentPlayer.Tactical.Selection.TargetPlayer == -1 || tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer] == ::klib::PLAYER_INDEX_INVALID || currentPlayer.Tactical.Selection.TargetUnit == -1) {
				messages.UserMiss													= ::gpk::view_const_string{"You need to select a valid target in order to attack!"};
				messages.LogMiss();
			}
			else {
				::klib::SGamePlayer																& targetPlayer									= players[tacticalInfo.Setup.Players[currentPlayer.Tactical.Selection.TargetPlayer]];
				::klib::CCharacter															& targetAgent									= *targetPlayer	.Tactical.Army[targetPlayer.Tactical.Squad.Agents[currentPlayer.Tactical.Selection.TargetUnit]];
				//CCharacter																	& playerAgent									= *currentPlayer.Army[currentPlayer.Squad.Agents[currentPlayer.Selection.PlayerUnit]];

				::gpk::SCoord3<int32_t>														coordPlayer										= playerAgent.Position;
				::gpk::SCoord3<int32_t>														coordTarget										= targetAgent.Position;
				::gpk::SCoord3<float>														distance										= (coordTarget-coordPlayer).Cast<float>();

				const ::klib::SEntityPoints													& playerAgentPoints								= playerAgent.FinalPoints;
				const ::klib::SEntityFlags													& playerAgentFlags								= playerAgent.FinalFlags;

				double																		finalSight										= ::klib::getFinalRange(playerAgent, playerAgentPoints);
				double																		finalRange										= ::klib::getFinalSight(playerAgent, playerAgentPoints);
				if( (distance.Length() > finalSight || distance.Length() > finalRange)
				 && ::gpk::bit_false(playerAgentFlags.Tech.AttackType, ::klib::ATTACK_TYPE_RANGED)
				 )
				{
					messages.UserMiss													= ::gpk::view_const_string{"You can't attack "};
					messages.UserMiss.append(targetAgent.Name);
					messages.UserMiss.append_string(" from that distance.");
					messages.LogMiss();
				}
				else {
					char																		formatCoords[256]								= {};
					sprintf_s(formatCoords, "{x=%i, y=%i, z=%i}", coordTarget.x, coordTarget.y, coordTarget.z);
					::gpk::array_pod<char_t>													targetType										= ::gpk::view_const_string{"Target Coords: "};
					targetType.append_string(formatCoords);
					targetType.append_string(". Target player index: ");
					sprintf_s(formatCoords, "%i", currentPlayer.Tactical.Selection.TargetPlayer);
					targetType.append_string(formatCoords);
					targetType.append_string(". Target player unit: ");
					sprintf_s(formatCoords, "%i", currentPlayer.Tactical.Selection.TargetUnit);
					targetType.append_string(formatCoords);
					targetType.append_string(". Target player name: ");
					targetType.append(targetPlayer.Tactical.Name);
					sprintf_s(formatCoords, "{x=%i, y=%i, z=%i}", coordPlayer.x, coordPlayer.y, coordPlayer.z);
					::gpk::array_pod<char_t>													userType										= ::gpk::view_const_string{"Origin Coords: "};
					userType.append_string(formatCoords);
					sprintf_s(formatCoords, ". Current player index: %i", tacticalInfo.CurrentPlayer);
					userType.append_string(formatCoords);
					userType.append_string(". Current player unit: ");
					sprintf_s(formatCoords, "%i", currentPlayer.Tactical.Selection.PlayerUnit);
					userType.append_string(". Current player name: ");
					userType.append(currentPlayer.Tactical.Name);

					static const HANDLE															hConsoleOut										= GetStdHandle( STD_OUTPUT_HANDLE );
					sprintf_s(messages.Aux, "%s.", targetType.begin());
					messages.LogAuxMessage();
					sprintf_s(messages.Aux, "%s.", userType.begin());
					messages.LogAuxMessage();

					::klib::attack(entityTables, messages, playerAgent, targetAgent);

					if(!targetAgent.IsAlive())
						::klib::handleAgentDeath(tacticalInfo, players, targetAgent, playerAgent, tacticalInfo.Setup.TeamPerPlayer[currentPlayer.Tactical.Selection.TargetPlayer], messages);
					if(!playerAgent.IsAlive()) {
						::klib::handleAgentDeath(tacticalInfo, players, playerAgent, targetAgent, tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer], messages);
						result															= ::CHARACTER_TURN_ACTION_CANCELED;
						endTurn(entityTables, tacticalInfo, players, messages);
					}

					--currentPlayer.Tactical.Squad.ActionsLeft[selectionPlayerUnit].Actions;
					//getchar();
				}
			}
		}// if(melee)

	} // if(TURN_ACTION_ATTACK)
	else if(combatOption == ::klib::TURN_ACTION_INVENTORY)
		result = ::CHARACTER_TURN_ACTION_INVENTORY;
	else if(combatOption == ::klib::TURN_ACTION_CANCEL) {
		messages.UserMessage												= currentPlayer.Tactical.Name;
		messages.UserMessage.append_string(" canceled the turn.");
		messages.LogMessage();
		result															= ::CHARACTER_TURN_ACTION_CANCELED;
		::klib::endTurn(entityTables, tacticalInfo, players, messages);
	}
	else if(combatOption == ::klib::TURN_ACTION_MOVE) {
		char																		buffer[128]										= {};
		::klib::CCharacter															& playerAgent									= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[selectionPlayerUnit]];
		//bool																		bSuccess										= false;
		if(tacticalInfo.Board.Tiles.IsTileAvailable(currentPlayer .Tactical.Squad.TargetPositions[selectionPlayerUnit].x, currentPlayer.Tactical.Squad.TargetPositions[selectionPlayerUnit].z)) {
			currentPlayer.Tactical.Squad.AgentStates[selectionPlayerUnit]			= ::klib::AGENT_STATE_MOVE;
			sprintf_s(messages.Aux, "Player %s moves %s to {%i, %i, %i}."
				, currentPlayer.Tactical.Name.begin()
				, playerAgent.Name.begin()
				, currentPlayer.Tactical.Squad.TargetPositions[selectionPlayerUnit].x
				, currentPlayer.Tactical.Squad.TargetPositions[selectionPlayerUnit].y
				, currentPlayer.Tactical.Squad.TargetPositions[selectionPlayerUnit].z
			);
			messages.LogAuxSuccess();
		}
		else {
			sprintf_s(buffer, "Cannot move to {%i, %i, %i}. Terrain occupied or unreachable."
				, currentPlayer.Tactical.Squad.TargetPositions[selectionPlayerUnit].x
				, currentPlayer.Tactical.Squad.TargetPositions[selectionPlayerUnit].y
				, currentPlayer.Tactical.Squad.TargetPositions[selectionPlayerUnit].z
			);
			messages.UserError													= ::gpk::view_const_string{buffer};
			messages.LogError();
		}
	}
	else if(combatOption != ::klib::TURN_ACTION_CONTINUE) {
		messages.UserError													= "This function isn't available!";
		messages.LogError();
	}

	return result;
}

template <size_t _Size1>
int32_t																	initInventoryMenu								(::klib::CCharacter& adventurer, klib::SMenuItem<int32_t> (&itemOptions)[_Size1], bool bPrintPrice = false, bool bSellPrice = true)	{
	char																		itemOption[128]									= {};

	for(uint32_t i=0; i<adventurer.Goods.Inventory.Items.Slots.size(); ++i) {
		const ::klib::SItem															& itemEntity									= adventurer.Goods.Inventory.Items[i].Entity;
		::gpk::array_pod<char_t>													itemName										= ::klib::getItemName(itemEntity);

		if(bPrintPrice) {
			int32_t																		finalPrice										= ::klib::getItemPrice(itemEntity, bSellPrice);
			sprintf_s(itemOption, "%i coins each - x%.2u %s", finalPrice, adventurer.Goods.Inventory.Items[i].Count, itemName.begin());
		}
		else
			sprintf_s(itemOption, "- x%.2u %s", adventurer.Goods.Inventory.Items[i].Count, itemName.begin());

		itemOptions[i].ReturnValue												= i;
 		itemOptions[i].Text														= itemOption;
	}

	return 0;
}

int32_t																	drawInventoryMenu								(::klib::SGame& instanceGame, ::klib::CCharacter& adventurer, const ::gpk::view_const_char& menuTitle)								{
	::klib::SMenuItem<int32_t>													itemOptions[4096]								= {};
	::gpk::SRenderTarget<char, uint16_t>										& globalDisplay									= instanceGame.GlobalDisplay.Screen;
	::klib::SCharacterInventory													& characterInventory							= adventurer.Goods.Inventory;
	//int32_t																		initMenu										=
		::initInventoryMenu(adventurer, itemOptions, false, false);
	static ::klib::SDrawMenuState												menuState;
	::gpk::view_array<const ::klib::SMenuItem<int32_t>>							menuItems										= {itemOptions, characterInventory.Items.Slots.size()};
	int32_t																		countMenuItems									= menuItems.size();
	return ::klib::drawMenu(menuState, globalDisplay.Color.View, globalDisplay.DepthStencil.begin(), menuTitle, menuItems, instanceGame.FrameInput, countMenuItems, -1, 50);

};

::klib::TURN_ACTION														useItems										(::klib::SGame& instanceGame, ::klib::SGameMessages & messages, ::klib::SGamePlayer& player, ::klib::CCharacter& user, const ::gpk::view_const_char& menuTitle, bool bIsAIControlled) {
	int32_t																		indexInventory									= ~0U;
	bool																		bUsedItem										= false;
	if(!bIsAIControlled)  {
		indexInventory															= ::drawInventoryMenu(instanceGame, user, menuTitle);
		if(indexInventory < (int32_t)user.Goods.Inventory.Items.Slots.size())
			bUsedItem																= true;
	}
	else { // not a player so execute choice by AI
		//indexInventory = selectItemsAI(user, target);
		indexInventory															= user.Goods.Inventory.Items.Slots.size();
		if(indexInventory < (int32_t)user.Goods.Inventory.Items.Slots.size())
			bUsedItem																= true;
	}

	if(indexInventory == (int32_t)user.Goods.Inventory.Items.Slots.size())
		return ::klib::TURN_ACTION_MAIN;
	else if(indexInventory == -1)
		return ::klib::TURN_ACTION_CONTINUE;

	if(bUsedItem) {
		const ::klib::SItem															& entityItem									= user.Goods.Inventory.Items[indexInventory].Entity;
		const ::klib::CItem															& itemDescription								= ::klib::itemDescriptions[entityItem.Definition];
		const ::klib::SEntityPoints													& userFinalPoints								= user.FinalPoints;
		// Only use potions if we have less than 60% HP
		if( ::klib::ITEM_TYPE_POTION == itemDescription.Type
			&&  (	(::klib::PROPERTY_TYPE_HEALTH	== itemDescription.Property && user.Points.LifeCurrent.Health	>= userFinalPoints.LifeMax.Health	)
				||	(::klib::PROPERTY_TYPE_SHIELD	== itemDescription.Property && user.Points.LifeCurrent.Shield	>= userFinalPoints.LifeMax.Shield	)
				||	(::klib::PROPERTY_TYPE_MANA		== itemDescription.Property && user.Points.LifeCurrent.Mana		>= userFinalPoints.LifeMax.Mana		)
				)
			) {
			messages.UserMessage												= ::gpk::view_const_string{"You don't need to use "};
			messages.UserMessage.append(::klib::getItemName(entityItem));
			messages.UserMessage.append_string("!");
			instanceGame.LogMessage();
			bUsedItem																= false;
		}
		else
			bUsedItem																= true;
	}

	if(bUsedItem) {
		const ::klib::SItem															& entityItem									= user.Goods.Inventory.Items[indexInventory].Entity;
		const ::klib::CItem															& itemDescription								= ::klib::itemDescriptions[entityItem.Definition];
		if( ::klib::ITEM_TYPE_POTION == itemDescription.Type )
			bUsedItem																= ::klib::executeItem(instanceGame.EntityTables, instanceGame.Messages, indexInventory, user, user);
		else {
		//if( klib::ITEM_TYPE_GRENADE == itemDescription.Type )
		//{
		//
		//}
		//else if( klib::ITEM_TYPE_POTION == itemDescription.Type )
			if(player.Tactical.Selection.TargetPlayer != -1 && player.Tactical.Selection.TargetUnit != -1) {
				::klib::SGamePlayer																& targetPlayer									= instanceGame.Players[instanceGame.TacticalInfo.Setup.Players[player.Tactical.Selection.TargetPlayer]];
				if(targetPlayer.Tactical.Squad.Agents[player.Tactical.Selection.TargetUnit] != -1) {
					::klib::CCharacter															& targetAgent									= *targetPlayer.Tactical.Army[targetPlayer.Tactical.Squad.Agents[player.Tactical.Selection.TargetUnit]];
					bUsedItem																= ::klib::executeItem(instanceGame.EntityTables, instanceGame.Messages, indexInventory, user, targetAgent);
				}
				else {
					bUsedItem																= false;
					::gpk::array_pod<char_t>													itemName										= ::klib::getItemName(entityItem);
					instanceGame.Messages.UserMessage												= ::gpk::view_const_string{"You need to select a valid target in order to use "};
					instanceGame.Messages.UserMessage.append(itemName);
					instanceGame.Messages.UserMessage.append_string("!");
					instanceGame.LogMessage();
				}
			}
			else {
				bUsedItem																= false;
				::gpk::array_pod<char_t>													itemName										= ::klib::getItemName(entityItem);
				instanceGame.Messages.UserMessage												= ::gpk::view_const_string{"You need to select a valid target in order to use "};
				instanceGame.Messages.UserMessage.append(itemName);
				instanceGame.Messages.UserMessage.append_string("!");
				instanceGame.LogMessage();
			}
		}
	}

	if(bUsedItem && 0 >= player.Tactical.Squad.ActionsLeft[player.Tactical.Selection.PlayerUnit].Moves)
		return ::klib::TURN_ACTION_CANCEL;

	return ::klib::TURN_ACTION_CONTINUE;
}

static	::klib::SGameState												endMission										( ::klib::SGame & instanceGame, bool aborted)						{
	::klib::determineOutcome(instanceGame.EntityTables, instanceGame.TacticalInfo, instanceGame.Players, instanceGame.Messages, aborted);						// Determine outcome before exiting tactical mode.
	::gpk::bit_clear(instanceGame.Flags, ::klib::GAME_FLAGS_TACTICAL);	// Tell the system that the tactical mode is over.
	return {::klib::GAME_STATE_WELCOME_COMMANDER};
}


static	void															updateBullets									(::klib::SGame & instanceGame, double secondsLastTick)									{
	::klib::STacticalInfo														& tacticalInfo									= instanceGame.TacticalInfo;
	::gpk::array_pod<::klib::SBullet>											& bullets										= tacticalInfo.Board.Shots.Bullet;
	for(uint32_t iBullet=0; iBullet < bullets.size(); ++iBullet) {
		double																		fSpeed											= 10.0;
		double																		fActualSpeed									= secondsLastTick * fSpeed;
		//double																	fActualSpeed									= 1.0 * fSpeed;
		if(fActualSpeed >= 0.25)
			fActualSpeed															= 0.25;
		::klib::STacticalCoord														& bulletPos										= bullets[iBullet].Position;
		::gpk::SCoord3<float>														& bulletDir										= bullets[iBullet].Direction;
		bulletPos.Offset.AddScaled(bulletDir, fActualSpeed);
		::klib::SBullet																newBullet										= bullets[iBullet];
		::klib::STacticalCoord														& newBulletPos									= newBullet.Position;
		::klib::STacticalCoord														oldBulletPos									= newBullet.Position;

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

			if( newBulletPos.Cell.x >= 0 && newBulletPos.Cell.x < (int32_t)tacticalInfo.Board.Tiles.Terrain.Geometry.metrics().x
				&& newBulletPos.Cell.z >= 0 && newBulletPos.Cell.z < (int32_t)tacticalInfo.Board.Tiles.Terrain.Geometry.metrics().y
			) {
				int32_t																		newx											= newBulletPos.Cell.x;
				int32_t																		newz											= newBulletPos.Cell.z;
				if(tacticalInfo.Board.Tiles.Entities.Agents	[newz][newx].AgentIndex	!= -1	&& tacticalInfo.Board.Tiles.Entities.Agents	[newz][newx].PlayerIndex != -1 ) {
					bImpact																	= true;
					if(tacticalInfo.Setup.Players[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].PlayerIndex] != ::klib::PLAYER_INDEX_INVALID) {
						::klib::SGamePlayer																& playerHit										= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].PlayerIndex]];
						::klib::SGamePlayer																& playerShooter									= instanceGame.Players[tacticalInfo.Setup.Players[newBullet.Shooter.PlayerIndex]];
						if( playerHit.Tactical.Squad.Agents[tacticalInfo.Setup.Players[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].AgentIndex]] != -1 ) {
							::klib::CCharacter															& agentHit										= *playerHit.Tactical.Army[playerHit.Tactical.Squad.Agents[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].AgentIndex]];
							if(agentHit.IsAlive()) {
								::klib::CCharacter															& agentShooter									= *playerShooter.Tactical.Army[playerShooter.Tactical.Squad.Agents[newBullet.Shooter.AgentIndex]];
								char																		agentName[128]									= {};
								sprintf_s(agentName, "Bullet shot by %s.", agentShooter.Name.begin());
 								applySuccessfulWeaponHit(instanceGame.EntityTables, instanceGame.Messages, agentShooter, agentHit, newBullet.Points.Damage, agentName);
								agentShooter	.Score.DamageDealt										+= newBullet.Points.Damage;
								++agentShooter	.Score.AttacksHit;
								agentHit		.Score.DamageTaken										+= newBullet.Points.Damage;
								++agentHit		.Score.AttacksReceived;
								agentShooter	.Recalculate(instanceGame.EntityTables);
								agentHit		.Recalculate(instanceGame.EntityTables);
								if(0 >= agentShooter	.Points.LifeCurrent.Health)	::klib::handleAgentDeath(tacticalInfo, instanceGame.Players, agentShooter	, agentHit		, tacticalInfo.Setup.TeamPerPlayer[newBullet.Shooter.PlayerIndex]									, instanceGame.Messages);
								if(0 >= agentHit		.Points.LifeCurrent.Health)	::klib::handleAgentDeath(tacticalInfo, instanceGame.Players, agentHit		, agentShooter	, tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.Board.Tiles.Entities.Agents[newz][newx].PlayerIndex], instanceGame.Messages);
							}
							else
								tacticalInfo.Board.Tiles.Entities.Agents[newz][newx]					= {::klib::TEAM_TYPE_INVALID, -1, -1, -1};
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
							bullets.push_back(newBullet);
							tacticalInfo.Board.Shots.Coords.push_back(newBulletPos.Cell);
						}
					}
				}
			}
			else
				instanceGame.Messages.UserMessage.clear();

			tacticalInfo.Board.Shots.Bullet.remove_unordered(iBullet);
			tacticalInfo.Board.Shots.Coords.remove_unordered(iBullet);

			if(bImpact
				&&	(	::gpk::bit_true(newBullet.Points.Tech.ProjectileClass, ::klib::PROJECTILE_CLASS_ROCKET	)
					||	::gpk::bit_true(newBullet.Points.Tech.AmmoEffect, ::klib::AMMO_EFFECT_EXPLOSIVE			)
					||	::gpk::bit_true(newBullet.Points.Tech.AmmoEffect, ::klib::AMMO_EFFECT_BLAST				)
					)
				)
			{
				PlaySound("..\\gpk_data\\sounds\\Explosion_Ultra_Bass-Mark_DiAngelo-1810420658.wav", 0, SND_ASYNC | SND_FILENAME);
				::klib::SAOE																newAOE										= {};
				newAOE.Position															= newBullet.Position;
				newAOE.RadiusOrHalfSize													= newBullet.Points.Level;
				newAOE.Caster															= newBullet.Shooter;
				newAOE.Flags.Effect.Attack												= newBullet.Points.Effect;
				newAOE.Flags.Tech														= newBullet.Points.Tech;
				newAOE.StatusInflict													= newBullet.Points.StatusInflict;
				newAOE.Level															= (uint8_t)newBullet.Points.Level;
				newAOE.TurnsLeft														= (uint8_t)newBullet.Points.Level;
				tacticalInfo.AddAOE(newAOE);
				if(::gpk::bit_true(newBullet.Points.Tech.AmmoEffect, ::klib::AMMO_EFFECT_EXPLOSIVE)) {
					int32_t y=0;
					for(int32_t z = (int32_t)(newAOE.Position.Cell.z-newAOE.RadiusOrHalfSize), maxz=int32_t(newAOE.Position.Cell.z+newAOE.RadiusOrHalfSize); z < maxz; ++z) {
						if(z < 0 || z >= (int32_t)tacticalInfo.Board.Tiles.Terrain.Geometry.metrics().y)
							continue;
						for(int32_t x = (int32_t)(newAOE.Position.Cell.x-newAOE.RadiusOrHalfSize), maxx=int32_t(newAOE.Position.Cell.x+newAOE.RadiusOrHalfSize); x < maxx; ++x) {
							if(x < 0 || x >= (int32_t)tacticalInfo.Board.Tiles.Terrain.Geometry.metrics().x)
								continue;

							const ::gpk::SCoord3<int32_t>	currentCoord	= {x, y, z};
							const ::gpk::SCoord3<float>		distance		= (currentCoord-newAOE.Position.Cell).Cast<float>();
							double							length			= distance.Length();
							if((length + 1.0000000000001) > newAOE.RadiusOrHalfSize)
								continue;

							if(tacticalInfo.Board.Tiles.Entities.Props[z][x].Level != -1 )
								tacticalInfo.Board.Tiles.Entities.Props[z][x].Level = -1;
							else {
								double							proportion		= length/newAOE.RadiusOrHalfSize;
								tacticalInfo.Board.Tiles.Terrain.Topology[z][x].Smooth -= (int8_t)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

								::klib::STileGeometry			& tileGeometry	= tacticalInfo.Board.Tiles.Terrain.Geometry[z][x];

								if(proportion > 1.0)
									continue;
								tileGeometry.fHeight[0]		-= (float)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

								length						= ::gpk::SCoord3<float>{distance.x+1, distance.y, distance.z}.Length();
								proportion					= length/newAOE.RadiusOrHalfSize;
								if(proportion <= 1.0)
									tileGeometry.fHeight[1]		-= (float)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

								length						= ::gpk::SCoord3<float>{distance.x, distance.y, distance.z+1}.Length();
								proportion					= length/newAOE.RadiusOrHalfSize;
								if(proportion <= 1.0)
									tileGeometry.fHeight[2]		-= (float)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

								length						= ::gpk::SCoord3<float>{distance.x+1, distance.y, distance.z+1}.Length();
								proportion					= length/newAOE.RadiusOrHalfSize;
								if(proportion <= 1.0)
									tileGeometry.fHeight[3]		-= (float)(newAOE.RadiusOrHalfSize/2*(1.0-proportion));

								if(false) {
									tileGeometry.fHeight[0]		= (float)(int32_t)(tileGeometry.fHeight[0]);
									tileGeometry.fHeight[1]		= (float)(int32_t)(tileGeometry.fHeight[1]);
									tileGeometry.fHeight[2]		= (float)(int32_t)(tileGeometry.fHeight[2]);
									tileGeometry.fHeight[3]		= (float)(int32_t)(tileGeometry.fHeight[3]);
								}
							}
						}
					}

					::klib::SAgentsReference				agentsInRange;
					::klib::getAgentsInSight(tacticalInfo, instanceGame.Players, newAOE.Position.Cell, newAOE.RadiusOrHalfSize, agentsInRange);
					::klib::SGamePlayer							& playerShooter			= instanceGame.Players[tacticalInfo.Setup.Players[newAOE.Caster.PlayerIndex]];
					::klib::CCharacter						& shooter				= *playerShooter.Tactical.Army[playerShooter.Tactical.Squad.Agents[newAOE.Caster.AgentIndex]];
					::klib::TEAM_TYPE						teamShooter				= tacticalInfo.Setup.TeamPerPlayer[newAOE.Caster.PlayerIndex];
					for(uint32_t iAgentInRange=0; iAgentInRange < agentsInRange.Count; ++iAgentInRange) {
						::klib::SGamePlayer							& playerVictim			= instanceGame.Players[tacticalInfo.Setup.Players[agentsInRange.Agents[iAgentInRange].Agent.PlayerIndex]];
						::klib::TEAM_TYPE						teamVictim				= tacticalInfo.Setup.TeamPerPlayer[agentsInRange.Agents[iAgentInRange].Agent.PlayerIndex];
						::klib::CCharacter						& agentVictim			= *playerVictim.Tactical.Army[playerVictim.Tactical.Squad.Agents[agentsInRange.Agents[iAgentInRange].Agent.AgentIndex]];
						//::gpk::SCoord3<int32_t> distance = agentVictim.Position-newAOE.Position.Cell;
						const ::gpk::SCoord3<int32_t>			& coordAgent			= agentVictim.Position;
						const ::gpk::SCoord3<float>				distance				= (coordAgent - newAOE.Position.Cell).Cast<float>();
						double									length					= distance.Length();
						if(length > newAOE.RadiusOrHalfSize)
							continue;

						double proportion = length/newAOE.RadiusOrHalfSize;
						if(proportion >= 1.0)
							continue;
						const ::gpk::array_pod<char_t>			weaponName				= ::klib::getEntityName(instanceGame.EntityTables.Weapon, shooter.CurrentEquip.Weapon);
						::klib::applySuccessfulWeaponHit(instanceGame.EntityTables, instanceGame.Messages, shooter, agentVictim, (int32_t)(newBullet.Points.Damage * (1.0 - proportion)), weaponName);
						if(false == agentVictim.IsAlive())
							::klib::handleAgentDeath(tacticalInfo, instanceGame.Players, agentVictim, shooter, teamVictim, instanceGame.Messages);
						if(false == shooter.IsAlive())
							::klib::handleAgentDeath(tacticalInfo, instanceGame.Players, shooter, agentVictim, teamShooter, instanceGame.Messages);
					}
				}
			}
			else if(bImpact) {

			}
		}
	}
}

bool																	initTacticalGame								(::klib::SGame& instanceGame);
::klib::SGameState														drawTacticalScreen								(::klib::SGame& instanceGame, const ::klib::SGameState& returnState)				{
	::klib::SGameState															exitState										= returnState;
	if(false == ::gpk::bit_true(instanceGame.Flags, ::klib::GAME_FLAGS_TACTICAL)) {
		if(false == ::initTacticalGame(instanceGame)) {
			::gpk::bit_clear(instanceGame.Flags, ::klib::GAME_FLAGS_TACTICAL);
			return {::klib::GAME_STATE_WELCOME_COMMANDER};
		}
		//int32_t finalCost = ::klib::missionCost(instanceGame.Players[PLAYER_INDEX_USER], instanceGame.Players[PLAYER_INDEX_USER].Squad, instanceGame.Players[PLAYER_INDEX_USER].Squad.Size);
		PlaySound("..\\gpk_data\\sounds\\Intro_Wind-Mark_DiAngelo-844491759.wav", 0, SND_ASYNC | SND_FILENAME);
		//instanceGame.Players[PLAYER_INDEX_USER].Money -= finalCost;
	}

	::klib::STacticalInfo														& tacticalInfo									= instanceGame.TacticalInfo;
	if(tacticalInfo.Board.Shots.Bullet.size() <= 0) {
		::handleUserInput(instanceGame, returnState);
		if(!::klib::updateCurrentPlayer(instanceGame.EntityTables, tacticalInfo, instanceGame.Players, instanceGame.Messages)) {
			::klib::endTurn(instanceGame.EntityTables, tacticalInfo, instanceGame.Players, instanceGame.Messages);

			 // If players have agents still alive we just continue in the tactical screen. Otherwise go back to main screen.
			if(::klib::isTacticalValid(instanceGame.TacticalInfo, instanceGame.Players))
				return exitState;

			return ::endMission(instanceGame, false);
		}
	}

	::gpk::SRenderTarget<char, uint16_t>	& tacticalDisplay			= instanceGame.TacticalDisplay.Screen;
	::klib::SGamePlayer							& currentPlayer				= instanceGame.Players[tacticalInfo.Setup.Players[tacticalInfo.CurrentPlayer]];
	::klib::drawTacticalBoard(instanceGame, tacticalInfo, tacticalDisplay.Color, tacticalDisplay.DepthStencil, tacticalInfo.CurrentPlayer, tacticalInfo.Setup.TeamPerPlayer[tacticalInfo.CurrentPlayer], currentPlayer.Tactical.Selection, true);

	::gpk::SRenderTarget<char, uint16_t>	& globalDisplay				= instanceGame.GlobalDisplay.Screen;
	::klib::clearGrid(globalDisplay.Color.View, ' ');
	::drawTileInfo(instanceGame, instanceGame.FrameInput, tacticalInfo, globalDisplay, tacticalDisplay);
	::drawPlayerInfo(instanceGame);

	::klib::TURN_ACTION					selectedAction				= ::klib::TURN_ACTION_CONTINUE;

	if(tacticalInfo.Board.Shots.Bullet.size() <= 0) {
		// Need to construct menu title
		::gpk::array_pod<char_t>			menuTitle					= ::gpk::view_const_string{"Mission Over"};
		const int32_t						selectionPlayerUnit			= currentPlayer.Tactical.Selection.PlayerUnit;
		char								playerUnitPlusOne	[64]	= {};
		sprintf_s(playerUnitPlusOne, "%i", selectionPlayerUnit + 1);
		if( selectionPlayerUnit != -1 && currentPlayer.Tactical.Squad.Agents[selectionPlayerUnit] != -1 && ::klib::GAME_SUBSTATE_CHARACTER != instanceGame.State.Substate) {
			menuTitle						= ::gpk::view_const_string{"Agent #"};
			menuTitle.append_string(playerUnitPlusOne);
			menuTitle.append_string(": ");
			menuTitle.append(currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[selectionPlayerUnit]]->Name);
		}
		else if(selectionPlayerUnit != -1) {
			menuTitle						= ::gpk::view_const_string{"Agent #"};
			menuTitle.append_string(playerUnitPlusOne);
		}

		if(currentPlayer.Tactical.Control.Type == ::klib::PLAYER_CONTROL_LOCAL)  {
 			if(instanceGame.State.Substate == ::klib::GAME_SUBSTATE_INVENTORY) {
				menuTitle.append_string(" - Inventory");
				::klib::CCharacter					& currentAgent			= *currentPlayer.Tactical.Army[currentPlayer.Tactical.Squad.Agents[selectionPlayerUnit]];
				selectedAction					= ::useItems(instanceGame, instanceGame.Messages, currentPlayer, currentAgent, menuTitle, false);
			}
 			else if(instanceGame.State.Substate == ::klib::GAME_SUBSTATE_EQUIPMENT) {
				menuTitle.append_string(" - Equipment");
				static ::klib::SDrawMenuState		menuState;
				exitState						= ::klib::drawMenu(menuState, instanceGame.GlobalDisplay.Screen.Color.View, globalDisplay.DepthStencil.begin(), menuTitle, ::gpk::view_array<const ::klib::SMenuItem<::klib::SGameState>>{::klib::optionsCombatTurnEquip}, instanceGame.FrameInput, {::klib::GAME_STATE_TACTICAL_CONTROL, ::klib::GAME_SUBSTATE_MAIN}, exitState, 20);
				selectedAction					= ::klib::TURN_ACTION_CONTINUE;
			}
			else {
				if(instanceGame.State.Substate != ::klib::GAME_SUBSTATE_MAIN) {
					instanceGame.ClearMessages();
					instanceGame.Messages.UserError = ::gpk::view_const_string{"Unrecognized game substate!"};
					instanceGame.LogError();
				}
				static ::klib::SDrawMenuState		menuState;
				selectedAction					= ::klib::drawMenu(menuState, instanceGame.GlobalDisplay.Screen.Color.View, globalDisplay.DepthStencil.begin(), menuTitle, ::gpk::view_array<const ::klib::SMenuItem<::klib::TURN_ACTION>>{::klib::optionsCombatTurn}, instanceGame.FrameInput, ::klib::TURN_ACTION_MENUS, ::klib::TURN_ACTION_CONTINUE, 20);
			}
		}
		else if(currentPlayer.Tactical.Control.Type == ::klib::PLAYER_CONTROL_AI)  {
			selectedAction = ::selectAIAction(tacticalInfo, instanceGame.Players);
//#if !(defined(GPK_DEBUG_ENABLED))
//			Sleep(70);
//#endif
		}
		else if(currentPlayer.Tactical.Control.Type == ::klib::PLAYER_CONTROL_REMOTE)
			selectedAction = ::selectRemoteAction(tacticalInfo, instanceGame.Players);
	}
	else {
		::gpk::SCoord3<float>				bulletPos				= {0.0f,0.0f,0.0f};
		const ::klib::SBullet				& bulletToPrint			= tacticalInfo.Board.Shots.Bullet[0];
		bulletPos.x						= bulletToPrint.Position.Cell.x + bulletToPrint.Position.Offset.x;
		bulletPos.y						= bulletToPrint.Position.Cell.y + bulletToPrint.Position.Offset.y;
		bulletPos.z						= bulletToPrint.Position.Cell.z + bulletToPrint.Position.Offset.z;
		sprintf_s(instanceGame.Messages.Aux, "Bullet at {%f, %f, %f} with direction {%f, %f, %f}", bulletPos.x, bulletPos.y, bulletPos.z, bulletToPrint.Direction.x, bulletToPrint.Direction.y, bulletToPrint.Direction.z);
		instanceGame.LogAuxMessage();
	}

	bool	bNotCanceled	= true;
		 if(selectedAction == ::klib::TURN_ACTION_MENUS			) return {::klib::GAME_STATE_WELCOME_COMMANDER};
	else if(selectedAction == ::klib::TURN_ACTION_INVENTORY		) return {exitState.State, ::klib::GAME_SUBSTATE_INVENTORY};
	else if(selectedAction == ::klib::TURN_ACTION_EQUIPMENT		) return {exitState.State, ::klib::GAME_SUBSTATE_EQUIPMENT};
	else if(selectedAction == ::klib::TURN_ACTION_MAIN			) return {exitState.State, ::klib::GAME_SUBSTATE_MAIN};
	else if(selectedAction == ::klib::TURN_ACTION_ABORT_MISSION	) {
		currentPlayer.Tactical.Squad.Size = 0;
		currentPlayer.Tactical.Squad.Clear(-1);
		return ::endMission(instanceGame, true);
	}
	else if(selectedAction == ::klib::TURN_ACTION_AUTOPLAY) {
		currentPlayer.Tactical.Control.Type = ::klib::PLAYER_CONTROL_AI;
		tacticalInfo.Setup.Controls[tacticalInfo.CurrentPlayer].Type = ::klib::PLAYER_CONTROL_AI;
		return exitState;
	}
	else {
		CHARACTER_TURN_ACTION result = ::characterTurn(instanceGame.EntityTables, tacticalInfo, instanceGame.Players, selectedAction, instanceGame.Messages);
		switch(result){
		case CHARACTER_TURN_ACTION_INVENTORY:
			 instanceGame.State.Substate												= ::klib::GAME_SUBSTATE_CHARACTER;
			 break;
		case CHARACTER_TURN_ACTION_CANCELED:
			 bNotCanceled																= false;
			 break;
		case CHARACTER_TURN_ACTION_CONTINUE:
		default:
			 break;
		};
	}

	bool						movesRemaining					= false;

	for(uint32_t i = 0; i < currentPlayer.Tactical.Squad.Size; i++) {
		if(currentPlayer.Tactical.Squad.ActionsLeft[i].Actions != 0 || currentPlayer.Tactical.Squad.ActionsLeft[i].Moves != 0){
			movesRemaining			= true;
			break;
		}
	}

	if( (tacticalInfo.Board.Shots.Bullet.size() <= 0) && bNotCanceled && !movesRemaining)
		::klib::endTurn(instanceGame.EntityTables, tacticalInfo, instanceGame.Players, instanceGame.Messages);

	 // If players have agents still alive we just continue in the tactical screen. Otherwise go back to main screen.
	if(::klib::isTacticalValid(tacticalInfo, instanceGame.Players)) {
		::updateBullets(instanceGame, instanceGame.FrameTimer.LastTimeSeconds);
		return exitState;
	}

	return ::endMission(instanceGame, false);
}

