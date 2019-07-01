// 7001	7013	7019	7027	7039	7043	7057	7069	7079	7103
//7109	7121	7127	7129	7151	7159	7177	7187	7193	7207
//7211	7213	7219	7229	7237	7243	7247	7253	7283	7297
//7307	7309	7321	7331	7333	7349	7351	7369	7393	7411
//7417	7433	7451	7457	7459	7477	7481	7487	7489	7499
//7507	7517	7523	7529	7537	7541	7547	7549	7559	7561
//7573	7577	7583	7589	7591	7603	7607	7621	7639	7643
//7649	7669	7673	7681	7687	7691	7699	7703	7717	7723
//7727	7741	7753	7757	7759	7789	7793	7817	7823	7829
//7841	7853	7867	7873	7877	7879	7883	7901	7907	7919
//7927	7933	7937	7949	7951	7963	7993	8009	8011	8017
//8039	8053	8059	8069	8081	8087	8089	8093	8101	8111
//8117	8123	8147	8161	8167	8171	8179	8191	8209	8219
//8221	8231	8233	8237	8243	8263	8269	8273	8287	8291
//8293	8297	8311	8317	8329	8353	8363	8369	8377	8387
//8389	8419	8423	8429	8431	8443	8447	8461	8467	8501
//8513	8521	8527	8537	8539	8543	8563	8573	8581	8597
//8599	8609	8623	8627	8629	8641	8647	8663	8669	8677
//8681	8689	8693	8699	8707	8713	8719	8731	8737	8741
//8747	8753	8761	8779	8783	8803	8807	8819	8821	8831
//8837	8839	8849	8861	8863	8867	8887	8893	8923	8929
//8933	8941	8951	8963	8969	8971	8999	9001	9007	9011
//9013	9029	9041	9043	9049	9059	9067	9091	9103	9109
//9127	9133	9137	9151	9157	9161	9173	9181	9187	9199
//9203	9209	9221	9227	9239	9241	9257	9277	9281	9283
//9293	9311	9319	9323	9337	9341	9343	9349	9371	9377
//9391	9397	9403	9413	9419	9421	9431	9433	9437	9439
//9461	9463	9467	9473	9479	9491	9497	9511	9521	9533
//9539	9547	9551	9587	9601	9613	9619	9623	9629	9631
//9643	9649	9661	9677	9679	9689	9697	9719	9721	9733
//9739	9743	9749	9767	9769	9781	9787	9791	9803	9811
//9817	9829	9833	9839	9851	9857	9859	9871	9883	9887
//9901	9907	9923	9929	9931	9941	9949	9967	9973	10007
//#define NOMINMAX

#include "tactical_draw.h"


#include "Enemy.h"
#include "Armor.h"
#include "Weapon.h"
#include "Accessory.h"
#include "Profession.h"

#include <algorithm>
#include "gpk_noise.h"

#include "klib_random_generator.h"


using namespace klib;

bool												initCampaignGame									(SGame& instanceGame);
bool												initTacticalGame									(SGame& instanceGame) {
	if(instanceGame.Mode == GAME_MODE_CAMPAIGN)
		return ::initCampaignGame(instanceGame);

	return false;
}

//static bool											isEnemyTeam											(TEAM_TYPE teamIdCurrent, TEAM_TYPE teamIdPossibleEnemy) { return isRelevantTeam(teamIdPossibleEnemy) && (teamIdCurrent != teamIdPossibleEnemy); }

