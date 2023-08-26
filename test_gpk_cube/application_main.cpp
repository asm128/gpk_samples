// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "gpk_bitmap_target.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"

stacxpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
stacxpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "Title");

					::SApplication										* g_ApplicationInstance						= 0;

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& app)											{
	const ::gpk::n2<uint16_t>													newSize										= app.Framework.RootWindow.Size;
	::gpk::updateSizeDependentTarget(app.Framework.RootWindow.BackBuffer->Color, newSize);
	::gpk::updateSizeDependentTarget(app.Framework.RootWindow.BackBuffer->DepthStencil, newSize);
	return 0;
}

// --- Cleanup application resources.
::gpk::error_t			cleanup		(::SApplication& app)											{
	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	g_ApplicationInstance													= 0;
	return 0;
}

// Vertex coordinates for cube faces
stacxpr const ::gpk::tri3f32						geometryCube	[12]						=
	{ {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}	// Right	- first			?? I have no idea if this is correct lol
	, {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Right	- second		?? I have no idea if this is correct lol

	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}	// Back		- first			?? I have no idea if this is correct lol
	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}	// Back		- second		?? I have no idea if this is correct lol

	, {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}	// Bottom	- first			?? I have no idea if this is correct lol
	, {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Bottom	- second		?? I have no idea if this is correct lol

	, {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Left		- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}	// Left		- second

	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}	// Front	- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Front	- second

	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}	// Top		- first
	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}}	// Top		- second
	};

