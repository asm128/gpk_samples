#include "gpk_engine_scene.h"


static	::gpk::error_t							transformTriangles						
	( ::gpk::SVSOutput									& output
	, ::gpk::view_array<const uint16_t>					indices			
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	normals		
	, ::gpk::view_array<const ::gpk::SCoord2<float>>	uv			
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
		::gpk::STriangle2<float>													transformedUVs								= {uv[vertexIndices.A], uv[vertexIndices.B], uv[vertexIndices.C]};
		if( transformedUVs.A.x > 1.0f
		 || transformedUVs.A.y > 1.0f
		 || transformedUVs.B.x > 1.0f
		 || transformedUVs.B.y > 1.0f
		 || transformedUVs.C.x > 1.0f
		 || transformedUVs.C.y > 1.0f
		) 
			_CrtDbgBreak();
		output.UVs.push_back(transformedUVs);
	}
	return 0; 
}

static	::gpk::error_t								drawBuffers								
	( ::gpk::view_grid<::gpk::SColorBGRA>					& backBufferColors
	, ::gpk::view_grid<uint32_t>							& backBufferDepth
	, ::gpk::SVSOutput										& inVS
	, ::gpk::SVSCache										& cacheVS
	, ::gpk::view_array<const uint16_t>						indices		
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		normals		
	, ::gpk::view_array<const ::gpk::SCoord2<float>>		uv			
	, const ::gpk::SRenderMaterial							& //material	
	, ::gpk::view_grid<const ::gpk::SColorBGRA>				surface
	, const ::gpk::SMatrix4<float>							& projection		
	, const ::gpk::SNearFar									& nearFar 
	, const ::gpk::SMatrix4<float>							& worldTransform	 
	, const ::gpk::SCoord3<float>							& cameraFront
	, const ::gpk::SCoord3<float>							& lightPos
	) {	// 
	(void)uv;(void)surface;
	inVS												= {};
	::transformTriangles(inVS, indices, positions, normals, uv, projection, worldTransform, cameraFront);
	::gpk::SCoord2<uint32_t>									surfaceUnit					= surface.metrics().Cast<uint32_t>();
	::gpk::array_pod<::gpk::STriangle<float>>					& triangleWeights			= cacheVS.TriangleWeights		;
	::gpk::array_pod<::gpk::SCoord2<int16_t>>					& trianglePixelCoords		= cacheVS.SolidPixelCoords		;
	//::gpk::array_pod<::gpk::SCoord2<int16_t>>					& wireframePixelCoords		= cacheVS.WireframePixelCoords	;
	const ::gpk::SCoord2<uint16_t>								offscreenMetrics			= backBufferColors.metrics().Cast<uint16_t>();
	for(uint32_t iTriangle = 0; iTriangle < inVS.Positions.size(); ++iTriangle) {
		const ::gpk::STriangle3<float>								& triPositions				= inVS.Positions	[iTriangle];
		const ::gpk::STriangle3<float>								& triNormals				= inVS.Normals		[iTriangle];
		const ::gpk::STriangle2<float>								& triUVs					= inVS.UVs			[iTriangle];

		trianglePixelCoords.clear();
		triangleWeights.clear();
		gerror_if(errored(::gpk::drawTriangle(backBufferDepth, nearFar, triPositions, trianglePixelCoords, triangleWeights)), "Not sure if these functions could ever fail");
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord) {
			const ::gpk::STriangle<float>								& vertexWeights				= triangleWeights[iCoord];
			double														lightFactor					= (triNormals.A * vertexWeights.A + triNormals.B * vertexWeights.B + triNormals.C * vertexWeights.C).Dot(lightPos);

			::gpk::SCoord2<float>										uvcoord						= ::gpk::SCoord2<float>{(triUVs.A * vertexWeights.A) + (triUVs.B * vertexWeights.B) + (triUVs.C * vertexWeights.C)};
			::gpk::SCoord2<float>										uvcoord2					= uvcoord;
			uvcoord2.x												= float((uint32_t)(uvcoord2.x * surfaceUnit.x) % surfaceUnit.x);
			uvcoord2.y												= float((uint32_t)(uvcoord2.y * surfaceUnit.y) % surfaceUnit.y);
			const ::gpk::SColorFloat									surfacecolor				= surface[(uint16_t)uvcoord2.y][(uint16_t)uvcoord2.x];
			const ::gpk::SColorBGRA										color						= ( surfacecolor * .1f + surfacecolor * lightFactor).Clamp();
			const ::gpk::SCoord2<uint16_t>								coord						= trianglePixelCoords[iCoord].Cast<uint16_t>();
			backBufferColors[coord.y][coord.x]						= color;
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

::gpk::error_t			gpk::drawScene									
	( ::gpk::view_grid<::gpk::SColorBGRA>	& backBufferColors
	, ::gpk::view_grid<uint32_t>			& backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, ::gpk::SEngineScene					& scene
	, const ::gpk::SMatrix4<float>			& projection		
	, const ::gpk::SNearFar					& nearFar 
	, const ::gpk::SCoord3<float>			& cameraFront
	, const ::gpk::SCoord3<float>			& lightPos
) {	//

	for(uint32_t iRenderNode = 0, countNodes = scene.ManagedRenderNodes.RenderNodes.size(); iRenderNode < countNodes; ++iRenderNode) {
		const ::gpk::SRenderNodeFlags							& renderNodeFlags		= scene.ManagedRenderNodes.RenderNodeFlags[iRenderNode];
		if(renderNodeFlags.NoDraw)
			continue; 

		const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
		if(renderNode.Mesh >= scene.ManagedMeshes.Meshes.size())
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
		const ::gpk::SSkin										& skin					= *scene.ManagedRenderNodes.Skins[renderNode.Skin];
		const ::gpk::SRenderMaterial							& material				= skin.Material;
		const uint32_t											tex						= skin.Textures[0];
		const ::gpk::SSurface									& surface				= *scene.ManagedSurfaces.Surfaces[tex];
		drawBuffers(backBufferColors, backBufferDepth, renderCache.OutputVertexShader, renderCache.CacheVertexShader, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, uv, material, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}, projection, nearFar, worldTransform, cameraFront, lightPos);
		
	}

	const ::gpk::SCoord2<uint16_t>					offscreenMetrics		= backBufferColors.metrics().Cast<uint16_t>();
	::gpk::array_pod<::gpk::SCoord2<int16_t>>		& wireframePixelCoords	= renderCache.CacheVertexShader.WireframePixelCoords;

	::gpk::SCoord3<float>							xyz	[3]					= 
		{ {1}
		, {0, 1}
		, {0, 0, 1}
		};
	const ::gpk::SColorBGRA colorXYZ[3] = 
		{ ::gpk::RED
		, ::gpk::GREEN
		, ::gpk::BLUE
		};
	for(uint32_t iVector = 0; iVector < 3; ++iVector) {
		wireframePixelCoords.clear();
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{{}, xyz[iVector]}, projection, wireframePixelCoords);
		for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
			::gpk::SCoord2<int16_t>								coord		= wireframePixelCoords[iCoord];
			backBufferColors[coord.y][coord.x]		= colorXYZ[iVector];
		}
	}
	return 0;
}
