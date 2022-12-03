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

#include <GL\Gl.h>

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

GPK_DEFINE_APPLICATION_ENTRY_POINT_MT(::SApplication, "Title");

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& app)											{
	::gpk::SWindowPlatformDetail												& displayDetail								= app.Framework.MainDisplay.PlatformDetail;
	// --- when the rendering context is no longer needed ...   
 	// make the rendering context not current  
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	return 0;
}

					::gpk::error_t										setup										(::SApplication & app)											{
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::SWindow																& mainWindow								= framework.MainDisplay;
	mainWindow.Size														= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");

	framework.MainDisplayOffscreen->resize(mainWindow.Size);

	::gpk::array_pod<byte_t>			fileBytes;
	::gpk::array_static<char, 256>		pathNameData		= {"../gpk_data"};

	::gpk::array_static<char, 256>		folderNameVox		= {"vox"};

	uint64_t							timeStart			= ::gpk::timeCurrentInMs();
	::gpk::array_pobj<::gpk::SVOXData>	voxModels									= {};
	for(uint32_t iModel = 0; iModel < ::gpk::size(fileNames); ++iModel) {
		gpk::vcc							fileName			= fileNames[iModel];
		char								pathToLoad[4096]	= {};
		sprintf_s(pathToLoad, "%s/%s/%s", pathNameData.Storage, folderNameVox.Storage, fileName.begin());
		gpk_necs(::gpk::fileToMemory(pathToLoad, fileBytes));

		::gpk::vcc							viewBytes			= fileBytes;
		gpk_necs(voxModels[voxModels.push_back({})]->Load(viewBytes));
		app.VOXModelNames.push_back(fileName);
		gpk_vox_info_printf("Loaded %s.", pathToLoad);
	}

	app.VOXModelMaps.resize(voxModels.size());
	for(uint32_t iModel = 0; iModel < voxModels.size(); ++iModel) {
		gpk_vox_info_printf("Constructing %s.", app.VOXModelNames[iModel].begin());

		const ::gpk::ptr_obj<::gpk::SVOXData>		& voxFile			= voxModels[iModel];
		::gpk::SVoxelMap<uint8_t>					& chunkMap			= app.VOXModelMaps[iModel];
		auto										coord				= voxFile->GetDimensions();
		chunkMap.Palette						= voxFile->GetBGRA();
		chunkMap.Dimensions						= voxFile->GetDimensions();
		::gpk::view_array<const ::gpk::SVOXVoxel>	voxels				= voxFile->GetXYZI();
		for(uint32_t iVoxel = 0; iVoxel < voxels.size(); ++iVoxel) {
			const ::gpk::SVOXVoxel						voxel				= voxels[iVoxel];
			chunkMap.SetValue({voxel.x, voxel.y, voxel.z}, voxel.i);
		}
	}

	app.VOXModels.resize(app.VOXModelMaps.size());
	for(uint32_t iModel = 0; iModel < app.VOXModelMaps.size(); ++iModel) 
		::gpk::geometryVoxelModel(app.VOXModels[iModel], app.VOXModelMaps[iModel]);

	uint64_t							timeStop			= ::gpk::timeCurrentInMs();
	warning_printf("Total time: %llu ms", timeStop - timeStart);

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
			case VK_F4:
				return 1;
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
	::gpk::sleep(1);


	return 0;
}


::gpk::error_t										drawVoxelFaceGeometry
	( ::gpk::view_grid<uint32_t>								targetDepth
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>					& Points
	, const ::gpk::SCoord3<float>								& voxelPos
	, const ::gpk::SMatrix4<float>								& mWVP
	, const ::gpk::SNearFar										& nearFar
	, const ::gpk::view_array<const ::gpk::SCoord3<float>>		verticesRaw
	, const ::gpk::view_array<const uint8_t>					& indices
	) {
	::gpk::SCoord3<float>									vertices [4]				= {}; 
	for(uint32_t iVertex = 0; iVertex < 4; ++iVertex) {
		vertices[iVertex] = mWVP.Transform(verticesRaw[iVertex] + voxelPos); 
	}
	::gpk::drawTriangleIndexed<float, uint8_t>(targetDepth, nearFar, 0, 0, vertices, indices, Points); 
	::gpk::drawTriangleIndexed<float, uint8_t>(targetDepth, nearFar, 3, 0, vertices, indices, Points); 
	return 0;
}

