#include "gpk_pool_game.h"

#ifndef GPK_THE_ONE_H_098273498237423
#define GPK_THE_ONE_H_098273498237423

namespace the1 
{
	struct STheOne {
		::the1::SPoolGame							Game;
	};

	::gpk::error_t								theOneSetup				(::the1::STheOne & app, the1::POOL_GAME_MODE mode = the1::POOL_GAME_MODE_8Ball);
	::gpk::error_t								theOneUpdate			(::the1::STheOne & app, double secondsElapsed);
	::gpk::error_t								theOneDraw				(::the1::STheOne & app, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds, uint64_t frameNumber);

} // namespace

#endif