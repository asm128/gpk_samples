#include "gpk_framework.h"

#include "gpk_tri3.h"

#ifndef TEST_GPK_OPENGL_H_23627
#define TEST_GPK_OPENGL_H_23627

struct SApplication {
	::gpk::SFramework	Framework				;

	::gpk::img8bgra		TextureFont				= {};
	::gpk::imgmonou64	TextureFontMonochrome	= {};
	::gpk::SGUI			GUI						= {};
	::gpk::tri3f32		CubePositions[12]		= {};
	int32_t				IdViewport				= -1;

	HDC					DrawingContext			= {};
	HGLRC				GLRenderContext			= {}; 

						SApplication			(::gpk::SRuntimeValues & runtimeValues)			noexcept	: Framework(runtimeValues) {}
};


#endif