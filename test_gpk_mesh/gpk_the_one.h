#include "gpk_pool_game.h"

#include "gpk_gui_desktop.h"

#ifndef GPK_THE_ONE_H_098273498237423
#define GPK_THE_ONE_H_098273498237423

namespace the1 
{
	struct SCamera {
		::gpk::SCoord3<float>							Position						= {};
		::gpk::SCoord3<float>							Target							= {1, 0, 0};
		uint8_t											BallLockAtPosition				= (uint8_t)-1;
		uint8_t											BallLockAtTarget				= (uint8_t)-1;
	};

	typedef ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> TRenderTarget;
	struct STheOneGame {
		::the1::SPoolGame								Game							= {};
		::gpk::SDesktop									Desktop							= {};
		::the1::TRenderTarget							GameRenderTarget				= {};
		::gpk::array_pod<::the1::SContactBall>			ContactsToDraw					= {};

		SCamera											CameraPlayer					= {{0, 20, -40}, {}};
		SCamera											CameraBalls[::the1::MAX_BALLS]	= {{0, 20, -40}, {}};
		uint32_t										Camera							= 1;
	};

	GDEFINE_ENUM_TYPE(APP_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(APP_STATE, Init		,  0);
	GDEFINE_ENUM_VALUE(APP_STATE, Welcome	,  1);
	GDEFINE_ENUM_VALUE(APP_STATE, Home		,  2);
	GDEFINE_ENUM_VALUE(APP_STATE, Profile	,  3);
	GDEFINE_ENUM_VALUE(APP_STATE, Shop		,  4);
	GDEFINE_ENUM_VALUE(APP_STATE, Play		,  5);
	GDEFINE_ENUM_VALUE(APP_STATE, Brief		,  6);
	GDEFINE_ENUM_VALUE(APP_STATE, Stage		,  7);
	GDEFINE_ENUM_VALUE(APP_STATE, Stats		,  8);
	GDEFINE_ENUM_VALUE(APP_STATE, Store		,  9);
	GDEFINE_ENUM_VALUE(APP_STATE, Score		, 10);
	GDEFINE_ENUM_VALUE(APP_STATE, About		, 11);
	GDEFINE_ENUM_VALUE(APP_STATE, Settings	, 12);
	GDEFINE_ENUM_VALUE(APP_STATE, Quit		, 13);
	GDEFINE_ENUM_VALUE(APP_STATE, Load		, 14);
	GDEFINE_ENUM_VALUE(APP_STATE, COUNT		, 15);
	struct STheOne {
		::the1::STheOneGame								MainGame			= {};
		::gpk::array_static<::the1::STheOneGame, 64>	TestGames			= {};

		APP_STATE										ActiveState			= {};
	};

	::gpk::error_t									theOneSetup			(::the1::STheOne & app, the1::POOL_GAME_MODE mode = the1::POOL_GAME_MODE_8Ball);
	::gpk::error_t									theOneUpdate		(::the1::STheOne & app, double secondsElapsed);
	::gpk::error_t									theOneDraw			(::the1::STheOne & app, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds);

} // namespace

#endif