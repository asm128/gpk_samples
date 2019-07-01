#include "StageProp.h"

#include "klib_grid.h"
#include "klib_ascii_reference.h"

#ifndef __TACTICALTILE_H__9238402734628937234__
#define __TACTICALTILE_H__9238402734628937234__

namespace klib
{
#pragma pack(push, 1)
	//struct STileData
	//{
	//	SEntityFlags	Flags;
	//};

	struct STileCounter {
				int8_t												Count[32]		= {};
		
		inline	void												Clear			()																					{ memset(Count, 0, sizeof(int8_t)*32); }
	};

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

	//struct STileStatus	{
	//	int16_t		Definition	;
	//	int16_t		Modifier	;
	//	int16_t		Level		;
	//};

#pragma pack(pop)

	struct STileGeometry {
				float												fHeight[4]		;		// west->east, north->south ordering
				int16_t												Flags			;		// GND v <= 1.5 // maybe a color key? a terrain property? We're going to use it to tell if the triangle is inverted.
	};

	template <size_t _Width, size_t _Depth> 
	struct STerrainTiles {
				::klib::SGrid<STileGeometry		, _Width, _Depth>	Geometry;
				::klib::SGrid<STopologyDetail	, _Width, _Depth>	Topology;

		inline	void												Clear			()											{
					clearGrid(Geometry, {{0, 0, 0, 0}, 0});
					clearGrid(Topology, {0, 0, 0});
				}
	};

	template <size_t _Width, size_t _Depth> 
	struct SEntityTiles
	{
				::klib::SGrid<STileCharacter	, _Width, _Depth>	Agents	;	
				::klib::SGrid<STileProp			, _Width, _Depth>	Props	;	
				::klib::SGrid<int32_t			, _Width, _Depth>	Coins	;	

		inline	void												Clear			()											{
			clearGrid(Agents, {TEAM_TYPE_INVALID, -1, -1, -1} );
			clearGrid(Props	, {-1, -1, -1, -1} );
			clearGrid(Coins	, 0 );
		}
	};

	template <size_t _Width, size_t _Depth> 
	struct SGameTiles {
				STerrainTiles	<_Width, _Depth>					Terrain;
				SEntityTiles	<_Width, _Depth>					Entities;

#define TILE_SCALE 16
#define FULL_COVER_HEIGHT		(TILE_SCALE-(TILE_SCALE>>2))
#define PARTIAL_COVER_HEIGHT	(TILE_SCALE>>1)
				bool												IsTileAvailable	(int32_t x, int32_t z)	const				{
			return	Entities.Agents		[z][x].AgentIndex	== -1 
				&&	Entities.Props		.Cells[z][x].Level	== -1
				&&	Terrain.Topology	[z][x].Sharp									< PARTIAL_COVER_HEIGHT
				&&	Terrain.Topology	[z][x].Smooth									< PARTIAL_COVER_HEIGHT
				&&	(Terrain.Topology	[z][x].Smooth+Terrain.Topology	[z][x].Sharp)	< PARTIAL_COVER_HEIGHT
				;
		}

		inline	void												Clear			()											{
			Entities.Clear();
			Terrain	.Clear();
		}

		static	const uint32_t										Width			= (uint32_t)_Width;
		static	const uint32_t										Depth			= (uint32_t)_Depth;
	};

			char												getASCIIWall	(const ::gpk::view_grid<const STileProp>& propGrid, int32_t x, int32_t z);
};


#endif // __TACTICALTILE_H__9238402734628937234__
