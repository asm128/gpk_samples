#include "StageProp.h"

#include "klib_grid.h"
#include "klib_ascii_reference.h"

#include "gpk_image.h"

#ifndef __TACTICALTILE_H__9238402734628937234__
#define __TACTICALTILE_H__9238402734628937234__

namespace klib
{
#pragma pack(push, 1)
	struct STopologyDetail {
				int8_t												Smooth			;
				int8_t												Sharp			;
				int8_t												Collision		;
	};

	// The difference between SItemTile and SEntity is that SEntity member values must be always valid whereas these can be -1.
	// Setting IndexDefinition to -1 will effectively disable the tile whereas setting only Modifier or Level to -1 must default to 0 when converting to an SEntity.
	struct STileProp {
				int8_t												Definition		;
				int8_t												Modifier		;
				int8_t												Level			;
				int8_t												Owner			;
	};

#pragma pack(pop)

	struct STileGeometry {
				float												fHeight[4]		;		// west->east, north->south ordering
				int16_t												Flags			;		// GND v <= 1.5 // maybe a color key? a terrain property? We're going to use it to tell if the triangle is inverted.
	};

	struct STerrainTiles {
				::gpk::SImage<::klib::STileGeometry		>			Geometry;
				::gpk::SImage<::klib::STopologyDetail	>			Topology;

		inline	int32_t												Resize			(::gpk::SCoord2<uint32_t> newSize)											{
			Geometry.resize(newSize, {});
			return Topology.resize(newSize, {});
		}
		inline	void												Clear			()											{
			::klib::clearGrid(Geometry.View, {});
			::klib::clearGrid(Topology.View, {});
		}
	};

	struct SEntityTiles {
				::gpk::SImage<STileCharacter>						Agents	;
				::gpk::SImage<STileProp		>						Props	;
				::gpk::SImage<int64_t		>						Coins	;

		inline	int32_t												Resize			(::gpk::SCoord2<uint32_t> newSize)											{
			Agents	.resize(newSize, {TEAM_TYPE_INVALID, -1, -1, -1} );
			Props	.resize(newSize, {-1, -1, -1, -1});
			return Coins	.resize(newSize, 0);
		}
		inline	void												Clear			()											{
			::klib::clearGrid(Agents.View, {TEAM_TYPE_INVALID, -1, -1, -1} );
			::klib::clearGrid(Props	.View, {-1, -1, -1, -1} );
			::klib::clearGrid(Coins	.View, 0LL);
		}
	};

	struct SGameTiles {
				::klib::STerrainTiles								Terrain ;
				::klib::SEntityTiles								Entities;

		inline	int32_t												Resize			(::gpk::SCoord2<uint32_t> newSize)											{
			Terrain.Resize(newSize);
			return Entities.Resize(newSize);
		}
#define TILE_SCALE 16
#define FULL_COVER_HEIGHT		(TILE_SCALE-(TILE_SCALE>>2))
#define PARTIAL_COVER_HEIGHT	(TILE_SCALE>>1)
				bool												IsTileAvailable	(int32_t x, int32_t z)	const				{
			return	Entities.Agents		[z][x].AgentIndex	== -1
				&&	Entities.Props		[z][x].Level		== -1
				&&	Terrain.Topology	[z][x].Sharp									< PARTIAL_COVER_HEIGHT
				&&	Terrain.Topology	[z][x].Smooth									< PARTIAL_COVER_HEIGHT
				&&	(Terrain.Topology	[z][x].Smooth + Terrain.Topology[z][x].Sharp)	< PARTIAL_COVER_HEIGHT
				;
		}

		inline	void												Clear			()											{
			Entities.Clear();
			Terrain	.Clear();
		}
	};

			char												getASCIIWall	(const ::gpk::view_array<const ::klib::SEntityRecord<::klib::SStageProp>> definitions, const ::gpk::view_grid<const STileProp>& propGrid, int32_t x, int32_t z);
};


#endif // __TACTICALTILE_H__9238402734628937234__
