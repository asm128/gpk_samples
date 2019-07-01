#include "Squad.h"
#include "EntityImpl.h"
#include "PlayerControl.h"
#include "TacticalTile.h"

#include "gpk_coord.h"

#ifndef __TACTICALINFO_H__9238640297346098273469823746982__
#define __TACTICALINFO_H__9238640297346098273469823746982__

namespace klib
{
#pragma pack(push, 1)

	// This struct holds both the tile coordinate as integers and the floating point vector indicating the position inside the tile. 
	// This is done this way for simplifying a bunch of operations for which we only need the integer part 
	// while improving the precision of the floating point values by only requiring them to represent a value between 0 and 1 for a very small distance.
	struct STacticalCoord
	{
		::gpk::SCoord3<int32_t>								Cell											;	// Position in tile map.
		::gpk::SCoord3<float>									Offset											;	// Position between 0 and 1 relative to the tile.

		bool													operator==										(const STacticalCoord& other)			const	noexcept	{
			return	(Cell.x		== other.Cell.x	) 
				 && (Cell.y		== other.Cell.y	) 
				 && (Cell.z		== other.Cell.z	) 
				 && (Offset.x	== Offset.x		) 
				 && (Offset.y	== Offset.y		) 
				 && (Offset.z	== Offset.z		);
		}
	};

	struct SBulletPoints
	{
		SEntityGrade											Tech											;
		ATTACK_EFFECT											Effect											;
		COMBAT_STATUS											StatusInflict									;
		int32_t													Damage											;
		SLifePoints												DirectDamage									;
		int32_t													Level											;

		constexpr bool											operator==										(const SBulletPoints& other)			const	noexcept	{
			return Tech					== other.Tech
				&& Effect				== other.Effect 
				&& StatusInflict		== other.StatusInflict 
				&& Damage				== other.Damage 
				&& DirectDamage			== other.DirectDamage
				&& Level				== other.Level
			;
		}
	};
	
	struct SAOE
	{
		STacticalCoord											Position										;	// AOE Center
		double													RadiusOrHalfSize								;	// For both radial AOE and square/cube AOE.
		STileCharacter											Caster											;	// We need to keep track of the one who spanwed this AOE in order to enable a variety of operations such as creating AOE that affect only a given character or team.
		SEntityFlags											Flags											;	// Can be status immunity, status inflict, attack effect, defend effect or passive effect and the allowance of tech, attack types and entity grades.
		COMBAT_STATUS											StatusInflict									;
		uint8_t													Level											;
		uint8_t													TurnsLeft										;

		bool													operator==										(const SAOE& other)						const	noexcept	{
			const bool													bResult											= 
				(Position			== other.Position) 
			 && (RadiusOrHalfSize	== other.RadiusOrHalfSize) 
			 && (Caster				== other.Caster) 
			 && (Flags				== other.Flags) 
			 && (StatusInflict		== other.StatusInflict) 
			 && (Level				== other.Level) 
			 && (TurnsLeft			== other.TurnsLeft) 
			;

			return bResult;
		}
	};


	struct SBullet
	{
		STacticalCoord											Position										;
		::gpk::SCoord3<float>									Direction										;
		STileCharacter											Shooter											;
		SBulletPoints											Points											;

		bool													operator==										(const SBullet& other)					const	noexcept	{
			const bool													bResult											= 
				(Position			== other.Position		) 
			 && (Direction.x		== other.Direction.x	) 
			 && (Direction.y		== other.Direction.y	) 
			 && (Direction.z		== other.Direction.z	) 
			 && (Shooter			== other.Shooter		) 
			 && (Points				== other.Points) 
			;

			return bResult;
		}
	};
#pragma pack(pop)
#define MAX_MAP_SHOTS 64
#define MAX_MAP_BULLET_COORDS ((MAX_MAP_SHOTS)*100) 
	typedef SEntityContainer<::gpk::SCoord3<int32_t>, MAX_MAP_BULLET_COORDS> SCoordContainerBullet;
	struct SMapShots
	{
		SEntityContainer<SBullet, MAX_MAP_SHOTS>				Bullet											= {};
		SCoordContainerBullet									Coords											= {};
	};

	typedef SEntityContainer<::gpk::SCoord3<int32_t>, MAX_INVENTORY_SLOTS> SCoordContainerEntity;
	struct SMapInventory
	{
		SEntityContainer<SProfession	, MAX_INVENTORY_SLOTS>	Profession										= {};
		SEntityContainer<SAccessory		, MAX_INVENTORY_SLOTS>	Accessory										= {};
		SEntityContainer<SArmor			, MAX_INVENTORY_SLOTS>	Armor											= {};
		SEntityContainer<SWeapon		, MAX_INVENTORY_SLOTS>	Weapon											= {};
		SEntityContainer<SVehicle		, MAX_INVENTORY_SLOTS>	Vehicle											= {};
		SEntityContainer<SFacility		, MAX_INVENTORY_SLOTS>	Facility										= {};
		SEntityContainer<SStageProp		, MAX_INVENTORY_SLOTS>	StageProp										= {};
		SEntityContainer<SItem			, MAX_INVENTORY_SLOTS>	Items											= {};

		SCoordContainerEntity									CoordsProfession								= {};
		SCoordContainerEntity									CoordsAccessory									= {};
		SCoordContainerEntity									CoordsArmor										= {};
		SCoordContainerEntity									CoordsWeapon									= {};
		SCoordContainerEntity									CoordsVehicle									= {};
		SCoordContainerEntity									CoordsFacility									= {};
		SCoordContainerEntity									CoordsStageProp									= {};
		SCoordContainerEntity									CoordsItems										= {};
	};