void												deployCampaignAgents
	( SPlayer																				& player
	, const int8_t																			playerIndex
	, STacticalSetup																		& tacticalSetup
	, const ::klib::SGrid<STopologyDetail, STacticalBoard::Width, STacticalBoard::Depth>	& terrainTopology	 
	, SEntityTiles<STacticalBoard::Width, STacticalBoard::Depth>							& terrainEntities	
	)
{
	const uint32_t											terrainWidth										= terrainTopology.Width, 
															terrainDepth										= terrainTopology.Depth;

	const TEAM_TYPE											teamId												= tacticalSetup.TeamPerPlayer[playerIndex];
	const int32_t											squadSize											= tacticalSetup.SquadSize[playerIndex];
	int64_t													seed												= tacticalSetup.Seed + playerIndex;

	int32_t													rangeX												= terrainWidth / 5;
	int32_t													rangeZ												= terrainDepth / 5;


	for(uint32_t iAgent=0, agentCount = squadSize; iAgent<agentCount; iAgent++)	{
		if(player.Squad.Agents[iAgent] == -1)
			continue;

		CCharacter												& playerAgent										= *player.Army[player.Squad.Agents[iAgent]];

		if(!playerAgent.IsAlive())
			continue;

		::gpk::SCoord3<int32_t>								agentPosition										= {0,0,0};
		if(::isRelevantTeam(teamId))  {
			agentPosition.x										= 1 + (int32_t)(rangeX * ::gpk::noiseNormal1D(iAgent, seed			) );
			agentPosition.z										= 1 + (int32_t)(rangeZ * ::gpk::noiseNormal1D(iAgent, seed << 8	) );
			if(teamId == TEAM_TYPE_ENEMY) {
				agentPosition.x										+= terrainWidth - rangeX - 2;
				agentPosition.z										+= terrainDepth - rangeZ - 2;
			}
		}
		else {
			agentPosition.x										= 1 + (int32_t)((terrainWidth - 1) * ::gpk::noiseNormal1D(iAgent, seed)	);
			agentPosition.z										= 1 + (int32_t)((terrainDepth - 1) * ::gpk::noiseNormal1D(iAgent, seed<<8)	);
		}

		while( terrainTopology			[agentPosition.z][agentPosition.x].Sharp		>=	PARTIAL_COVER_HEIGHT
			|| terrainTopology			[agentPosition.z][agentPosition.x].Smooth		>=	PARTIAL_COVER_HEIGHT
			|| (terrainTopology			[agentPosition.z][agentPosition.x].Sharp		
			  + terrainTopology			[agentPosition.z][agentPosition.x].Smooth)		>=	PARTIAL_COVER_HEIGHT
			|| terrainEntities.Agents	[agentPosition.z][agentPosition.x].PlayerIndex	!= -1
			|| terrainEntities.Props	[agentPosition.z][agentPosition.x].Definition	!= -1 
			)
		{
			if(::isRelevantTeam(teamId)) {
				agentPosition.x										= 1 + (int32_t)(rangeX * ::gpk::noiseNormal1D(	(1+iAgent)		* agentPosition.z * (iAgent+agentPosition.x), seed+rangeZ) );
				agentPosition.z										= 1 + (int32_t)(rangeZ * ::gpk::noiseNormal1D(((1+iAgent)<<16)	* agentPosition.x, seed = (int32_t)time(0))	);
				if(teamId == TEAM_TYPE_ENEMY)  {
					agentPosition.x										+= terrainWidth-rangeX-2;
					agentPosition.z										+= terrainDepth-rangeZ-2;
				}
			}
			else {
				agentPosition.x										= 1+(int32_t)((terrainWidth-1) * ::gpk::noiseNormal1D((1+iAgent)*agentPosition.z*(iAgent+agentPosition.x), seed+rangeZ)	);
				agentPosition.z										= 1+(int32_t)((terrainDepth-1) * ::gpk::noiseNormal1D(((1+iAgent)<<16)*agentPosition.x, seed = (int32_t)time(0))			);
			}
		}

		playerAgent.Position								= agentPosition;
		player.Squad.TargetPositions[iAgent]				= agentPosition;
		::klib::STileCharacter									& terrainAgent										= terrainEntities.Agents[agentPosition.z][agentPosition.x];
		terrainAgent.TeamId									= teamId;
		terrainAgent.PlayerIndex							= playerIndex;
		terrainAgent.SquadIndex								= 0;
		terrainAgent.AgentIndex								= (int8_t)iAgent;

		//playerAgent.Recalculate();
		player.Squad.ActionsLeft[iAgent].Moves				= (int8_t)playerAgent.FinalPoints.Fitness.Movement;
	}
}

