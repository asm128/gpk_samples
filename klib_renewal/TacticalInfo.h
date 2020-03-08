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
	struct STacticalCoord {
		::gpk::SCoord3<int32_t>									Cell											;	// Position in tile map.
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

	struct SBulletPoints {
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

	struct SAOE {
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


	struct SBullet {
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
	struct SMapShots {
		::gpk::array_pod<::klib::SBullet>			Bullet											= {};
		::gpk::array_pod<::gpk::SCoord3<int32_t>>	Coords											= {};
	};

	typedef SEntityContainer<::gpk::SCoord3<int32_t>> SCoordContainerEntity;
	struct SMapInventory {
		SEntityContainer<SProfession>				Profession										= {};
		SEntityContainer<SAccessory	>				Accessory										= {};
		SEntityContainer<SArmor		>				Armor											= {};
		SEntityContainer<SWeapon	>				Weapon											= {};
		SEntityContainer<SVehicle	>				Vehicle											= {};
		SEntityContainer<SFacility	>				Facility										= {};
		SEntityContainer<SStageProp	>				StageProp										= {};
		SEntityContainer<SItem		>				Items											= {};

		SEntityContainer<::gpk::SCoord3<int32_t>>	CoordsProfession								= {};
		SEntityContainer<::gpk::SCoord3<int32_t>>	CoordsAccessory									= {};
		SEntityContainer<::gpk::SCoord3<int32_t>>	CoordsArmor										= {};
		SEntityContainer<::gpk::SCoord3<int32_t>>	CoordsWeapon									= {};
		SEntityContainer<::gpk::SCoord3<int32_t>>	CoordsVehicle									= {};
		SEntityContainer<::gpk::SCoord3<int32_t>>	CoordsFacility									= {};
		SEntityContainer<::gpk::SCoord3<int32_t>>	CoordsStageProp									= {};
		SEntityContainer<::gpk::SCoord3<int32_t>>	CoordsItems										= {};
	};


	// We need a way to identify the local player type in the player array. Other types don't really matter because they are used depending on the user's choice.
	GDEFINE_ENUM_TYPE(PLAYER_INDEX, int8_t);
	GDEFINE_ENUM_VALUE(PLAYER_INDEX, USER, 0);

	struct STacticalSetup {
		uint64_t													Seed				;//= 15731;
		uint32_t													TotalPlayers		;//= 0;
		uint32_t													TotalTeams			;//= 0;
		::gpk::array_static<PLAYER_INDEX	, MAX_TACTICAL_PLAYERS>	Players				;//= {};
		::gpk::array_static<SPlayerControl	, MAX_TACTICAL_PLAYERS>	Controls			;//= {};
		::gpk::array_static<TEAM_TYPE		, MAX_TACTICAL_PLAYERS>	TeamPerPlayer		;//= {};
		int8_t														PlayersPerTeam		[MAX_TACTICAL_PLAYERS][MAX_TACTICAL_PLAYERS]	;//= {};
		::gpk::array_static<int8_t			, MAX_TACTICAL_PLAYERS>	PlayerCountPerTeam	;//= {};
		::gpk::array_static<uint8_t			, MAX_TACTICAL_PLAYERS>	SquadSize			;//= {};

		void													Clear											()															{
			Seed													= 15731;
			TotalPlayers											= 0;
			TotalTeams												= 0;
			memset(PlayersPerTeam, -1, sizeof(int8_t) * MAX_TACTICAL_PLAYERS * MAX_TACTICAL_PLAYERS);

			memset(Players				.begin(), -1, sizeof(PLAYER_INDEX	) * Players				.size());
			memset(Controls				.begin(),  0, sizeof(SPlayerControl	) * Controls			.size());
			memset(TeamPerPlayer		.begin(), -1, sizeof(TEAM_TYPE		) * TeamPerPlayer		.size());
			memset(PlayerCountPerTeam	.begin(),  0, sizeof(int8_t			) * PlayerCountPerTeam	.size());
			memset(SquadSize			.begin(),  0, sizeof(uint8_t		) * SquadSize			.size());
		};
	};

#define MAX_AOE_SLOTS		64
#define MAX_MAP_AOE_COORDS	((MAX_AOE_SLOTS)*100)
	typedef SEntityContainer<::gpk::SCoord3<int32_t>> SCoordContainerAOE;
	struct SMapAOE {
		SEntityContainer<SAOE>									AOE												= {};
		SEntityContainer<::gpk::SCoord3<int32_t>>				Coords											= {};
	};

	struct STacticalBoard {
		SMapShots												Shots											= {};
		SMapAOE													AreaOfEffect									= {};
		SGameTiles												Tiles											= {};

		inline	int32_t											Resize											(::gpk::SCoord2<uint32_t> newSize)	{ return Tiles.Resize(newSize); }
		void													Clear											()									{
			Tiles.Clear();
			Shots													= {};
			AreaOfEffect											= {};
		};
	};

	static constexpr	const uint32_t								GAME_MAP_DEPTH									= 36				;
	static constexpr	const uint32_t								GAME_MAP_WIDTH									= (GAME_MAP_DEPTH * 2);

	// This structure holds all the data that is only valid once we enter the tactical mode and that is of no use outside of it.
	struct STacticalInfo {
		STacticalSetup											Setup											= STacticalSetup();
		int8_t													CurrentPlayer									= -1;	// this is an index to the local "Players" member (not SGame::Players!)
		int8_t													CurrentTeam										= -1;	// this is an index to the local "Players" member (not SGame::Players!)
		int8_t													CurrentPlayerPerTeam	[MAX_TACTICAL_PLAYERS]	= {};
		SAgentsReference										AgentsInTeamSight		[MAX_TACTICAL_PLAYERS]	= {};
		STacticalBoard											Board											= {};
		SMapInventory											Drops											= {};

		inline	int32_t											ResizeBoard										(::gpk::SCoord2<uint32_t> newSize)							{ Board.Resize(newSize); }
		bool													AddBullet										(const SBullet& newBullet)									{
			Board.Shots.Bullet.push_back(newBullet);
			Board.Shots.Coords.push_back(newBullet.Position.Cell);
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
			return ( (Board.Tiles.Entities.Coins[coord.z][coord.x] != 0)
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
