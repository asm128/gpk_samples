#include "gpk_engine.h"
#include "gpk_image.h"

#ifndef GPK_THE_ONE_H_098273498237423
#define GPK_THE_ONE_H_098273498237423

#pragma pack(push, 1)
namespace the1
{
	static constexpr	uint8_t					MAX_BALLS					= 16;
	struct SPoolBall {
		uint32_t									Entity;
	};

	struct SPoolStartState {
		uint64_t									Seed						= (uint64_t)::gpk::timeCurrentInUs();
		uint32_t									BallCount					= MAX_BALLS;
		::the1::SPoolBall							Balls		[MAX_BALLS]		= {};
		uint32_t									BallOrder	[MAX_BALLS]		= {};
		::gpk::SColorFloat							BallColors	[MAX_BALLS]		= 
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

	struct SBallContactResult {
		::gpk::SCoord3<float>							ContactPosition				= {};
		::gpk::SCoord3<float>							InitialVelocityA			= {};
		::gpk::SCoord3<float>							InitialVelocityB			= {};
		::gpk::SCoord3<float>							DistanceDirection			= {};
		::gpk::SCoord3<float>							FinalVelocityA				= {};
		::gpk::SCoord3<float>							FinalVelocityB				= {};
		float											ForceTransferRatioA			= 0;
		float											ForceTransferRatioB			= 0;
	};

	struct SBallContact {
		uint32_t										BallA						= 0;
		uint32_t										BallB						= 0;
		::gpk::SCoord3<float>							Distance					= {};
		::the1::SBallContactResult						Result						= {};
	};


#pragma pack(pop)


	struct SPoolGame {
		::the1::SPoolStartState							StartState					= {};
		::gpk::SEngine									Engine						= {};
		::gpk::array_pod<::gpk::SLine3<float>>			PositionDeltas	[MAX_BALLS]	= {};
		::gpk::array_pod<::the1::SBallContact>			Contacts;
		::gpk::array_pod<::the1::SBallContact>			ContactsToDraw				= {};
	};

	::gpk::error_t								poolGameReset				(::the1::SPoolGame & pool);
	::gpk::error_t								poolGameSetup				(::the1::SPoolGame & pool);
	::gpk::error_t								poolGameUpdate				(::the1::SPoolGame & pool, double secondsElapsed);
	::gpk::error_t								poolGameDraw				(::the1::SPoolGame & pool, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds, uint64_t frameNumber);


	bool										revert						(const ::gpk::SCoord3<float> & distanceDirection, const ::gpk::SCoord3<float> & initialVelocity);
} // namespace

#endif