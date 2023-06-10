#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog.h"
#include "gpk_camera.h"
#include "gpk_matrix.h"
#include "gpk_scene.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SVertexCache {
							::gpk::array_pod<::gpk::tri3<float>>							Triangle3dTransformed						= {};
							::gpk::array_pod<::gpk::bgra>									Triangle3dColorList							= {};
	};

	struct SCamera {
							::gpk::n3f32												Position;
							::gpk::n3f32												Target;
							::gpk::SNearFar														NearFar							= {0.01f , 1000.0f};
	};

	struct SViewportScene {
		stacxpr	const ::gpk::n3f32											CameraUp						= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
							::gme::SCamera														Camera							= {{10, 5, 0}, {}};

							::gpk::n3f32												LightPos						= {10, 5, 0};

							::gpk::m4<float>												Projection						= {};
							::gpk::m4<float>												ViewMatrix						= {};
	};

	struct SApplication {
							::std::mutex														LockRender						;

							::gpk::SFramework													Framework;
							::gpk::pobj<::gpk::rtbgra8d32>	Offscreen						= {};
							::gpk::aobj<::gpk::img8bgra>					PNGImages						= {};

							int32_t																IdExit							= -1;

							::std::mutex														LockViewport					;
							::gpk::SDialog														DialogMain						= {};
							int32_t																Slider							= -1;
							int32_t																NumericTuner					= -1;
							int32_t																CheckBox						= -1;
							int32_t																Viewport						= -1;
							//::gpk::tri3<float>											CubePositions	[12]			= {};
							::gpk::SModelGeometryIndexed<float, uint8_t>						ModelGeometry;

							::gpk::pobj<::gpk::rtbgra8d32>	Buffer3D;
							SVertexCache														VertexCache;

							int32_t																IdFrameRateUpdate				= -1;
							int32_t																IdFrameRateRender				= -1;
							char																StringFrameRateUpdate	[256]	= {};
							char																StringFrameRateRender	[256]	= {};

							SViewportScene														Scene;

																								SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace


#endif // APPLICATION_H_2078934982734
