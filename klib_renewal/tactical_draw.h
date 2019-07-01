//#define NOMINMAX
#include "Game.h"
#include "StageProp.h"

#include "klib_ascii_reference.h"

#ifndef __TACTICAL_DRAW_H__2309748209346029374609821764__
#define __TACTICAL_DRAW_H__2309748209346029374609821764__

namespace klib
{
	GDEFINE_ENUM_TYPE (TILE_SIDE, uint8_t)
	GDEFINE_ENUM_VALUE(TILE_SIDE, LEFT	, 0x01);
	GDEFINE_ENUM_VALUE(TILE_SIDE, RIGHT	, 0x02);
	GDEFINE_ENUM_VALUE(TILE_SIDE, BACK	, 0x04);
	GDEFINE_ENUM_VALUE(TILE_SIDE, FRONT	, 0x08);
	GDEFINE_ENUM_VALUE(TILE_SIDE, DOWN	, 0x10);
	GDEFINE_ENUM_VALUE(TILE_SIDE, UP	, 0x20);

#define TILE_SIDE_BOTTOM				TILE_SIDE_DOWN
#define TILE_SIDE_TOP					TILE_SIDE_UP	

	struct STileASCIIWalls {
		char								Vertical					;
		char								Horizontal					;
		char								CornerTopLeft				;
		char								CornerTopRight				;
		char								CornerBottomLeft			;
		char								CornerBottomRight			;
		char								ConnectionVerticalLeft		;
		char								ConnectionVerticalRight		;
		char								ConnectionHorizontalUp		;
		char								ConnectionHorizontalDown	;
		char								ConnectionCross				;
	};

	struct STileASCIITopology {
		char								HeightNonBlockingSmooth		;
		char								HeightNonBlockingSharp		;
		char								HeightNonBlockingCombined	;
		char								HeightPartialSmooth			;
		char								HeightPartialSharp			;
		char								HeightPartialCombined		;
		char								HeightFullSmooth			;
		char								HeightFullSharp 			;
		char								HeightFullCombined 			;
	};

	struct STileASCIIGender {
		char								Male						;
		char								Female						;
		char								Hermaphrodite				;
	};

	struct STileASCII {
		char								Unexplored					;
		char								Ally						;
		char								Enemy						;
		char								Chest						;
		char								Coins						;
		STileASCIIGender					Gender						;
		STileASCIIWalls						WallsThin					;
		STileASCIIWalls						WallsThick					;
		STileASCIITopology					Topology					;
	};

	uint16_t							getPlayerColor				( const STacticalInfo& tacticalInfo, const SPlayer& boardPlayer, int8_t indexBoardPlayer, int8_t indexPlayerViewer, bool bIsSelected );

	struct SStatusColor {
		int8_t								Bright	;//: 4;
		int8_t								Dark	;//: 4;
		
											SStatusColor
			(	int8_t			bright	= COLOR_WHITE 
			,	int8_t			dark	= COLOR_BLACK 
			)
			: Bright	(bright	)
			, Dark	(dark	)
		{}
	};

	template<typename _T>
	int32_t									getBitIndex				(const _T& bitToFind, int32_t maxCount)																																						{
		static const int32_t						typeSizeInBits			= sizeof(_T)*8;
		maxCount = (maxCount > typeSizeInBits) ? typeSizeInBits : maxCount;
		for(int32_t i = 0; i < maxCount; ++i)
			if( ::gpk::bit_true(bitToFind, (_T)(1ULL << i)) )
				return i;
		return -1;
	}

	template<size_t _StatusCount>
	int32_t									initStatusColors		(SStatusColor (&statusColors)[_StatusCount])																																				{
		int32_t										nBitIndex;
		// Stun
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_FROZEN		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_DARKGREY		, COLOR_WHITE		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_PANIC		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_DARKGREY		, COLOR_ORANGE		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_SLEEP		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_DARKGREY		, COLOR_BLACK		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_STUN		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_DARKGREY		, COLOR_DARKRED		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_SHOCK		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_DARKGREY		, COLOR_CYAN		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_PETRIFY		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_DARKGREY		, COLOR_GREEN		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_CHARMED		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_DARKGREY		, COLOR_MAGENTA		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_BERSERK		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_DARKGREY		, COLOR_YELLOW		};

