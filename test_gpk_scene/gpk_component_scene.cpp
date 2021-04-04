#include "gpk_component_scene.h"
#include "gpk_storage.h"
#include "gpk_stdstring.h"

static ::gpk::error_t					createFromSTL			(::gpk::SComponentScene & scene, ::gpk::SSTLFile & stlFile, ::gpk::view_const_string componentName)  {
	::gpk::SComponentData						newComponentGroup		= {};
	::gpk::array_obj<::gpk::SComponentData>		newComponentNodes		= {};
	newComponentGroup.IndexParent	= -1;
	::gpk::SNodeRenderer						& renderer				= scene.Renderer;
	int32_t										indexNewNode			= renderer.Nodes.push_back({});
	::gpk::SRenderNode							& newNode				= renderer.Nodes[indexNewNode];
	::gpk::SNodeRenderer::TVertexBuffer			& bufferVertices		= renderer.Vertices			[newNode.Vertices		= renderer.Vertices			.push_back({})];
	::gpk::SNodeRenderer::TNormalBuffer			& bufferNormals			= renderer.Normals			[newNode.Normals		= renderer.Normals			.push_back({})];
	::gpk::SNodeRenderer::TVertexColorBuffer	& bufferVertexColors	= renderer.VertexColors		[newNode.VertexColor	= renderer.VertexColors		.push_back({})];
	::gpk::SMaterial							& material				= renderer.Materials		[newNode.Material		= renderer.Materials		.push_back({})];
	::gpk::SRenderNodeTransform					& transform				= renderer.Transforms		[newNode.Transform		= renderer.Transforms		.push_back({})];
	::gpk::SBoundingVolume						& boundingVolume		= renderer.BoundingVolumes	[newNode.BoundingVolume	= renderer.BoundingVolumes	.push_back({})];

	transform.SetIdentity();

	material.Ambient						= ::gpk::DARKGRAY * .5	;
	material.Diffuse						= ::gpk::WHITE			;
	material.Specular						= ::gpk::GRAY			;
	material.Emissive						= ::gpk::BLACK			;
	material.SpecularFactor					= 10.0f					;

	bufferVertices		.resize(stlFile.Triangles.size() * 3);
	bufferNormals		.resize(stlFile.Triangles.size());
	bufferVertexColors	.resize(stlFile.Triangles.size());
	newNode.PerFaceColor					= 1;
	newNode.PerFaceNormal					= 1;
	for(uint32_t iTriangle = 0; iTriangle < stlFile.Triangles.size(); ++iTriangle) {
		const ::gpk::SSTLTriangle					& triangle				= stlFile.Triangles[iTriangle];
		bufferVertices		[iTriangle * 3 + 0]	= triangle.Triangle.A;
		bufferVertices		[iTriangle * 3 + 1]	= triangle.Triangle.B;
		bufferVertices		[iTriangle * 3 + 2]	= triangle.Triangle.C;
		bufferNormals		[iTriangle]			= triangle.Normal;
		bufferVertexColors	[iTriangle].FromBGR16(triangle.Attribute);
		boundingVolume.Limits.Min.x				= ::gpk::min(triangle.Triangle.A.x, ::gpk::min(triangle.Triangle.B.x, ::gpk::min(triangle.Triangle.C.x, boundingVolume.Limits.Min.x)));
		boundingVolume.Limits.Min.y				= ::gpk::min(triangle.Triangle.A.x, ::gpk::min(triangle.Triangle.B.x, ::gpk::min(triangle.Triangle.C.x, boundingVolume.Limits.Min.y)));
		boundingVolume.Limits.Min.z				= ::gpk::min(triangle.Triangle.A.x, ::gpk::min(triangle.Triangle.B.x, ::gpk::min(triangle.Triangle.C.x, boundingVolume.Limits.Min.z)));
		boundingVolume.Limits.Max.x				= ::gpk::max(triangle.Triangle.A.x, ::gpk::max(triangle.Triangle.B.x, ::gpk::max(triangle.Triangle.C.x, boundingVolume.Limits.Max.x)));
		boundingVolume.Limits.Max.y				= ::gpk::max(triangle.Triangle.A.x, ::gpk::max(triangle.Triangle.B.x, ::gpk::max(triangle.Triangle.C.x, boundingVolume.Limits.Max.y)));
		boundingVolume.Limits.Max.z				= ::gpk::max(triangle.Triangle.A.x, ::gpk::max(triangle.Triangle.B.x, ::gpk::max(triangle.Triangle.C.x, boundingVolume.Limits.Max.z)));
		boundingVolume.Center					= boundingVolume.Limits.Min + (boundingVolume.Limits.Max - boundingVolume.Limits.Min) / 2;
		boundingVolume.Radius					= ::gpk::max(boundingVolume.Limits.Min.Length(), boundingVolume.Limits.Max.Length()) / 2;
		boundingVolume.Type						= ::gpk::BOUNDINGPRIMITIVE_TYPE_SPHERE;
	}

	newComponentGroup.RenderNodes.push_back(indexNewNode);
	scene.ComponentNames.push_back(componentName);
	return scene.Components.push_back(newComponentGroup);
}

