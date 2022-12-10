#include "gpk_engine.h"
#include "gpk_image.h"

#ifndef GPK_POOL_GAME_H_098273498237423
#define GPK_POOL_GAME_H_098273498237423

namespace the1
{
	GDEFINE_ENUM_TYPE(POOL_GAME_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 8Ball, 0);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 9Ball, 1);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 10Ball, 2);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, Test2Balls, 3);

	static constexpr	uint8_t					MAX_BALLS						= 16;

#pragma pack(push, 1)
	struct SContactResultBall {
		::gpk::SCoord3<float>						ContactPosition					= {};
		::gpk::SCoord3<float>						DistanceDirection				= {};
		float										ForceTransferRatioA				= 0;
		float										ForceTransferRatioB				= 0;

		::gpk::SCoord3<float>						InitialVelocityA				= {};
		::gpk::SCoord3<float>						InitialVelocityB				= {};
		::gpk::SCoord3<float>						FinalVelocityA					= {};
		::gpk::SCoord3<float>						FinalVelocityB					= {};

		::gpk::SCoord3<float>						InitialRotationA				= {};
		::gpk::SCoord3<float>						InitialRotationB				= {};
		::gpk::SCoord3<float>						FinalRotationA					= {};
		::gpk::SCoord3<float>						FinalRotationB					= {};


	};

	struct SContactBall {
		uint32_t									BallA							= 0;
		uint32_t									BallB							= 0;
		::gpk::SCoord3<float>						Distance						= {};
		::the1::SContactResultBall					Result							= {};
	};

	struct SContactResultCushion {
		::gpk::SCoord3<float>						ContactPosition					= {};
		::gpk::SCoord3<float>						InitialVelocityA				= {};
		::gpk::SCoord3<float>						FinalVelocityA					= {};
		::gpk::ALIGN								CushionSides					= ::gpk::ALIGN_CENTER;
	};

	struct SContactCushion {
		uint32_t									BallA							= 0;
		::gpk::SCoord3<float>						Distance						= {};
		::the1::SContactResultCushion				Result							= {};
	};

	struct SPoolBall {
		uint32_t									Entity							= (uint32_t)-1;
		float										BallRadius						= .5f;
	};

	struct SPoolTable {
		uint32_t									Entity							= (uint32_t)-1;
		::gpk::SCoord2<float>						Size							= {44, 22};
		float										Height							= .65f;
	};

	struct SPoolStartState {
		POOL_GAME_MODE								Mode							= POOL_GAME_MODE_8Ball;
		uint64_t									Seed							= (uint64_t)::gpk::timeCurrentInUs();
		uint32_t									BallCount						= 16;
		float										DampingClothDisplacement		= .7f;
		float										DampingClothRotation			= .01f;
		float										DampingCollision				= .975f;
		float										DampingCushion					= .95f;
		float										DampingGround					= .5f;

		::the1::SPoolTable							Table							= {};
		::the1::SPoolBall							Balls			[MAX_BALLS]		= {};
		::gpk::SCoord3<float>						BallPositions	[MAX_BALLS]		= {};
		uint32_t									BallOrder		[MAX_BALLS]		= {};
		::gpk::SColorFloat							BallColors		[MAX_BALLS]		= 
			{ ::gpk::WHITE
			, ::gpk::YELLOW
			, ::gpk::BLUE
			, ::gpk::RED
			, ::gpk::PURPLE 
			, ::gpk::ORANGE
			, ::gpk::GREEN
			, ::gpk::DARKRED * .5f
			, ::gpk::BLACK
			, ::gpk::YELLOW
			, ::gpk::BLUE
			, ::gpk::RED
			, ::gpk::PURPLE 
			, ::gpk::ORANGE
			, ::gpk::GREEN
			, ::gpk::DARKRED * .5f
			};
	};
#pragma pack(pop)

	struct SPoolGame {
		::the1::SPoolStartState						StartState						= {};
		::gpk::array_pod<::gpk::SLine3<float>>		PositionDeltas	[MAX_BALLS]		= {};
		::gpk::array_pod<::the1::SContactBall>		LastFrameContactsBall			= {};
		::gpk::array_pod<::the1::SContactCushion>	LastFrameContactsCushion		= {};
		::gpk::SEngine								Engine							= {};

		::gpk::error_t								GetBallPosition					(uint32_t iBall, ::gpk::SCoord3<float> & ballPosition) {
			ballPosition = Engine.Integrator.Centers[Engine.ManagedEntities.Entities[StartState.Balls[iBall].Entity].RigidBody].Position;
			return 0;
		}
	};

	::gpk::error_t								poolGameReset					(::the1::SPoolGame & pool, POOL_GAME_MODE mode);
	::gpk::error_t								poolGameSetup					(::the1::SPoolGame & pool, POOL_GAME_MODE mode = POOL_GAME_MODE_8Ball);
	::gpk::error_t								poolGameUpdate					(::the1::SPoolGame & pool, double secondsElapsed);
	::gpk::error_t								poolGameDraw
		( ::the1::SPoolGame									& pool
		, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>	& backBuffer
		, const ::gpk::SCoord3<float>						& cameraPosition
		, const ::gpk::SCoord3<float>						& cameraTarget
		, const ::gpk::SCoord3<float>						& cameraUp
		, double											totalSeconds
		);
} // namespace

#endif // GPK_POOL_GAME_H_098273498237423

// Pool is played on a six pocket table. Modern pool tables generally range in size from 3.5 by 7 feet (1.07 by 2.13 m), to 4.5 by 9 feet (1.37 by 2.74 m).
// Under World Pool-Billiard Association (WPA) official equipment specifications, pool balls have a diameter of 2.25 inches (57.15 mm) and weigh from 5.5 to 6 oz (160 to 170 g) with a diameter of 2.25 inches (57 mm), 
// plus or minus 0.005 inches (0.13 mm). Modern coin-operated pool tables generally use one of three methods to distinguish and return the cue ball to the front of the table while the numbered balls 
// return to an inaccessible receptacle until paid for again: the cue ball is larger and heavier than the other balls, or denser and heavier, or has a magnetic core.
// Modern cue sticks are generally 58.5 inches (148.6 cm) long for pool while cues prior to 1980 were designed for straight pool and had an average length of 57.5 inches (146.1 cm). 
// By comparison, carom billiards cues are generally shorter with larger tips, and snooker cues longer with smaller tips.
//
// Source: https://en.wikipedia.org/wiki/Pool_(cue_sports)

