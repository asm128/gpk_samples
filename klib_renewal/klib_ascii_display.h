#include "klib_ascii_target.h"

#ifndef KLIB_ASCII_CONSOLE_H_92384709283749082
#define KLIB_ASCII_CONSOLE_H_92384709283749082

namespace klib
{
	struct SASCIIDisplayInfo {
						::gpk::SCoord2<uint32_t>						DisplaySizeMax		;			// In character units.
						::gpk::SCoord2<uint32_t>						DisplaySizeCurrent	;			// In character units.
						uint32_t										ColorWidth			;			// The color width in bytes.
						uint32_t										PaletteSize			;			// The amount of colors that can be stored in the palette. At the time of writing this comment, a maximum of 64k colors is supported.
						bool											PaletteSet			: 1;		
						bool											PaletteReset		: 1;		
						bool											TitleSet			: 1;
						bool											Resize				: 1;		
	};
//
//					::gpk::error_t									asciiDisplayPresentBGRA				(const ::gpk::view_array<const uint8_t>& characters, const ::gpk::view_array<const uint32_t>& colors);
					::gpk::error_t									asciiDisplayInfo					(SASCIIDisplayInfo& displayInfo);
					::gpk::error_t									asciiDisplayCreate					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::gpk::error_t									asciiDisplayDestroy					();
					::gpk::error_t									asciiDisplayPresent					(const ::gpk::view_array<const uint8_t>& characters, const ::gpk::view_array<const uint16_t>& colorRefs);
					::gpk::error_t									asciiDisplayClear					(uint8_t character = ' ', uint16_t colorRef = ::klib::ASCII_COLOR_INDEX_WHITE);
					::gpk::error_t									asciiDisplaySize					(::gpk::SCoord2<uint32_t>& size);

					::gpk::error_t									asciiDisplayResize					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::gpk::error_t									asciiDisplayPaletteSet				(const ::gpk::view_array<::gpk::SColorRGBA>& palette);
					::gpk::error_t									asciiDisplayPaletteReset			();
					::gpk::error_t									asciiDisplayTitleSet				(const ::gpk::view_array<const char>& title);

	template <typename _tUnit>
	static inline	::gpk::error_t									asciiDisplayCreate					(const ::gpk::SCoord2<_tUnit>& displaySize)				{ return ::klib::asciiDisplayCreate(displaySize.x, displaySize.y); }
	static inline	::gpk::error_t									asciiDisplayPresent					(const ::klib::SASCIITarget& renderTarget)					{
		return ::klib::asciiDisplayPresent
			( ::gpk::view_array<const uint8_t >{renderTarget.Characters	.begin(), renderTarget.Characters	.size()}
			, ::gpk::view_array<const uint16_t>{renderTarget.Colors		.begin(), renderTarget.Colors		.size()}
			);
	}
}

#endif // KLIB_ASCII_CONSOLE_H_92384709283749082