	// We need a way to identify the local player type in the player array. Other types don't really matter because they are used depending on the user's choice.
	GDEFINE_ENUM_TYPE(PLAYER_INDEX, int8_t);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, USER, 0);

	struct STacticalSetup {
		uint64_t												Seed																;//= 15731;
		uint32_t												TotalPlayers														;//= 0;
		uint32_t												TotalTeams															;//= 0;
		PLAYER_INDEX											Players				[MAX_TACTICAL_PLAYERS]							;//= {};
		SPlayerControl											Controls			[MAX_TACTICAL_PLAYERS]							;//= {};
		TEAM_TYPE												TeamPerPlayer		[MAX_TACTICAL_PLAYERS]							;//= {};
		int8_t													PlayersPerTeam		[MAX_TACTICAL_PLAYERS][MAX_TACTICAL_PLAYERS]	;//= {};
		int8_t													PlayerCountPerTeam	[MAX_TACTICAL_PLAYERS]							;//= {};
		uint8_t													SquadSize			[MAX_TACTICAL_PLAYERS]							;//= {};

		void													Clear											()															{
			Seed													= 15731;
			TotalPlayers											= 0;
			TotalTeams												= 0;
	
			memset(Players				, -1, sizeof(PLAYER_INDEX	)*::gpk::size(Players				));
			memset(Controls				,  0, sizeof(SPlayerControl	)*::gpk::size(Controls				));
			memset(TeamPerPlayer		, -1, sizeof(TEAM_TYPE		)*::gpk::size(TeamPerPlayer			));
			memset(PlayersPerTeam		, -1, sizeof(int8_t			)*::gpk::size(PlayersPerTeam		));
			memset(PlayerCountPerTeam	,  0, sizeof(int8_t			)*::gpk::size(PlayerCountPerTeam	));
			memset(SquadSize			,  0, sizeof(uint8_t		)*::gpk::size(SquadSize				));
		};
	};

#define MAX_AOE_SLOTS		64
#define MAX_MAP_AOE_COORDS	((MAX_AOE_SLOTS)*100)
	typedef SEntityContainer<::gpk::SCoord3<int32_t>, MAX_MAP_AOE_COORDS> SCoordContainerAOE;
	struct SMapAOE
	{
		SEntityContainer<SAOE, MAX_AOE_SLOTS>					AOE												= {};
		SCoordContainerAOE										Coords											= {};
	};

	template <size_t _Width, size_t _Depth>
	struct STacticalBoardStatic
	{
		static const uint32_t									Width											= (uint32_t)_Width;
		static const uint32_t									Depth											= (uint32_t)_Depth;

		SMapShots												Shots											= {};
		SMapAOE													AreaOfEffect									= {};
		SGameTiles<_Width, _Depth>								Tiles											= {};

		void													Clear											()															{
			Tiles.Clear();
			Shots													= SMapShots	();
			AreaOfEffect											= SMapAOE	();
		};
	};

#define GAME_MAP_DEPTH	36
#define GAME_MAP_WIDTH	(GAME_MAP_DEPTH*2)
	typedef STacticalBoardStatic<GAME_MAP_WIDTH, GAME_MAP_DEPTH>	STacticalBoard;

	// This structure holds all the data that is only valid once we enter the tactical mode and that is of no use outside of it.
	struct STacticalInfo
	{
		STacticalSetup											Setup											= STacticalSetup();
		int8_t													CurrentPlayer									= -1;	// this is an index to the local "Players" member (not SGame::Players!)
		int8_t													CurrentTeam										= -1;	// this is an index to the local "Players" member (not SGame::Players!)
		int8_t													CurrentPlayerPerTeam	[MAX_TACTICAL_PLAYERS]	= {};
		SAgentsReference										AgentsInTeamSight		[MAX_TACTICAL_PLAYERS]	= {};
		STacticalBoard											Board											= {};
		SMapInventory											Drops											= {};

		bool													AddBullet										(const SBullet& newBullet)									{
			if(Board.Shots.Bullet.AddElement(newBullet))
				Board.Shots.Coords.AddElement(newBullet.Position.Cell);
			else 
				return false;

			return true;
		}

		bool													AddAOE											(const SAOE& newAOE)										{
			if(Board.AreaOfEffect.AOE.AddElement(newAOE))
				Board.AreaOfEffect.Coords.AddElement(newAOE.Position.Cell);
			else 
				return false;

			return true;
		}

		void													Clear											()															{
			Setup.Clear();
			CurrentPlayer											= -1;
			CurrentTeam												= -1;
			Drops													= SMapInventory();
			memset(CurrentPlayerPerTeam	, -1, sizeof(int8_t				)*::gpk::size(CurrentPlayerPerTeam	));
			memset(AgentsInTeamSight	,  0, sizeof(SAgentsReference	)*::gpk::size(AgentsInTeamSight	));
			Board.Clear();
		}

		bool													HasDrops										(const ::gpk::SCoord3<int32_t>& coord)	const	noexcept	{
			return ( (Board.Tiles.Entities.Coins	.Cells[coord.z][coord.x] != 0)				
				|| (-1) != Drops.CoordsProfession	.FindElement(coord)
				|| (-1) != Drops.CoordsWeapon		.FindElement(coord)
				|| (-1) != Drops.CoordsArmor		.FindElement(coord)
				|| (-1) != Drops.CoordsAccessory	.FindElement(coord)
				|| (-1) != Drops.CoordsVehicle		.FindElement(coord)
				|| (-1) != Drops.CoordsFacility		.FindElement(coord)
				|| (-1) != Drops.CoordsStageProp	.FindElement(coord)
				|| (-1) != Drops.CoordsItems		.FindElement(coord)
				);
		}
	};

};

#endif // __TACTICALINFO_H__9238640297346098273469823746982__
