#include "gpk_engine_scene.h"
#include "gpk_engine_shader.h"
#include "gpk_noise.h"
#include "gpk_pool_draw.h"
#include "gpk_ascii_color.h"

static	::gpk::error_t							transformTrianglesBall					
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

	const ::gpk::SMatrix4<float>							mWVPS				= worldTransform * projection;
	for(uint32_t iTriangle = 0; iTriangle < view_indices.size(); ++iTriangle) {
		const ::gpk::STriangle<uint16_t>											vertexIndices								= view_indices[iTriangle];
		::gpk::STriangle3<float>													transformedNormals							= {normals[vertexIndices.A], normals[vertexIndices.B], normals[vertexIndices.C]};
		::gpk::transformDirection(transformedNormals, worldTransform);
		transformedNormals.A.Normalize();
		transformedNormals.B.Normalize();
		transformedNormals.C.Normalize();
		double																		directionFactorA							= transformedNormals.A.Dot(cameraFront);
		double																		directionFactorB							= transformedNormals.B.Dot(cameraFront);
		double																		directionFactorC							= transformedNormals.C.Dot(cameraFront);
		if(directionFactorA > .35 && directionFactorB > .35 && directionFactorC > .35)
			continue;

		output.Normals.push_back(transformedNormals);

		::gpk::STriangle3<float>													transformedPositions						= {positions[vertexIndices.A], positions[vertexIndices.B], positions[vertexIndices.C]};
		::gpk::transform(transformedPositions, worldTransform);
		output.PositionsWorld.push_back(transformedPositions);
		
		::gpk::transform(transformedPositions, projection);
		output.PositionsScreen.push_back(transformedPositions);
		
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

static	::gpk::error_t								drawBuffersBall							
	( ::gpk::view_grid<::gpk::SColorBGRA>					& backBufferColors
	, ::gpk::view_grid<uint32_t>							& backBufferDepth
	, ::gpk::SVSOutput										& inVS
	, ::gpk::SVSCache										& cacheVS
	, ::gpk::view_array<const uint16_t>						indices		
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		normals		
	, ::gpk::view_array<const ::gpk::SCoord2<float>>		uv			
	, const ::gpk::SRenderMaterial							& /*material*/
	, ::gpk::view_grid<const ::gpk::SColorBGRA>				surface
	, const ::gpk::SMatrix4<float>							& worldTransform	 
	, const ::gpk::SEngineSceneConstants					& constants
	) {	// 
	(void)uv;(void)surface;
	inVS												= {};
	::transformTrianglesBall(inVS, indices, positions, normals, uv, constants.Projection, worldTransform, constants.CameraFront);
	::gpk::SCoord2<uint32_t>									surfaceUnit					= surface.metrics().Cast<uint32_t>();
	::gpk::array_pod<::gpk::STriangle<float>>					& triangleWeights			= cacheVS.TriangleWeights		;
	::gpk::array_pod<::gpk::SCoord2<int16_t>>					& trianglePixelCoords		= cacheVS.SolidPixelCoords		;
	//::gpk::array_pod<::gpk::SCoord2<int16_t>>					& wireframePixelCoords		= cacheVS.WireframePixelCoords	;
	const ::gpk::SCoord2<uint16_t>								offscreenMetrics			= backBufferColors.metrics().Cast<uint16_t>();
	const ::gpk::SCoord3<float>									lightDirectionNormalized	= ::gpk::SCoord3<float>{constants.LightDirection}.Normalize();
	for(uint32_t iTriangle = 0; iTriangle < inVS.PositionsScreen.size(); ++iTriangle) {
		const ::gpk::STriangle3<float>								& triPositions				= inVS.PositionsScreen	[iTriangle];
		const ::gpk::STriangle3<float>								& triPositionsWorld			= inVS.PositionsWorld	[iTriangle];
		const ::gpk::STriangle3<float>								& triNormals				= inVS.Normals			[iTriangle];
		const ::gpk::STriangle2<float>								& triUVs					= inVS.UVs				[iTriangle];

		trianglePixelCoords.clear();
		triangleWeights.clear();
		gerror_if(errored(::gpk::drawTriangle(backBufferDepth, constants.NearFar, triPositions, trianglePixelCoords, triangleWeights)), "Not sure if these functions could ever fail");
		//const bool													stripped					= surface[0][0] == ::gpk::SColorBGRA{gpk::WHITE};
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord) {
			const ::gpk::STriangle<float>								& vertexWeights				= triangleWeights[iCoord];
			const ::gpk::SCoord3<float>									weightedPosition			= triPositionsWorld.A * vertexWeights.A + triPositionsWorld.B * vertexWeights.B + triPositionsWorld.C * vertexWeights.C;
			const ::gpk::SCoord3<float>									weightedNormal				= (triNormals.A * vertexWeights.A + triNormals.B * vertexWeights.B + triNormals.C * vertexWeights.C).Normalize();
			::gpk::SCoord2<float>										weightedUV					= triUVs.A * vertexWeights.A + triUVs.B * vertexWeights.B + triUVs.C * vertexWeights.C;
			const ::gpk::SColorBGRA										surfacecolor				= surface
				[(uint32_t)(weightedUV.y * surfaceUnit.y) % surfaceUnit.y]
				[(uint32_t)(weightedUV.x * surfaceUnit.x) % surfaceUnit.x]
				;
			::gpk::SColorFloat											materialcolor				= surfacecolor;		
			//materialcolor											+= materialcolor * ((::gpk::noiseNormal1D(iTriangle * ::gpk::primes16bit[rand() % ::gpk::size(::gpk::primes16bit)] + iCoord, trianglePixelCoords.size()) - .5f) * .05f);
			const ::gpk::SCoord3<float>									lightVecW					= (constants.LightPosition - weightedPosition).Normalize();
			const ::gpk::SColorFloat									specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, 20.f, gpk::WHITE, ::gpk::WHITE, weightedPosition, weightedNormal, lightVecW);
			const ::gpk::SColorFloat									diffuse						= ::gpk::lightCalcDiffuse(materialcolor, weightedNormal, lightVecW);
			const ::gpk::SColorFloat									ambient						= materialcolor * .1f;

			const ::gpk::SColorBGRA										color						= ::gpk::SColorFloat(ambient + diffuse + specular).Clamp();
			const ::gpk::SCoord2<uint16_t>								coord						= trianglePixelCoords[iCoord].Cast<uint16_t>();
			backBufferColors[coord.y][coord.x]						= color;
		}

		//cacheVS.WireframePixelCoords.clear();
		//::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.A.Cast<int32_t>(), triPositions.B.Cast<int32_t>()}, cacheVS.WireframePixelCoords);
		//::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.B.Cast<int32_t>(), triPositions.C.Cast<int32_t>()}, cacheVS.WireframePixelCoords);
		//::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.C.Cast<int32_t>(), triPositions.A.Cast<int32_t>()}, cacheVS.WireframePixelCoords);
		//const ::gpk::SColorBGRA wireColor = ::gpk::ASCII_PALETTE[iTriangle % ::gpk::size(::gpk::ASCII_PALETTE)];
		//for(uint32_t iCoord = 0; iCoord < cacheVS.WireframePixelCoords.size(); ++iCoord) {
		//	::gpk::SCoord2<int16_t>								coord		= cacheVS.WireframePixelCoords[iCoord];
		//	backBufferColors[coord.y][coord.x]		= wireColor;
		//}
	}
	return 0;
}

