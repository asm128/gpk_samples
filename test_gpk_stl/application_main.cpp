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

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "Title");

					::SApplication										* g_ApplicationInstance						= 0;

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& app)											{
	const ::gpk::SCoord2<uint32_t>												newSize										= app.Framework.RootWindow.Size;
	::gpk::updateSizeDependentTarget(app.Framework.RootWindow.BackBuffer->Color, newSize);
	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& app)											{
	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	g_ApplicationInstance													= 0;
	return 0;
}


					::gpk::error_t										setup										(::SApplication& app)											{
	g_ApplicationInstance													= &app;
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::SWindow																& mainWindow								= framework.RootWindow;
	mainWindow.Size														= {640, 480};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");

	::gpk::SSTLFile																& stlFile									= app.STLFile;
	::gpk::stlFileLoad("Base Flat v5.stl", stlFile);
	app.CubePositions.resize(stlFile.Triangles.size());

	static constexpr const ::gpk::SCoord3<float>								cubeCenter									= {0.5f, 0.5f, 0.5f};
	for(uint32_t iTriangle = 0; iTriangle < app.CubePositions.size(); ++iTriangle) {
		::gpk::STriangle3<float>													& transformedTriangle						= app.CubePositions[iTriangle];
		transformedTriangle														= stlFile.Triangles[iTriangle].Triangle;

		float a = transformedTriangle.A.z
			, b = transformedTriangle.B.z
			, c = transformedTriangle.C.z
			;
		transformedTriangle.A.z = transformedTriangle.A.y;
		transformedTriangle.B.z = transformedTriangle.B.y;
		transformedTriangle.C.z = transformedTriangle.C.y;
		transformedTriangle.A.y = a;
		transformedTriangle.B.y = b;
		transformedTriangle.C.y = c;

		::gpk::SCoord3<float>	triangleSwap = transformedTriangle.B;
		transformedTriangle.B = transformedTriangle.C;
		transformedTriangle.C = triangleSwap;

		//transformedTriangle.A													-= cubeCenter;
		//transformedTriangle.B													-= cubeCenter;
		//transformedTriangle.C													-= cubeCenter;
		//transformedTriangle.A.Scale(.05);
		//transformedTriangle.B.Scale(.05);
		//transformedTriangle.C.Scale(.05);
	}
	ree_if	(errored(::updateSizeDependentResources	(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	return 0;
}

					::gpk::error_t										update										(::SApplication& app, bool systemRequestedExit)					{
	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::gpk::error_t																frameworkResult								= ::gpk::updateFramework(app.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	//-----------------------------
	::gpk::STimer																& timer										= app.Framework.Timer;
	::gpk::SWindow																& mainWindow								= app.Framework.RootWindow;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}


struct SCamera {
						::gpk::SCoord3<float>								Position, Target;
};

					::gpk::error_t										draw										(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= app.Framework;

	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			backBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), 0xFF000080, (uint32_t)-1);

	::gpk::SSTLFile																& stlFile									= app.STLFile;

	//------------------------------------------------
	::gpk::array_pod<::gpk::STriangle3<float>>									triangle3dList								= {};
	::gpk::array_pod<::gpk::SColorBGRA>											triangle3dColorList							= {};
	triangle3dList.resize(app.CubePositions.size());
	triangle3dColorList.resize(app.CubePositions.size());
	::gpk::SMatrix4<float>														projection									= {};
	::gpk::SMatrix4<float>														viewMatrix									= {};
	projection.Identity();
	::gpk::SFrameInfo															& frameInfo									= framework.FrameInfo;
	const ::gpk::SCoord3<float>													tilt										= {10, };	// ? cam't remember what is this. Radians? Eulers?
	const ::gpk::SCoord3<float>													rotation									= {0, (float)frameInfo.FrameNumber / 100, 0};

	::gpk::SNearFar																nearFar										= {0.01f , 1000.0f};

	static constexpr const ::gpk::SCoord3<float>								cameraUp									= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
	::SCamera																	camera										= {{150, 75, 0}, {}};
	::gpk::SCoord3<float>														lightPos									= {150, 75, 0};
	static float																cameraRotation								= 0;
	cameraRotation															+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	camera.Position	.RotateY(frameInfo.Microseconds.Total / 1000000.0f);
	lightPos		.RotateY(frameInfo.Microseconds.Total /  500000.0f * -2);
	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= backBuffer->Color.View.metrics();
	projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far );
	projection																= viewMatrix * projection;
	lightPos.Normalize();

	::gpk::SMatrix4<float>														viewport									= {};
	viewport._11															= 2.0f / offscreenMetrics.x;
	viewport._22															= 2.0f / offscreenMetrics.y;
	viewport._33															= 1.0f / (float)(nearFar.Far - nearFar.Near);
	viewport._43															= (float)(-nearFar.Near * ( 1.0f / (nearFar.Far - nearFar.Near) ));
	viewport._44															= 1.0f;
	projection																= projection * viewport.GetInverse();
	for(uint32_t iTriangle = 0; iTriangle < app.CubePositions.size(); ++iTriangle) {
		::gpk::STriangle3<float>													& transformedTriangle						= triangle3dList[iTriangle];
		transformedTriangle														= app.CubePositions[iTriangle];
		::gpk::transform(transformedTriangle, projection);
	}
	::gpk::array_pod<::gpk::STriangle2<int32_t>>								triangle2dList								= {};
	triangle2dList.resize(app.CubePositions.size());
	const ::gpk::SCoord2<int32_t>												screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
	for(uint32_t iTriangle = 0; iTriangle < app.CubePositions.size(); ++iTriangle) { // Maybe the scale
		::gpk::STriangle3<float>													& transformedTriangle3D						= triangle3dList[iTriangle];
		::gpk::STriangle2<int32_t>													& transformedTriangle2D						= triangle2dList[iTriangle];
		transformedTriangle2D.A													= {(int32_t)transformedTriangle3D.A.x, (int32_t)transformedTriangle3D.A.y};
		transformedTriangle2D.B													= {(int32_t)transformedTriangle3D.B.x, (int32_t)transformedTriangle3D.B.y};
		transformedTriangle2D.C													= {(int32_t)transformedTriangle3D.C.x, (int32_t)transformedTriangle3D.C.y};
		::gpk::translate(transformedTriangle2D, screenCenter);
	}

	for(uint32_t iTriangle = 0; iTriangle < app.CubePositions.size(); ++iTriangle) {
		double																		lightFactor									= stlFile.Triangles[iTriangle].Normal.Dot(lightPos);
		triangle3dColorList[iTriangle]											= (::gpk::RED * lightFactor).Clamp();
	}
	::gpk::array_pod<::gpk::SCoord2<int32_t>>									trianglePixelCoords;
	::gpk::array_pod<::gpk::SCoord2<int32_t>>									wireframePixelCoords;
	::gpk::SCoord3<float> cameraFront = (camera.Target - camera.Position).Normalize();
	for(uint32_t iTriangle = 0; iTriangle < app.CubePositions.size(); ++iTriangle) {
		//double																		lightFactor									= stlFile.Triangles[iTriangle].Normal.Dot(cameraFront);
		//if(lightFactor > 0)
		//	continue;
		gerror_if(errored(::gpk::drawTriangle(backBuffer->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])), "Not sure if these functions could ever fail");
		//::gpk::drawLine(backBuffer->Color.View, (::gpk::SColorBGRA)::gpk::GREEN	, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B});
		//::gpk::drawLine(backBuffer->Color.View, (::gpk::SColorBGRA)::gpk::BLUE	, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C});
		//::gpk::drawLine(backBuffer->Color.View, (::gpk::SColorBGRA)::gpk::RED	, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A});
		//trianglePixelCoords.clear(); ::gpk::drawTriangle(offscreenMetrics, triangle2dList[iTriangle], trianglePixelCoords);
		//for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord)
		//	::gpk::drawPixelLight(backBuffer->Color.View, trianglePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::BLUE, 0.05f, 2.5);
		//wireframePixelCoords.clear(); ::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::GREEN, 0.05f, 1.5);
		//wireframePixelCoords.clear(); ::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::CYAN, 0.05f, 1.5);
		//wireframePixelCoords.clear(); ::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
		//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::RED, 0.05f, 1.5);

		//::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		//::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		//::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	}
	//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
	//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::GREEN, 0.05f, 1.5);

	::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.MainDisplayOffscreen = backBuffer;
	//------------------------------------------------
	return 0;
}

