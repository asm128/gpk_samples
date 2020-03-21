#include "tactical_draw.h"

void									klib::boardToDisplay			(::klib::SGame& instanceGame, const STacticalBoard& board, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int8_t indexBoardPlayer, TEAM_TYPE teamId, const SPlayerSelection& selection, bool bFogOfWar)	{
	const STacticalInfo							& tacticalInfo			= instanceGame.TacticalInfo;
	const STacticalSetup						& tacticalSetup			= tacticalInfo.Setup;

	static const int32_t						maxSwaps				= 32;
	static bool									bSwaps[maxSwaps]		= {false};
	static int8_t								swapCounter				= 0;

	uint32_t									y						= 0;
	for(uint32_t z = 0; z < board.Tiles.Terrain.Geometry.metrics().y; ++z) {
	for(uint32_t x = 0; x < board.Tiles.Terrain.Geometry.metrics().x; ++x) {
		int32_t										topologyHeight			= board.Tiles.Terrain.Topology[z][x].Smooth + board.Tiles.Terrain.Topology[z][x].Sharp;
		float										cornerHeight[4]			= {};
		::memcpy(cornerHeight, board.Tiles.Terrain.Geometry[z][x].fHeight, sizeof(float)*4);
		::gpk::SCoord3<int32_t>						currentCoord			= {(int32_t)x, (int32_t)y, (int32_t)z};
		bool										bInRange				= false;
		double										initialSight			= 0.0;
		double										finalSight				= 0.0;
		::gpk::SCoord3<float>						currentTilePos			= currentCoord.Cast<float>();

		if(false == bFogOfWar)
			bInRange								= true;
		else {
			for(uint32_t iTacticalPlayer = 0, playerCount = tacticalInfo.Setup.TotalPlayers; iTacticalPlayer < playerCount; ++iTacticalPlayer) {
				if(tacticalSetup.Players[iTacticalPlayer] == PLAYER_INDEX_INVALID)
					continue;

				if(tacticalSetup.TeamPerPlayer[iTacticalPlayer] != teamId)
					continue;

				::klib::STacticalPlayer								& currentPlayer			= instanceGame.Players[tacticalSetup.Players[iTacticalPlayer]].Tactical;

				for(uint32_t iAgent = 0, agentCount = tacticalSetup.SquadSize[iTacticalPlayer]; iAgent < agentCount; ++iAgent) {
					if(currentPlayer.Squad.Agents[iAgent] == -1)
						continue;

					CCharacter									& playerAgent			= *currentPlayer.Army[currentPlayer.Squad.Agents[iAgent]];
					if(false == playerAgent.IsAlive())
						continue;

					::gpk::SCoord3<int32_t>						coordPlayer				= playerAgent.Position;
					::gpk::SCoord3<float>						distance				= currentTilePos - coordPlayer.Cast<float>();;

					SEntityPoints								playerAgentPoints		= playerAgent.FinalPoints	;
					SEntityFlags								playerAgentFlags		= playerAgent.FinalFlags	;

					initialSight							= (playerAgentPoints.Fitness.Sight+SIGHT_OFFSET);
					finalSight								= ::klib::getFinalSight(initialSight, playerAgent);

					if(distance.Length() < finalSight) {
						bInRange								= true;
						break;
					}
				}
				if(bInRange)
					break;
			}
		}

		if(false == bInRange)
			continue;

		if(board.Tiles.Entities.Agents[z][x].PlayerIndex != -1) {
			int32_t											agentIndex					= board.Tiles.Entities.Agents[z][x].AgentIndex;
			int32_t											cellPlayerIndex				= board.Tiles.Entities.Agents[z][x].PlayerIndex;

			bool											bIsAlly						= tacticalInfo.Setup.TeamPerPlayer[indexBoardPlayer] == tacticalInfo.Setup.TeamPerPlayer[cellPlayerIndex];
			display[z][x]								= ::klib::ascii_face[bIsAlly ? FACE_BLACK : FACE_WHITE];
			//target.Screen.Cells[z][x] = std::to_string(agentIndex+1)[0];


			bool											bIsSelected
				=  (cellPlayerIndex == indexBoardPlayer			&& board.Tiles.Entities.Agents[z][x].AgentIndex == selection.PlayerUnit)
				|| (cellPlayerIndex == selection.TargetPlayer	&& board.Tiles.Entities.Agents[z][x].AgentIndex == selection.TargetUnit)
				;
			const STacticalPlayer							& boardPlayer				= instanceGame.Players[tacticalInfo.Setup.Players[cellPlayerIndex]].Tactical;
			const SCharacter								& agent						= *boardPlayer.Army[boardPlayer.Squad.Agents[agentIndex]];
			uint16_t										color						= ::klib::ASCII_COLOR_INDEX_BLACK;
			if(agent.IsAlive()) {
				double fractionLife		= agent.Points.LifeCurrent.Health / (double)agent.FinalPoints.LifeMax.Health;
				color = (int8_t)getPlayerColor(tacticalInfo, boardPlayer, (int8_t)cellPlayerIndex, indexBoardPlayer, bSwaps[4] && bIsSelected);
				if(agent.ActiveBonus.Status.Status && bSwaps[1])
					color = getStatusColor(agent.ActiveBonus.Status.Status, bSwaps[3], color);

				if(bSwaps[5] && fractionLife < 0.25) {
					display[z][x] = ::klib::ascii_fraction[1];
					color = bSwaps[0] ? color : ::klib::ASCII_COLOR_INDEX_RED;
				}
				else if(bSwaps[5] && fractionLife < 0.5) {
					display[z][x] = ::klib::ascii_fraction[2];
					color = bSwaps[0] ? color : ::klib::ASCII_COLOR_INDEX_YELLOW;
				}
				else {
					bool				bIsMale				= agent.Flags.Tech.Gender == ::klib::GENDER_MALE;
					bool				bIsHermaphrodite	= agent.Flags.Tech.Gender == ::klib::GENDER_HERMAPHRODITE;
					display[z][x] = ::klib::ascii_gender[bIsMale ? MALE : bIsHermaphrodite ? HERMAPHRODITE : FEMALE];
				}
			}
			else if(bSwaps[4] && tacticalInfo.HasDrops(currentCoord))
				color = ::klib::ASCII_COLOR_INDEX_DARKYELLOW;

			textAttributes[z][x] |= color;
		}
		else if( board.Shots.Coords.find(currentCoord) != -1) {
			static const float GAME_EPSILON = 0.000001f;
			char						bulletAscii		= '*';
			uint16_t					bulletColor		= bSwaps[2] ? ::klib::ASCII_COLOR_INDEX_DARKGREY : ::klib::ASCII_COLOR_INDEX_DARKGREY;
			for(uint32_t iBullet=0, bulletCount = board.Shots.Bullet.size(); iBullet < bulletCount; ++iBullet) {
				const SBullet				& bullet		= board.Shots.Bullet[iBullet];
				if(bullet.Points.StatusInflict)
					bulletColor = getStatusColor(bullet.Points.StatusInflict, bSwaps[2], bulletColor);

				if(::gpk::bit_true(bullet.Points.Tech.ProjectileClass, PROJECTILE_CLASS_ROCKET))
					bulletAscii = 0x0F; // bigger asterisk
				else if(::gpk::bit_false(bullet.Points.Tech.ProjectileClass, PROJECTILE_CLASS_SHELL)) {
					const ::gpk::SCoord2<float> dirVector = {bullet.Direction.x, bullet.Direction.z};
					if( ( dirVector.x < (-GAME_EPSILON) && dirVector.y < (-GAME_EPSILON) )
						|| ( dirVector.x > GAME_EPSILON && dirVector.y > GAME_EPSILON )
						)
						bulletAscii = '\\'; // draw the enemy as an ascii character
					else if( ( dirVector.x < (-GAME_EPSILON) && dirVector.y > GAME_EPSILON )
						|| ( dirVector.x > GAME_EPSILON && dirVector.y < (-GAME_EPSILON) )
						)
						bulletAscii = '/'; // draw the enemy as an ascii character
					else if( dirVector.x < (-GAME_EPSILON) || dirVector.x > GAME_EPSILON )
						bulletAscii = '-'; // draw the enemy as an ascii character
					else
						bulletAscii = '|'; // draw the enemy as an ascii character
				}
			}
			display			[z][x] = bulletAscii;
			textAttributes	[z][x] |= bulletColor;
		}
		else if( board.Tiles.Entities.Coins[z][x] !=  0)	{
			display			[z][x] = ::klib::ascii_cards[DECK_DIAMONDS];
			textAttributes	[z][x] |= bSwaps[6] ? ::klib::ASCII_COLOR_INDEX_DARKYELLOW : ::klib::ASCII_COLOR_INDEX_ORANGE;
		}
		else if(board.Tiles.Entities.Props[z][x].Definition != -1) {
			static const ::gpk::view_const_string chestLabel	= "Chest";
			static const ::gpk::view_const_string wallLabel		= "Wall";
			if(chestLabel == instanceGame.EntityTables.StageProp.Definitions[board.Tiles.Entities.Props[z][x].Definition].Name) {
				display			[z][x] = ::klib::ascii_cards[DECK_CLUBS];
				textAttributes	[z][x] |= bSwaps[10] ? ::klib::ASCII_COLOR_INDEX_YELLOW : ::klib::ASCII_COLOR_INDEX_BLACK;
			}
			else if(wallLabel == instanceGame.EntityTables.StageProp.Definitions[board.Tiles.Entities.Props[z][x].Definition].Name){
				display			[z][x] = ::klib::getASCIIWall(instanceGame.EntityTables.StageProp.Definitions, ::gpk::view_grid<const STileProp>{board.Tiles.Entities.Props.Texels.begin(), board.Tiles.Entities.Props.metrics()}, x, z);
				textAttributes	[z][x] |= ::klib::ASCII_COLOR_INDEX_BLACK;
			}
			else {
				display			[z][x] = instanceGame.EntityTables.StageProp.Definitions[board.Tiles.Entities.Props[z][x].Definition].Name[0];
				textAttributes	[z][x] |= ::klib::ASCII_COLOR_INDEX_BLACK;
			}
			if(board.Tiles.Entities.Props[z][x].Level == -1)
				textAttributes[z][x] |= ::klib::ASCII_COLOR_INDEX_DARKGREY;
		}
		else if(topologyHeight || cornerHeight[0] || cornerHeight[1] || cornerHeight[2] || cornerHeight[3]) {
			// I suppose this chooses a color depending on the slope between tile corners.
				 if(cornerHeight[0]	<	cornerHeight[3] && cornerHeight[0]	<	cornerHeight[1] && cornerHeight[0]	<	cornerHeight[2]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[1]	<	cornerHeight[0] && cornerHeight[1]	<	cornerHeight[3] && cornerHeight[1]	<	cornerHeight[2]) { display[z][x] = -78; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[2]	<	cornerHeight[0] && cornerHeight[2]	<	cornerHeight[1] && cornerHeight[2]	<	cornerHeight[3]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | 0					; }
			else if(cornerHeight[3]	<	cornerHeight[0] && cornerHeight[3]	<	cornerHeight[1] && cornerHeight[3]	<	cornerHeight[2]) { display[z][x] = -79; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }

			else if(cornerHeight[0]	>	cornerHeight[3] && cornerHeight[0]	>	cornerHeight[1] && cornerHeight[0]	>	cornerHeight[2]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[1]	>	cornerHeight[0] && cornerHeight[1]	>	cornerHeight[3] && cornerHeight[1]	>	cornerHeight[2]) { display[z][x] = -78; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[2]	>	cornerHeight[0] && cornerHeight[2]	>	cornerHeight[1] && cornerHeight[2]	>	cornerHeight[3]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | 0					; }
			else if(cornerHeight[3]	>	cornerHeight[0] && cornerHeight[3]	>	cornerHeight[1] && cornerHeight[3]	>	cornerHeight[2]) { display[z][x] = -79; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			///----------------------
			else if(cornerHeight[0]	>	cornerHeight[3] && cornerHeight[0]	<	cornerHeight[1] && cornerHeight[0]	<	cornerHeight[2]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[1]	>	cornerHeight[0] && cornerHeight[1]	<	cornerHeight[3] && cornerHeight[1]	<	cornerHeight[2]) { display[z][x] = -78; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[2]	>	cornerHeight[0] && cornerHeight[2]	<	cornerHeight[1] && cornerHeight[2]	<	cornerHeight[3]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | 0					; }
			else if(cornerHeight[3]	>	cornerHeight[0] && cornerHeight[3]	<	cornerHeight[1] && cornerHeight[3]	<	cornerHeight[2]) { display[z][x] = -79; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }

			else if(cornerHeight[0]	<	cornerHeight[3] && cornerHeight[0]	>	cornerHeight[1] && cornerHeight[0]	<	cornerHeight[2]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[1]	<	cornerHeight[0] && cornerHeight[1]	>	cornerHeight[3] && cornerHeight[1]	<	cornerHeight[2]) { display[z][x] = -78; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[2]	<	cornerHeight[0] && cornerHeight[2]	>	cornerHeight[1] && cornerHeight[2]	<	cornerHeight[3]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | 0					; }
			else if(cornerHeight[3]	<	cornerHeight[0] && cornerHeight[3]	>	cornerHeight[1] && cornerHeight[3]	<	cornerHeight[2]) { display[z][x] = -79; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }

			else if(cornerHeight[0]	<	cornerHeight[3] && cornerHeight[0]	<	cornerHeight[1] && cornerHeight[0]	>	cornerHeight[2]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[1]	<	cornerHeight[0] && cornerHeight[1]	<	cornerHeight[3] && cornerHeight[1]	>	cornerHeight[2]) { display[z][x] = -78; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[2]	<	cornerHeight[0] && cornerHeight[2]	<	cornerHeight[1] && cornerHeight[2]	>	cornerHeight[3]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | 0					; }
			else if(cornerHeight[3]	<	cornerHeight[0] && cornerHeight[3]	<	cornerHeight[1] && cornerHeight[3]	>	cornerHeight[2]) { display[z][x] = -79; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			///----------------------
			else if(cornerHeight[0]	>	cornerHeight[3] && cornerHeight[0]	<	cornerHeight[1] && cornerHeight[0]	>	cornerHeight[2]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[1]	>	cornerHeight[0] && cornerHeight[1]	<	cornerHeight[3] && cornerHeight[1]	>	cornerHeight[2]) { display[z][x] = -78; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[2]	>	cornerHeight[0] && cornerHeight[2]	<	cornerHeight[1] && cornerHeight[2]	>	cornerHeight[3]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | 0					; }
			else if(cornerHeight[3]	>	cornerHeight[0] && cornerHeight[3]	<	cornerHeight[1] && cornerHeight[3]	>	cornerHeight[2]) { display[z][x] = -79; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }

			else if(cornerHeight[0]	>	cornerHeight[3] && cornerHeight[0]	>	cornerHeight[1] && cornerHeight[0]	<	cornerHeight[2]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[1]	>	cornerHeight[0] && cornerHeight[1]	>	cornerHeight[3] && cornerHeight[1]	<	cornerHeight[2]) { display[z][x] = -78; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[2]	>	cornerHeight[0] && cornerHeight[2]	>	cornerHeight[1] && cornerHeight[2]	<	cornerHeight[3]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | 0					; }
			else if(cornerHeight[3]	>	cornerHeight[0] && cornerHeight[3]	>	cornerHeight[1] && cornerHeight[3]	<	cornerHeight[2]) { display[z][x] = -79; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }

			else if(cornerHeight[0]	<	cornerHeight[3] && cornerHeight[0]	>	cornerHeight[1] && cornerHeight[0]	>	cornerHeight[2]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[1]	<	cornerHeight[0] && cornerHeight[1]	>	cornerHeight[3] && cornerHeight[1]	>	cornerHeight[2]) { display[z][x] = -78; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN	<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }
			else if(cornerHeight[2]	<	cornerHeight[0] && cornerHeight[2]	>	cornerHeight[1] && cornerHeight[2]	>	cornerHeight[3]) { display[z][x] = -80; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | 0					; }
			else if(cornerHeight[3]	<	cornerHeight[0] && cornerHeight[3]	>	cornerHeight[1] && cornerHeight[3]	>	cornerHeight[2]) { display[z][x] = -79; textAttributes[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN		<< 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY		; }

			//	 if(cornerHeight[0]	<	cornerHeight[1])	{	target.Screen.Cells[z][x] = -78; target.Screen.DepthStencil.Cells[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN << 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY; }
			//else if(cornerHeight[0]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.Screen.DepthStencil.Cells[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN << 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY; }
			//else if(cornerHeight[1]	<	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.Screen.DepthStencil.Cells[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN << 4 | ::klib::ASCII_COLOR_INDEX_DARKGREY; }
			//else if(cornerHeight[2]	<	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.Screen.DepthStencil.Cells[z][x] = ::klib::ASCII_COLOR_INDEX_GREEN << 4 | COLOR_DARKGREEN; }
			//
			//else if(cornerHeight[0]	>	cornerHeight[1])	{	target.Screen.Cells[z][x] = -80; target.Screen.DepthStencil.Cells[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN << 4; }
			//else if(cornerHeight[0]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.Screen.DepthStencil.Cells[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN << 4; }
			//else if(cornerHeight[1]	>	cornerHeight[3])	{	target.Screen.Cells[z][x] = -78; target.Screen.DepthStencil.Cells[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN << 4; }
			//else if(cornerHeight[2]	>	cornerHeight[3])	{	target.Screen.Cells[z][x] = -79; target.Screen.DepthStencil.Cells[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN << 4; }
			else
			{
				display			[z][x] = ' ';
				textAttributes	[z][x] = ::klib::ASCII_COLOR_INDEX_DARKGREEN << 4;
			}
		}
		//else if(topologyHeight)
		//{
		//	if(topologyHeight < -FULL_COVER_HEIGHT)
		//	{
		//		if(z&1) {
		//			if(x&1)	{ target.Screen.Cells[z][x] = ascii_water[(bSwaps[4]	) ? 1+(x&1) : 2-(x&1)]; target.Screen.DepthStencil.Cells[z][x] = iif(bSwaps[8]	) COLOR_BLUE		| (COLOR_DARKGREEN <<4) : COLOR_DARKBLUE	| (COLOR_DARKGREEN <<4); }
		//			else	{ target.Screen.Cells[z][x] = ascii_water[(bSwaps[5]	) ? 1+(x&1) : 2-(x&1)]; target.Screen.DepthStencil.Cells[z][x] = iif(bSwaps[9]	) COLOR_DARKBLUE	| (COLOR_DARKGREEN <<4) : COLOR_BLUE		| (COLOR_DARKGREEN <<4); }
		//		}
		//		else {
		//			if(x&1)	{ target.Screen.Cells[z][x] = ascii_water[(bSwaps[6]	) ? 2-(x&1) : 1+(x&1)]; target.Screen.DepthStencil.Cells[z][x] = iif(bSwaps[2]	) COLOR_DARKBLUE	| (COLOR_DARKGREEN  <<4) : COLOR_BLUE		| (COLOR_DARKGREEN  <<4); }
		//			else	{ target.Screen.Cells[z][x] = ascii_water[(bSwaps[7]	) ? 2-(x&1) : 1+(x&1)]; target.Screen.DepthStencil.Cells[z][x] = iif(bSwaps[3]	) COLOR_BLUE		| (COLOR_DARKGREEN  <<4) : COLOR_DARKBLUE	| (COLOR_DARKGREEN  <<4); }
		//		}
		//	}
		//	else if(topologyHeight >= FULL_COVER_HEIGHT		)	{ target.Screen.Cells[z][x] = ascii_tones[(topologyHeight == FULL_COVER_HEIGHT		) ? 1 : topologyHeight		-FULL_COVER_HEIGHT		]	; target.Screen.DepthStencil.Cells[z][x] = (COLOR_DARKBLUE	) | (COLOR_DARKGREEN	<<4)	; }
		//	else if(topologyHeight >= PARTIAL_COVER_HEIGHT	)	{ target.Screen.Cells[z][x] = ascii_tones[(topologyHeight == PARTIAL_COVER_HEIGHT	) ? 1 : topologyHeight		-PARTIAL_COVER_HEIGHT	]	; target.Screen.DepthStencil.Cells[z][x] = (COLOR_ORANGE		) | (COLOR_DARKGREEN	<<4)	; }
		//	else if(topologyHeight > 0						)	{ target.Screen.Cells[z][x] = ascii_tones[1+(topologyHeight)/3]																				; target.Screen.DepthStencil.Cells[z][x] = (COLOR_GREEN		) | (COLOR_DARKGREEN	<<4)	; }
		//	else if(topologyHeight <= -FULL_COVER_HEIGHT	)	{ target.Screen.Cells[z][x] = ascii_tones[(topologyHeight == -FULL_COVER_HEIGHT		) ? 1 : (topologyHeight*-1)	-FULL_COVER_HEIGHT		]	; target.Screen.DepthStencil.Cells[z][x] = (COLOR_DARKGREEN	) | (COLOR_BLACK		<<4)	; }
		//	else if(topologyHeight <= -PARTIAL_COVER_HEIGHT	)	{ target.Screen.Cells[z][x] = ascii_tones[(topologyHeight == -PARTIAL_COVER_HEIGHT	) ? 1 : (topologyHeight*-1)	-PARTIAL_COVER_HEIGHT	]	; target.Screen.DepthStencil.Cells[z][x] = (COLOR_DARKGREEN	) | (COLOR_DARKBLUE		<<4)	; }
		//	else if(topologyHeight < 0						)	{ target.Screen.Cells[z][x] = ascii_tones[1+(topologyHeight*-1)/3]																			; target.Screen.DepthStencil.Cells[z][x] = (COLOR_DARKGREEN	) | (COLOR_DARKGREY			<<4)	; }
		//}
		else {
			display			[z][x]	= ' ';
			textAttributes	[z][x]	= (::klib::ASCII_COLOR_INDEX_DARKGREEN << 4);
		}

		if(initialSight != finalSight)
			textAttributes[z][x]		= (textAttributes[z][x] & 0xF) | (::klib::ASCII_COLOR_INDEX_WHITE << 4);

		for(uint32_t iAOE = 0, countAOE = board.AreaOfEffect.AOE.size(); iAOE <countAOE; ++iAOE) {
			const ::klib::SAOE						& aoeInstance			= board.AreaOfEffect.AOE[iAOE];
			const ::gpk::SCoord3<int32_t>& aoeCell = aoeInstance.Position.Cell;
			::gpk::SCoord3<float> aoePos = {(float)aoeCell.x, (float)aoeCell.y, (float)aoeCell.z};
			aoePos += aoeInstance.Position.Offset;

			bool failBVTest =
				 ( x < (aoePos.x - aoeInstance.RadiusOrHalfSize)
				|| y < (aoePos.y - aoeInstance.RadiusOrHalfSize)
				|| z < (aoePos.z - aoeInstance.RadiusOrHalfSize)
				) ||
				 ( x > (aoePos.x + aoeInstance.RadiusOrHalfSize)
				|| y > (aoePos.y + aoeInstance.RadiusOrHalfSize)
				|| z > (aoePos.z + aoeInstance.RadiusOrHalfSize)
				);

			if(failBVTest)
				continue;

			if((aoePos-currentTilePos).Length() <= aoeInstance.RadiusOrHalfSize) {
				if(aoeInstance.StatusInflict)
					textAttributes[z][x] = (textAttributes[z][x] & 0xF) | (getStatusColor(aoeInstance.StatusInflict, bSwaps[2], ::klib::ASCII_COLOR_INDEX_ORANGE) << 4);
				else
					textAttributes[z][x] = (textAttributes[z][x] & 0xF) | (::klib::ASCII_COLOR_INDEX_GREEN << 4);
			}
		}
		}
	}

	static ::klib::STimer						animationTimer					= {};
	static ::klib::SAccumulator<double>			animationAccum					= {0.0, 0.20};

	animationTimer.Frame();

	if( animationAccum.Accumulate(animationTimer.LastTimeSeconds) ) {
		for(uint32_t i=0; i<maxSwaps; ++i)
			if(0 == swapCounter%(i+1))
				bSwaps[i]								= !bSwaps[i];

		++swapCounter;
		animationAccum.Value					= 0;
	}
}

void									klib::drawTacticalBoard				(::klib::SGame& instanceGame, STacticalInfo& tacticalInfo, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int8_t playerIndex, TEAM_TYPE teamId, const SPlayerSelection& selection, bool bFogOfWar)			{
	char										hiddenTile						= 0;//-78;
	memset(display.begin(), hiddenTile, display.area());

	uint16_t									gridColor						= ::klib::ASCII_COLOR_INDEX_DARKGREEN << 4;
	uint64_t									gridColorPacked					= gridColor;
	gridColorPacked							|= (gridColorPacked << 16) | (gridColorPacked << 32) | (gridColorPacked << 48);
	int32_t										widthOverColorSize				= display.metrics().x >> 2;
	for(uint32_t z = 0; z < display.metrics().y; ++z) {
		::klib::valueToGrid(textAttributes, z, 0, ::klib::SCREEN_LEFT, (uint16_t*)&gridColorPacked, 4, widthOverColorSize);
	}
	::klib::boardToDisplay(instanceGame, tacticalInfo.Board, display, textAttributes, playerIndex, teamId, selection, bFogOfWar);
}