struct SFragmentCache {
	::gpk::array_pod<::gpk::SCoord2<int16_t>>			Points				[6]	= {};
	::gpk::array_pod<::gpk::STriangle<double>>			TriangleWeights		[6]	= {};	
	::gpk::view_grid<::gpk::SColorBGRA>					TargetPixels;
	::gpk::view_grid<uint32_t>							TargetDepth	;
};

static	::gpk::error_t						drawVoxelFace
	( uint32_t						iFace
	, const ::gpk::SCoord3<float>	& voxelPos
	, const ::gpk::SCoord3<float>	& voxelCenter
	, const ::gpk::SColorFloat		& cellColor
	, const ::gpk::SMatrix4<float>	& mVP
	, const ::gpk::SNearFar			& nearFar
	, const ::gpk::SCoord3<float>	& lightPosition
	, const double					lightFactorDistance
	, const ::gpk::SColorFloat		& colorAmbient
	, ::SFragmentCache				& pixelCache
	) {
	pixelCache.Points			[iFace].clear();
	pixelCache.TriangleWeights	[iFace].clear();

	::gpk::view_array<const ::gpk::SCoord3<float>>	rawVertices				= {&::gpk::VOXEL_FACE_VERTICES[iFace].A, 4};
	::gpk::view_array<const uint8_t>				rawIndices				= ::gpk::VOXEL_FACE_INDICES[iFace];

	::drawVoxelFaceGeometry(pixelCache.TargetDepth, pixelCache.Points[iFace], voxelPos, mVP, nearFar, rawVertices, rawIndices); 

	::gpk::SColorFloat							faceColor	[6]				= {};
	faceColor[iFace]						= colorAmbient * lightFactorDistance;
	if(lightFactorDistance > 0) {
		const double							lightFactorDirectional		= ::gpk::max(0.0, (lightPosition - voxelCenter).Normalize().Dot(::gpk::VOXEL_FACE_NORMALS[iFace]));	
		::gpk::SColorFloat						colorDiffuse				= cellColor * lightFactorDirectional;
		(faceColor[iFace] += colorDiffuse * lightFactorDistance).Clamp(); 
	}
	for(uint32_t iPoint = 0; iPoint < pixelCache.Points[iFace].size(); ++iPoint) {
		::gpk::SCoord2<int16_t>					point					= pixelCache.Points[iFace][iPoint];
		::gpk::SColorFloat						finalColor				= faceColor[iFace];
		pixelCache.TargetPixels[pixelCache.TargetPixels.metrics().y - 1 - point.y][point.x]	= finalColor;
	}

	return 0;
}

