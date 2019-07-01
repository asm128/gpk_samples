#include "gpk_view_grid.h"

#include "klib_text.h"

#ifndef KLIB_GRID_H_902837982374
#define KLIB_GRID_H_902837982374
namespace klib
{
	template<typename _tCell, size_t _sizeWidth, size_t _sizeDepth>
	struct SGrid : public ::gpk::view_grid<_tCell> {
		typedef				_tCell										TCell;
		typedef				::gpk::view_grid<_tCell>					TGridView;

		//using				TGridView									::gpk::view_grid;

		static constexpr	const uint32_t								Width									= (uint32_t)_sizeWidth;
		static constexpr	const uint32_t								Depth									= (uint32_t)_sizeDepth;

							TCell										Cells[_sizeDepth][_sizeWidth]			= {};

		inline constexpr												SGrid									()																		: TGridView::view_grid{&Cells[0][0], _sizeWidth, _sizeDepth}	{}
	};

	template<typename _tCell>
	void																fillCellsFromNoise						(::gpk::view_grid<_tCell>& grid, const _tCell& value, int64_t seed, int32_t diceFaces=10)														{
		_tCell																	* cells									= grid.begin();
		for(uint32_t i = 0, count = grid.size();  i < count; ++i) {
			double																	noise									= ::gpk::noiseNormal1D(i + 1, seed);
			int32_t																	dice									= int32_t(noise * diceFaces);
			if(0 == dice)
				cells[i]															= value;
		}
	}

	template<typename _tCell>
	void																clearGrid								(::gpk::view_grid<_tCell>& grid, const _tCell& clearValue = ' ')																												{
		_tCell																	* cells									= grid.begin();
		for(uint32_t i = 0, count = grid.size(); i < count; ++i )
			cells[i]															= clearValue;
	}

	template<typename _tCell>
	void																drawGridBorders							( ::gpk::view_grid<_tCell>& grid, const _tCell& value)																												{
		for(uint32_t z = 0, maxZ = grid.height(); z < maxZ; ++z) {
			grid[z][0]															= value; 
			grid[z][grid.width() - 1]											= value; 
		}
		for(uint32_t x=0, maxX = grid.width() ; x < maxX; ++x) {
			grid[0][x]															= value; 
			grid[grid.height() - 1][x]											= value; 
		}
	}

	template<typename _tCell>
	inline			int32_t							valueToGrid						( ::gpk::view_grid<_tCell>& grid, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const _tCell* values, int32_t valueCount, int32_t repeat = 0 )	{
		return ::klib::valueToRect(grid.begin(), grid.metrics().x, grid.metrics().y, offsetLine, offsetColumn, align, values, valueCount, repeat);
	}

	static inline	int32_t							lineToGrid						( ::gpk::view_grid<char>& grid, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF )				{
		return ::klib::lineToRect(grid.begin(), grid.metrics().x, grid.metrics().y, offsetLine, offsetColumn, align, text, charCount);
	}

	static inline	int32_t							lineToGridColored				( ::gpk::view_grid<char>& display, ::gpk::view_grid<uint16_t>& textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF ) {
		int32_t												actualX							= ::klib::lineToRect(display.begin(), display.metrics().x, display.metrics().y, offsetLine, offsetColumn, align, text, charCount);
		::klib::valueToGrid(textAttributes, offsetLine, actualX, ::klib::SCREEN_LEFT, &messageColor, 1, (int32_t)::gpk::min((uint32_t)::strlen(text), charCount));
		return actualX;
	}

	template<typename _tCell, typename... _Args>
	int32_t											printfToGrid					( ::gpk::view_grid<_tCell>& display, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* format, _Args&&... args )						{
		int32_t												actualX							= ::klib::printfToRect(display.begin(), display.metrics().x, display.metrics().y, offsetLine, offsetColumn, align, format, args...);
		return actualX;
	}

	template<typename _tCell, typename... _Args>
	int32_t											printfToGridColored				( ::gpk::view_grid<_tCell>& display, ::gpk::view_grid<uint16_t>& textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* format, _Args&&... args ) {
		char												precookStr[1024]				= {};
		int32_t												precookLen						= ::sprintf_s(precookStr, format, args...);
		int32_t												actualX							= ::klib::lineToGridColored(display, textAttributes, messageColor, offsetLine, offsetColumn, align, precookStr);
		return actualX;
	}

} // namespace

#endif