void												generateTopology									( ::klib::SGrid<STopologyDetail, STacticalBoard::Width, STacticalBoard::Depth>	& terrainTopology, int64_t seed )		{
	const uint32_t											terrainWidth										= terrainTopology.Width
		,													terrainDepth										= terrainTopology.Depth
		;
	::klib::fillCellsFromNoise(terrainTopology, {- 1,  0}, (int32_t)(seed+ 987+ 1), 200);
	::klib::fillCellsFromNoise(terrainTopology, {-15,  0}, (int32_t)(seed+7331+ 5), 200);

	STopologyDetail											* cellsHeight										= &terrainTopology	.Cells[0][0];
	for(uint32_t i=0, count = terrainDepth*terrainWidth; i<count; i++) 
		cellsHeight[i].Collision							= cellsHeight[i].Sharp + cellsHeight[i].Smooth;
}
//
void												populateProps
	( ::klib::SGrid<STopologyDetail, STacticalBoard::Width, STacticalBoard::Depth>	& terrainTopology	 
	, SEntityTiles<STacticalBoard::Width, STacticalBoard::Depth>					& terrainEntities	
	, int64_t																		seed
	, int32_t																		maxCoins
	)
{
	const uint32_t											terrainWidth										= terrainTopology.Width, 
															terrainDepth										= terrainTopology.Depth;

	static const ::gpk::label								labelWall											= "Wall";
	for(uint32_t z=0; z<terrainDepth; ++z)
		for(uint32_t x=0; x<terrainWidth; ++x) {
			double													noise[]												= 
				{	::gpk::noiseNormal1D(z*terrainDepth+x, seed)
				,	::gpk::noiseNormal1D(z*terrainDepth+x, seed*7187)
				,	::gpk::noiseNormal1D(z*terrainDepth+x, seed*6719)
				,	::gpk::noiseNormal1D(z*terrainDepth+x, seed*8443)
				,	::gpk::noiseNormal1D(z*terrainDepth+x, seed*7883)
				,	::gpk::noiseNormal1D(z*terrainDepth+x, seed*8087)
				,	::gpk::noiseNormal1D(z*terrainDepth+x, seed*8081)
				,	::gpk::noiseNormal1D(z*terrainDepth+x, seed*9419)
				,	::gpk::noiseNormal1D(z*terrainDepth+x, seed*9413)			
				}; 
			bool													bReinforced											= noise[3] > .5;
			if( terrainTopology.Cells[z][x].Sharp	< PARTIAL_COVER_HEIGHT 
			 && terrainTopology.Cells[z][x].Smooth	< PARTIAL_COVER_HEIGHT 
			 && (terrainTopology.Cells[z][x].Smooth + terrainTopology.Cells[z][x].Sharp) < PARTIAL_COVER_HEIGHT 
			 && terrainEntities.Props.Cells[z][x].Definition == -1 
			 && noise[0] > 0.98
			 ) 
			{ 
				int16_t													defCheck											= 1+(int16_t)(rand()%(::gpk::size(definitionsStageProp)-1));
				terrainEntities.Props.Cells[z][x].Definition		= (int8_t)defCheck;
				terrainEntities.Props.Cells[z][x].Modifier			= bReinforced ? 1 : 0;
				terrainEntities.Props.Cells[z][x].Level				= 1;
				if(definitionsStageProp[defCheck].Name == labelWall) {
					uint32_t												wallmaxz											= ::std::min(z+3+uint32_t(noise[1]*10), terrainDepth-1);
					uint32_t												wallmaxx											= ::std::min(x+3+uint32_t(noise[2]*10), terrainWidth-1);
					for(uint32_t wallz=z; wallz<=wallmaxz; ++wallz)	{	if(noise[5] > 0.95 || ::klib::randNoise(9941) > 0.95)	continue;	terrainEntities.Props.Cells[wallz]		[x]			.Definition = (int8_t)defCheck; terrainEntities.Props.Cells[wallz]		[x]			.Modifier = bReinforced ? 1 : 0; terrainEntities.Props.Cells[wallz]		[x]			.Level = 1; }
					for(uint32_t wallz=z; wallz<=wallmaxz; ++wallz)	{	if(noise[6] > 0.95 || ::klib::randNoise(9941) > 0.95)	continue;	terrainEntities.Props.Cells[wallz]		[wallmaxx]	.Definition = (int8_t)defCheck; terrainEntities.Props.Cells[wallz]		[wallmaxx]	.Modifier = bReinforced ? 1 : 0; terrainEntities.Props.Cells[wallz]		[wallmaxx]	.Level = 1; }
					for(uint32_t wallx=x; wallx<=wallmaxx; ++wallx)	{	if(noise[7] > 0.95 || ::klib::randNoise(9941) > 0.95)	continue;	terrainEntities.Props.Cells[z]			[wallx]		.Definition = (int8_t)defCheck; terrainEntities.Props.Cells[z]			[wallx]		.Modifier = bReinforced ? 1 : 0; terrainEntities.Props.Cells[z]			[wallx]		.Level = 1; }
					for(uint32_t wallx=x; wallx<=wallmaxx; ++wallx)	{	if(noise[8] > 0.95 || ::klib::randNoise(9941) > 0.95)	continue;	terrainEntities.Props.Cells[wallmaxz]	[wallx]		.Definition = (int8_t)defCheck; terrainEntities.Props.Cells[wallmaxz]	[wallx]		.Modifier = bReinforced ? 1 : 0; terrainEntities.Props.Cells[wallmaxz]	[wallx]		.Level = 1; }
				}
			}
			else if(terrainTopology.Cells[z][x].Sharp < PARTIAL_COVER_HEIGHT && terrainTopology.Cells[z][x].Smooth < PARTIAL_COVER_HEIGHT 
				&& (terrainTopology.Cells[z][x].Smooth+terrainTopology.Cells[z][x].Sharp) < PARTIAL_COVER_HEIGHT 
				&& terrainEntities.Props.Cells[z][x].Definition == -1 
				&& noise[2] > 0.95
			)
				terrainEntities.Coins.Cells[z][x] = ::rand()%(1+maxCoins);
		}
}