::gpk::error_t	gpk::SComponentScene::CreateFromFile		(::gpk::view_const_string filename)		{
	::gpk::view_const_char					extension				= {};
	::gpk::array_pod<char_t>				ext_lwr					= {};
	gpk_necall(filename.slice(extension, filename.size() - 4, 4), "File extension not supported for file '%s'", filename.begin());
	gpk_necall(ext_lwr.append(extension), "%s", "");
	::gpk::tolower(ext_lwr);
	static const ::gpk::vcs					str_stl					= ".stl";
	if(ext_lwr == str_stl) {
		::gpk::SSTLFile							stlFile					= {};
		gpk_necall(::gpk::stlFileLoad(filename, stlFile), "Failed to load file '%s'.", ::gpk::toString(filename).begin());
		return ::createFromSTL(*this, stlFile, filename);
	}
	else {
		error_printf("File format not supported for file '%s'", ::gpk::toString(filename).begin());
		return -1;
	}
	return 0;
}

::gpk::error_t											gpk::nodeRendererDrawNode
	( ::gpk::SNodeRenderer					& renderer
	, uint32_t								iNode
	, const ::gpk::SMatrix4<float>			& view
	, const ::gpk::SMatrix4<float>			& projection
	, ::gpk::view_grid<::gpk::SColorBGRA>	target_image
	, ::gpk::view_grid<uint32_t>			target_depth
	) {
	if(0 == target_image.size())
		return 1;
	const ::gpk::SRenderNode									& nodeToDraw			= renderer.Nodes[iNode];
	const ::gpk::SMatrix4<float>								& matrixWorld			= renderer.Transforms[nodeToDraw.Transform].Matrix;
	const ::gpk::SMatrix4<float>								& matrixWorldInverse	= renderer.Transforms[nodeToDraw.Transform].MatrixInverse;
	(void)nodeToDraw		;
	(void)matrixWorld		;
	(void)matrixWorldInverse;
	(void)view				;
	(void)projection		;
	(void)target_depth		;
	return 0;
}

::gpk::error_t											gpk::nodeRendererDraw
	( ::gpk::SNodeRenderer					& renderer
	, int32_t								iCamera
	, ::gpk::view_grid<::gpk::SColorBGRA>	target_image
	, ::gpk::view_grid<uint32_t>			target_depth
	, bool									drawHidden
	) {
	if(0 == target_image.size())
		return 1;
	memset(target_depth.begin(), 0xFFFFFFFFU, sizeof(uint32_t) * target_depth.size());
	::gpk::SMatrix4<float>										matrixView				= ::gpk::SMatrix4<float>::GetIdentity();
	::gpk::SMatrix4<float>										matrixProjection		= ::gpk::SMatrix4<float>::GetIdentity();
	const double												aspectRatio				= target_image.metrics().x / (double)target_image.metrics().y;
	::gpk::SCamera												& camera				= renderer.Cameras[iCamera];
	matrixView.LookAt(camera.Position, camera.Target, camera.Up);
	matrixProjection.FieldOfView(camera.Angle, aspectRatio, camera.ClipPlanes.Near, camera.ClipPlanes.Far);
	renderer.RenderCache.NodesToRender.clear();

	for(uint32_t iNode = 0; iNode < renderer.Nodes.size(); ++iNode) {
		const ::gpk::SRenderNode									& nodeToRender			= renderer.Nodes[iNode];
		if(drawHidden || false == nodeToRender.Hidden) {
			renderer.RenderCache.NodesToRender.push_back(iNode);
			::gpk::array_pod<uint32_t>									& nodeLights			= renderer.RenderCache.NodeLights[renderer.RenderCache.NodeLights.push_back({})];
			for(uint32_t iLight = 0; iLight < renderer.Lights.size(); ++iLight) {
				const ::gpk::SLight											& light					= renderer.Lights[iLight];
				if(light.Type & ::gpk::GLIGHT_STATE_ENABLED) {
					if((light.Position - renderer.Transforms[nodeToRender.Transform].Matrix.GetTranslation()).LengthSquared() < light.RangeSquared) {
						nodeLights.push_back(iLight);
					}
				}
			}
		}
	}
	for(uint32_t iNode = 0; iNode < renderer.RenderCache.NodesToRender.size(); ++iNode) {
		const int32_t indexNode = renderer.RenderCache.NodesToRender[iNode];
		e_if(::gpk::nodeRendererDrawNode(renderer, indexNode, matrixView, matrixProjection, target_image, target_depth), "Failed to render node ");
	}
	return 0;
}
