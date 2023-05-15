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

#include "tactical_draw.h"


#include "Enemy.h"

#include "gpk_eval.h"
#include "gpk_noise.h"

#include "klib_random_generator.h"

template<typename _tCell>
void												fillCellsFromNoise									(::gpk::view_grid<_tCell> grid, const _tCell& value, int64_t seed, int32_t diceFaces=10)														{
	_tCell													* cells												= grid.begin();
	for(uint32_t i = 0, count = grid.size();  i < count; ++i) {
		double													noise												= ::gpk::noiseNormal1D(i + 1, seed);
		int32_t													dice												= int32_t(noise * diceFaces);
		if(0 == dice)
			cells[i]											= value;
	}
}

bool												initCampaignGame									(::klib::SGame& instanceGame);
bool												initTacticalGame									(::klib::SGame& instanceGame) {
	if(instanceGame.Mode == ::klib::GAME_MODE_CAMPAIGN)
		return ::initCampaignGame(instanceGame);

	return false;
}

//static bool											isEnemyTeam											(TEAM_TYPE teamIdCurrent, TEAM_TYPE teamIdPossibleEnemy) { return isRelevantTeam(teamIdPossibleEnemy) && (teamIdCurrent != teamIdPossibleEnemy); }

void												deployCampaignAgents
	( ::klib::STacticalPlayer							& player
	, const int8_t										playerIndex
	, ::klib::STacticalSetup							& tacticalSetup
	, const ::gpk::view_grid<::klib::STopologyDetail>	terrainTopology
	, ::klib::SEntityTiles								& terrainEntities
	)
{
	const uint32_t											terrainWidth										= terrainTopology.metrics().x
		,													terrainDepth										= terrainTopology.metrics().y
		;

	const ::klib::TEAM_TYPE									teamId												= tacticalSetup.TeamPerPlayer[playerIndex];
	const int32_t											squadSize											= tacticalSetup.SquadSize[playerIndex];
	int64_t													seed												= tacticalSetup.Seed + playerIndex;

	const int32_t											rangeX												= terrainWidth / 5;
	const int32_t											rangeZ												= terrainDepth / 5;

	for(uint32_t iAgent = 0, agentCount = squadSize; iAgent < agentCount; iAgent++)	{
		if(player.Squad.Agents[iAgent] == -1)
			continue;

		::klib::CCharacter										& playerAgent										= *player.Army[player.Squad.Agents[iAgent]];

		if(!playerAgent.IsAlive())
			continue;

		::gpk::n3<int32_t>									agentPosition										= {0,0,0};
		if(::klib::isRelevantTeam(teamId))  {
			agentPosition.x										= 1 + (int32_t)(rangeX * ::gpk::noiseNormal1D(iAgent, seed			) );
			agentPosition.z										= 1 + (int32_t)(rangeZ * ::gpk::noiseNormal1D(iAgent, seed << 8	) );
			if(teamId == ::klib::TEAM_TYPE_ENEMY) {
				agentPosition.x										+= terrainWidth - rangeX - 2;
				agentPosition.z										+= terrainDepth - rangeZ - 2;
			}
		}
		else {
			agentPosition.x										= 1 + (int32_t)((terrainWidth - 1) * ::gpk::noiseNormal1D(iAgent, seed)	);
			agentPosition.z										= 1 + (int32_t)((terrainDepth - 1) * ::gpk::noiseNormal1D(iAgent, seed<<8)	);
		}
		bool check0 = true;
		bool check1 = true;
		bool check2 = true;
		bool check3 = true;
		bool check4 = true;
		while( check0
			|| check1
			|| check2
			|| check3
			|| check4
			)
		{
			if(::klib::isRelevantTeam(teamId)) {
				agentPosition.x										= 1 + (int32_t)(rangeX * ::gpk::noiseNormal1D(	(1+iAgent)		* agentPosition.z * (iAgent+agentPosition.x), seed+rangeZ) );
				agentPosition.z										= 1 + (int32_t)(rangeZ * ::gpk::noiseNormal1D(((1+iAgent)<<16)	* agentPosition.x, seed = (int32_t)time(0))	);
				if(teamId == ::klib::TEAM_TYPE_ENEMY)  {
					agentPosition.x										+= terrainWidth-rangeX-2;
					agentPosition.z										+= terrainDepth-rangeZ-2;
				}
			}
			else {
				agentPosition.x										= 1+(int32_t)((terrainWidth-1) * ::gpk::noiseNormal1D((1+iAgent)*agentPosition.z*(iAgent+agentPosition.x), seed+rangeZ)	);
				agentPosition.z										= 1+(int32_t)((terrainDepth-1) * ::gpk::noiseNormal1D(((1+iAgent)<<16)*agentPosition.x, seed = (int32_t)time(0))			);
			}
			const ::klib::STopologyDetail							& topologyCell					= terrainTopology[agentPosition.z][agentPosition.x];


			check0						= topologyCell.Sharp							>=	PARTIAL_COVER_HEIGHT;
			check1						= topologyCell.Smooth							>=	PARTIAL_COVER_HEIGHT;
			check2						= (topologyCell.Sharp + topologyCell.Smooth)	>=	PARTIAL_COVER_HEIGHT;
			check3						= terrainEntities.Agents	[agentPosition.z][agentPosition.x].PlayerIndex	!= -1;
			check4						= terrainEntities.Props		[agentPosition.z][agentPosition.x].Definition	!= -1;
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

void												generateTopology									(::gpk::view_grid<::klib::STopologyDetail>	terrainTopology, int64_t seed )		{
	//const uint32_t											terrainWidth										= terrainTopology.metrics().x
	//	,													terrainDepth										= terrainTopology.metrics().y
	//	;
	::fillCellsFromNoise(terrainTopology, {- 1,  0}, (int32_t)(seed +  987 + 1), 200);
	::fillCellsFromNoise(terrainTopology, {-15,  0}, (int32_t)(seed + 7331 + 5), 200);

	::klib::STopologyDetail									* cellsHeight										= terrainTopology.begin();
	for(uint32_t i=0, count = terrainTopology.size(); i<count; i++)
		cellsHeight[i].Collision							= cellsHeight[i].Sharp + cellsHeight[i].Smooth;
}
//
void												populateProps
	( const ::klib::SEntityTable<::klib::SStageProp>	& entityTable
	, ::gpk::view_grid<::klib::STopologyDetail>			terrainTopology
	, ::klib::SEntityTiles								& terrainEntities
	, int64_t											seed
	, int64_t											maxCoins
	)
{
	const uint32_t											terrainWidth										= terrainTopology.metrics().x
		,													terrainDepth										= terrainTopology.metrics().y
		;

	static const ::gpk::view_const_string					labelWall											= "Wall";
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
		if( terrainTopology	[z][x].Sharp	< PARTIAL_COVER_HEIGHT
		 && terrainTopology	[z][x].Smooth	< PARTIAL_COVER_HEIGHT
		 && (terrainTopology[z][x].Smooth + terrainTopology[z][x].Sharp) < PARTIAL_COVER_HEIGHT
		 && terrainEntities.Props[z][x].Definition	== -1
		 && noise[0] > 0.98
		 ) {
			int16_t													defCheck											= 1+(int16_t)(rand() % (entityTable.Definitions.size()-1));
			terrainEntities.Props[z][x].Definition		= (int8_t)defCheck;
			terrainEntities.Props[z][x].Modifier			= bReinforced ? 1 : 0;
			terrainEntities.Props[z][x].Level				= 1;
			if(entityTable.Definitions[defCheck].Name == labelWall) {
				uint32_t												wallmaxz											= ::gpk::min(z + 3 + uint32_t(noise[1]*10), terrainDepth-1);
				uint32_t												wallmaxx											= ::gpk::min(x + 3 + uint32_t(noise[2]*10), terrainWidth-1);
				for(uint32_t wallz=z; wallz<=wallmaxz; ++wallz)	{	if(noise[5] > 0.95 || ::klib::randNoise(9941) > 0.95)	continue;	terrainEntities.Props[wallz]	[x]			.Definition = (int8_t)defCheck; terrainEntities.Props[wallz]	[x]			.Modifier = bReinforced ? 1 : 0; terrainEntities.Props[wallz]		[x]			.Level = 1; }
				for(uint32_t wallz=z; wallz<=wallmaxz; ++wallz)	{	if(noise[6] > 0.95 || ::klib::randNoise(9941) > 0.95)	continue;	terrainEntities.Props[wallz]	[wallmaxx]	.Definition = (int8_t)defCheck; terrainEntities.Props[wallz]	[wallmaxx]	.Modifier = bReinforced ? 1 : 0; terrainEntities.Props[wallz]		[wallmaxx]	.Level = 1; }
				for(uint32_t wallx=x; wallx<=wallmaxx; ++wallx)	{	if(noise[7] > 0.95 || ::klib::randNoise(9941) > 0.95)	continue;	terrainEntities.Props[z]		[wallx]		.Definition = (int8_t)defCheck; terrainEntities.Props[z]		[wallx]		.Modifier = bReinforced ? 1 : 0; terrainEntities.Props[z]			[wallx]		.Level = 1; }
				for(uint32_t wallx=x; wallx<=wallmaxx; ++wallx)	{	if(noise[8] > 0.95 || ::klib::randNoise(9941) > 0.95)	continue;	terrainEntities.Props[wallmaxz]	[wallx]		.Definition = (int8_t)defCheck; terrainEntities.Props[wallmaxz]	[wallx]		.Modifier = bReinforced ? 1 : 0; terrainEntities.Props[wallmaxz]	[wallx]		.Level = 1; }
			}
		}
		else if(terrainTopology[z][x].Sharp < PARTIAL_COVER_HEIGHT && terrainTopology[z][x].Smooth < PARTIAL_COVER_HEIGHT
			&& (terrainTopology[z][x].Smooth + terrainTopology[z][x].Sharp) < PARTIAL_COVER_HEIGHT
			&& terrainEntities.Props[z][x].Definition == -1
			&& noise[2] > 0.95
		)
			terrainEntities.Coins[z][x]				= ::rand()%(1+maxCoins);
	}
}

static	int64_t										getEnemyCoinsForTerrainFun							(::klib::SGame& instanceGame)																	{
	int64_t													maxCoins											= 0;
	::klib::STacticalSetup									& tacticalSetup										= instanceGame.TacticalInfo.Setup;
	int32_t													totalAgents											= 0;
	for(uint32_t iTacticalPlayer=0, playerCount = tacticalSetup.TotalPlayers; iTacticalPlayer < playerCount; ++iTacticalPlayer) {
		if(tacticalSetup.Players[iTacticalPlayer] == -1)
			continue;

		::klib::SGamePlayer										& enemy												= instanceGame.Players[tacticalSetup.Players[iTacticalPlayer]];
		if( enemy.Tactical.Control.Type == ::klib::PLAYER_CONTROL_AI && enemy.Tactical.Control.AIMode != ::klib::PLAYER_AI_TEAMERS )
			continue;

		for(uint32_t iAgent=0, count=tacticalSetup.SquadSize[iTacticalPlayer]; iAgent<count; ++iAgent) {
			if(enemy.Tactical.Squad.Agents[iAgent] != -1) {
				::klib::CCharacter										& characterAgent									= * enemy.Tactical.Army[enemy.Tactical.Squad.Agents[iAgent]];
				if(characterAgent.IsAlive()) {
					maxCoins											+= characterAgent.FinalPoints.CostMaintenance;
					++totalAgents;
				}
			}
		}
	}

	return totalAgents ? maxCoins : 1;	// totalAgents / 4
}

void									recalculateAgentsInRangeAndSight					(::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players);

::gpk::error_t							klib::initTacticalMap								(::klib::SGame& instanceGame)																	{
	STacticalInfo											& tacticalInfo										= instanceGame.TacticalInfo;
	tacticalInfo.Board.Clear();
	tacticalInfo.Board.Resize({::klib::GAME_MAP_WIDTH, ::klib::GAME_MAP_DEPTH});

	::gpk::view_grid<::klib::STopologyDetail>				terrainTopology										= tacticalInfo.Board.Tiles.Terrain.Topology;
	SEntityTiles											& terrainEntities									= tacticalInfo.Board.Tiles.Entities;
	int64_t													seed												= tacticalInfo.Setup.Seed;

	int64_t													maxCoins											= ::getEnemyCoinsForTerrainFun(instanceGame);

	::generateTopology(terrainTopology, seed);
	::populateProps(instanceGame.EntityTables.StageProp, terrainTopology, terrainEntities, seed, maxCoins);

	// We need to deploy the agents after we generated the map so all the player initialization is done before calling this function
	for(uint32_t iTacticalPlayer = 0, tacticalPlayerCount = tacticalInfo.Setup.TotalPlayers; iTacticalPlayer<tacticalPlayerCount; ++iTacticalPlayer) {
		if(tacticalInfo.Setup.Players[iTacticalPlayer] != -1)
			::deployCampaignAgents(instanceGame.Players[tacticalInfo.Setup.Players[iTacticalPlayer]].Tactical, (int8_t)iTacticalPlayer, tacticalInfo.Setup, terrainTopology, terrainEntities);

	}
	::recalculateAgentsInRangeAndSight(tacticalInfo, instanceGame.Players);
	return 0;
}


static	void										initTacticalPlayer									(::klib::SGame& instanceGame, int32_t playerSlot, const ::klib::STacticalSetup& tacticalSetup)			{
	::klib::SGamePlayer										& player											= instanceGame.Players[tacticalSetup.Players[playerSlot]];
	player.Tactical.Squad.Size							= tacticalSetup.SquadSize	[playerSlot];
	player.Tactical.Control								= tacticalSetup.Controls	[playerSlot];
	player.Tactical.Selection							= {0, 0, -1, -1, -1};
	player.Tactical.Squad.LockedAgent					= -1;

	for(uint32_t iAgent = 0, count = tacticalSetup.SquadSize[playerSlot]; iAgent<count; ++iAgent) {	// Resize non-human armies.
		if(player.Tactical.Squad.Agents[iAgent] == -1)
			continue;

		::klib::CCharacter										& playerAgent										= *player.Tactical.Army[player.Tactical.Squad.Agents[iAgent]];
		playerAgent.ActiveBonus								= ::klib::SCharacterTurnBonus();
		playerAgent.Recalculate(instanceGame.EntityTables);
		const ::klib::SEntityPoints								& agentPoints										= playerAgent.FinalPoints;
		playerAgent.Points.LifeCurrent						= agentPoints.LifeMax;

		player.Tactical.Squad.ActionsLeft		[iAgent].Moves		= (int8_t)agentPoints.Fitness.Movement;
		player.Tactical.Squad.ActionsLeft		[iAgent].Actions	= 1;
		player.Tactical.Squad.TargetPositions	[iAgent]			= playerAgent.Position;
		player.Tactical.Squad.TargetAgents		[iAgent]			= {(::klib::TEAM_TYPE)-1, -1, -1, -1};
	}
}

template<size_t _Size>
uint32_t											getRelevantTeams									(::klib::SGame& instanceGame, const ::klib::STacticalInfo& tacticalInfo, ::klib::TEAM_TYPE (&teams)[_Size] )	{
	uint32_t												teamCount											= 0;

	for(size_t iPlayer = 0; iPlayer < _Size; ++iPlayer) {
		if(!::klib::isRelevantTeam(tacticalInfo.Setup.TeamPerPlayer[iPlayer]))
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

static	bool										initFromTacticalSetup								(::klib::SGame& instanceGame, const ::klib::STacticalSetup& tacticalSetup)		{
	//STacticalInfo											& tacticalInfo										= instanceGame.TacticalInfo;
	uint32_t												effectivePlayers									= 0;
	for(uint32_t iTacticalPlayer = 0, playerCount = tacticalSetup.TotalPlayers; iTacticalPlayer < playerCount; ++iTacticalPlayer)
		::initTacticalPlayer(instanceGame, effectivePlayers++, tacticalSetup);

	return true;
}

static const ::klib::STacticalSetup					tacticalSetupForCampaign							=
	{	15731U										//, uint64_t		Seed					= 15731;
	,	4U											//,	uint32_t		TotalPlayers			= 0;
	,	2U											//	uint32_t		TotalTeams				= 0;
	,	{ ::klib::PLAYER_INDEX_USER
		, (::klib::PLAYER_INDEX)1
		, (::klib::PLAYER_INDEX)2
		, (::klib::PLAYER_INDEX)3
		, (::klib::PLAYER_INDEX)4
		, (::klib::PLAYER_INDEX)5
		, (::klib::PLAYER_INDEX)6
		, (::klib::PLAYER_INDEX)7
		, (::klib::PLAYER_INDEX)8
		, (::klib::PLAYER_INDEX)9
		}											//PLAYER_INDEX	Players						[MAX_TACTICAL_PLAYERS]	= {};
	,	{ ::klib::SPlayerControl{::klib::PLAYER_CONTROL_LOCAL, ::klib::PLAYER_AI_TEAMERS}
		, ::klib::SPlayerControl{::klib::PLAYER_CONTROL_AI, ::klib::PLAYER_AI_TEAMERS}
		, ::klib::SPlayerControl{::klib::PLAYER_CONTROL_AI, ::klib::PLAYER_AI_TEAMERS}
		, ::klib::SPlayerControl{::klib::PLAYER_CONTROL_AI, ::klib::PLAYER_AI_TEAMERS}
		, ::klib::SPlayerControl{::klib::PLAYER_CONTROL_AI, ::klib::PLAYER_AI_NEUTRAL}
		, ::klib::SPlayerControl{::klib::PLAYER_CONTROL_AI, ::klib::PLAYER_AI_CURIOUS}
		, ::klib::SPlayerControl{::klib::PLAYER_CONTROL_AI, ::klib::PLAYER_AI_FEARFUL}
		, ::klib::SPlayerControl{::klib::PLAYER_CONTROL_AI, ::klib::PLAYER_AI_ASSISTS}
		, ::klib::SPlayerControl{::klib::PLAYER_CONTROL_AI, ::klib::PLAYER_AI_RIOTERS}
		, ::klib::SPlayerControl{::klib::PLAYER_CONTROL_AI, ::klib::PLAYER_AI_VIOLENT}
		}											//SPlayerControl	Controls				[MAX_TACTICAL_PLAYERS]	= {};
	,	{ ::klib::TEAM_TYPE_ALLY
		, ::klib::TEAM_TYPE_ALLY
		, ::klib::TEAM_TYPE_ENEMY
		, ::klib::TEAM_TYPE_ENEMY
		, ::klib::TEAM_TYPE_CIVILIAN
		, ::klib::TEAM_TYPE_CIVILIAN
		, ::klib::TEAM_TYPE_CIVILIAN
		, ::klib::TEAM_TYPE_CIVILIAN
		, ::klib::TEAM_TYPE_CIVILIAN
		, ::klib::TEAM_TYPE_CIVILIAN
		}											//TEAM_TYPE			Teams					[MAX_TACTICAL_PLAYERS]	= {};
	,	{2,2,5}										//int8_t			PlayerCountPerTeam		[MAX_TACTICAL_PLAYERS]	= {};
	,	{::klib::DEFAULT_SQUAD_SIZE,::klib::DEFAULT_SQUAD_SIZE,::klib::DEFAULT_SQUAD_SIZE,::klib::DEFAULT_SQUAD_SIZE, 2,2,2,2, 1,1}						//uint8_t			SquadSize				[MAX_TACTICAL_PLAYERS]	= {};
	,	{{0,1}, {2,3}, {4,5,6,7,8,9}}				//int8_t			PlayersPerTeam			[MAX_TACTICAL_PLAYERS][MAX_TACTICAL_PLAYERS]	= {};
	};

void												getDefaultTacticalSetupForCampaign					(::klib::STacticalSetup& tacticalSetup)							{ tacticalSetup = ::tacticalSetupForCampaign; }

uint32_t											resolveNextPlayer									(const ::klib::SEntityTables & entityTables, ::klib::STacticalInfo & tacticalInfo, ::gpk::view_array<::klib::SGamePlayer> players, ::klib::SGameMessages & messages);
bool												initCampaignPlayers									(::klib::SGame & instanceGame)									{
	::klib::STacticalInfo									& tacticalInfo										= instanceGame.TacticalInfo;
	::klib::STacticalSetup									& tacticalSetup										= tacticalInfo.Setup;
	// Team players
	//instanceGame.Players[tacticalSetup.Players[0]].Name = "ALLY_0";
	instanceGame.Players[tacticalSetup.Players[1]].Tactical.Name	= ::gpk::view_const_string{"Ivan"	};
	instanceGame.Players[tacticalSetup.Players[2]].Tactical.Name	= ::gpk::view_const_string{"G0"		};
	instanceGame.Players[tacticalSetup.Players[3]].Tactical.Name	= ::gpk::view_const_string{"G1"		};

	// Civilian players
	for(uint32_t iTacticalPlayer=0, maxPlayers=tacticalSetup.TotalPlayers; iTacticalPlayer<maxPlayers; ++iTacticalPlayer)
		if(tacticalSetup.Controls[iTacticalPlayer].Type == ::klib::PLAYER_CONTROL_AI && ::gpk::bit_false(tacticalSetup.Controls[iTacticalPlayer].AIMode, ::klib::PLAYER_AI_TEAMERS))
			instanceGame.Players[tacticalSetup.Players[iTacticalPlayer]].Tactical.Name = ::gpk::get_value_label(tacticalSetup.Controls[iTacticalPlayer].AIMode);

	// Clear selection and reset player stuff relevant to the tactical mode.
	::klib::STacticalPlayer										& playerUser										= instanceGame.Players[::klib::PLAYER_INDEX_USER].Tactical;
	// Set up AI player's agents using the main human player as reference in order to determine the level and equipment.
	for(uint32_t iPlayer = 0; iPlayer<tacticalSetup.TotalPlayers; ++iPlayer) {
		if(tacticalSetup.Players[iPlayer] == (::klib::PLAYER_INDEX)-1 || tacticalSetup.Players[iPlayer] == ::klib::PLAYER_INDEX_USER)
			continue;

		// Set up AI squads
		if(tacticalSetup.Controls[iPlayer].Type != ::klib::PLAYER_CONTROL_AI)
			continue;

		::klib::STacticalPlayer										& playerAI											= instanceGame.Players[tacticalSetup.Players[iPlayer]].Tactical;
		if(tacticalSetup.Controls[iPlayer].Type == ::klib::PLAYER_CONTROL_AI && tacticalSetup.Players[iPlayer] != ::klib::PLAYER_INDEX_USER)
			playerAI.Army.resize(tacticalSetup.SquadSize[iPlayer]);

		bool													bHeroSet											= true;
		for(uint32_t iSquadAgentSlot=0, squadAgentCount=tacticalSetup.SquadSize[iPlayer]; iSquadAgentSlot<squadAgentCount; ++iSquadAgentSlot) {
			if( playerUser.Squad.Agents[iSquadAgentSlot] == -1 || 0 == playerUser.Army[playerUser.Squad.Agents[iSquadAgentSlot]].get_ref()) {
				playerAI.Squad.Agents[iSquadAgentSlot]				= -1;
				continue;
			}
			else
				playerAI.Squad.Agents[iSquadAgentSlot]				= (int16_t)iSquadAgentSlot;

			//if( 0 == playerAI.Army[playerAI.Squad.Agents[iSquadAgentSlot]] )
			//{
				::gpk::ptr_obj<::klib::CCharacter>										newAgent											;
				newAgent.create(::klib::enemyDefinitions[1+rand()%3]);
				playerAI.Army[playerAI.Squad.Agents[iSquadAgentSlot]] = newAgent;
			//}
			::klib::CCharacter										& agentAI											= *playerAI		.Army[playerAI	.Squad.Agents[iSquadAgentSlot]];
			const ::klib::CCharacter								& agentUser											= *playerUser	.Army[playerUser.Squad.Agents[iSquadAgentSlot]];
			if(playerUser.Score.BattlesWon <= 0) {
				if(tacticalSetup.TeamPerPlayer[iPlayer] != ::klib::TEAM_TYPE_ALLY)
					agentAI												= ::klib::enemyDefinitions[2+(rand()&1)];
				if(bHeroSet) {
					::klib::setupAgent(instanceGame.EntityTables, agentAI, agentAI);
					agentAI.Flags.Tech.Gender							= ::klib::GENDER_FEMALE;
				}
				else {
					if(tacticalSetup.TeamPerPlayer[iPlayer] != ::klib::TEAM_TYPE_ALLY)
						::klib::setupAgent(instanceGame.EntityTables, agentAI, agentAI);
					else {
						bHeroSet											= true;
						agentAI												= ::klib::enemyDefinitions[::gpk::size(::klib::enemyDefinitions)-1];
						::klib::setupAgent(instanceGame.EntityTables, agentAI, agentAI);
						agentAI.CurrentEquip.Weapon		.Definition			= (int16_t)instanceGame.EntityTables.Weapon		.Definitions.size()-1;
						agentAI.CurrentEquip.Armor		.Definition			= (int16_t)instanceGame.EntityTables.Armor		.Definitions.size()-1;
						agentAI.CurrentEquip.Profession	.Definition			= (int16_t)instanceGame.EntityTables.Profession	.Definitions.size()-1;
						agentAI.CurrentEquip.Accessory	.Definition			= (int16_t)instanceGame.EntityTables.Accessory	.Definitions.size()-1;
						agentAI.CurrentEquip.Weapon		.Modifier			= (int16_t)instanceGame.EntityTables.Weapon		.Modifiers	.size()-1;
						agentAI.CurrentEquip.Armor		.Modifier			= (int16_t)instanceGame.EntityTables.Armor		.Modifiers	.size()-1;
						agentAI.CurrentEquip.Profession	.Modifier			= (int16_t)instanceGame.EntityTables.Profession	.Modifiers	.size()-1;
						agentAI.CurrentEquip.Accessory	.Modifier			= (int16_t)instanceGame.EntityTables.Accessory	.Modifiers	.size()-1;
						agentAI.CurrentEquip.Weapon		.Level				=
						agentAI.CurrentEquip.Armor		.Level				=
						agentAI.CurrentEquip.Profession	.Level				=
						agentAI.CurrentEquip.Accessory	.Level				= 15;
						agentAI.Recalculate(instanceGame.EntityTables);
						const ::klib::SEntityPoints								& finalAgentPoints									= agentAI.FinalPoints;
						agentAI.Points.LifeCurrent							= finalAgentPoints.LifeMax;
						agentAI.Points.Coins								= agentAI.Points.CostMaintenance;
						agentAI.Flags.Tech.Gender							= ::klib::GENDER_MALE;
					}
				}
			}
			else {
				::klib::setupAgent(instanceGame.EntityTables, agentUser, agentAI);
			}
		}
	}
	::initFromTacticalSetup(instanceGame, tacticalSetup);
	return false;
}

bool												initCampaignGame									(::klib::SGame& instanceGame)											{
	::klib::STacticalInfo									& tacticalInfo										= instanceGame.TacticalInfo;
	tacticalInfo.Clear();

	::klib::STacticalSetup									& tacticalSetup										= tacticalInfo.Setup;
	::getDefaultTacticalSetupForCampaign(tacticalSetup);
	tacticalSetup.Seed									= instanceGame.Seed + instanceGame.Players[::klib::PLAYER_INDEX_USER].Tactical.Score.BattlesWon;
	::initCampaignPlayers(instanceGame);
	::klib::initTacticalMap(instanceGame);
	::klib::drawTacticalBoard(instanceGame, tacticalInfo, instanceGame.TacticalDisplay.Screen.Color, instanceGame.TacticalDisplay.Screen.DepthStencil, ::klib::PLAYER_INDEX_USER, ::klib::TEAM_TYPE_CIVILIAN, instanceGame.Players[::klib::PLAYER_INDEX_USER].Tactical.Selection, true);

	::gpk::bit_set(instanceGame.Flags, ::klib::GAME_FLAGS_TACTICAL);
	tacticalInfo.CurrentPlayer							= (int8_t)::resolveNextPlayer(instanceGame.EntityTables, tacticalInfo, instanceGame.Players, instanceGame.Messages);
	return true;
}
