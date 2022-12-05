// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "test_gpk_mesh.h"

#include "gpk_bitmap_target.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"
#include "gpk_geometry_lh.h"

#include "gpk_app_impl.h"
#include "gpk_voxel.h"

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "Title");

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& app)											{
	const ::gpk::SCoord2<uint32_t>												newSize										= app.Framework.MainDisplay.Size;
	::gpk::updateSizeDependentTarget(app.Framework.MainDisplayOffscreen->Color, newSize);
	return 0;
}

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
	mainWindow.Size															= {640, 480};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");
	app.EntitySphere				= app.Engine.CreateSphere	();
	app.EntityBox					= app.Engine.CreateBox		();

	//app.EntityCamera				= app.Engine.CreateCamera	();
	//app.EntityLightDirectional		= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Directional	);
	//app.EntityLightPoint			= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Point		);
	//app.EntityLightSpot				= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Spot			);

	if(-1 != app.EntityCamera				)	app.Engine.Integrator.SetPosition(app.Engine.ManagedEntities.Entities[app.EntityCamera				].RigidBody, {0, 0, 0});
	if(-1 != app.EntityLightDirectional		)	app.Engine.Integrator.SetPosition(app.Engine.ManagedEntities.Entities[app.EntityLightDirectional	].RigidBody, {0, 0, 0});
	if(-1 != app.EntityLightPoint			)	app.Engine.Integrator.SetPosition(app.Engine.ManagedEntities.Entities[app.EntityLightPoint			].RigidBody, {0, 0, 0});
	if(-1 != app.EntityLightSpot			)	app.Engine.Integrator.SetPosition(app.Engine.ManagedEntities.Entities[app.EntityLightSpot			].RigidBody, {0, 0, 0});
	if(-1 != app.EntityBox					)	app.Engine.Integrator.SetPosition(app.Engine.ManagedEntities.Entities[app.EntityBox					].RigidBody, {0, 0.5f, 0});
	if(-1 != app.EntitySphere				)	app.Engine.Integrator.SetPosition(app.Engine.ManagedEntities.Entities[app.EntitySphere				].RigidBody, {0, 0.5f, 2});

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
	::gpk::SWindow																& mainWindow								= app.Framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	return 0;
}

::gpk::error_t										transformTriangles						
	( ::SVSOutput										& output
	, ::gpk::view_array<const uint16_t>					indices			
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	normals		
	, const ::gpk::SMatrix4<float>						& projection		
	, const ::gpk::SMatrix4<float>						& worldTransform	
	, const ::gpk::SCoord3<float>						& cameraFront
)	{ 
	::gpk::view_array<const ::gpk::STriangle<uint16_t>>		view_indices		= {(const ::gpk::STriangle<uint16_t>*)indices.begin(), indices.size() / 3};

	const ::gpk::SMatrix4<float>							mWVP				= worldTransform * projection;
	for(uint32_t iTriangle = 0; iTriangle < view_indices.size(); ++iTriangle) {
		const ::gpk::STriangle<uint16_t>											vertexIndices								= view_indices[iTriangle];
		::gpk::STriangle3<float>													transformedNormals							= {normals[vertexIndices.A], normals[vertexIndices.B], normals[vertexIndices.C]};
		transformedNormals.A = worldTransform.TransformDirection(transformedNormals.A);
		transformedNormals.B = worldTransform.TransformDirection(transformedNormals.B);
		transformedNormals.C = worldTransform.TransformDirection(transformedNormals.C);
		double																		directionFactorA							= transformedNormals.A.Dot(cameraFront);
		double																		directionFactorB							= transformedNormals.B.Dot(cameraFront);
		double																		directionFactorC							= transformedNormals.C.Dot(cameraFront);
		if(directionFactorA > 0 && directionFactorB > 0 && directionFactorC > 0)
			continue;

		::gpk::STriangle3<float>													transformedPositions						= {positions[vertexIndices.A], positions[vertexIndices.B], positions[vertexIndices.C]};
		::gpk::transform(transformedPositions, mWVP);
		output.Positions.push_back(transformedPositions);
		output.Normals	.push_back(transformedNormals);
	}
	return 0; 
}
::gpk::error_t										transformFragments						()	{ return 0; }

