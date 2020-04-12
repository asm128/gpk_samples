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
		::klib::STileASCIIGender			Gender						;
		::klib::STileASCIIWalls				WallsThin					;
		::klib::STileASCIIWalls				WallsThick					;
		::klib::STileASCIITopology			Topology					;
	};

	uint16_t							getPlayerColor				( const ::klib::STacticalInfo& tacticalInfo, const ::klib::STacticalPlayer& boardPlayer, int8_t indexBoardPlayer, int8_t indexPlayerViewer, bool bIsSelected );

	struct SStatusColor {
		int8_t								Bright	;//: 4;
		int8_t								Dark	;//: 4;

											SStatusColor
			(	int8_t			bright	= ::klib::ASCII_COLOR_INDEX_WHITE
			,	int8_t			dark	= ::klib::ASCII_COLOR_INDEX_BLACK
			)
			: Bright	(bright	)
			, Dark		(dark	)
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
	int32_t									initStatusColors		(::klib::SStatusColor (&statusColors)[_StatusCount])																																				{
		int32_t										nBitIndex;
		// Stun
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_FROZEN		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_DARKGREY		, ::klib::ASCII_COLOR_INDEX_WHITE		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_PANIC		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_DARKGREY		, ::klib::ASCII_COLOR_INDEX_ORANGE		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_SLEEP		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_DARKGREY		, ::klib::ASCII_COLOR_INDEX_BLACK		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_STUN		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_DARKGREY		, ::klib::ASCII_COLOR_INDEX_DARKRED		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_SHOCK		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_DARKGREY		, ::klib::ASCII_COLOR_INDEX_CYAN		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_PETRIFY		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_DARKGREY		, ::klib::ASCII_COLOR_INDEX_GREEN		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_CHARMED		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_DARKGREY		, ::klib::ASCII_COLOR_INDEX_MAGENTA		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_BERSERK		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_DARKGREY		, ::klib::ASCII_COLOR_INDEX_YELLOW		};
		// Poison damage
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_BLEEDING	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_GREEN			, ::klib::ASCII_COLOR_INDEX_RED			};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_POISON		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_GREEN			, ::klib::ASCII_COLOR_INDEX_DARKGREEN	};
		// Shieldable damage
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_BURN		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_RED			, ::klib::ASCII_COLOR_INDEX_DARKRED		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_FREEZING	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_RED			, ::klib::ASCII_COLOR_INDEX_WHITE		};
		// Debuff
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_WEAKNESS	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_BLUE			, ::klib::ASCII_COLOR_INDEX_BLACK		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_SLOW		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_BLUE			, ::klib::ASCII_COLOR_INDEX_DARKRED		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_BULLIED		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_BLUE			, ::klib::ASCII_COLOR_INDEX_DARKMAGENTA	};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_DRUNK		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_BLUE			, ::klib::ASCII_COLOR_INDEX_GREEN		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_BLIND		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_BLUE			, ::klib::ASCII_COLOR_INDEX_WHITE		};
		//
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_RAGE		, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_ORANGE			, ::klib::ASCII_COLOR_INDEX_GREEN		};
		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_INVISIBLE	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_ORANGE			, ::klib::ASCII_COLOR_INDEX_WHITE		};

		nBitIndex			= ::klib::getBitIndex(COMBAT_STATUS_BLACKOUT	, MAX_COMBAT_STATUS_COUNT); statusColors[nBitIndex] = {::klib::ASCII_COLOR_INDEX_WHITE			, ::klib::ASCII_COLOR_INDEX_BLACK		};
		return 0;
	}

	uint16_t								getStatusColor			(::klib::COMBAT_STATUS status, bool bSwap, uint16_t defaultColor);
	void									boardToDisplay			(::klib::SGame& instanceGame, const STacticalBoard& board, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int8_t indexBoardPlayer	, TEAM_TYPE teamId, const SPlayerSelection& selection, bool bFogOfWar);
	void									drawTacticalBoard		(::klib::SGame& instanceGame, STacticalInfo& tacticalInfo, ::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int8_t playerIndex		, TEAM_TYPE teamId, const SPlayerSelection& selection, bool bFogOfWar);
}

#endif // __TACTICAL_DRAW_H__2309748209346029374609821764__
