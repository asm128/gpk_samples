#include "gpk_engine.h"
#include "gpk_image.h"

#ifndef GPK_THE_ONE_H_098273498237423
#define GPK_THE_ONE_H_098273498237423

#pragma pack(push, 1)
struct SPoolBall {
	uint32_t									Entity;
};

struct SPoolStartState {
	uint64_t									Seed					= (uint64_t)::gpk::timeCurrentInUs();
	uint32_t									BallCount				= 16;
	SPoolBall									Balls		[16]		= {};
	uint32_t									BallOrder	[16]		= {};
	::gpk::SColorFloat							BallColors	[16]		= 
		{ ::gpk::WHITE
		, ::gpk::LIGHTYELLOW
		, ::gpk::LIGHTBLUE
		, ::gpk::LIGHTRED
		, ::gpk::PURPLE * 1.25
		, ::gpk::LIGHTORANGE
		, ::gpk::LIGHTGREEN
		, ::gpk::LIGHTRED
		, ::gpk::BLACK
		, ::gpk::DARKYELLOW
		, ::gpk::DARKBLUE
		, ::gpk::DARKRED
		, ::gpk::PURPLE * .5
		, ::gpk::DARKORANGE
		, ::gpk::DARKGREEN
		, ::gpk::DARKRED
		};
};
#pragma pack(pop)

struct SPoolGame {
	::SPoolStartState							StartState				= {};
	::gpk::SEngine								Engine					= {};
	::gpk::array_pod<::gpk::SLine3<float>>		Deltas		[16]		= {};
};

::gpk::error_t								poolGameReset			(::SPoolGame & pool);
::gpk::error_t								poolGameSetup			(::SPoolGame & pool);
::gpk::error_t								poolGameUpdate			(::SPoolGame & pool, double secondsElapsed);
::gpk::error_t								poolGameDraw			(::SPoolGame & pool, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds, uint64_t frameNumber);

#endif