		// Poison damage
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_BLEEDING	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_GREEN			, COLOR_RED			};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_POISON		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_GREEN			, COLOR_DARKGREEN	};
		
		// Shieldable damage
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_BURN		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_RED			, COLOR_DARKRED		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_FREEZING	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_RED			, COLOR_WHITE		};

		// Debuff
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_WEAKNESS	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_BLUE			, COLOR_BLACK		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_SLOW		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_BLUE			, COLOR_DARKRED		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_BULLIED		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_BLUE			, COLOR_DARKMAGENTA	};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_DRUNK		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_BLUE			, COLOR_GREEN		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_BLIND		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_BLUE			, COLOR_WHITE		};

		// 
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_RAGE		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_ORANGE			, COLOR_GREEN		};
		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_INVISIBLE	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_ORANGE			, COLOR_WHITE		};

		nBitIndex								= ::klib::getBitIndex(COMBAT_STATUS_BLACKOUT	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {COLOR_WHITE			, COLOR_BLACK		};
		return 0;
	}

	uint16_t								getStatusColor			(COMBAT_STATUS status, bool bSwap, uint16_t defaultColor);

	template<size_t _Width, size_t _Depth>
	void									boardToDisplay			(SGame& instanceGame, const STacticalBoard& board, SWeightedDisplay<_Width, _Depth>& target, int8_t indexBoardPlayer, TEAM_TYPE teamId, const SPlayerSelection& selection, bool bFogOfWar)	{
		const STacticalInfo							& tacticalInfo			= instanceGame.TacticalInfo;
		const STacticalSetup						& tacticalSetup			= tacticalInfo.Setup;

		static const int32_t						maxSwaps				= 32;
		static bool									bSwaps[maxSwaps]		= {false};
		static int8_t								swapCounter				= 0;

		uint32_t									y						= 0;
		for(uint32_t z = 0; z < _Depth; ++z) {
			for(uint32_t x = 0; x < _Width; ++x) {
				int32_t										topologyHeight			= board.Tiles.Terrain.Topology.Cells[z][x].Smooth+board.Tiles.Terrain.Topology.Cells[z][x].Sharp;
				float										cornerHeight[4]			= {};
				memcpy(cornerHeight, board.Tiles.Terrain.Geometry.Cells[z][x].fHeight, sizeof(float)*4);
				::gpk::SCoord3<int32_t>					currentCoord			= {(int32_t)x, (int32_t)y, (int32_t)z};
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

						SPlayer										& currentPlayer			= instanceGame.Players[tacticalSetup.Players[iTacticalPlayer]];

						for(uint32_t iAgent = 0, agentCount = tacticalSetup.SquadSize[iTacticalPlayer]; iAgent < agentCount; ++iAgent) {
							if(currentPlayer.Squad.Agents[iAgent] == -1)
								continue;

							CCharacter									& playerAgent			= *currentPlayer.Army[currentPlayer.Squad.Agents[iAgent]];
							if(false == playerAgent.IsAlive())
								continue;

							::gpk::SCoord3<int32_t>					coordPlayer				= playerAgent.Position; 
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

				if(board.Tiles.Entities.Agents.Cells[z][x].PlayerIndex != -1) {
					int32_t											agentIndex					= board.Tiles.Entities.Agents.Cells[z][x].AgentIndex;
					int32_t											cellPlayerIndex				= board.Tiles.Entities.Agents.Cells[z][x].PlayerIndex;

					bool											bIsAlly						= tacticalInfo.Setup.TeamPerPlayer[indexBoardPlayer] == tacticalInfo.Setup.TeamPerPlayer[cellPlayerIndex];
					target.Screen.Cells[z][x]					= ::klib::ascii_face[bIsAlly ? FACE_BLACK : FACE_WHITE];
					//target.Screen.Cells[z][x] = std::to_string(agentIndex+1)[0]; 


					bool											bIsSelected					=	(cellPlayerIndex == indexBoardPlayer		&& board.Tiles.Entities.Agents.Cells[z][x].AgentIndex == selection.PlayerUnit) 
									||	(cellPlayerIndex == selection.TargetPlayer	&& board.Tiles.Entities.Agents.Cells[z][x].AgentIndex == selection.TargetUnit);

					const SPlayer& boardPlayer	= instanceGame.Players[tacticalInfo.Setup.Players[cellPlayerIndex]];
					const SCharacter& agent		= *boardPlayer.Army[boardPlayer.Squad.Agents[agentIndex]];
					uint16_t color = COLOR_BLACK;
					if(agent.IsAlive()) {
						double fractionLife		= agent.Points.LifeCurrent.Health / (double)agent.FinalPoints.LifeMax.Health;
						color = (int8_t)getPlayerColor(tacticalInfo, boardPlayer, (int8_t)cellPlayerIndex, indexBoardPlayer, bSwaps[4] && bIsSelected);
						if(agent.ActiveBonus.Status.Status && bSwaps[1])
							color = getStatusColor(agent.ActiveBonus.Status.Status, bSwaps[3], color);

						if(bSwaps[5] && fractionLife < 0.25) {
							target.Screen.Cells[z][x] = ::klib::ascii_fraction[1];
							color = bSwaps[0] ? color : COLOR_RED;
						}
						else if(bSwaps[5] && fractionLife < 0.5) {
							target.Screen.Cells[z][x] = ::klib::ascii_fraction[2];
							color = bSwaps[0] ? color : COLOR_YELLOW;
						}
						else {
							bool bIsMale			= agent.Flags.Tech.Gender == GENDER_MALE;
							bool bIsHermaphrodite	= agent.Flags.Tech.Gender == GENDER_HERMAPHRODITE;
							target.Screen.Cells[z][x] = ::klib::ascii_gender[bIsMale ? MALE : bIsHermaphrodite ? HERMAPHRODITE : FEMALE];
						}
					}
					else if(bSwaps[4] && tacticalInfo.HasDrops(currentCoord))
						color = COLOR_DARKYELLOW;

					target.TextAttributes.Cells[z][x] |= color; 
				}
				else if( board.Shots.Coords.FindElement(currentCoord) != -1) { 
					static const float GAME_EPSILON = 0.000001f;
					char bulletAscii = '*';
					uint16_t bulletColor = bSwaps[2] ? COLOR_DARKGREY : COLOR_DARKGREY;
					for(uint32_t iBullet=0, bulletCount = board.Shots.Bullet.Count; iBullet < bulletCount; iBullet++)
					{
						const SBullet& bullet = board.Shots.Bullet[iBullet].Entity;
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
					target.Screen.Cells[z][x] = bulletAscii; 
					target.TextAttributes.Cells[z][x] |= bulletColor;
				} 
				else if( board.Tiles.Entities.Coins	.Cells[z][x] !=  0)	{ 
					target.Screen.Cells			[z][x] = ::klib::ascii_cards[DECK_DIAMONDS]; 
					target.TextAttributes.Cells	[z][x] |= bSwaps[6] ? COLOR_DARKYELLOW : COLOR_ORANGE; 
				} 
				else if(board.Tiles.Entities.Props.Cells[z][x].Definition != -1) { 
					static const ::gpk::label chestLabel	= "Chest";
					static const ::gpk::label wallLabel		= "Wall";
					if(chestLabel == definitionsStageProp[board.Tiles.Entities.Props.Cells[z][x].Definition].Name) {
						target.Screen.Cells[z][x] = ::klib::ascii_cards[DECK_CLUBS];
						target.TextAttributes.Cells[z][x] |= bSwaps[10] ? COLOR_YELLOW : COLOR_BLACK; 
					}
					else if(wallLabel == definitionsStageProp[board.Tiles.Entities.Props.Cells[z][x].Definition].Name){
						target.Screen.Cells[z][x] = ::klib::getASCIIWall(::gpk::view_grid<const STileProp>{board.Tiles.Entities.Props.begin(), board.Tiles.Entities.Props.metrics()}, x, z);
						target.TextAttributes.Cells[z][x] |= COLOR_BLACK; 
					}
					else {
						target.Screen.Cells[z][x] = definitionsStageProp[board.Tiles.Entities.Props	.Cells[z][x].Definition].Name[0];
						target.TextAttributes.Cells[z][x] |= COLOR_BLACK; 
					}
					if(board.Tiles.Entities.Props.Cells[z][x].Level == -1)
						target.TextAttributes.Cells[z][x] |= COLOR_DARKGREY; 
				} 
				else if(topologyHeight || cornerHeight[0] || cornerHeight[1] || cornerHeight[2] || cornerHeight[3]) {
					// I suppose this chooses a color depending on the slope between tile corners.
						 if(cornerHeight[0]	<	cornerHeight[3] && cornerHeight[0]	<	cornerHeight[1] && cornerHeight[0]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[1]	<	cornerHeight[0] && cornerHeight[1]	<	cornerHeight[3] && cornerHeight[1]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[2]	<	cornerHeight[0] && cornerHeight[2]	<	cornerHeight[1] && cornerHeight[2]	<	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | 0				; }
					else if(cornerHeight[3]	<	cornerHeight[0] && cornerHeight[3]	<	cornerHeight[1] && cornerHeight[3]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | COLOR_DARKGREY		; }

					else if(cornerHeight[0]	>	cornerHeight[3] && cornerHeight[0]	>	cornerHeight[1] && cornerHeight[0]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[1]	>	cornerHeight[0] && cornerHeight[1]	>	cornerHeight[3] && cornerHeight[1]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[2]	>	cornerHeight[0] && cornerHeight[2]	>	cornerHeight[1] && cornerHeight[2]	>	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | 0				; }
					else if(cornerHeight[3]	>	cornerHeight[0] && cornerHeight[3]	>	cornerHeight[1] && cornerHeight[3]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | COLOR_DARKGREY		; }
					///----------------------
					else if(cornerHeight[0]	>	cornerHeight[3] && cornerHeight[0]	<	cornerHeight[1] && cornerHeight[0]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[1]	>	cornerHeight[0] && cornerHeight[1]	<	cornerHeight[3] && cornerHeight[1]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[2]	>	cornerHeight[0] && cornerHeight[2]	<	cornerHeight[1] && cornerHeight[2]	<	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | 0				; }
					else if(cornerHeight[3]	>	cornerHeight[0] && cornerHeight[3]	<	cornerHeight[1] && cornerHeight[3]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | COLOR_DARKGREY		; }

					else if(cornerHeight[0]	<	cornerHeight[3] && cornerHeight[0]	>	cornerHeight[1] && cornerHeight[0]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[1]	<	cornerHeight[0] && cornerHeight[1]	>	cornerHeight[3] && cornerHeight[1]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[2]	<	cornerHeight[0] && cornerHeight[2]	>	cornerHeight[1] && cornerHeight[2]	<	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | 0				; }
					else if(cornerHeight[3]	<	cornerHeight[0] && cornerHeight[3]	>	cornerHeight[1] && cornerHeight[3]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | COLOR_DARKGREY		; }

					else if(cornerHeight[0]	<	cornerHeight[3] && cornerHeight[0]	<	cornerHeight[1] && cornerHeight[0]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[1]	<	cornerHeight[0] && cornerHeight[1]	<	cornerHeight[3] && cornerHeight[1]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[2]	<	cornerHeight[0] && cornerHeight[2]	<	cornerHeight[1] && cornerHeight[2]	>	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | 0				; }
					else if(cornerHeight[3]	<	cornerHeight[0] && cornerHeight[3]	<	cornerHeight[1] && cornerHeight[3]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | COLOR_DARKGREY		; }
					///----------------------
					else if(cornerHeight[0]	>	cornerHeight[3] && cornerHeight[0]	<	cornerHeight[1] && cornerHeight[0]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[1]	>	cornerHeight[0] && cornerHeight[1]	<	cornerHeight[3] && cornerHeight[1]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[2]	>	cornerHeight[0] && cornerHeight[2]	<	cornerHeight[1] && cornerHeight[2]	>	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | 0				; }
					else if(cornerHeight[3]	>	cornerHeight[0] && cornerHeight[3]	<	cornerHeight[1] && cornerHeight[3]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | COLOR_DARKGREY		; }

					else if(cornerHeight[0]	>	cornerHeight[3] && cornerHeight[0]	>	cornerHeight[1] && cornerHeight[0]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[1]	>	cornerHeight[0] && cornerHeight[1]	>	cornerHeight[3] && cornerHeight[1]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[2]	>	cornerHeight[0] && cornerHeight[2]	>	cornerHeight[1] && cornerHeight[2]	<	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | 0				; }
					else if(cornerHeight[3]	>	cornerHeight[0] && cornerHeight[3]	>	cornerHeight[1] && cornerHeight[3]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | COLOR_DARKGREY		; }

					else if(cornerHeight[0]	<	cornerHeight[3] && cornerHeight[0]	>	cornerHeight[1] && cornerHeight[0]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[1]	<	cornerHeight[0] && cornerHeight[1]	>	cornerHeight[3] && cornerHeight[1]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN	<< 4 | COLOR_DARKGREY		; }
					else if(cornerHeight[2]	<	cornerHeight[0] && cornerHeight[2]	>	cornerHeight[1] && cornerHeight[2]	>	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | 0				; }
					else if(cornerHeight[3]	<	cornerHeight[0] && cornerHeight[3]	>	cornerHeight[1] && cornerHeight[3]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_GREEN		<< 4 | COLOR_DARKGREY		; }

					//	 if(cornerHeight[0]	<	cornerHeight[1])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_GREEN << 4 | COLOR_DARKGREY; }
					//else if(cornerHeight[0]	<	cornerHeight[2])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_GREEN << 4 | COLOR_DARKGREY; }
					//else if(cornerHeight[1]	<	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN << 4 | COLOR_DARKGREY; }
					//else if(cornerHeight[2]	<	cornerHeight[3])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_GREEN << 4 | COLOR_DARKGREEN; }
					//
					//else if(cornerHeight[0]	>	cornerHeight[1])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN << 4; }
					//else if(cornerHeight[0]	>	cornerHeight[2])	{	target.Screen.Cells[z][x] = -80; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN << 4; }
					//else if(cornerHeight[1]	>	cornerHeight[3])	{	target.Screen.Cells[z][x] = -78; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN << 4; }
					//else if(cornerHeight[2]	>	cornerHeight[3])	{	target.Screen.Cells[z][x] = -79; target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN << 4; }
					else											
					{	
						target.Screen.Cells[z][x] = ' '; 
						target.TextAttributes.Cells[z][x] = COLOR_DARKGREEN << 4; 
					}
				}
				//else if(topologyHeight)
				//{
				//	if(topologyHeight < -FULL_COVER_HEIGHT)	
				//	{ 
				//		if(z&1) {
				//			if(x&1)	{ target.Screen.Cells[z][x] = ascii_water[(bSwaps[4]	) ? 1+(x&1) : 2-(x&1)]; target.TextAttributes.Cells[z][x] = iif(bSwaps[8]	) COLOR_BLUE		| (COLOR_DARKGREEN <<4) : COLOR_DARKBLUE	| (COLOR_DARKGREEN <<4); }
				//			else	{ target.Screen.Cells[z][x] = ascii_water[(bSwaps[5]	) ? 1+(x&1) : 2-(x&1)]; target.TextAttributes.Cells[z][x] = iif(bSwaps[9]	) COLOR_DARKBLUE	| (COLOR_DARKGREEN <<4) : COLOR_BLUE		| (COLOR_DARKGREEN <<4); }
				//		}
				//		else {
				//			if(x&1)	{ target.Screen.Cells[z][x] = ascii_water[(bSwaps[6]	) ? 2-(x&1) : 1+(x&1)]; target.TextAttributes.Cells[z][x] = iif(bSwaps[2]	) COLOR_DARKBLUE	| (COLOR_DARKGREEN  <<4) : COLOR_BLUE		| (COLOR_DARKGREEN  <<4); }
				//			else	{ target.Screen.Cells[z][x] = ascii_water[(bSwaps[7]	) ? 2-(x&1) : 1+(x&1)]; target.TextAttributes.Cells[z][x] = iif(bSwaps[3]	) COLOR_BLUE		| (COLOR_DARKGREEN  <<4) : COLOR_DARKBLUE	| (COLOR_DARKGREEN  <<4); }
				//		}
				//	} 
				//	else if(topologyHeight >= FULL_COVER_HEIGHT		)	{ target.Screen.Cells[z][x] = ascii_tones[(topologyHeight == FULL_COVER_HEIGHT		) ? 1 : topologyHeight		-FULL_COVER_HEIGHT		]	; target.TextAttributes.Cells[z][x] = (COLOR_DARKBLUE	) | (COLOR_DARKGREEN	<<4)	; } 
				//	else if(topologyHeight >= PARTIAL_COVER_HEIGHT	)	{ target.Screen.Cells[z][x] = ascii_tones[(topologyHeight == PARTIAL_COVER_HEIGHT	) ? 1 : topologyHeight		-PARTIAL_COVER_HEIGHT	]	; target.TextAttributes.Cells[z][x] = (COLOR_ORANGE		) | (COLOR_DARKGREEN	<<4)	; }
				//	else if(topologyHeight > 0						)	{ target.Screen.Cells[z][x] = ascii_tones[1+(topologyHeight)/3]																				; target.TextAttributes.Cells[z][x] = (COLOR_GREEN		) | (COLOR_DARKGREEN	<<4)	; } 
				//	else if(topologyHeight <= -FULL_COVER_HEIGHT	)	{ target.Screen.Cells[z][x] = ascii_tones[(topologyHeight == -FULL_COVER_HEIGHT		) ? 1 : (topologyHeight*-1)	-FULL_COVER_HEIGHT		]	; target.TextAttributes.Cells[z][x] = (COLOR_DARKGREEN	) | (COLOR_BLACK		<<4)	; } 
				//	else if(topologyHeight <= -PARTIAL_COVER_HEIGHT	)	{ target.Screen.Cells[z][x] = ascii_tones[(topologyHeight == -PARTIAL_COVER_HEIGHT	) ? 1 : (topologyHeight*-1)	-PARTIAL_COVER_HEIGHT	]	; target.TextAttributes.Cells[z][x] = (COLOR_DARKGREEN	) | (COLOR_DARKBLUE		<<4)	; }
				//	else if(topologyHeight < 0						)	{ target.Screen.Cells[z][x] = ascii_tones[1+(topologyHeight*-1)/3]																			; target.TextAttributes.Cells[z][x] = (COLOR_DARKGREEN	) | (COLOR_DARKGREY			<<4)	; } 
				//}
				else { 
					target.Screen			.Cells[z][x]	= ' '; 
					target.TextAttributes	.Cells[z][x]	= (COLOR_DARKGREEN << 4); 
				}

				if(initialSight != finalSight)
					target.TextAttributes.Cells[z][x]		= (target.TextAttributes.Cells[z][x] & 0xF) | (COLOR_WHITE << 4);
				
				for(uint32_t iAOE=0, countAOE=board.AreaOfEffect.AOE.Count; iAOE <countAOE; ++iAOE) {
					const SAOE& aoeInstance = board.AreaOfEffect.AOE[iAOE].Entity;
					const ::gpk::SCoord3<int32_t>& aoeCell = aoeInstance.Position.Cell;
					::gpk::SCoord3<float> aoePos = {(float)aoeCell.x, (float)aoeCell.y, (float)aoeCell.z};
					aoePos += aoeInstance.Position.Offset;

					bool failBVTest = 
						 ( x < (aoePos.x - aoeInstance.RadiusOrHalfSize)
						|| y < (aoePos.y - aoeInstance.RadiusOrHalfSize)
						|| z < (aoePos.z - aoeInstance.RadiusOrHalfSize)
						) //
					  || ( x > (aoePos.x + aoeInstance.RadiusOrHalfSize)
						|| y > (aoePos.y + aoeInstance.RadiusOrHalfSize)
						|| z > (aoePos.z + aoeInstance.RadiusOrHalfSize)
						);

					if(failBVTest)
						continue;

					if((aoePos-currentTilePos).Length() <= aoeInstance.RadiusOrHalfSize) {
						if(aoeInstance.StatusInflict)
							target.TextAttributes.Cells[z][x] = (target.TextAttributes.Cells[z][x] & 0xF) | (getStatusColor(aoeInstance.StatusInflict, bSwaps[2], COLOR_ORANGE) << 4);
						else
							target.TextAttributes.Cells[z][x] = (target.TextAttributes.Cells[z][x] & 0xF) | (COLOR_GREEN << 4);
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

	template<size_t _Width, size_t _Depth>
	void									drawTacticalBoard				(SGame& instanceGame, STacticalInfo& tacticalInfo, SWeightedDisplay<_Width, _Depth>& target, int8_t playerIndex, TEAM_TYPE teamId, const SPlayerSelection& selection, bool bFogOfWar)			{
		char										hiddenTile						= -78;
		memset(&target.Screen.Cells[0][0], hiddenTile, _Width*_Depth);
				
		uint16_t									gridColor						= COLOR_DARKGREEN << 4;
		uint64_t									gridColorPacked					= gridColor;
		gridColorPacked							|= (gridColorPacked << 16) | (gridColorPacked << 32) | (gridColorPacked << 48);
		int32_t										widthOverColorSize				= _Width>>2;
		for(uint32_t z = 0; z < _Depth; ++z) {
			::klib::valueToGrid(target.TextAttributes, z, 0, ::klib::SCREEN_LEFT, (uint16_t*)&gridColorPacked, 4, widthOverColorSize);
		}
		::klib::boardToDisplay(instanceGame, tacticalInfo.Board, target, playerIndex, teamId, selection, bFogOfWar);
	}
}

#endif // __TACTICAL_DRAW_H__2309748209346029374609821764__
