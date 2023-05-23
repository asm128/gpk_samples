#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_stl.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
						::gpk::SFramework								Framework									;

						::gpk::SImage<::gpk::bgra>				TextureFont									= {};
						::gpk::SImageMonochrome<uint32_t>				TextureFontMonochrome						= {};
						::gpk::SGUI										GUI											= {};
						::gpk::array_pod<::gpk::tri3<float>>		CubePositions								= {};
						::gpk::SSTLFile									STLFile										= {};
						int32_t											IdViewport									= -1;

																		SApplication								(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