::gpk::error_t										drawVoxelModel						
	( const ::gpk::SVoxelGeometry						& voxelGeometry
	, const ::gpk::SCoord3<float>						& position
	, const ::gpk::SMatrix4<float>						& mVP
	, const ::gpk::SNearFar								& nearFar
	, const ::gpk::SCoord3<float>						& cameraPos
	, const ::gpk::SCoord3<float>						& cameraFront
	, const ::gpk::SCoord3<float>						& lightPosition
	, ::SFragmentCache									& pixelCache
	) {	
	for(uint32_t iFace = 0; iFace < 6; ++iFace) {
		::gpk::view_array<const ::gpk::SGeometryGroup>		faceSlices				= {(const ::gpk::SGeometryGroup*)voxelGeometry.GeometrySlices[iFace].begin(), voxelGeometry.GeometrySlices[iFace].size()};
		for(uint32_t iSlice = 0, countSlices = faceSlices.size(); iSlice < countSlices; ++iSlice) {
			// Clear out output
			::gpk::array_pod<::gpk::SCoord2<int16_t>>			& facePixelCoords		= pixelCache.Points[iFace];
			::gpk::array_pod<::gpk::STriangle<double>>			& faceTriangleWeights	= pixelCache.TriangleWeights[iFace];
			::gpk::array_pod<::gpk::STriangle3<float>>			trianglePositions		= {};
			::gpk::array_pod<::gpk::SRange<uint32_t>>			triangleSlices			= {};
			::gpk::array_pod<uint32_t>							triangleIndices			= {};
			facePixelCoords		.clear();
			faceTriangleWeights	.clear();

			// Raster triangles
			const ::gpk::SGeometryGroup							& faceSlice				= faceSlices[iSlice];
			const ::gpk::SRenderMaterial						& sliceMaterial			= voxelGeometry.Materials[faceSlice.Material];
			for(uint32_t iIndex = 0, countIndices = faceSlice.Slice.Count; iIndex < countIndices; ++iIndex) {
				uint32_t											offsetPositionIndex		= voxelGeometry.Geometry.PositionIndices[iIndex];
				const ::gpk::STriangle3<float>						triangle				= 
					{ mVP.Transform((voxelGeometry.Geometry.Positions[voxelGeometry.Geometry.PositionIndices[offsetPositionIndex + 0]] + position))
					, mVP.Transform((voxelGeometry.Geometry.Positions[voxelGeometry.Geometry.PositionIndices[offsetPositionIndex + 1]] + position))
					, mVP.Transform((voxelGeometry.Geometry.Positions[voxelGeometry.Geometry.PositionIndices[offsetPositionIndex + 2]] + position))
					};
				trianglePositions.push_back(triangle);
				::gpk::SRange<uint32_t>								triangleSlice			= {facePixelCoords.size()};
				::gpk::drawTriangle<float>(pixelCache.TargetDepth, nearFar, triangle, facePixelCoords, faceTriangleWeights); 
				triangleSlice.Count								= facePixelCoords.size() - triangleSlice.Offset;
				if(triangleSlice.Count)
					triangleSlices.push_back(triangleSlice);
			}

			const double							lightFactorAmbient			= .075;
			const ::gpk::SColorFloat				colorAmbient				= sliceMaterial.Diffuse * lightFactorAmbient;
			
			for(uint32_t iTriangle = 0; iTriangle < triangleSlices.size(); ++iTriangle) {
				const ::gpk::SRange<uint32_t>			slice						= triangleSlices[iTriangle];
				// Process pixel fragments
				for(uint32_t iPoint = slice.Offset, pixelCount = slice.Count + slice.Offset; iPoint < pixelCount; ++iPoint) {
					const ::gpk::STriangle3<float>			& triangle					= trianglePositions[iTriangle];
					const ::gpk::STriangle<double>			& triangleWeights			= faceTriangleWeights[iPoint];
					const ::gpk::SCoord2<int16_t>			pixelPosIn2DSpace			= facePixelCoords[iPoint];
					const ::gpk::SCoord3<float>				pixelPosIn3DSpace			
						= triangle.A * triangleWeights.A
						+ triangle.B * triangleWeights.B
						+ triangle.C * triangleWeights.C
						;

					::gpk::SCoord3<float>					lightDistance				= lightPosition - pixelPosIn3DSpace;
					const double							lightFactorDistance			= ::gpk::clamp(1.0 - lightDistance.Length() * .001, 0.0, 1.0);
					::gpk::SColorFloat						faceColor					= colorAmbient * lightFactorDistance;
					if(lightFactorDistance > 0 && lightFactorDistance < 0) {
						const double							lightFactorDirectional		= ::gpk::max(0.0, lightDistance.Normalize().Dot(::gpk::VOXEL_FACE_NORMALS[iFace]));	
						::gpk::SColorFloat						colorDiffuse				= sliceMaterial.Diffuse * lightFactorDirectional;
						(faceColor += colorDiffuse * lightFactorDistance).Clamp(); 
					}

					::gpk::SCoord2<int16_t>					point					= pixelCache.Points[iFace][iPoint];
					::gpk::SColorFloat						finalColor				= faceColor;
					pixelCache.TargetPixels[pixelCache.TargetPixels.metrics().y - 1 - point.y][point.x]	= finalColor;
				}
			}
		}
	}
	(void)mVP			;
	(void)voxelGeometry	;
	(void)nearFar		;
	(void)position		;
	(void)cameraPos		;
	(void)cameraFront	;
	(void)lightPosition	;
	(void)pixelCache	;


	return 0;
}
::gpk::error_t										drawVoxelModel						
	( const ::gpk::SVoxelMap<uint8_t>					& voxelMap	
	, const ::gpk::SCoord3<float>						& position
	, const ::gpk::SMatrix4<float>						& mVP
	, const ::gpk::SNearFar								& nearFar
	, const ::gpk::SCoord3<float>						& cameraPos
	, const ::gpk::SCoord3<float>						& cameraFront
	, const ::gpk::SCoord3<float>						& lightPosition
	, ::SFragmentCache									& pixelCache
	) {	
	const ::gpk::view_array<const ::gpk::SVoxel<uint8_t>>	voxels						= voxelMap.Voxels;
	::gpk::view_array<const ::gpk::SColorBGRA>				rgba						= voxelMap.Palette;
	const ::gpk::SCoord3<uint8_t>							dimensions					= voxelMap.Dimensions;
	if(0 == rgba.size())
		rgba												= ::gpk::VOX_PALETTE_DEFAULT;

	::gpk::SCoord3<float>									vertices		[8]			= {};
	bool													culled						= true;
	for(uint32_t i = 0; i < 8; ++i) {
		vertices[i]											= ::gpk::VOXEL_VERTICES[i];
		vertices[i].Scale(dimensions.Cast<float>());
		vertices[i].x										+= cameraPos.x;
		vertices[i].z										+= cameraPos.z;
		double cameraDot = (vertices[i] - cameraPos).Normalize().Dot(cameraFront);
		if(cameraDot > 0.01) {
			culled = false;
			break;
		}
	}

	if(culled)
		return 0;

	for(uint32_t iVoxel = 0; iVoxel < voxels.size(); ++iVoxel) {
		const ::gpk::SVoxel<uint8_t>			voxel						= voxels[iVoxel];
		const ::gpk::SCoord3<float>				voxelPos					= position + voxel.Position.Cast<float>();
		uint8_t									cellValue					= voxel.ColorIndex;
		uint8_t									cellValues	[6]				= {};
		//voxelMap.GetValue({voxel.Position.x, voxel.Position.y, voxel.Position.z}, cellValue);
		for(uint32_t iFace = 0; iFace < 6; ++iFace) {
			if(false == voxel.Limits(::gpk::VOXEL_FACE(iFace))) 
				voxelMap.GetValue((voxel.Position.Cast<int32_t>() + ::gpk::VOXEL_DELTAS[iFace].Cast<int32_t>()).Cast<uint8_t>(), cellValues[iFace]);
		}

		if(0 == cellValue)
			continue;

		if((voxelPos - cameraPos).Normalize().Dot(cameraFront) <= 0.5)
			continue;

		const ::gpk::SColorFloat				cellColor					= {rgba[voxel.ColorIndex ? voxel.ColorIndex - 1 : 0]};
		bool									renderFaces	[6]				= {};
		bool									hasFace						= false;
		for(uint32_t iFace = 0; iFace < 6; ++iFace) {
			::gpk::SColorBGRA						color						= rgba[cellValues[iFace] ? cellValues[iFace] - 1 : 0];
			if(0 == cellValues[iFace] || color.a < 0xFF)
				hasFace = (renderFaces[iFace] = cameraFront.Dot(::gpk::VOXEL_FACE_NORMALS[iFace]) <= 0.35) || hasFace;
		}

		if(false == hasFace)
			continue;

		::gpk::SCoord3<float>					voxelCenter					= voxelPos + ::gpk::SCoord3<float>{.5f, .5f, .5f};
		const double							lightFactorDistance			= ::gpk::clamp(1.0 - (lightPosition - voxelCenter).Length() * .001, 0.0, 1.0);
		const double							lightFactorAmbient			= .075;
		const ::gpk::SColorFloat				colorAmbient				= cellColor * lightFactorAmbient;

		for(uint32_t iFace = 0; iFace < 6; ++iFace) {
			if(false == renderFaces[iFace]) 
				continue;

			drawVoxelFace(iFace, voxelPos, voxelCenter, cellColor, mVP, nearFar, lightPosition, lightFactorDistance, colorAmbient, pixelCache);
		}
	}
	return 0;
}

