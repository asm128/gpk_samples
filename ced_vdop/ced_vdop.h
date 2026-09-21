#include "gpk_model.h"
#include "gpk_geometry_lh.h"

#include "gpk_image.h"
#include "gpk_framework.h"

#include "draw.h"

#include "gpk_udp_client.h"

#ifndef CED_DEMO_08_H_23627
#define CED_DEMO_08_H_23627

struct SDrawCache {
	::gpk::array_pod<::gpk::n2s1_t>	PixelCoords				= {};
	::gpk::array_pod<::gpk::trif32>	PixelVertexWeights		= {};
	::gpk::array_pod<::gpk::n3f2_t>	LightPointsModel		= {};
	::gpk::array_pod<::gpk::bgra>	LightColorsModel		= {};
};

struct STextOverlay {
	::gpk::n3f2_t					LightVector0			= {-15, 50, -15};

	::gpk::n3f2_t					CameraTarget			= {0, 0, 0};
	::gpk::n3f2_t					CameraPosition			= {.0001f, 1000.1f, -0.00001f};
	::gpk::n3f2_t					CameraUp				= {0, 0, 1};
	::gpk::SGeometryQuads			GeometryLetters	[256]	= {};
	::gpk::m4<float>				MatrixProjection		= {};
	::SDrawCache					DrawCache				= {};

	stacxpr	const ::gpk::n3f2_t		ControlTranslation		= {0, 0, 20.0f};
	stacxpr	const ::gpk::n2u2_t		MetricsLetter			= {12, 12};
	stacxpr	const ::gpk::n2u2_t		MetricsMap				= {16, 16};
};

struct SApplication {
	::gpk::SFramework				Framework;
	::gpk::pobj<::gpk::rtbgra8d32>	Offscreen					= {};

	::gpk::cid_t					IdExit						= -1;

	::std::mutex					LockGUI;
	::std::mutex					LockRender;

	::STextOverlay					TextOverlay					= {};
	::klib::SGame					Game						= {};

	::gpk::SUDPClient				TacticalClient				= {};
	::gpk::apobj<::gpk::SUDPMessage>TacticalMessages;

	uint8_t							MapToDraw[5][32]			=
		{ {0xDA, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xBF}
		, {0xB3, '\0', '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	,  '\0',  '\0',  '\0'	, '\0'	, '\0'	, '\0'	,  '\0'	,  '\0'	,  '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, 0xB3}
		, {0xC3, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xB4}
		, {0xB3,  'T',  'i'	,  'm'	,  'e'	,  ':'	,  '0'	,  '0'	,  ':'	,  '0'	,  '0'	, ':'	, '0'	, '0'	, '.'	, '0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0',  '\0'	, '\0'	, '\0'	, '\0'	, 0xB3}
		, {0xC0, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xD9}
		};

														SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
};

#endif // CED_DEMO_08_H_23627
