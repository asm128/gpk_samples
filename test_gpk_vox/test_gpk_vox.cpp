// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "test_gpk_vox.h"

#include "gpk_bitmap_target.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "Title");

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& app)											{
	::gpk::SWindowPlatformDetail												& displayDetail								= app.Framework.MainDisplay.PlatformDetail;
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	return 0;
}


					::gpk::error_t										setup										(::SApplication& app)											{
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::SWindow																& mainWindow								= framework.MainDisplay;
	mainWindow.Size														= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");



	::gpk::array_pod<byte_t>			fileBytes;
	::gpk::array_static<char, 256>		pathNameData		= {"../gpk_data"};

	::gpk::array_static<char, 256>		folderNameVox		= {"vox"};

	uint64_t							timeStart			= ::gpk::timeCurrentInMs();
	for(uint32_t iModel = 0; iModel < ::gpk::size(fileNames); ++iModel) {
		gpk::vcc							fileName			= fileNames[iModel];
		char								pathToLoad[4096]	= {};
		sprintf_s(pathToLoad, "%s/%s/%s", pathNameData.Storage, folderNameVox.Storage, fileName.begin());
		gpk_necs(::gpk::fileToMemory(pathToLoad, fileBytes));
		::gpk::vcc							viewBytes			= fileBytes;
		gpk_necs(app.VOXModels[app.VOXModels.push_back({})]->Load(viewBytes));
		app.VOXModelNames.push_back(fileName);
		gpk_vox_info_printf("Loaded %s.", pathToLoad);
	}

	app.VOXModelMaps.resize(app.VOXModels.size());
	for(uint32_t iModel = 0; iModel < app.VOXModels.size(); ++iModel) {
		const ::gpk::ptr_obj<::gpk::SVOXData>		& voxFile			= app.VOXModels[iModel];
		::gpk::SVoxelMap<uint8_t>					& chunkMap			= app.VOXModelMaps[iModel];
		auto										coord				= voxFile->GetDimensions();
		gpk_vox_info_printf("Constructing %s.", app.VOXModelNames[iModel].begin());
		gpk_vox_info_printf("Model size: %i, %i, %i.", coord.x, coord.y, coord.z);
		::gpk::view_array<const ::gpk::SVOXVoxel>	voxels				= voxFile->GetXYZI();
		for(uint32_t iVoxel = 0; iVoxel < voxels.size(); ++iVoxel) {
			const ::gpk::SVOXVoxel						voxel				= voxels[iVoxel];
			chunkMap.SetValue({voxel.x, voxel.y, voxel.z}, voxel.i ? 1 : 0);
		}

	}
	uint64_t							timeStop			= ::gpk::timeCurrentInMs();
	info_printf("Total time: %llu ms", timeStop - timeStart);
	return 0;
}

					::gpk::error_t										update										(::SApplication& app, bool systemRequestedExit)					{
	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::gpk::SWindow																& mainWindow								= app.Framework.MainDisplay;
	for(uint32_t iEvent = 0; iEvent < mainWindow.EventQueue.size(); ++iEvent) {
		switch(mainWindow.EventQueue[iEvent].Type) {
		case ::gpk::SYSEVENT_ACTIVATE:
			break;
		case ::gpk::SYSEVENT_DEACTIVATE:
			break;
		case ::gpk::SYSEVENT_SYSKEY_DOWN:
		case ::gpk::SYSEVENT_KEY_DOWN:
			switch(mainWindow.EventQueue[iEvent].Data[0]) {
			case VK_RETURN:
				if(GetAsyncKeyState(VK_MENU) & 0xFFF0)
					gpk_necs(::gpk::fullScreenToggle(mainWindow));
				break;
			}
		}
	}

	::gpk::error_t																frameworkResult								= ::gpk::updateFramework(app.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	//-----------------------------
	::gpk::STimer																& timer										= app.Framework.Timer;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	return 0;
}


struct SCamera {
						::gpk::SCoord3<float>								Position, Target;
};

::gpk::error_t										drawVoxelFace			
	( ::gpk::view_grid<uint32_t>								targetDepth
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>					& out_Points
	, const ::gpk::SCoord3<float>								& voxelPos
	, const ::gpk::SCoord2<uint16_t>							screenCenter
	, const ::gpk::SMatrix4<float>								& mWVP
	, const ::gpk::SNearFar										& nearFar
	, const ::gpk::view_array<const ::gpk::SCoord3<float>>		verticesRaw
	, const ::gpk::view_array<const uint8_t>					& indices
	) {
	::gpk::SCoord3<float>									vertices [4]				= {}; 
	for(uint32_t iVertex = 0; iVertex < 4; ++iVertex) {
		vertices[iVertex] = mWVP.Transform(verticesRaw[iVertex] + voxelPos); 
		vertices[iVertex].x += screenCenter.x;
		vertices[iVertex].y += screenCenter.y;
	}
	::gpk::drawTriangleIndexed<float, uint8_t>(targetDepth, nearFar, 0, 0, vertices, indices, out_Points); 
	::gpk::drawTriangleIndexed<float, uint8_t>(targetDepth, nearFar, 3, 0, vertices, indices, out_Points); 
	return 0;
}

					::gpk::error_t					draw								(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework										& framework							= app.Framework;

	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	backBuffer							= app.BackBuffer;
	backBuffer->resize(framework.MainDisplayOffscreen->metrics());
	framework.MainDisplayOffscreen->resize(framework.MainDisplay.Size);
	backBuffer->Color.View.fill(0x3060C0);
	memset(backBuffer->DepthStencil.begin(), (uint32_t)-1, backBuffer->DepthStencil.View.byte_count());

	::gpk::SMatrix4<float>									projection									= {};
	::gpk::SMatrix4<float>									viewMatrix									= {};
	projection.Identity();
	::gpk::SFrameInfo										& frameInfo									= framework.FrameInfo;
	const ::gpk::SCoord3<float>								tilt										= {10, };	// ? cam't remember what is this. Radians? Eulers?
	const ::gpk::SCoord3<float>								rotation									= {0, (float)frameInfo.FrameNumber / 100, 0};

	::gpk::SNearFar											nearFar										= {0.1f , 1000.0f};

	static constexpr const ::gpk::SCoord3<float>			cameraUp									= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
	::SCamera												camera										= {{350, 50, 0}, {}};
	//camera.Position *= 2.0f;
	::gpk::SCoord3<float>									lightPos									= {150, 50, 0};
	static float											cameraRotation								= 0;
	cameraRotation										+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	camera.Position	.RotateY(frameInfo.Seconds.Total * 0.1f);
	//camera.Position	.RotateX(frameInfo.Microseconds.Total / 10000000.0f);
	lightPos		.RotateY(frameInfo.Seconds.Total * -1.f);
	camera.Target.z = camera.Target.x = 75;
	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	const ::gpk::SCoord2<uint32_t>							& offscreenMetrics							= backBuffer->Color.View.metrics();
	projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far );
	projection											= viewMatrix * projection;
	lightPos.x += 100;
	lightPos.y *= (float)fabs(sin(frameInfo.Seconds.Total * .1f));

	::gpk::SMatrix4<float>									viewport									= {};
	viewport._11										= 2.0f / offscreenMetrics.x;
	viewport._22										= 2.0f / offscreenMetrics.y;
	viewport._33										= 1.0f / (float)(nearFar.Far - nearFar.Near);
	viewport._43										= (float)(-nearFar.Near * ( 1.0f / (nearFar.Far - nearFar.Near) ));
	viewport._44										= 1.0f;
	projection											= projection * viewport.GetInverse();
	const ::gpk::SCoord2<uint16_t>							screenCenter				= {(uint16_t)(offscreenMetrics.x / 2), (uint16_t)(offscreenMetrics.y / 2)};

	::gpk::SCoord3<float>									cameraFront					= (camera.Target - camera.Position).Normalize();

	::gpk::array_pod<::gpk::SCoord2<int16_t>>				out_Points			[6]		= {};
	::gpk::array_pod<::gpk::STriangleWeights<double>>		triangleWeights		[6]		= {};	
	uint32_t												zOffset						= 0;
	uint32_t												xOffset						= 0;
	::gpk::SRange<uint32_t>									faceMaterialRange	[6]		= {};

	for(uint32_t iModel = 0; iModel < app.VOXModels.size(); ++iModel) {
		::gpk::ptr_obj<::gpk::SVOXData>							voxFile						= app.VOXModels[iModel];
		const ::gpk::SCoord3<uint16_t>							dimensions					= voxFile->GetDimensions();
		::gpk::view_array<const ::gpk::SVOXVoxel>				voxels						= voxFile->GetXYZI();
		::gpk::view_array<const uint32_t>						rgba						= voxFile->GetRGBA();
		if(0 == rgba.size())
			rgba												= ::gpk::VOX_PALETTE_DEFAULT;

		const ::gpk::SVoxelMap<uint8_t>							& voxelMap					= app.VOXModelMaps[iModel];

		if(zOffset > 100) {
			xOffset												+= ::gpk::max(uint16_t(dimensions.x), uint16_t(30));
			zOffset												= 0;
		}
		else
			zOffset												+= dimensions.z + 4;

		::gpk::SCoord3<float>									vertices		[8]			= {};
		bool													culled						= true;
		for(uint32_t i = 0; i < 8; ++i) {
			vertices[i]											= ::gpk::VOXEL_VERTICES[i];
			vertices[i].Scale(dimensions.Cast<float>());
			vertices[i].x										+= xOffset;
			vertices[i].z										+= zOffset;
			if((vertices[i] - camera.Position).Normalize().Dot(cameraFront) > 0) {
				culled = false;
				break;
			}
		}

		if(culled)
			continue;

		for(uint32_t iVoxel = 0; iVoxel < voxels.size(); ++iVoxel) {
			const ::gpk::SVOXVoxel					voxel						= voxels[iVoxel];
			const ::gpk::SCoord3<float>				voxelPos					= ::gpk::SCoord3<float>{(float)voxel.x + xOffset, (float)voxel.y, (float)voxel.z + zOffset};
			uint8_t									cellValue					= 0;
			uint8_t									cellValues	[6]				= {};
			voxelMap.GetValue({voxel.x, voxel.y, voxel.z}, cellValue);
			voxelMap.GetValue({voxel.x, uint32_t(voxel.y + 1), voxel.z}, cellValues[::gpk::VOXEL_FACE_TOP	]);
			voxelMap.GetValue({uint32_t(voxel.x + 1), voxel.y, voxel.z}, cellValues[::gpk::VOXEL_FACE_FRONT	]);
			voxelMap.GetValue({voxel.x, voxel.y, uint32_t(voxel.z + 1)}, cellValues[::gpk::VOXEL_FACE_RIGHT	]);
			if(voxel.y) voxelMap.GetValue({voxel.x, uint32_t(voxel.y - 1), voxel.z}, cellValues[::gpk::VOXEL_FACE_BOTTOM]);
			if(voxel.x) voxelMap.GetValue({uint32_t(voxel.x - 1), voxel.y, voxel.z}, cellValues[::gpk::VOXEL_FACE_BACK	]);
			if(voxel.z) voxelMap.GetValue({voxel.x, voxel.y, uint32_t(voxel.z - 1)}, cellValues[::gpk::VOXEL_FACE_LEFT	]);

			if(0 == cellValue)
				continue;

			if((voxelPos - camera.Position).Normalize().Dot(cameraFront) < .5)
				continue;

			const ::gpk::SColorFloat				cellColor					= {rgba[voxel.i ? voxel.i - 1 : 0]};
			bool									renderFaces	[6]				= {};
			::gpk::SColorFloat						faceColor	[6]				= {};
			for(uint32_t iFace = 0; iFace < 6; ++iFace) {
				faceColor[iFace]					= {rgba[cellValues[iFace] ? cellValues[iFace] - 1 : 0]};
				if(0 == cellValues[iFace])
					renderFaces[iFace] = ::gpk::VOXEL_FACE_NORMALS[iFace].Dot(cameraFront) <= 0; 
			}

			const double							lightFactorDistance			= ::gpk::clamp(1.0 - (lightPos - voxelPos).Length() * .001, 0.0, 1.0);
			const double							lightFactorAmbient			= .4;
			const ::gpk::SColorFloat				colorAmbient				= cellColor * lightFactorAmbient;
			for(uint32_t iFace = 0; iFace < 6; ++iFace) {
				if(false == renderFaces[iFace]) 
					continue;

				out_Points		[iFace].clear();
				triangleWeights	[iFace].clear();
				::drawVoxelFace(backBuffer->DepthStencil.View, out_Points[iFace], voxelPos, screenCenter.Cast<uint16_t>(), projection, nearFar, ::gpk::view_array<const ::gpk::SCoord3<float>>{&::gpk::VOXEL_FACE_VERTICES[iFace].A, 4}, ::gpk::VOXEL_FACE_INDICES[iFace]); 

				if(lightFactorDistance <= 0) {
					faceColor[iFace]					= colorAmbient * lightFactorDistance;
					continue;
				}

				const double							lightFactorDirectional		= ::gpk::max(0.0, (lightPos - voxelPos).Normalize().Dot(::gpk::VOXEL_FACE_NORMALS[iFace]));	
				::gpk::SColorFloat						colorDiffuse				= cellColor * lightFactorDirectional;
				faceColor[iFace]					= (colorAmbient + colorDiffuse).Clamp() * lightFactorDistance; 
				for(uint32_t iPoint = 0; iPoint < out_Points[iFace].size(); ++iPoint) {
					::gpk::SCoord2<int16_t>					point					= out_Points[iFace][iPoint];
					::gpk::SColorFloat						finalColor				= faceColor[iFace];
					backBuffer->Color[point.y][point.x]	= finalColor;
				}
			}		
		}
	}
	::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	//framework.MainDisplayOffscreen = backBuffer;
	//------------------------------------------------
	return 0;
}