::gpk::error_t										drawBuffers								
	( ::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	backBuffer
	, ::SVSOutput											& outVS
	, ::SVSCache											& cacheVS
	, ::gpk::view_array<const uint16_t>						indices		
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		normals		
	, ::gpk::view_array<const ::gpk::SCoord2<float>>		uv			
	, const ::gpk::SRenderMaterial							& material	
	, const ::gpk::SMatrix4<float>							& projection		
	, const ::gpk::SNearFar									& nearFar 
	, const ::gpk::SMatrix4<float>							& worldTransform	 
	, const ::gpk::SCoord3<float>							& cameraFront
	, const ::gpk::SCoord3<float>							& lightPos
	) {	// 
	(void)uv;
	outVS												= {};
	::transformTriangles(outVS, indices, positions, normals, projection, worldTransform, cameraFront);
	::gpk::array_pod<::gpk::STriangle<float>>									& triangleWeights			= cacheVS.TriangleWeights		;
	::gpk::array_pod<::gpk::SCoord2<int16_t>>									& trianglePixelCoords		= cacheVS.SolidPixelCoords		;
	::gpk::array_pod<::gpk::SCoord2<int16_t>>									& wireframePixelCoords		= cacheVS.WireframePixelCoords	;
	const ::gpk::SCoord2<uint16_t>												offscreenMetrics			= backBuffer->Color.View.metrics().Cast<uint16_t>();
	for(uint32_t iTriangle = 0; iTriangle < outVS.Positions.size(); ++iTriangle) {
		::gpk::STriangle3<float>													& triNormals								= outVS.Normals		[iTriangle];
		::gpk::STriangle3<float>													& triPositions								= outVS.Positions	[iTriangle];
		double																		directionFactor								= triNormals.A.Dot(cameraFront);
		if(directionFactor > 0)
			continue;

		double																		lightFactor									= triNormals.A.Dot(lightPos);
		::gpk::SColorBGRA															color										= (material.Color.Ambient + material.Color.Diffuse * lightFactor).Clamp();
		trianglePixelCoords.clear();
		triangleWeights.clear();
		gerror_if(errored(::gpk::drawTriangle(backBuffer->DepthStencil.View, nearFar, triPositions, trianglePixelCoords, triangleWeights)), "Not sure if these functions could ever fail");
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord) {
			//const ::gpk::STriangle<float>						& vertexWeights	= triangleWeights[iCoord];
			const ::gpk::SCoord2<int16_t>						coord			= trianglePixelCoords[iCoord];
			backBuffer->Color.View[coord.y][coord.x]		= color;
		}

		wireframePixelCoords.clear();
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.A.Cast<int32_t>(), triPositions.B.Cast<int32_t>()}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.B.Cast<int32_t>(), triPositions.C.Cast<int32_t>()}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.C.Cast<int32_t>(), triPositions.A.Cast<int32_t>()}, wireframePixelCoords);
		const ::gpk::SColorBGRA wireColor = ::gpk::ASCII_PALETTE[iTriangle % ::gpk::size(::gpk::ASCII_PALETTE)];
		for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
			::gpk::SCoord2<int16_t>								coord		= wireframePixelCoords[iCoord];
			backBuffer->Color.View[coord.y][coord.x]		= wireColor;
		}
	}
	return 0;
}

::gpk::error_t										drawIndexed								
	( ::SApplication					& app
	, ::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	backBuffer
	, const ::gpk::SMatrix4<float>		& projection		
	, const ::gpk::SNearFar				& nearFar 
	, const ::gpk::SMatrix4<float>		& worldTransform	 
	, const ::gpk::SCoord3<float>		& cameraFront
	, const ::gpk::SCoord3<float>		& lightPos
	) {	//
	//------------------------------------------------
	::gpk::view_array<const ::gpk::SCoord3<float>>		positions				= {&::gpk::VOXEL_FACE_VERTICES	[0].A, 24};
	::gpk::view_array<const ::gpk::SCoord3<float>>		normals					= {&::gpk::VOXEL_FACE_NORMALS	[0].A, 24};
	::gpk::view_array<const ::gpk::SCoord2<float>>		uv						= {&::gpk::VOXEL_FACE_UV		[0].A, 24};
	::gpk::view_array<const uint16_t>					indices					= {::gpk::VOXEL_FACE_INDICES_16	[0], 36};
	::gpk::SRenderMaterial								material				= {::gpk::MAGENTA * .1, ::gpk::MAGENTA, ::gpk::DARKGREEN};

	drawBuffers(backBuffer, app.OutputVertexShader, app.CacheVertexShader, indices, positions, normals, uv, material, projection, nearFar, worldTransform, cameraFront, lightPos);
	return 0;
}

