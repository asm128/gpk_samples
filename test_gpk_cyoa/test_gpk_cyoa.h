#include "rgb_cyoa.h"

#include "gpk_framework.h"
#include "gpk_gui.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{

	enum GAME_MODE {
		GAME_MODE_CYOA,
		GAME_MODE_SOLAR_SYSTEM,
		GAME_MODE_GALAXY_HELL,
	};

	struct SApplication {
		::gpk::SFramework													Framework;
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen					= {};
		::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>					PNGImages					= {};

		int32_t																IdExit						= -1;

		::std::mutex														LockGUI;
		::std::mutex														LockRender;

		::rgbg::SGame														CYOA;
		::gpk::array_pod<int32_t>											IdJumps						= {};
		::gpk::array_pod<int32_t>											IdLines						= {};

		GAME_MODE															ActiveGameMode				= GAME_MODE_CYOA;

																			SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
