#include "gpk_model.h"
#include "gpk_geometry_lh.h"

#include "gpk_image.h"
#include "gpk_framework.h"

#include "draw.h"

#include "gpk_udp_client.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837

struct SDrawCache {
	::gpk::array_pod<::gpk::SCoord2<int16_t>>			PixelCoords				= {};
	::gpk::array_pod<::gpk::STriangle<float>>			PixelVertexWeights		= {};
	::gpk::array_pod<::gpk::SCoord3<float>>				LightPointsModel		= {};
	::gpk::array_pod<::gpk::SColorBGRA>					LightColorsModel		= {};
};

struct STextOverlay {
	::gpk::SCoord3<float>								LightVector0			= {-15, 50, -15};

	::gpk::SCoord3<float>								CameraTarget			= {0, 0, 0};
	::gpk::SCoord3<float>								CameraPosition			= {.0001f, 1000.1f, -0.00001f};
	::gpk::SCoord3<float>								CameraUp				= {0, 0, 1};
	::gpk::SGeometryQuads								GeometryLetters	[256]	= {};
	::gpk::SMatrix4<float>								MatrixProjection		= {};
	::SDrawCache										DrawCache				= {};

	static constexpr	const ::gpk::SCoord3<float>		ControlTranslation		= {0, 0, 20.0f};
	static constexpr	const ::gpk::SCoord2<uint32_t>	MetricsLetter			= {12, 12};
	static constexpr	const ::gpk::SCoord2<uint32_t>	MetricsMap				= {16, 16};
};

struct SApplication {
				::gpk::SFramework													Framework;
				::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen					= {};

				int32_t																IdExit						= -1;

				::std::mutex														LockGUI;
				::std::mutex														LockRender;

				::STextOverlay														TextOverlay					= {};
				::klib::SGame														Game						= {};

				::gpk::SUDPClient													TacticalClient				= {};
				::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>		TacticalMessages;

				uint8_t																MapToDraw[5][32]			=
					{ {0xDA, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xBF}
					, {0xB3, '\0', '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	,  '\0',  '\0',  '\0'	, '\0'	, '\0'	, '\0'	,  '\0'	,  '\0'	,  '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, 0xB3}
					, {0xC3, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xB4}
					, {0xB3,  'T',  'i'	,  'm'	,  'e'	,  ':'	,  '0'	,  '0'	,  ':'	,  '0'	,  '0'	, ':'	, '0'	, '0'	, '.'	, '0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0',  '\0'	, '\0'	, '\0'	, '\0'	, 0xB3}
					, {0xC0, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xD9}
					};

																	SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
};

#endif // CED_DEMO_08_H_298837492837
