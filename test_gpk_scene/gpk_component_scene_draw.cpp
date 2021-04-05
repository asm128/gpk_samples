#include "gpk_component_scene.h"
#include "gpk_raster_lh.h"

::gpk::error_t							gpk::nodeRendererDrawNode
	( ::gpk::SNodeRenderer					& renderer
	, uint32_t								iNode
	, const ::gpk::SMatrix4<float>			& view
	, const ::gpk::SMatrix4<float>			& projection
	, const ::gpk::SMatrix4<float>			& viewProjection
	, ::gpk::view_grid<::gpk::SColorBGRA>	target_image
	, ::gpk::view_grid<uint32_t>			target_depth
	) {
	if(0 == target_image.size())
		return 1;
	const ::gpk::SRenderNode					& nodeToDraw			= renderer.Nodes[iNode];
	const ::gpk::SMatrix4<float>				& matrixWorld			= renderer.Transforms[nodeToDraw.Transform].Matrix;
	const ::gpk::SMatrix4<float>				& matrixWorldInverse	= renderer.Transforms[nodeToDraw.Transform].MatrixInverse;
	::gpk::array_pod<::gpk::SCoord2<int32_t>>	pixelCoords;
	::gpk::SMatrix4<float>						matrixWVP				= matrixWorld * viewProjection;
	if(nodeToDraw.Indices >= 0) { // draw indexed triangles
	}
	else { // draw ordered vertices
		::gpk::STriangle3<float>					triangleTransformed		= {};
		::gpk::STriangle2<int16_t>					triangleFinal			= {};
		::gpk::SColorBGRA							nodeColor				= (nodeToDraw.Color >= 0) ? renderer.Colors[nodeToDraw.Color] : ::gpk::SColorBGRA{0xFFFF00FF};
		if(nodeToDraw.PerFaceNormal) {
			if(nodeToDraw.VertexColor < 0) {
				::gpk::drawTriangle(target_image, triangleFinal, nodeColor);
			}
			else if(nodeToDraw.PerFaceColor) {
				::gpk::array_pod<::gpk::SCoord3<float>>		& nodeVertices			= renderer.Vertices[nodeToDraw.Vertices];
				::gpk::array_pod<::gpk::SCoord3<float>>		& nodeNormals			= renderer.Normals [nodeToDraw.Normals];
				for(uint32_t iTriangle = 0; iTriangle < nodeVertices.size() / 3; ++iTriangle) {
					::gpk::SColorBGRA							triangleColor			= (nodeToDraw.VertexColor >= 0) ? renderer.VertexColors[nodeToDraw.VertexColor][iTriangle] : nodeColor;
					;
					triangleTransformed.A = nodeVertices[iTriangle * 3 + 0];
					triangleTransformed.B = nodeVertices[iTriangle * 3 + 1];
					triangleTransformed.C = nodeVertices[iTriangle * 3 + 2];
					::gpk::transform(triangleTransformed, matrixWVP);

					::gpk::SMatrix4<float>						matrixViewport			= {};
					matrixViewport.ViewportLH(target_image.metrics());
					::gpk::transform(triangleTransformed, matrixViewport);

					triangleFinal.A		= {(int16_t)triangleTransformed.A.x, (int16_t)triangleTransformed.A.y};
					triangleFinal.B		= {(int16_t)triangleTransformed.B.x, (int16_t)triangleTransformed.B.y};
					triangleFinal.C		= {(int16_t)triangleTransformed.C.x, (int16_t)triangleTransformed.C.y};

					::gpk::drawTriangle(target_image, triangleFinal, triangleColor * nodeNormals[iTriangle].Dot(renderer.Lights[0].Direction));
				}
			}
			else {
			}
		}
		else {
			if(nodeToDraw.VertexColor < 0) {}
			else if(nodeToDraw.PerFaceColor) {}
			else {}
		}
	}
	(void)nodeToDraw		;
	(void)matrixWorld		;
	(void)matrixWorldInverse;
	(void)view				;
	(void)projection		;
	(void)target_depth		;
	return 0;
}

::gpk::error_t							gpk::nodeRendererDraw
	( ::gpk::SNodeRenderer					& renderer
	, int32_t								iCamera
	, ::gpk::view_grid<::gpk::SColorBGRA>	target_image
	, ::gpk::view_grid<uint32_t>			target_depth
	, bool									drawHidden
	) {
	if(0 == target_image.size())
		return 1;
	memset(target_depth.begin(), 0xFFFFFFFFU, sizeof(uint32_t) * target_depth.size());
	::gpk::SMatrix4<float>						matrixView				= ::gpk::SMatrix4<float>::GetIdentity();
	::gpk::SMatrix4<float>						matrixProjection		= ::gpk::SMatrix4<float>::GetIdentity();
	::gpk::SMatrix4<float>						matrixVP				= ::gpk::SMatrix4<float>::GetIdentity();
	const double								aspectRatio				= target_image.metrics().x / (double)target_image.metrics().y;
	::gpk::SCamera								& camera				= renderer.Cameras[iCamera];
	matrixView.LookAt(camera.Position, camera.Target, camera.Up);
	matrixProjection.FieldOfView(camera.Angle, aspectRatio, camera.ClipPlanes.Near, camera.ClipPlanes.Far);
	renderer.RenderCache.NodesToRender.clear();
	matrixVP								= matrixView * matrixProjection;

	for(uint32_t iNode = 0; iNode < renderer.Nodes.size(); ++iNode) {
		const ::gpk::SRenderNode					& nodeToRender			= renderer.Nodes[iNode];
		if(drawHidden || false == nodeToRender.Hidden) {
			renderer.RenderCache.NodesToRender.push_back(iNode);
			::gpk::array_pod<uint32_t>					& nodeLights			= renderer.RenderCache.NodeLights[renderer.RenderCache.NodeLights.push_back({})];
			::gpk::SCoord3<float>						nodePosition			= renderer.Transforms[nodeToRender.Transform].Matrix.GetTranslation();
			for(uint32_t iLight = 0; iLight < renderer.Lights.size(); ++iLight) {
				const ::gpk::SLight							& light					= renderer.Lights[iLight];
				if(light.Type & ::gpk::GLIGHT_STATE_ENABLED) {
					if((light.Position - nodePosition).LengthSquared() < light.RangeSquared) {
						nodeLights.push_back(iLight);
					}
				}
			}
		}
	}
	for(uint32_t iNode = 0; iNode < renderer.RenderCache.NodesToRender.size(); ++iNode) {
		const uint32_t												indexNode				= renderer.RenderCache.NodesToRender[iNode];
		e_if(::gpk::nodeRendererDrawNode(renderer, indexNode, matrixView, matrixProjection, matrixVP, target_image, target_depth), "Failed to render node %i", indexNode);
	}
	return 0;
}