int32_t												getEnemyCoinsForTerrainFun							(SGame& instanceGame)																	{
	int32_t													maxCoins											= 0;
	STacticalSetup											& tacticalSetup										= instanceGame.TacticalInfo.Setup;
	int32_t													totalAgents											= 0;
	for(size_t iTacticalPlayer=0, playerCount = tacticalSetup.TotalPlayers; iTacticalPlayer<playerCount; ++iTacticalPlayer) {
		if(tacticalSetup.Players[iTacticalPlayer] == -1)
			continue;

		SPlayer													& enemy												= instanceGame.Players[tacticalSetup.Players[iTacticalPlayer]];
		if( enemy.Control.Type == PLAYER_CONTROL_AI && enemy.Control.AIMode != PLAYER_AI_TEAMERS )
			continue;

		for(size_t iAgent=0, count=tacticalSetup.SquadSize[iTacticalPlayer]; iAgent<count; ++iAgent) {
			if(enemy.Squad.Agents[iAgent] != -1) {
				CCharacter												& characterAgent									= *enemy.Army[enemy.Squad.Agents[iAgent]];
				if(characterAgent.IsAlive()) {
					maxCoins											+= characterAgent.FinalPoints.CostMaintenance;
					++totalAgents;
				}
			}
		}
	}

	return totalAgents ? maxCoins / totalAgents / 4 : 1;
}

void												recalculateAgentsInRangeAndSight					(SGame& instanceGame);

