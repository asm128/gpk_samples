#include "gpk_color.h"

/// Copyright 2016-2017 - asm128
#ifndef KLIB_ASCII_COLOR_H_1209387102983710923709128370192873
#define KLIB_ASCII_COLOR_H_1209387102983710923709128370192873

namespace klib
{
	struct STextColorASCII {
							uint8_t													Foreground			: 4;
							uint8_t													Background			: 4;

		//inline				operator												uint8_t				()	const	noexcept	{ return *(const uint8_t*)this;				}
		inline	constexpr	operator												uint8_t				()	const	noexcept	{ return Foreground | (Background << 4);	}
	};

	enum ASCII_COLOR : uint32_t
		{ ASCII_COLOR_0					= 0x000000
		, ASCII_COLOR_1 				= 0x800000
		, ASCII_COLOR_2 				= 0x008000
		, ASCII_COLOR_3 				= 0x808000
		, ASCII_COLOR_4 				= 0x000080
		, ASCII_COLOR_5 				= 0x800080
		, ASCII_COLOR_6 				= 0x008080
		, ASCII_COLOR_7 				= 0x00A5FF	// 0xC0C0C0	// LIGHTGREY or ORANGE		   0xC0C0C0
		, ASCII_COLOR_8 				= 0x808080
		, ASCII_COLOR_9 				= 0xFF0000
		, ASCII_COLOR_10				= 0x00FF00
		, ASCII_COLOR_11				= 0xFFFF00
		, ASCII_COLOR_12				= 0x0000FF
		, ASCII_COLOR_13				= 0xFF00FF
		, ASCII_COLOR_14				= 0x00FFFF
		, ASCII_COLOR_15				= 0xFFFFFF
		};

	enum ASCII_COLOR_INDEX : uint16_t
		{ ASCII_COLOR_INDEX_BLACK		= 0
		, ASCII_COLOR_INDEX_DARKBLUE	= 1
		, ASCII_COLOR_INDEX_DARKGREEN	= 2
		, ASCII_COLOR_INDEX_DARKCYAN	= 3
		, ASCII_COLOR_INDEX_DARKRED		= 4
		, ASCII_COLOR_INDEX_DARKMAGENTA	= 5
		, ASCII_COLOR_INDEX_DARKYELLOW	= 6
		, ASCII_COLOR_INDEX_LIGHTGREY	= 7
		, ASCII_COLOR_INDEX_DARKGREY	= 8
		, ASCII_COLOR_INDEX_BLUE		= 9
		, ASCII_COLOR_INDEX_GREEN		= 10
		, ASCII_COLOR_INDEX_CYAN		= 11
		, ASCII_COLOR_INDEX_RED			= 12
		, ASCII_COLOR_INDEX_MAGENTA		= 13
		, ASCII_COLOR_INDEX_YELLOW		= 14
		, ASCII_COLOR_INDEX_WHITE		= 15
		};

	stacxpr	const uint32_t			ASCII_COLOR_INDEX_ORANGE		= ::klib::ASCII_COLOR_INDEX_LIGHTGREY;

} // namespace

// BLINK	128

#define ascii_color_invert(_col) (((_col & 0xF0)>>4) | ((_col & 0xF)<<4))

#endif // KLIB_ASCII_COLOR_H_1209387102983710923709128370192873
