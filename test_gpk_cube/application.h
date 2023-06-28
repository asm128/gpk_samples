#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_tri3.h"

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627

struct SApplication {
	::gpk::SFramework		Framework				;

	::gpk::img8bgra			TextureFont				= {};
	::gpk::imgmonou64		TextureFontMonochrome	= {};
	::gpk::SGUI				GUI						= {};
	::gpk::tri3f32			CubePositions[12]		= {};
	int32_t					IdViewport				= -1;

							SApplication			(::gpk::SRuntimeValues& runtimeValues)	noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_23627
