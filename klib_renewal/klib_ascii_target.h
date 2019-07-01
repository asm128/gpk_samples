/// Copyright 2017 - asm128
#include "gpk_view_grid.h"
#include "gpk_color.h"
#include "gpk_coord.h"
#include "klib_ascii_color.h"

#ifndef KLIB_ASCII_TARGET_H_9823740982374
#define KLIB_ASCII_TARGET_H_9823740982374

namespace klib 
{
#pragma pack(push, 1)
	typedef					::gpk::view_array<::gpk::SColorRGBA>			PaletteRGBA;

	struct SASCIITarget {
								::gpk::view_grid<uint8_t>						Characters									= {};
								::gpk::view_grid<uint16_t>						Colors										= {};

		inline constexpr		uint32_t										Width										()															const	noexcept	{ return Characters.metrics().x; }
		inline constexpr		uint32_t										Height										()															const	noexcept	{ return Characters.metrics().y; }
	};
#pragma pack(pop)
							::gpk::error_t									asciiTargetCreate							(::klib::SASCIITarget& console, uint32_t width, uint32_t height);
							::gpk::error_t									asciiTargetDestroy							(::klib::SASCIITarget& console);
							::gpk::error_t									asciiTargetClear							(::klib::SASCIITarget& console, uint8_t character = ' ', uint16_t color = ::klib::ASCII_COLOR_INDEX_WHITE);	
	template<typename _tUnit>
	static inline			::gpk::error_t									asciiTargetCreate							(::klib::SASCIITarget& console, const ::gpk::SCoord2<_tUnit>& size)					{ return ::klib::asciiTargetCreate(console, (uint32_t)size.x, (uint32_t)size.y); }
} // namespace 

#endif // KLIB_ASCII_TARGET_H_9823740982374