void												klib::initTacticalMap								(SGame& instanceGame)																	{
	STacticalInfo											& tacticalInfo										= instanceGame.TacticalInfo;
	tacticalInfo.Board.Clear();

	::klib::SGrid<STopologyDetail
		, STacticalBoard::Width
		, STacticalBoard::Depth
		>													& terrainTopology									= tacticalInfo.Board.Tiles.Terrain.Topology;
	SEntityTiles
		< STacticalBoard::Width
		, STacticalBoard::Depth
		>													& terrainEntities									= tacticalInfo.Board.Tiles.Entities;
	int64_t													seed												= tacticalInfo.Setup.Seed;

	int32_t													maxCoins											= ::getEnemyCoinsForTerrainFun(instanceGame);

	::generateTopology(terrainTopology, seed);
	::populateProps(terrainTopology, terrainEntities, seed, maxCoins);

	// We need to deploy the agents after we generated the map so all the player initialization is done before calling this function
	for(uint32_t iTacticalPlayer = 0, tacticalPlayerCount = tacticalInfo.Setup.TotalPlayers; iTacticalPlayer<tacticalPlayerCount; ++iTacticalPlayer) {
		if(tacticalInfo.Setup.Players[iTacticalPlayer] != -1)
			::deployCampaignAgents(instanceGame.Players[tacticalInfo.Setup.Players[iTacticalPlayer]], (int8_t)iTacticalPlayer, tacticalInfo.Setup, terrainTopology, terrainEntities);

	}
	::recalculateAgentsInRangeAndSight(instanceGame);

}


void												initTacticalPlayer									(SGame& instanceGame, int32_t playerSlot, const STacticalSetup& tacticalSetup)			{
	SPlayer													& player											= instanceGame.Players[tacticalSetup.Players[playerSlot]];
	player.Squad.Size									= tacticalSetup.SquadSize	[playerSlot];
	player.Control										= tacticalSetup.Controls	[playerSlot];
	player.Selection									= {0, 0, -1, -1, -1};
	player.Squad.LockedAgent							= -1;

	for(uint32_t iAgent = 0, count = tacticalSetup.SquadSize[playerSlot]; iAgent<count; ++iAgent) {	// Resize non-human armies.
		if(player.Squad.Agents[iAgent] == -1)
			continue;

		CCharacter												& playerAgent										= *player.Army[player.Squad.Agents[iAgent]];
		playerAgent.ActiveBonus								= SCharacterTurnBonus();
		playerAgent.Recalculate();
		const SEntityPoints										& agentPoints										= playerAgent.FinalPoints;
		playerAgent.Points.LifeCurrent						= agentPoints.LifeMax;

		player.Squad.ActionsLeft		[iAgent].Moves		= (int8_t)agentPoints.Fitness.Movement;
		player.Squad.ActionsLeft		[iAgent].Actions	= 1;
		player.Squad.TargetPositions	[iAgent]			= playerAgent.Position;
		player.Squad.TargetAgents		[iAgent]			= {TEAM_TYPE_INVALID, -1, -1, -1};
	}
}

template<size_t _Size>
uint32_t											getRelevantTeams									(SGame& instanceGame, const STacticalInfo& tacticalInfo, TEAM_TYPE (&teams)[_Size] )	{
	uint32_t												teamCount											= 0;

	for(size_t iPlayer = 0; iPlayer < _Size; ++iPlayer) {
		if(!::isRelevantTeam(tacticalInfo.Setup.TeamPerPlayer[iPlayer]))
			continue;

		bool													bAdded												= false;
		for(size_t iPlayerOther=0; iPlayerOther < teamCount; ++iPlayerOther) {
			if(tacticalInfo.Setup.TeamPerPlayer[iPlayer] == teams[iPlayerOther]) {
				bAdded												= true;
				break;
			}
		}

		if(bAdded)
			continue;

		teams[teamCount++]									= tacticalInfo.Setup.TeamPerPlayer[iPlayer];
	}
	return teamCount;
};