int32_t												the1::shaderBall				
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view_grid<uint32_t>			backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	if(renderNode.Mesh >= scene.ManagedMeshes.Meshes.size())
		return 0;

	const ::gpk::SRenderMesh								& mesh					= *scene.ManagedMeshes.Meshes[renderNode.Mesh];
	::gpk::vcc												meshName				= scene.ManagedMeshes.MeshNames[renderNode.Mesh];

	info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());

	const ::gpk::SRenderNodeTransforms						& transforms				= scene.ManagedRenderNodes.RenderNodeTransforms[iRenderNode];
	const ::gpk::SMatrix4<float>							& worldTransform			= transforms.World;
	const ::gpk::view_array<const uint16_t>					indices						= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}					: ::gpk::view_array<const uint16_t>					{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	positions					= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	normals						= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord2<float>>	uv							= (mesh.GeometryBuffers.size() > 3) ? ::gpk::view_array<const ::gpk::SCoord2<float>>	{(const ::gpk::SCoord2<float>	*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[3]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[3]]->Data.size() / sizeof(const ::gpk::SCoord2<float>)}	: ::gpk::view_array<const ::gpk::SCoord2<float>>	{};
	const ::gpk::SSkin										& skin						= *scene.ManagedRenderNodes.Skins[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const uint32_t											tex							= skin.Textures[0];
	const ::gpk::SSurface									& surface					= *scene.ManagedSurfaces.Surfaces[tex];

	const ::gpk::SGeometrySlice								slice						= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};

	drawBuffersBall(backBufferColors, backBufferDepth, renderCache.OutputVertexShader, renderCache.CacheVertexShader
		, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, uv, material, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}
		, worldTransform, constants
	);

	return 0;
}
static	::gpk::error_t							transformTrianglesWireframe
	( ::gpk::SVSOutput									& output
	, ::gpk::view_array<const uint16_t>					indices			
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	normals		
	, const ::gpk::SMatrix4<float>						& projection		
	, const ::gpk::SMatrix4<float>						& worldTransform	
	, const ::gpk::SCoord3<float>						& cameraFront
)	{ 
	::gpk::view_array<const ::gpk::STriangle<uint16_t>>		view_indices		= {(const ::gpk::STriangle<uint16_t>*)indices.begin(), indices.size() / 3};

	const ::gpk::SMatrix4<float>							mWVPS				= worldTransform * projection;
	for(uint32_t iTriangle = 0; iTriangle < view_indices.size(); ++iTriangle) {
		const ::gpk::STriangle<uint16_t>											vertexIndices								= view_indices[iTriangle];
		::gpk::STriangle3<float>													transformedNormals							= {normals[vertexIndices.A], normals[vertexIndices.B], normals[vertexIndices.C]};
		::gpk::transformDirection(transformedNormals, worldTransform);
		transformedNormals.A.Normalize();
		transformedNormals.B.Normalize();
		transformedNormals.C.Normalize();
		double																		directionFactorA							= transformedNormals.A.Dot(cameraFront);
		double																		directionFactorB							= transformedNormals.B.Dot(cameraFront);
		double																		directionFactorC							= transformedNormals.C.Dot(cameraFront);
		if(directionFactorA > .35 && directionFactorB > .35 && directionFactorC > .35)
			continue;

		output.Normals.push_back(transformedNormals);

		::gpk::STriangle3<float>													transformedPositions						= {positions[vertexIndices.A], positions[vertexIndices.B], positions[vertexIndices.C]};
		::gpk::transform(transformedPositions, worldTransform);
		output.PositionsWorld.push_back(transformedPositions);
		
		::gpk::transform(transformedPositions, projection);
		output.PositionsScreen.push_back(transformedPositions);
	
	}
	return 0; 
}

