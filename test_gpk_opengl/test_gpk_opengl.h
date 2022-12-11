﻿#include "gpk_framework.h"

#ifndef TEST_GPK_OPENGL_H_284987234
#define TEST_GPK_OPENGL_H_284987234

struct SApplication {
	::gpk::SFramework					Framework				;

	::gpk::SImage<::gpk::SColorBGRA>	TextureFont				= {};
	::gpk::SImageMonochrome<uint32_t>	TextureFontMonochrome	= {};
	::gpk::SGUI							GUI						= {};
	::gpk::STriangle3	<float>			CubePositions[12]		= {};
	int32_t								IdViewport				= -1;

	HDC									DrawingContext			= {};
	HGLRC								GLRenderContext			= {}; 

										SApplication			(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};


#endif