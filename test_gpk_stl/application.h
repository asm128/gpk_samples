#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_stl.h"

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627

struct SApplication {
	::gpk::SFramework			Framework				;

	::gpk::img8bgra				TextureFont				= {};
	::gpk::imgmonou64			TextureFontMonochrome	= {};
	::gpk::SGUI					GUI						= {};
	::gpk::apod<::gpk::tri3f32>	CubePositions			= {};
	::gpk::SSTLFile				STLFile					= {};
	int32_t						IdViewport				= -1;

								SApplication			(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_23627