struct SCamera {
	::gpk::SCoord3<float>								Position, Target;
};

					::gpk::error_t					draw								(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework										& framework							= app.Framework;

	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	backBuffer							= app.BackBuffer;
	if(backBuffer.get_ref() && backBuffer.get_ref()->References > 2)
		return 0;

	backBuffer->resize(framework.MainDisplay.Size);
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
	::SCamera												camera										= {{350, 100, 0}, {25, 0, 25}};
	//camera.Position *= 2.0f;
	::gpk::SCoord3<float>									lightPos									= {150, 50, 0};
	static float											cameraRotation								= 0;
	cameraRotation										+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	camera.Position	.RotateY(frameInfo.Seconds.Total * 0.1f);
	camera.Position.y *= (float)fabs(sin(frameInfo.Seconds.Total * .1f));
	//camera.Position	.RotateX(frameInfo.Microseconds.Total / 10000000.0f);
	lightPos		.RotateY(frameInfo.Seconds.Total * 1.f);

	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	const ::gpk::SCoord2<uint32_t>							& offscreenMetrics							= backBuffer->metrics();
	projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far );
	projection											= viewMatrix * projection;
	lightPos.x += 100;
	lightPos.y *= (float)fabs(sin(frameInfo.Seconds.Total * .1f));

	const ::gpk::SCoord2<uint16_t>							screenCenter				= {(uint16_t)(offscreenMetrics.x / 2), (uint16_t)(offscreenMetrics.y / 2)};
	::gpk::SMatrix4<float>									viewport									= {};
	viewport._11										= 2.0f / offscreenMetrics.x;
	viewport._22										= 2.0f / offscreenMetrics.y;
	viewport._33										= 1.0f / (float)(nearFar.Far - nearFar.Near);
	viewport._41										= -1.0f;
	viewport._42										= -1.0f;
	viewport._43										= (float)(-nearFar.Near * ( 1.0f / (nearFar.Far - nearFar.Near) ));
	viewport._44										= 1.0f;
	projection											= projection * viewport.GetInverse();

	::gpk::SCoord3<float>									cameraFront					= (camera.Target - camera.Position).Normalize();

	int32_t													zOffset						= 0;
	int32_t													xOffset						= 0;

	::SFragmentCache										pixelCache;
	pixelCache.TargetPixels								= backBuffer->Color;
	pixelCache.TargetDepth								= backBuffer->DepthStencil;
	bool													drawFromSource				= false;
	for(uint32_t iModel = 0; iModel < app.VOXModelMaps.size(); ++iModel) {
		const ::gpk::SVoxelMap<uint8_t>							& voxelMap					= app.VOXModelMaps[iModel];
		const ::gpk::SCoord3<float>								position					= {(float)xOffset, 0.0f, (float)zOffset};
		if(drawFromSource) 
			::drawVoxelModel(voxelMap, position, projection, nearFar, camera.Position, cameraFront, lightPos, pixelCache);
		else {
			const ::gpk::SVoxelGeometry								& voxelModel					= app.VOXModels[iModel];
			::drawVoxelModel(voxelModel, position, projection, nearFar, camera.Position, cameraFront, lightPos, pixelCache);
		}

		if(zOffset < 300) 
			zOffset												+= voxelMap.Dimensions.z + 4;
		else {
			xOffset												+= ::gpk::max(uint16_t(voxelMap.Dimensions.x), uint16_t(30));
			zOffset												= 0;
		}
	}

	//::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	::std::swap(framework.MainDisplayOffscreen, app.BackBuffer);
	//------------------------------------------------
	return 0;
}