static	::gpk::error_t								drawBuffersWireframe
	( ::gpk::view_grid<::gpk::SColorBGRA>					& backBufferColors
	, ::gpk::view_grid<uint32_t>							& /*backBufferDepth*/
	, ::gpk::SVSOutput										& inVS
	, ::gpk::SVSCache										& cacheVS
	, ::gpk::view_array<const uint16_t>						indices		
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		normals		
	, const ::gpk::SRenderMaterial							& material
	, const ::gpk::SMatrix4<float>							& worldTransform	 
	, const ::gpk::SEngineSceneConstants					& constants
	) {	// 
	inVS												= {};
	::transformTrianglesWireframe(inVS, indices, positions, normals, constants.Projection, worldTransform, constants.CameraFront);
	const ::gpk::SCoord2<uint16_t>								offscreenMetrics			= backBufferColors.metrics().Cast<uint16_t>();
	for(uint32_t iTriangle = 0; iTriangle < inVS.PositionsScreen.size(); ++iTriangle) {
		const ::gpk::STriangle3<float>								& triPositions				= inVS.PositionsScreen	[iTriangle];
		//const ::gpk::STriangle3<float>								& triNormals				= inVS.Normals			[iTriangle];
		cacheVS.WireframePixelCoords.clear();
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.A.Cast<int32_t>(), triPositions.B.Cast<int32_t>()}, cacheVS.WireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.B.Cast<int32_t>(), triPositions.C.Cast<int32_t>()}, cacheVS.WireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.C.Cast<int32_t>(), triPositions.A.Cast<int32_t>()}, cacheVS.WireframePixelCoords);
		const ::gpk::SColorBGRA										wireColor					= material.Color.Diffuse;
		for(uint32_t iCoord = 0; iCoord < cacheVS.WireframePixelCoords.size(); ++iCoord) {
			::gpk::SCoord2<int16_t>								coord		= cacheVS.WireframePixelCoords[iCoord];
			backBufferColors[coord.y][coord.x]		= wireColor;
		}
	}
	return 0;
}