namespace klib
{
	// We use these values to identify the default player usage when generating a new board
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, ALLY_0		,  1);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, ALLY_1		,  2);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, ALLY_2		,  3);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, ALLY_3		,  4);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, ENEMY_0	,  5);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, ENEMY_1	,  6);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, ENEMY_2	,  7);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, ENEMY_3	,  8);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, AI_NEUTRAL	,  9);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, AI_ASSISTS	, 10);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, AI_RIOTERS	, 11);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, AI_VIOLENT	, 12);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, AI_FEARFUL	, 13);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, AI_CURIOUS	, 14);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, REMOTE_0	, 15);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, REMOTE_1	, 16);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, REMOTE_2	, 17);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, REMOTE_3	, 18);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, REMOTE_4	, 19);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, REMOTE_5	, 20);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, REMOTE_6	, 21);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, REMOTE_7	, 22);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, SPECTATOR	, 23);
}

void												setupAIPlayer										(SGame& /*instanceGame*/)										{ }

bool												initFromTacticalSetup								(SGame& instanceGame, const STacticalSetup& tacticalSetup)		{
	//STacticalInfo											& tacticalInfo										= instanceGame.TacticalInfo;
	uint32_t												effectivePlayers									= 0;
	for(uint32_t iTacticalPlayer = 0, playerCount = tacticalSetup.TotalPlayers; iTacticalPlayer < playerCount; ++iTacticalPlayer)
		::initTacticalPlayer(instanceGame, effectivePlayers++, tacticalSetup);

	return true;
}

static const STacticalSetup							tacticalSetupForCampaign							= 
	{	15731U										//, uint64_t		Seed					= 15731;
	,	4U											//,	uint32_t		TotalPlayers			= 0;
	,	2U											//	uint32_t		TotalTeams				= 0;
	,	{ PLAYER_INDEX_USER
		, (PLAYER_INDEX)1
		, (PLAYER_INDEX)2
		, (PLAYER_INDEX)3
		, (PLAYER_INDEX)4
		, (PLAYER_INDEX)5
		, (PLAYER_INDEX)6
		, (PLAYER_INDEX)7
		, (PLAYER_INDEX)8
		, (PLAYER_INDEX)9
		}											//PLAYER_INDEX	Players						[MAX_TACTICAL_PLAYERS]	= {};
	,	{ {PLAYER_CONTROL_LOCAL, PLAYER_AI_TEAMERS}
		, {PLAYER_CONTROL_AI, PLAYER_AI_TEAMERS}
		, {PLAYER_CONTROL_AI, PLAYER_AI_TEAMERS}
		, {PLAYER_CONTROL_AI, PLAYER_AI_TEAMERS}
		, {PLAYER_CONTROL_AI, PLAYER_AI_NEUTRAL}
		, {PLAYER_CONTROL_AI, PLAYER_AI_CURIOUS}
		, {PLAYER_CONTROL_AI, PLAYER_AI_FEARFUL}
		, {PLAYER_CONTROL_AI, PLAYER_AI_ASSISTS}
		, {PLAYER_CONTROL_AI, PLAYER_AI_RIOTERS}
		, {PLAYER_CONTROL_AI, PLAYER_AI_VIOLENT}
		}											//SPlayerControl	Controls				[MAX_TACTICAL_PLAYERS]	= {};
	,	{ TEAM_TYPE_ALLY
		, TEAM_TYPE_ALLY
		, TEAM_TYPE_ENEMY
		, TEAM_TYPE_ENEMY
		, TEAM_TYPE_CIVILIAN
		, TEAM_TYPE_CIVILIAN
		, TEAM_TYPE_CIVILIAN
		, TEAM_TYPE_CIVILIAN
		, TEAM_TYPE_CIVILIAN
		, TEAM_TYPE_CIVILIAN
		}											//TEAM_TYPE			Teams					[MAX_TACTICAL_PLAYERS]	= {};
	,	{{0,1}, {2,3}, {4,5,6,7,8,9}}				//int8_t			PlayersPerTeam			[MAX_TACTICAL_PLAYERS][MAX_TACTICAL_PLAYERS]	= {};
	,	{2,2,5}										//int8_t			PlayerCountPerTeam		[MAX_TACTICAL_PLAYERS]	= {};
	,	{DEFAULT_SQUAD_SIZE,DEFAULT_SQUAD_SIZE,DEFAULT_SQUAD_SIZE,DEFAULT_SQUAD_SIZE, 2,2,2,2, 1,1}						//uint8_t			SquadSize				[MAX_TACTICAL_PLAYERS]	= {};
	};

