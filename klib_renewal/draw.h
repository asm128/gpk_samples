#include "Game.h"

#include "gpk_noise.h"

#include <algorithm>

#ifndef __DRAW_H_2394623987462983746823749623__
#define __DRAW_H_2394623987462983746823749623__

namespace klib
{

	void				drawFireBackground			( ::klib::SWeightedDisplay& display, double lastTimeSeconds, uint32_t disturbance = 2, uint32_t disappearChanceDivisor=10, bool bReverse=false, bool bDontSlowdown=true );

	template<typename _TCell, size_t _LineCount>
	SGameState			drawCredits					(_TCell* display, uint32_t width, uint32_t depth, double lastFrameTime, const ::gpk::view_const_string (&namesCredits)[_LineCount], const SGameState& returnValue) {
		static double			offset						= (double)depth;
		int32_t					curLine						= (int32_t)offset;
		static int32_t			maxDifference				= curLine;
		int32_t					curDifference				= curLine;
		double					bbHeight					= (double)depth;

		for(uint32_t i=0, count = (uint32_t)::gpk::size(namesCredits); i < count && curLine < bbHeight; ++i)
			if((curLine+=2) >= 0)
				printfToRect((char_t*)display, width, depth, curLine, 0, ::klib::SCREEN_CENTER, "%s", namesCredits[i].begin());

		maxDifference			= ::gpk::max(curLine - curDifference, maxDifference);

		offset					-= lastFrameTime*6.0;

		if( offset <= -maxDifference )
			offset += depth+maxDifference;

		return returnValue;
	}

	template<typename _TCell, size_t _LineCount>
	SGameState drawCredits(::gpk::view_grid<_TCell> display, double lastFrameTime, const ::gpk::view_const_string (&namesCredits)[_LineCount], const SGameState& returnValue) {
		return ::klib::drawCredits(&display[0][0], display.metrics().x, display.metrics().y, lastFrameTime, namesCredits, returnValue);
	}

	void drawAndPresentGame( SGame& instanceGame, ::klib::SASCIITarget& target );

} // namespace

#define TACTICAL_DISPLAY_POSY 6

#endif // __DRAW_H_2394623987462983746823749623__