int32_t												shaderWireframe
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view_grid<uint32_t>			backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	if(renderNode.Mesh >= scene.ManagedMeshes.Meshes.size())
		return 0;

	const ::gpk::SRenderMesh								& mesh					= *scene.ManagedMeshes.Meshes[renderNode.Mesh];
	::gpk::vcc												meshName				= scene.ManagedMeshes.MeshNames[renderNode.Mesh];

	info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());

	const ::gpk::SRenderNodeTransforms						& transforms				= scene.ManagedRenderNodes.RenderNodeTransforms[iRenderNode];
	const ::gpk::SMatrix4<float>							& worldTransform			= transforms.World;
	const ::gpk::view_array<const uint16_t>					indices						= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}					: ::gpk::view_array<const uint16_t>					{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	positions					= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	normals						= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.ManagedBuffers.Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::SSkin										& skin						= *scene.ManagedRenderNodes.Skins[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const ::gpk::SGeometrySlice								slice						= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};

	return drawBuffersWireframe(backBufferColors, backBufferDepth, renderCache.OutputVertexShader, renderCache.CacheVertexShader, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, material, worldTransform, constants);
}

::gpk::error_t										gpk::drawScene									
	( ::gpk::view_grid<::gpk::SColorBGRA>	& backBufferColors
	, ::gpk::view_grid<uint32_t>			& backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
) {	//

	for(uint32_t iRenderNode = 0, countNodes = scene.ManagedRenderNodes.RenderNodes.size(); iRenderNode < countNodes; ++iRenderNode) {
		const ::gpk::SRenderNodeFlags					& renderNodeFlags		= scene.ManagedRenderNodes.RenderNodeFlags[iRenderNode];
		if(renderNodeFlags.NoDraw)
			continue; 

		const ::gpk::SRenderNode						& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
		if(renderNode.Shader >= scene.ManagedShaders.Shaders.size()) 
			::shaderWireframe(backBufferColors, backBufferDepth, renderCache, scene, constants, iRenderNode); 
			//::the1::shaderBall(backBufferColors, backBufferDepth, renderCache, scene, constants, iRenderNode); 
		else {
			const std::function<TFuncEffect>				& fx					= scene.ManagedShaders.Shaders[renderNode.Shader];
			fx(backBufferColors, backBufferDepth, renderCache, scene, constants, iRenderNode);
		}
	}

	const ::gpk::SCoord2<uint16_t>					offscreenMetrics		= backBufferColors.metrics().Cast<uint16_t>();
	::gpk::array_pod<::gpk::SCoord2<int16_t>>		& wireframePixelCoords	= renderCache.CacheVertexShader.WireframePixelCoords;

	// ---- Draw axis vectors at the origin (0, 0, 0)
	constexpr ::gpk::SCoord3<float>					xyz	[3]					= 
		{ {1}
		, {0, 1}
		, {0, 0, 1}
		};
	constexpr ::gpk::SColorBGRA						colorXYZ[3]				= 
		{ ::gpk::RED
		, ::gpk::GREEN
		, ::gpk::BLUE
		};

	for(uint32_t iVector = 0; iVector < 3; ++iVector) {
		wireframePixelCoords.clear();
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{{}, xyz[iVector]}, constants.Projection, wireframePixelCoords);
		for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
			::gpk::SCoord2<int16_t>					coord		= wireframePixelCoords[iCoord];
			backBufferColors[coord.y][coord.x]	= colorXYZ[iVector];
		}
	}
	return 0;
}