void												getDefaultTacticalSetupForCampaign					(STacticalSetup& tacticalSetup)									{ tacticalSetup = tacticalSetupForCampaign; }

uint32_t											resolveNextPlayer									(SGame& instanceGame);
bool												initCampaignPlayers									(SGame& instanceGame)											{
	STacticalInfo											& tacticalInfo										= instanceGame.TacticalInfo;
	STacticalSetup											& tacticalSetup										= tacticalInfo.Setup;
	// Team players
	//instanceGame.Players[tacticalSetup.Players[0]].Name = "ALLY_0";
	instanceGame.Players[tacticalSetup.Players[1]].Name	= "Ivan"	;
	instanceGame.Players[tacticalSetup.Players[2]].Name	= "G0"		;
	instanceGame.Players[tacticalSetup.Players[3]].Name	= "G1"		;
	
	// Civilian players
	for(uint32_t iTacticalPlayer=0, maxPlayers=tacticalSetup.TotalPlayers; iTacticalPlayer<maxPlayers; ++iTacticalPlayer)
		if(tacticalSetup.Controls[iTacticalPlayer].Type == PLAYER_CONTROL_AI && ::gpk::bit_false(tacticalSetup.Controls[iTacticalPlayer].AIMode, PLAYER_AI_TEAMERS))
			instanceGame.Players[tacticalSetup.Players[iTacticalPlayer]].Name = ::gpk::get_value_label(tacticalSetup.Controls[iTacticalPlayer].AIMode);

	// Clear selection and reset player stuff relevant to the tactical mode.
	SPlayer													& playerUser										= instanceGame.Players[PLAYER_INDEX_USER];
	// Set up AI player's agents using the main human player as reference in order to determine the level and equipment.
	for(uint32_t iPlayer = 0; iPlayer<tacticalSetup.TotalPlayers; ++iPlayer) {
		if(tacticalSetup.Players[iPlayer] == PLAYER_INDEX_INVALID || tacticalSetup.Players[iPlayer] == PLAYER_INDEX_USER)
			continue;

		// Set up AI squads
		if(tacticalSetup.Controls[iPlayer].Type != PLAYER_CONTROL_AI)
			continue;

		SPlayer													& playerAI											= instanceGame.Players[tacticalSetup.Players[iPlayer]];
		if(tacticalSetup.Controls[iPlayer].Type == PLAYER_CONTROL_AI && tacticalSetup.Players[iPlayer] != PLAYER_INDEX_USER) 
			playerAI.Army.resize(tacticalSetup.SquadSize[iPlayer]);

		bool													bHeroSet											= true;
		for(uint32_t iSquadAgentSlot=0, squadAgentCount=tacticalSetup.SquadSize[iPlayer]; iSquadAgentSlot<squadAgentCount; ++iSquadAgentSlot) {
			if( playerUser.Squad.Agents[iSquadAgentSlot] == -1 || 0 == playerUser.Army[playerUser.Squad.Agents[iSquadAgentSlot]] ) {
				playerAI.Squad.Agents[iSquadAgentSlot]				= -1;
				continue;
			}
			else 
				playerAI.Squad.Agents[iSquadAgentSlot]				= (int16_t)iSquadAgentSlot;

			//if( 0 == playerAI.Army[playerAI.Squad.Agents[iSquadAgentSlot]] ) 
			//{
				::gpk::ptr_obj<::klib::CCharacter>										newAgent											;
				newAgent.create(enemyDefinitions[1+rand()%3]);
				playerAI.Army[playerAI.Squad.Agents[iSquadAgentSlot]] = newAgent;
			//}
			CCharacter												& agentAI											= *playerAI		.Army[playerAI	.Squad.Agents[iSquadAgentSlot]];
			const CCharacter										& agentUser											= *playerUser	.Army[playerUser.Squad.Agents[iSquadAgentSlot]];
			if(playerUser.Score.BattlesWon <= 0) {
				if(tacticalSetup.TeamPerPlayer[iPlayer] != TEAM_TYPE_ALLY)
					agentAI												= enemyDefinitions[2+(rand()&1)];
				if(bHeroSet) {
					::setupAgent(agentAI, agentAI);
					agentAI.Flags.Tech.Gender							= GENDER_FEMALE;
				}
				else {
					if(tacticalSetup.TeamPerPlayer[iPlayer] != TEAM_TYPE_ALLY)
						::setupAgent(agentAI, agentAI);
					else {
						bHeroSet											= true;
						agentAI												= enemyDefinitions[::gpk::size(enemyDefinitions)-1];
						::setupAgent(agentAI, agentAI);
						agentAI.CurrentEquip.Weapon		.Definition			= (int16_t)::gpk::size(definitionsWeapon)-1;
						agentAI.CurrentEquip.Armor		.Definition			= (int16_t)::gpk::size(definitionsArmor)-1;
						agentAI.CurrentEquip.Profession	.Definition			= (int16_t)::gpk::size(definitionsProfession)-1;
						agentAI.CurrentEquip.Accessory	.Definition			= (int16_t)::gpk::size(definitionsAccessory)-1;
						agentAI.CurrentEquip.Weapon		.Modifier			= (int16_t)::gpk::size(modifiersWeapon)-1;
						agentAI.CurrentEquip.Armor		.Modifier			= (int16_t)::gpk::size(modifiersArmor)-1;
						agentAI.CurrentEquip.Profession	.Modifier			= (int16_t)::gpk::size(modifiersProfession)-1;
						agentAI.CurrentEquip.Accessory	.Modifier			= (int16_t)::gpk::size(modifiersAccessory)-1;
						agentAI.CurrentEquip.Weapon		.Level				= 
						agentAI.CurrentEquip.Armor		.Level				=  
						agentAI.CurrentEquip.Profession	.Level				= 
						agentAI.CurrentEquip.Accessory	.Level				= 15;
						agentAI.Recalculate();
						const SEntityPoints										& finalAgentPoints									= agentAI.FinalPoints;
						agentAI.Points.LifeCurrent							= finalAgentPoints.LifeMax;
						agentAI.Points.Coins								= agentAI.Points.CostMaintenance;
						agentAI.Flags.Tech.Gender							= GENDER_MALE;
					}
				}
			}
			else {
				::setupAgent(agentUser, agentAI);
			}
		}
	}
	::initFromTacticalSetup(instanceGame, tacticalSetup);
	return false;
}

bool												initCampaignGame									(SGame& instanceGame)											{
	STacticalInfo											& tacticalInfo										= instanceGame.TacticalInfo;
	tacticalInfo.Clear();

	STacticalSetup											& tacticalSetup										= tacticalInfo.Setup;
	::getDefaultTacticalSetupForCampaign(tacticalSetup);
	tacticalSetup.Seed									= instanceGame.Seed + instanceGame.Players[PLAYER_INDEX_USER].Score.BattlesWon;
	::initCampaignPlayers(instanceGame);
	::klib::initTacticalMap(instanceGame);
	::klib::drawTacticalBoard(instanceGame, tacticalInfo, instanceGame.PostEffectDisplay, PLAYER_INDEX_USER, TEAM_TYPE_CIVILIAN, instanceGame.Players[PLAYER_INDEX_USER].Selection, true);

	::gpk::bit_set(instanceGame.Flags, klib::GAME_FLAGS_TACTICAL);
	tacticalInfo.CurrentPlayer							= (int8_t)::resolveNextPlayer(instanceGame);
	return true;
}