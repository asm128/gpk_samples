#include "gpk_the_one.h"
#include "gpk_bitmap_target.h"
#include "gpk_timer.h"

static	::gpk::error_t							transformTriangles						
	( ::gpk::SVSOutput									& output
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
	( ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>		& backBuffer
	, ::gpk::SVSOutput										& inVS
	, ::gpk::SVSCache										& cacheVS
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
	inVS												= {};
	::transformTriangles(inVS, indices, positions, normals, projection, worldTransform, cameraFront);
	::gpk::array_pod<::gpk::STriangle<float>>									& triangleWeights			= cacheVS.TriangleWeights		;
	::gpk::array_pod<::gpk::SCoord2<int16_t>>									& trianglePixelCoords		= cacheVS.SolidPixelCoords		;
	//::gpk::array_pod<::gpk::SCoord2<int16_t>>									& wireframePixelCoords		= cacheVS.WireframePixelCoords	;
	const ::gpk::SCoord2<uint16_t>												offscreenMetrics			= backBuffer.Color.View.metrics().Cast<uint16_t>();
	for(uint32_t iTriangle = 0; iTriangle < inVS.Positions.size(); ++iTriangle) {
		::gpk::STriangle3<float>													& triNormals								= inVS.Normals		[iTriangle];
		::gpk::STriangle3<float>													& triPositions								= inVS.Positions	[iTriangle];



		trianglePixelCoords.clear();
		triangleWeights.clear();
		gerror_if(errored(::gpk::drawTriangle(backBuffer.DepthStencil.View, nearFar, triPositions, trianglePixelCoords, triangleWeights)), "Not sure if these functions could ever fail");
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord) {
			const ::gpk::STriangle<float>						& vertexWeights	= triangleWeights[iCoord];
			double												lightFactor		= (triNormals.A * vertexWeights.A + triNormals.B * vertexWeights.B + triNormals.C * vertexWeights.C).Dot(lightPos);
			const ::gpk::SColorBGRA								color			= (material.Color.Ambient + material.Color.Diffuse * lightFactor).Clamp();
			const ::gpk::SCoord2<int16_t>						coord			= trianglePixelCoords[iCoord];
			backBuffer.Color.View[coord.y][coord.x]		= color;
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

::gpk::error_t										drawScene									
	( ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>					& backBuffer
	, ::gpk::SEngineRenderCache											& renderCache
	, ::gpk::SEngineScene												& scene
	, const ::gpk::SMatrix4<float>										& projection		
	, const ::gpk::SNearFar												& nearFar 
	, const ::gpk::SCoord3<float>										& cameraFront
	, const ::gpk::SCoord3<float>										& lightPos
	) {	//
	for(uint32_t iRenderNode = 0, countNodes = scene.ManagedRenderNodes.RenderNodes.size(); iRenderNode < countNodes; ++iRenderNode) {
		::gpk::SRenderNode										& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
		if((uint32_t)renderNode.Mesh >= scene.ManagedMeshes.Meshes.size())
			continue;

		::gpk::SRenderMesh										& mesh					= *scene.ManagedMeshes.Meshes[renderNode.Mesh];
		::gpk::vcc												& meshName				= scene.ManagedMeshes.MeshNames[renderNode.Mesh];
		::gpk::SGeometrySlice									& slice					= mesh.GeometrySlices[renderNode.Slice];

		info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());

		const ::gpk::SMatrix4<float>							& worldTransform		= scene.ManagedRenderNodes.RenderNodeTransforms[iRenderNode];
		::gpk::view_array<const uint16_t>						indices					= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}					: ::gpk::view_array<const uint16_t>					{};
		::gpk::view_array<const ::gpk::SCoord3<float>>			positions				= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
		::gpk::view_array<const ::gpk::SCoord3<float>>			normals					= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
		::gpk::view_array<const ::gpk::SCoord2<float>>			uv						= (mesh.GeometryBuffers.size() > 3) ? ::gpk::view_array<const ::gpk::SCoord2<float>>	{(const ::gpk::SCoord2<float>	*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[3]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[3]]->Data.size() / sizeof(const ::gpk::SCoord2<float>)}	: ::gpk::view_array<const ::gpk::SCoord2<float>>	{};
		const ::gpk::SRenderMaterial							& material				= scene.ManagedMeshes.Skins[slice.Skin]->Material;
		drawBuffers(backBuffer, renderCache.OutputVertexShader, renderCache.CacheVertexShader, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, uv, material, projection, nearFar, worldTransform, cameraFront, lightPos);
		
	}
	return 0;
}

struct SCamera {
	::gpk::SCoord3<float>						Position, Target;
};

bool										the1::revert				(const ::gpk::SCoord3<float> & distanceDirection, const ::gpk::SCoord3<float> & initialVelocity)	{
	(void)distanceDirection;
	//return false;
	return (distanceDirection.z > 0) || initialVelocity.x < 0;
	//return (distanceDirection.z > 0) || (distanceDirection.x < 0);
}

::gpk::error_t								the1::poolGameDraw			(::the1::SPoolGame & pool, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds, uint64_t frameNumber)	{
	::gpk::SMatrix4<float>							projection				= {};
	::gpk::SMatrix4<float>							viewMatrix				= {};
	::gpk::SMatrix4<float>							worldTransform			= {};
	worldTransform.Identity();
	worldTransform.SetTranslation({-.5f, -.5f, -.5f}, false);
	projection.Identity();
	const ::gpk::SCoord3<float>						tilt					= {10, };	// ? cam't remember what is this. Radians? Eulers?
	const ::gpk::SCoord3<float>						rotation				= {0, frameNumber / 100.0f, 0};

	::gpk::SNearFar									nearFar					= {0.01f , 1000.0f};

	static constexpr const ::gpk::SCoord3<float>	cameraUp				= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
	::SCamera										camera					= {{-30, 25, 0}, {}};
	::gpk::SCoord3<float>							lightPos				= {10, 5, 0};
	//static float									cameraRotation			= 0;
	//cameraRotation								+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	//camera.Position	.RotateY(frameInfo.Seconds.Total * .25f);
	lightPos		.RotateY(-totalSeconds);
	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	const ::gpk::SCoord2<uint16_t>					offscreenMetrics		= backBuffer.Color.View.metrics().Cast<uint16_t>();
	projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far );
	projection									= viewMatrix * projection;
	lightPos.Normalize();

	::gpk::SMatrix4<float>							viewport				= {};
	viewport._11								= 2.0f / offscreenMetrics.x;
	viewport._22								= 2.0f / offscreenMetrics.y;
	viewport._33								= 1.0f / (float)(nearFar.Far - nearFar.Near);
	viewport._41								= -1.0f;
	viewport._42								= -1.0f;
	viewport._43								= (float)(-nearFar.Near * ( 1.0f / (nearFar.Far - nearFar.Near) ));
	viewport._44								= 1.0f;
	projection									= projection * viewport.GetInverse();

	::gpk::SCoord3<float>							cameraFront				= (camera.Target - camera.Position).Normalize();

	::gpk::STimer									timer;
	::gpk::SEngine									& engine				= pool.Engine;

	::gpk::array_pod<::gpk::SCoord2<int16_t>>		& wireframePixelCoords	= engine.Scene->RenderCache.CacheVertexShader.WireframePixelCoords;
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		for(uint32_t iDelta = ::gpk::max(0, (int32_t)pool.PositionDeltas[iBall].size() - 20); iDelta < pool.PositionDeltas[iBall].size(); ++iDelta) {
			::gpk::SLine3<float>							screenDelta				= pool.PositionDeltas[iBall][iDelta];
			screenDelta.A								= projection.Transform(screenDelta.A);
			screenDelta.B								= projection.Transform(screenDelta.B);
			wireframePixelCoords.clear();
			::gpk::drawLine(offscreenMetrics, screenDelta, wireframePixelCoords);
			for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
				::gpk::SCoord2<int16_t>							coord					= wireframePixelCoords[iCoord];
				backBuffer.Color.View[coord.y][coord.x]		= pool.StartState.BallColors[pool.StartState.BallOrder[iBall]];
			}
			//::gpk::SCoord3<float>							velocityA				= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
			//velocityA									= screenDelta.A + velocityA * 2;
			//screemPelocityA								= projection.Transform(screemPelocityA);

		}
	}
			//	const ::gpk::SCoord3<float>						& positionA				= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
	//	::gpk::SCoord3<float>							velocityA				= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
	//	for(uint32_t iBall2 = iBall + 1; iBall2 < pool.StartState.BallCount; ++iBall2) {
	//		::gpk::SCoord3<float>							& positionB				= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Position;
	//		::gpk::SCoord3<float>							distance				= positionB - positionA;
	//		::gpk::SCoord3<float>							distanceDirection		= distance;
	//		distanceDirection.Normalize();

	//		if(iBall == 0 && iBall2 == 15) {
	//			bool											revert					= ::the1::revert(distanceDirection, velocityA);
	//			::gpk::SCoord3<float>							perpendicular			= positionA + distanceDirection.Cross({0, revert ? -1 : 1.0f, 0}).Normalize() * 2;
	//			perpendicular								= projection.Transform(perpendicular);

	//			wireframePixelCoords.clear();
	//			::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{screemPositionA, perpendicular}, wireframePixelCoords);
	//			for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
	//				::gpk::SCoord2<int16_t>							coord					= wireframePixelCoords[iCoord];
	//				backBuffer.Color.View[coord.y][coord.x]		= ::gpk::GREEN;
	//			}
	//		}
	//	}
	//}

	::drawScene(backBuffer, engine.Scene->RenderCache, *engine.Scene, projection, nearFar, cameraFront, lightPos);
	timer.Frame();
	always_printf("Render scene in %f seconds", timer.LastTimeSeconds);
	return 0;
}
