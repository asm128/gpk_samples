#include "gpk_grid.h"

#include "klib_text.h"

#ifndef KLIB_GRID_H_23627
#define KLIB_GRID_H_23627
namespace klib
{
	template<typename _tCell>
	inline	void		clearGrid			(::gpk::grid<_tCell> grid, const _tCell& clearValue = ' ')		{ ::gpk::drawRectangle(grid, clearValue, ::gpk::rect2<uint32_t>{{}, grid.metrics()}); }

	template<typename _tCell>
	inline	int32_t		valueToGrid			( ::gpk::grid<_tCell> grid, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const _tCell* values, int32_t valueCount, int32_t repeat = 0 )	{
		return ::klib::valueToRect(grid.begin(), grid.metrics().x, grid.metrics().y, offsetLine, offsetColumn, align, values, valueCount, repeat);
	}

	stainli int32_t		lineToGrid			( ::gpk::grid<char> grid, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF )				{
		return ::klib::lineToRect(grid.begin(), grid.metrics().x, grid.metrics().y, offsetLine, offsetColumn, align, text, charCount);
	}

	stainli int32_t		lineToGridColored	( ::gpk::grid<char> display, ::gpk::grid<uint16_t> textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF ) {
		int32_t					actualX				= ::klib::lineToRect(display.begin(), display.metrics().x, display.metrics().y, offsetLine, offsetColumn, align, text, charCount);
		::klib::valueToGrid(textAttributes, offsetLine, actualX, ::klib::SCREEN_LEFT, &messageColor, 1, (int32_t)::gpk::min((uint32_t)::strlen(text), charCount));
		return actualX;
	}

	template<typename _tCell, typename... _Args>
	int32_t				printfToGrid		( ::gpk::grid<_tCell> display, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* format, _Args&&... args )						{
		int32_t					actualX				= ::klib::printfToRect(display.begin(), display.metrics().x, display.metrics().y, offsetLine, offsetColumn, align, format, args...);
		return actualX;
	}

	template<typename _tCell, typename... _Args>
	int32_t				printfToGridColored	( ::gpk::grid<_tCell> display, ::gpk::grid<uint16_t> textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* format, _Args&&... args ) {
		char					precookStr[1024]	= {};
		//int32_t					precookLen			=
			::sprintf_s(precookStr, format, args...);
		int32_t					actualX				= ::klib::lineToGridColored(display, textAttributes, messageColor, offsetLine, offsetColumn, align, precookStr);
		return actualX;
	}

} // namespace

#endif