::gpk::error_t			setup		(::SApplication& app)											{
	g_ApplicationInstance													= &app;
	::gpk::SFramework				& framework									= app.Framework;
	::gpk::SWindow					& mainWindow								= framework.RootWindow;
	mainWindow.Size														= {640, 480};
	es_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));

	stacxpr const ::gpk::n3f32								cubeCenter									= {0.5f, 0.5f, 0.5f};
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		::gpk::tri3f32													& transformedTriangle						= app.CubePositions[iTriangle];
		transformedTriangle														= geometryCube[iTriangle];
		transformedTriangle.A													-= cubeCenter;
		transformedTriangle.B													-= cubeCenter;
		transformedTriangle.C													-= cubeCenter;
	}
	ree_if	(errored(::updateSizeDependentResources	(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	return 0;
}

::gpk::error_t			update		(::SApplication& app, bool systemRequestedExit)					{
	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::gpk::error_t																frameworkResult								= ::gpk::updateFramework(app.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	//-----------------------------
	::gpk::STimer																& timer										= app.Framework.Timer;
	::gpk::SWindow					& mainWindow								= app.Framework.RootWindow;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

stacxpr const ::gpk::n3f32						geometryCubeNormals	[12]						=
	{ {0.0f, 0.0f, -1.0f}	// Right	- first			?? I have no idea if this is correct lol
	, {0.0f, 0.0f, -1.0f}	// Right	- second		?? I have no idea if this is correct lol

	, {-1.0f, 0.0f, 0.0f}	// Back		- first			?? I have no idea if this is correct lol
	, {-1.0f, 0.0f, 0.0f}	// Back		- second		?? I have no idea if this is correct lol

	, {0.0f, -1.0f, 0.0f}	// Bottom	- first			?? I have no idea if this is correct lol
	, {0.0f, -1.0f, 0.0f}	// Bottom	- second		?? I have no idea if this is correct lol

	, {0.0f, 0.0f, 1.0f}	// Left		- first
	, {0.0f, 0.0f, 1.0f}	// Left		- second

	, {1.0f, 0.0f, 0.0f}	// Front	- first
	, {1.0f, 0.0f, 0.0f}	// Front	- second

	, {0.0f, 1.0f, 0.0f}	// Top		- first
	, {0.0f, 1.0f, 0.0f}	// Top		- second
	};

struct SCamera {
						::gpk::n3f32								Position, Target;
};

::gpk::error_t			draw		(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework				& framework									= app.Framework;

	::gpk::pobj<::gpk::rtbgra8d32>	backBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), 0xFF000080, (uint32_t)-1);

	//------------------------------------------------
	::gpk::apod<::gpk::tri3f32>									triangle3dList								= {};
	::gpk::apod<::gpk::bgra>											triangle3dColorList							= {};
	triangle3dList.resize(12);
	triangle3dColorList.resize(12);
	::gpk::m4f32														projection									= {};
	::gpk::m4f32														viewMatrix									= {};
	projection.Identity();
	::gpk::SFrameInfo															& frameInfo									= framework.FrameInfo;
	const ::gpk::n3f32													tilt										= {10, };	// ? cam't remember what is this. Radians? Eulers?
	const ::gpk::n3f32													rotation									= {0, (float)frameInfo.FrameMeter.FrameNumber / 100, 0};

	::gpk::minmaxf32													nearFar										= {0.01f , 1000.0f};

	stacxpr const ::gpk::n3f32								cameraUp									= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
	::SCamera																	camera										= {{10, 5, 0}, {}};
	::gpk::n3f32														lightPos									= {10, 5, 0};
	static float																cameraRotation								= 0;
	cameraRotation															+= (float)framework.RootWindow.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	camera.Position	.RotateY(frameInfo.Microseconds.Total / 1000000.0f);
	lightPos		.RotateY(frameInfo.Microseconds.Total /  500000.0f * -2);
	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	const ::gpk::n2<uint16_t>												& offscreenMetrics							= backBuffer->Color.View.metrics().Cast<uint16_t>();
	projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar);
	projection																= viewMatrix * projection;
	lightPos.Normalize();

	::gpk::m4f32														viewport									= {};
	viewport._11															= 2.0f / offscreenMetrics.x;
	viewport._22															= 2.0f / offscreenMetrics.y;
	viewport._33															= 1.0f / (float)(nearFar.Max - nearFar.Min);
	viewport._43															= (float)(-nearFar.Min * ( 1.0f / (nearFar.Max - nearFar.Min) ));
	viewport._44															= 1.0f;
	projection																= projection * viewport.GetInverse();
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		::gpk::tri3f32													& transformedTriangle						= triangle3dList[iTriangle];
		transformedTriangle														= app.CubePositions[iTriangle];
		::gpk::transform(transformedTriangle, projection);
	}
	::gpk::apod<::gpk::tri2<int32_t>>								triangle2dList								= {};
	triangle2dList.resize(12);
	const ::gpk::n2<int32_t>												screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) { // Maybe the scale
		::gpk::tri3f32													& transformedTriangle3D						= triangle3dList[iTriangle];
		::gpk::tri2<int32_t>													& transformedTriangle2D						= triangle2dList[iTriangle];
		transformedTriangle2D.A													= {(int32_t)transformedTriangle3D.A.x, (int32_t)transformedTriangle3D.A.y};
		transformedTriangle2D.B													= {(int32_t)transformedTriangle3D.B.x, (int32_t)transformedTriangle3D.B.y};
		transformedTriangle2D.C													= {(int32_t)transformedTriangle3D.C.x, (int32_t)transformedTriangle3D.C.y};
		::gpk::translate(transformedTriangle2D, screenCenter);
	}

	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																		lightFactor									= geometryCubeNormals[iTriangle].Dot(lightPos);
		triangle3dColorList[iTriangle]											= (::gpk::RED * lightFactor).Clamp();
	}
	::gpk::apod<::gpk::n2<int16_t>>									trianglePixelCoords;
	::gpk::apod<::gpk::n2<int16_t>>									wireframePixelCoords;
	::gpk::n3f32														cameraFront					= (camera.Target - camera.Position).Normalize();
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																		lightFactor									= geometryCubeNormals[iTriangle].Dot(cameraFront);
		if(lightFactor > 0)
			continue;
		es_if(errored(::gpk::drawTriangle(backBuffer->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])));
		//::gpk::drawLine(backBuffer->Color.View, (::gpk::bgra)::gpk::GREEN	, ::gpk::line2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B});
		//::gpk::drawLine(backBuffer->Color.View, (::gpk::bgra)::gpk::BLUE	, ::gpk::line2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C});
		//::gpk::drawLine(backBuffer->Color.View, (::gpk::bgra)::gpk::RED	, ::gpk::line2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A});
		//trianglePixelCoords.clear(); ::gpk::drawTriangle(offscreenMetrics, triangle2dList[iTriangle], trianglePixelCoords);
		//for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord)
		//	::gpk::drawPixelLight(backBuffer->Color.View, trianglePixelCoords[iCoord], (::gpk::bgra)::gpk::BLUE, 0.05f, 2.5);
		//wireframePixelCoords.clear(); ::gpk::drawLine(offscreenMetrics, ::gpk::line2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::bgra)::gpk::GREEN, 0.05f, 1.5);
		//wireframePixelCoords.clear(); ::gpk::drawLine(offscreenMetrics, ::gpk::line2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::bgra)::gpk::CYAN, 0.05f, 1.5);
		//wireframePixelCoords.clear(); ::gpk::drawLine(offscreenMetrics, ::gpk::line2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
		//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::bgra)::gpk::RED, 0.05f, 1.5);
		::gpk::drawLine(offscreenMetrics, ::gpk::line2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::line2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::line2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	}
	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::bgra)::gpk::GREEN, 0.05f, 1.5);

	::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.MainDisplayOffscreen = backBuffer;
	//------------------------------------------------
	return 0;
}

