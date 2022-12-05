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

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "The One");

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
	//app.EntitySphere				= app.Engine.CreateSphere	();


	::gpk::SColorFloat					ballColors[16]	= 
		{ ::gpk::WHITE
		, ::gpk::LIGHTYELLOW
		, ::gpk::LIGHTBLUE
		, ::gpk::LIGHTRED
		, ::gpk::PURPLE * 1.25
		, ::gpk::LIGHTORANGE
		, ::gpk::LIGHTGREEN
		, ::gpk::LIGHTRED
		, ::gpk::BLACK
		, ::gpk::DARKYELLOW
		, ::gpk::DARKBLUE
		, ::gpk::DARKRED
		, ::gpk::PURPLE * .5
		, ::gpk::DARKORANGE
		, ::gpk::DARKGREEN
		, ::gpk::DARKRED
		};

	srand((int)time(0));

	::gpk::array_pod<uint32_t>			ballPool		= {};
	uint32_t							ball1			= 1 + rand() % 7;
	uint32_t							ball5			= 9 + rand() % 7;
	for(uint32_t iBall = 0; ballPool.size() < 12; ++iBall) {
		if(iBall == 8)
			continue;
		if(iBall == 0)
			continue;
		if(iBall == ball1)
			continue;
		if(iBall == ball5)
			continue;
		ballPool.push_back(iBall);
	}
	uint32_t							ballOrder[]		= 
		{ 0
		, ball1
		, ballPool[0]
		, ballPool[ballPool.size() - 1]
		, ballPool[1]
		, ball5
		, ballPool[ballPool.size() - 2]
		, ballPool[2]
		, ballPool[ballPool.size() - 3]
		, ballPool[3]
		, ballPool[ballPool.size() - 4]
		, 8
		, ballPool[4]
		, ballPool[ballPool.size() - 5]
		, ballPool[5]
		, ballPool[ballPool.size() - 6]
		};

	for(uint32_t iBall = 0; iBall < 16; ++iBall) {
		app.Balls[iBall]				= app.Engine.CreateSphere();
		app.Engine.SetDampingLinear(app.Balls[iBall], .90f);
		const ::gpk::SVirtualEntity			& entity		= app.Engine.ManagedEntities.Entities[app.Balls[iBall]];
		const ::gpk::SRenderNode			& renderNode	= app.Engine.Scene->ManagedRenderNodes.RenderNodes[entity.RenderNode];
		const ::gpk::SRenderMesh			& mesh			= *app.Engine.Scene->ManagedMeshes.Meshes[renderNode.Mesh];
		const ::gpk::SGeometrySlice			& slice			= mesh.GeometrySlices[renderNode.Slice];
		::gpk::SSkin						& skin			= *app.Engine.Scene->ManagedMeshes.Skins[slice.Skin];
		::gpk::SRenderMaterial				& material		= skin.Material;
		material.Color.Specular = material.Color.Diffuse = ballColors[ballOrder[iBall]];
  		material.Color.Ambient = material.Color.Diffuse *.1f;
	}

	app.Engine.SetPosition(app.Balls[0], {-5, 0.5f, 0});
	::gpk::SCoord3<float>	velocity{20.f, 0, 0};
	velocity.RotateY((rand() % RAND_MAX) / (65535.0f * 3)* ((rand() % 2) ? -1 : 1));
	app.Engine.SetVelocity(app.Balls[0], velocity);
	uint8_t rowLen = 5;
	::gpk::SCoord3<float>	diagonal = {1, 0, 1};
	//diagonal.Normalize();
	for(uint32_t iRow = 0, iBall = 1; iRow < 5; ++iRow, --rowLen) {
		::gpk::SCoord3<float>					offsetZ			= {0, 0, -(rowLen / 2.0f) + .5f};
		for(uint32_t iColumn = 0; iColumn < rowLen; ++iColumn) {
			::gpk::SCoord3<float> position = offsetZ + ::gpk::SCoord3<float>{(+5.f + diagonal.x * 5) - iRow * diagonal.x, 0.5f, iColumn * 1.f};
			uint32_t iEntity = app.Balls[iBall++];
			app.Engine.SetPosition(iEntity, position);
		}
	}
	//app.EntityCamera					= app.Engine.CreateCamera	();
	//app.EntityLightDirectional		= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Directional	);
	//app.EntityLightPoint				= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Point		);
	//app.EntityLightSpot				= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Spot			);
	//app.EntityBox					= app.Engine.CreateBox		();

	if(-1 != app.EntityCamera				)	app.Engine.SetPosition(app.EntityCamera				, {0, 0, 0});
	if(-1 != app.EntityLightDirectional		)	app.Engine.SetPosition(app.EntityLightDirectional	, {0, 0, 0});
	if(-1 != app.EntityLightPoint			)	app.Engine.SetPosition(app.EntityLightPoint			, {0, 0, 0});
	if(-1 != app.EntityLightSpot			)	app.Engine.SetPosition(app.EntityLightSpot			, {0, 0, 0});
	if(-1 != app.EntityBox					)	app.Engine.SetPosition(app.EntityBox				, {0, 0.5f, 0});
	if(-1 != app.EntitySphere				)	app.Engine.SetPosition(app.EntitySphere				, {0, 0.5f, 2});

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
		transformedNormals.A = worldTransform.TransformDirection(transformedNormals.A); transformedNormals.A.Normalize();
		transformedNormals.B = worldTransform.TransformDirection(transformedNormals.B); transformedNormals.B.Normalize();
		transformedNormals.C = worldTransform.TransformDirection(transformedNormals.C); transformedNormals.C.Normalize();
		(void)cameraFront;
		double																		directionFactorA							= transformedNormals.A.Dot(cameraFront);
		double																		directionFactorB							= transformedNormals.B.Dot(cameraFront);
		double																		directionFactorC							= transformedNormals.C.Dot(cameraFront);
		if(directionFactorA > .1 && directionFactorB > .1 && directionFactorC > .1)
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
	//::gpk::array_pod<::gpk::SCoord2<int16_t>>									& wireframePixelCoords		= cacheVS.WireframePixelCoords	;
	const ::gpk::SCoord2<uint16_t>												offscreenMetrics			= backBuffer->Color.View.metrics().Cast<uint16_t>();
	for(uint32_t iTriangle = 0; iTriangle < outVS.Positions.size(); ++iTriangle) {
		::gpk::STriangle3<float>													& triNormals								= outVS.Normals		[iTriangle];
		::gpk::STriangle3<float>													& triPositions								= outVS.Positions	[iTriangle];



		trianglePixelCoords.clear();
		triangleWeights.clear();
		gerror_if(errored(::gpk::drawTriangle(backBuffer->DepthStencil.View, nearFar, triPositions, trianglePixelCoords, triangleWeights)), "Not sure if these functions could ever fail");
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord) {
			const ::gpk::STriangle<float>						& vertexWeights	= triangleWeights[iCoord];
			double												lightFactor		= (triNormals.A * vertexWeights.A + triNormals.B * vertexWeights.B + triNormals.C * vertexWeights.C).Dot(lightPos);
			const ::gpk::SColorBGRA								color			= (material.Color.Ambient + material.Color.Diffuse * lightFactor).Clamp();
			const ::gpk::SCoord2<int16_t>						coord			= trianglePixelCoords[iCoord];
			backBuffer->Color.View[coord.y][coord.x]		= color;
		}

		//wireframePixelCoords.clear();
		//::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.A.Cast<int32_t>(), triPositions.B.Cast<int32_t>()}, wireframePixelCoords);
		//::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.B.Cast<int32_t>(), triPositions.C.Cast<int32_t>()}, wireframePixelCoords);
		//::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.C.Cast<int32_t>(), triPositions.A.Cast<int32_t>()}, wireframePixelCoords);
		//const ::gpk::SColorBGRA wireColor = ::gpk::ASCII_PALETTE[iTriangle % ::gpk::size(::gpk::ASCII_PALETTE)];
		//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
		//	::gpk::SCoord2<int16_t>								coord		= wireframePixelCoords[iCoord];
		//	backBuffer->Color.View[coord.y][coord.x]		= wireColor;
		//}
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
	::SCamera																	camera										= {{-30, 25, 0}, {}};
	::gpk::SCoord3<float>														lightPos									= {10, 5, 0};
	static float																cameraRotation								= 0;
	cameraRotation															+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	//camera.Position	.RotateY(frameInfo.Seconds.Total * .25f);
	lightPos		.RotateY(-frameInfo.Seconds.Total);
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
	projection											= projection * viewport.GetInverse();

	::gpk::SCoord3<float>								cameraFront				= (camera.Target - camera.Position).Normalize();

	{
		::gpk::STimer										timer;
		app.Engine.Update(frameInfo.Seconds.LastFrame);
		for(uint32_t iBall = 0; iBall < 16; ++iBall) {
			::gpk::SCoord3<float>			& positionA			= app.Engine.Integrator.Centers[app.Engine.ManagedEntities.Entities[app.Balls[iBall]].RigidBody].Position;
			::gpk::SCoord3<float>			& velocityA			= app.Engine.Integrator.Forces[app.Engine.ManagedEntities.Entities[app.Balls[iBall]].RigidBody].Velocity;
			for(uint32_t iBall2 = iBall + 1; iBall2 < 16; ++iBall2) {
				const ::gpk::SCoord3<float>		& positionB			= app.Engine.Integrator.Centers[app.Engine.ManagedEntities.Entities[app.Balls[iBall2]].RigidBody].Position;
				::gpk::SCoord3<float>			distance			= positionB - positionA;
				::gpk::SCoord3<float>			distanceDirection	= distance;
				distanceDirection.Normalize();
				if(distance.LengthSquared() < 1) {
					::gpk::SCoord3<float>			& velocityB			= app.Engine.Integrator.Forces[app.Engine.ManagedEntities.Entities[app.Balls[iBall2]].RigidBody].Velocity;
					const ::gpk::SCoord3<float>		prevA				= velocityA;
					const ::gpk::SCoord3<float>		prevB				= velocityB;
					velocityB					= {};
					velocityA					= {};
					if(prevA.LengthSquared()) {
						::gpk::SCoord3<float>			directionA			= prevA;	
						directionA.Normalize();
						float							factorA				= ::gpk::max(0.0f, (float)distanceDirection.Dot(directionA));
						velocityB					+= distanceDirection * prevA.Length() * factorA;//::gpk::interpolate_linear(directionA, distanceDirection, factorA) * prevA.Length() * factorA;
						velocityA					+= distanceDirection.Cross({0, 1, 0}).Normalize() * prevA.Length() * (1.0f - factorA);
					}
					if(prevB.LengthSquared()) {
						distanceDirection			*= -1;
						::gpk::SCoord3<float>			directionB			= prevB;	
						directionB.Normalize();
						float							factorB				= ::gpk::max(0.0f, (float)distanceDirection.Dot(directionB));
						velocityA					+= distanceDirection * prevB.Length() * factorB;//::gpk::interpolate_linear(directionA, distanceDirection, factorA) * prevA.Length() * factorA;
						velocityB					+= distanceDirection.Cross({0, 1, 0}).Normalize() * prevB.Length() * (1.0f - factorB);
					}
				}
			}
			if(positionA.x	< -20) {
				positionA.x = -20;
				velocityA.x *= -1;
				velocityA *= .9f;
			}
			if(positionA.x	> 20) {
				positionA.x = 20;
				velocityA.x *= -1;
				velocityA *= .9f;
			}
			if(positionA.z	< -10) {
				positionA.z = -10;
				velocityA.z *= -1;
				velocityA *= .9f;
			}
			if(positionA.z	> 10) {
				positionA.z = 10;
				velocityA.z *= -1;
				velocityA *= .9f;
			}

		}


		timer.Frame();
		always_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}
	//{
	//	::gpk::STimer	timer;
	//	//::drawIndexed(app, backBuffer, projection, nearFar, worldTransform, cameraFront, lightPos);
	//	timer.Frame();
	//	always_printf("Render indexed in %f seconds", timer.LastTimeSeconds);
	//}
	{
		::gpk::STimer	timer;
		::drawScene(app, backBuffer, projection, nearFar, cameraFront, lightPos);
		timer.Frame();
		always_printf("Render scene in %f seconds", timer.LastTimeSeconds);
	}


	//memcpy(framework.MainDisplayOffscreen->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	//framework.MainDisplayOffscreen = backBuffer;
	//------------------------------------------------
	return 0;
}