::gpk::error_t										drawScene									
	( ::SApplication					& app
	, ::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	backBuffer
	, const ::gpk::SMatrix4<float>		& projection		
	, const ::gpk::SNearFar				& nearFar 
	, const ::gpk::SCoord3<float>		& cameraFront
	, const ::gpk::SCoord3<float>		& lightPos
	) {	//
	for(uint32_t iRenderNode = 0, countNodes = app.Engine.Scene->ManagedRenderNodes.RenderNodes.size(); iRenderNode < countNodes; ++iRenderNode) {
		::gpk::SRenderNode										& renderNode			= app.Engine.Scene->ManagedRenderNodes.RenderNodes[iRenderNode];
		if((uint32_t)renderNode.Mesh >= app.Engine.Scene->ManagedMeshes.Meshes.size())
			continue;

		::gpk::SRenderMesh										& mesh					= *app.Engine.Scene->ManagedMeshes.Meshes[renderNode.Mesh];
		::gpk::vcc												& meshName				= app.Engine.Scene->ManagedMeshes.MeshNames[renderNode.Mesh];
		::gpk::SGeometrySlice									& slice					= mesh.GeometrySlices[renderNode.Slice];

		info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());

		const ::gpk::SMatrix4<float>							& worldTransform		= app.Engine.Scene->ManagedRenderNodes.RenderNodeTransforms[iRenderNode];
		::gpk::view_array<const uint16_t>						indices					= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)app.Engine.Scene->ManagedBuffers.Buffers[mesh.GeometryBuffers[0]]->Data.begin(), app.Engine.Scene->ManagedBuffers.Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}					: ::gpk::view_array<const uint16_t>					{};
		::gpk::view_array<const ::gpk::SCoord3<float>>			positions				= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)app.Engine.Scene->ManagedBuffers.Buffers[mesh.GeometryBuffers[1]]->Data.begin(), app.Engine.Scene->ManagedBuffers.Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
		::gpk::view_array<const ::gpk::SCoord3<float>>			normals					= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)app.Engine.Scene->ManagedBuffers.Buffers[mesh.GeometryBuffers[2]]->Data.begin(), app.Engine.Scene->ManagedBuffers.Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
		::gpk::view_array<const ::gpk::SCoord2<float>>			uv						= (mesh.GeometryBuffers.size() > 3) ? ::gpk::view_array<const ::gpk::SCoord2<float>>	{(const ::gpk::SCoord2<float>	*)app.Engine.Scene->ManagedBuffers.Buffers[mesh.GeometryBuffers[3]]->Data.begin(), app.Engine.Scene->ManagedBuffers.Buffers[mesh.GeometryBuffers[3]]->Data.size() / sizeof(const ::gpk::SCoord2<float>)}	: ::gpk::view_array<const ::gpk::SCoord2<float>>	{};
		const ::gpk::SRenderMaterial							& material				= app.Engine.Scene->ManagedMeshes.Skins[slice.Skin]->Material;
		drawBuffers(backBuffer, app.OutputVertexShader, app.CacheVertexShader, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, uv, material, projection, nearFar, worldTransform, cameraFront, lightPos);
		
	}
	(void)app, (void)backBuffer;
	return 0;
}

struct SCamera {
	::gpk::SCoord3<float>								Position, Target;
};

					::gpk::error_t										draw										(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			backBuffer;
	backBuffer->resize(framework.MainDisplayOffscreen->Color.metrics(), 0xFF000080, (uint32_t)-1);

	::gpk::SMatrix4<float>														projection									= {};
	::gpk::SMatrix4<float>														viewMatrix									= {};
	::gpk::SMatrix4<float>														worldTransform								= {};
	worldTransform.Identity();
	worldTransform.SetTranslation({-.5f, -.5f, -.5f}, false);
	projection.Identity();
	::gpk::SFrameInfo															& frameInfo									= framework.FrameInfo;
	const ::gpk::SCoord3<float>													tilt										= {10, };	// ? cam't remember what is this. Radians? Eulers?
	const ::gpk::SCoord3<float>													rotation									= {0, (float)frameInfo.FrameNumber / 100, 0};

	::gpk::SNearFar																nearFar										= {0.01f , 1000.0f};

	static constexpr const ::gpk::SCoord3<float>								cameraUp									= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
	::SCamera																	camera										= {{10, 5, 0}, {}};
	::gpk::SCoord3<float>														lightPos									= {10, 5, 0};
	static float																cameraRotation								= 0;
	cameraRotation															+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	camera.Position	.RotateY(frameInfo.Microseconds.Total / 1000000.0f);
	lightPos		.RotateY(frameInfo.Microseconds.Total /  500000.0f * -2);
	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	const ::gpk::SCoord2<uint16_t>												offscreenMetrics							= backBuffer->Color.View.metrics().Cast<uint16_t>();
	projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far );
	projection																= viewMatrix * projection;
	lightPos.Normalize();

	::gpk::SMatrix4<float>														viewport									= {};
	viewport._11										= 2.0f / offscreenMetrics.x;
	viewport._22										= 2.0f / offscreenMetrics.y;
	viewport._33										= 1.0f / (float)(nearFar.Far - nearFar.Near);
	viewport._41										= -1.0f;
	viewport._42										= -1.0f;
	viewport._43										= (float)(-nearFar.Near * ( 1.0f / (nearFar.Far - nearFar.Near) ));
	viewport._44										= 1.0f;
	projection																= projection * viewport.GetInverse();

	::gpk::SCoord3<float>								cameraFront				= (camera.Target - camera.Position).Normalize();

	app.Engine.Update(frameInfo.Seconds.LastFrame);
	{
		::gpk::STimer	timer;
		//::drawIndexed(app, backBuffer, projection, nearFar, worldTransform, cameraFront, lightPos);
		timer.Frame();
		always_printf("Render indexed in %f seconds", timer.LastTimeSeconds);
	}
	//{
	//	::gpk::STimer	timer;
		::drawScene(app, backBuffer, projection, nearFar, cameraFront, lightPos);
	//	timer.Frame();
	//	always_printf("Render scene in %f seconds", timer.LastTimeSeconds);
	//}


	//memcpy(framework.MainDisplayOffscreen->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	//framework.MainDisplayOffscreen = backBuffer;
	//------------------------------------------------
	return 0;
}

