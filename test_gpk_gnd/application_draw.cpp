#include "gpk_bitmap_target.h"
#include "gpk_ro_rsw.h"
#include "gpk_dialog_controls.h"
#include "gpk_scene.h"
#include "gpk_ro_gnd.h"
namespace gpk
{
	template<typename _tCoord>
	static					::gpk::error_t									drawTriangle
		( ::gpk::view_grid<uint32_t>						& targetDepth
		, const ::gpk::SNearFar								& fNearFar
		, const ::gpk::STriangle3<_tCoord>					& triangle
		, ::gpk::array_pod<::gpk::n2<int16_t>>			& out_Points
		, ::gpk::array_pod<::gpk::STriangle<float>>			& triangleWeights
		) {
		int32_t																		pixelsDrawn									= 0;
		const ::gpk::n2<uint32_t>												& _targetMetrics							= targetDepth.metrics();
		::gpk::n2	<float>														areaMin										= {(float)::gpk::min(::gpk::min(triangle.A.x, triangle.B.x), triangle.C.x), (float)::gpk::min(::gpk::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::gpk::n2	<float>														areaMax										= {(float)::gpk::max(::gpk::max(triangle.A.x, triangle.B.x), triangle.C.x), (float)::gpk::max(::gpk::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		const float																	xStop										= ::gpk::min(areaMax.x, (float)_targetMetrics.x);
		for(float y = ::gpk::max(areaMin.y, 0.f), yStop = ::gpk::min(areaMax.y, (float)_targetMetrics.y); y < yStop; ++y)
		for(float x = ::gpk::max(areaMin.x, 0.f); x < xStop; ++x) {
			const ::gpk::n2<int32_t>												cellCurrent									= {(int32_t)x, (int32_t)y};
			const ::gpk::STriangle2<int32_t>											triangle2D									=
				{ {(int32_t)triangle.A.x, (int32_t)triangle.A.y}
				, {(int32_t)triangle.B.x, (int32_t)triangle.B.y}
				, {(int32_t)triangle.C.x, (int32_t)triangle.C.y}
				};
			{
				int32_t																		w0											= ::gpk::orient2d({triangle2D.B, triangle2D.A}, cellCurrent);	// Determine barycentric coordinates
				int32_t																		w1											= ::gpk::orient2d({triangle2D.C, triangle2D.B}, cellCurrent);
				int32_t																		w2											= ::gpk::orient2d({triangle2D.A, triangle2D.C}, cellCurrent);
				if(w0 <= -1 || w1 <= -1 || w2 <= -1) // ---- If p is on or inside all edges, render pixel.
					continue;
			}
			const ::gpk::n2<float>											cellCurrentF								= {x, y};
			::gpk::STriangle<float>												proportions									=
				{ ::gpk::orient2d3d({triangle.C, triangle.B}, cellCurrentF)	// notice how having to type "template" every time before "Cast" totally defeats the purpose of the template. I really find this rule very stupid and there is no situation in which the compiler is unable to resolve it from the code it already has.
				, ::gpk::orient2d3d({triangle.A, triangle.C}, cellCurrentF)
				, ::gpk::orient2d3d({triangle.B, triangle.A}, cellCurrentF)	// Determine barycentric coordinates
				};
			float																proportABC									= proportions.A + proportions.B + proportions.C; //(w0, w1, w2)
			if(proportABC <= 0)
				continue;
			proportions.A															/= proportABC;
			proportions.B															/= proportABC;
			proportions.C															= 1.0f - (proportions.A + proportions.B);
			float																finalZ
				= triangle.A.z * proportions.A
				+ triangle.B.z * proportions.B
				+ triangle.C.z * proportions.C
				;
			float																depth										= float((finalZ - fNearFar.Min) / (fNearFar.Max - fNearFar.Min));
			if(depth >= 1 || depth <= 0) // discard from depth planes
				continue;
			uint32_t															finalDepth									= (uint32_t)(depth * 0x00FFFFFFU);
			if (targetDepth[(uint32_t)y][(uint32_t)x] > (uint32_t)finalDepth) { // check against depth buffer
				targetDepth[(uint32_t)y][(uint32_t)x]							= finalDepth;
				triangleWeights.push_back({proportions.A, proportions.B, proportions.C});
				out_Points.push_back(cellCurrent.Cast<int16_t>());
				++pixelsDrawn;
			}
		}
		return pixelsDrawn;
	}
}
					::gpk::error_t										drawPixelGND
	( ::gpk::SRenderCache									& renderCache
	, ::gpk::SColorBGRA										& targetColorCell
	, const ::gpk::STriangle<float>						& pixelWeights
	, const ::gpk::STriangle3<float>						& positions
	, const ::gpk::STriangle2<float>						& uvs
	, const ::gpk::view_grid<::gpk::SColorBGRA>				& textureColors
	, int32_t												iTriangle
	, const ::gpk::n3<double>							& lightDir
	, const ::gpk::SColorFloat								& diffuseColor
	, const ::gpk::SColorFloat								& ambientColor
	, const ::gpk::view_array<const ::gpk::SLightInfoRSW>	& lights
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::gpk::STriangle3<float>												& normals									= renderCache.TransformedNormalsVertex[iTriangle];
	::gpk::STriangle3<double>													weightedNormals								=
		{ normals.A.Cast<double>() * pixelWeights.A
		, normals.B.Cast<double>() * pixelWeights.B
		, normals.C.Cast<double>() * pixelWeights.C
		};
	const ::gpk::n3<double>												interpolatedNormal							= (weightedNormals.A + weightedNormals.B + weightedNormals.C).Normalize();
	::gpk::SColorFloat															directionalColor							= diffuseColor * interpolatedNormal.Dot(lightDir);
	const ::gpk::n2<uint32_t>												textureMetrics								= textureColors.metrics();
	const ::gpk::n2<double>												uv											=
		{ uvs.A.x * pixelWeights.A + uvs.B.x * pixelWeights.B + uvs.C.x * pixelWeights.C
		, uvs.A.y * pixelWeights.A + uvs.B.y * pixelWeights.B + uvs.C.y * pixelWeights.C
		};
	const ::gpk::STriangle3<double>												weightedPositions							=
		{ positions.A.Cast<double>() * pixelWeights.A
		, positions.B.Cast<double>() * pixelWeights.B
		, positions.C.Cast<double>() * pixelWeights.C
		};

	const ::gpk::n3<double>												interpolatedPosition						= weightedPositions.A + weightedPositions.B + weightedPositions.C;
	::gpk::SColorBGRA															interpolatedBGRA;
	::gpk::SColorFloat															lightColor									= {0, 0, 0, 1}; //((::gpk::RED * pixelWeights.A) + (::gpk::GREEN * pixelWeights.B) + (::gpk::BLUE * pixelWeights.C));
	if( 0 == textureMetrics.x
	 ||	0 == textureMetrics.y
	 ) {
		for(uint32_t iLight = 0; iLight < lights.size(); ++iLight) {
			const ::gpk::SLightInfoRSW													& rswLight									= lights[iLight];
			::gpk::n3<float>														rswColor									= rswLight.Color * (1.0 - (rswLight.Position.Cast<double>() - interpolatedPosition).Length() / 10.0);
			lightColor																+= ::gpk::SColorFloat(rswColor.x, rswColor.y, rswColor.z, 1.0f) / 2.0;
		}
		interpolatedBGRA														= (directionalColor + lightColor + (ambientColor / 2.0)).Clamp();
	}
	else {
		const ::gpk::n2<int32_t>												uvcoords									=
			{ (int32_t)((uint32_t)(uv.x * textureMetrics.x) % textureMetrics.x)
			, (int32_t)((uint32_t)(uv.y * textureMetrics.y) % textureMetrics.y)
			};
		const ::gpk::SColorFloat													& srcTexel									= textureColors[uvcoords.y][uvcoords.x];
		if (srcTexel == ::gpk::SColorFloat{ ::gpk::SColorBGRA{0xFF, 0, 0xFF, 0xFF} })
			return 1;
		for(uint32_t iLight = 0, lightCount = lights.size(); iLight < lightCount; ++iLight) {
			const ::gpk::SLightInfoRSW													& rswLight									= lights[rand() % lights.size()];
			double																		distFactor									= 1.0 - (rswLight.Position.Cast<double>() - interpolatedPosition).Length() / 10.0;
			if(distFactor > 0) {
				::gpk::n3<float>														rswColor									= rswLight.Color * distFactor;
				lightColor																+= srcTexel * ::gpk::SColorFloat(rswColor.x, rswColor.y, rswColor.z, 1.0f) / 2.0;
			}
		}
		interpolatedBGRA														= (srcTexel * directionalColor + lightColor + srcTexel * (ambientColor / 2.0)).Clamp();
	}
	if( targetColorCell == interpolatedBGRA )
		return 1;

	targetColorCell															= interpolatedBGRA;
	return 0;
}

static				::gpk::error_t										transformTriangles
	( const ::gpk::view_array<::gpk::STriangle<uint32_t>>	& vertexIndexList
	, const ::gpk::view_array<::gpk::n3<float>>				& vertices
	, const ::gpk::SNearFar									& nearFar
	, const ::gpk::SMatrix4<float>									& xWorld
	, const ::gpk::SMatrix4<float>									& xWV
	, const ::gpk::SMatrix4<float>									& xProjection
	, const ::gpk::n2<int32_t>									& targetMetrics
	, ::gpk::SRenderCache											& out_transformed
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(uint32_t iTriangle = 0, triCount = vertexIndexList.size(); iTriangle < triCount; ++iTriangle) {
		const ::gpk::STriangle<uint32_t>										& vertexIndices								= vertexIndexList[iTriangle];
		::gpk::STriangle3<float>													triangle3DWorld								=
			{	vertices[vertexIndices.A]
			,	vertices[vertexIndices.B]
			,	vertices[vertexIndices.C]
			};
		::gpk::STriangle3<float>													transformedTriangle3D						= triangle3DWorld;
		::gpk::transform(transformedTriangle3D, xWV);
		// Check against far and near planes
		if(transformedTriangle3D.CulledZSpecial(nearFar))
			continue;
		float																		oldzA										= transformedTriangle3D.A.z;
		float																		oldzB										= transformedTriangle3D.B.z;
		float																		oldzC										= transformedTriangle3D.C.z;
		::gpk::transform(transformedTriangle3D, xProjection);
		transformedTriangle3D.A.z												= oldzA;
		transformedTriangle3D.B.z												= oldzB;
		transformedTriangle3D.C.z												= oldzC;
		// Check against screen limits
		if(transformedTriangle3D.CulledX({0, (float)targetMetrics.x})) continue;
		if(transformedTriangle3D.CulledY({0, (float)targetMetrics.y})) continue;
		gpk_necall(out_transformed.Triangle3dToDraw		.push_back(transformedTriangle3D)	, "Out of memory?");

		::gpk::transform(triangle3DWorld, xWorld);
		gpk_necall(out_transformed.Triangle3dWorld		.push_back(triangle3DWorld)			, "Out of memory?");
		gpk_necall(out_transformed.Triangle3dIndices	.push_back(iTriangle)				, "Out of memory?");
	}
	return 0;
}

static				::gpk::error_t										transformNormals
	( const ::gpk::view_array<::gpk::STriangle<uint32_t>>	& vertexIndexList
	, const ::gpk::view_array<::gpk::n3<float>>				& normals
	, const ::gpk::SMatrix4<float>									& xWorld
	, ::gpk::SRenderCache											& renderCache
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		for(uint32_t iTriangle = 0, triCount = renderCache.Triangle3dIndices.size(); iTriangle < triCount; ++iTriangle) { // transform normals
			const ::gpk::STriangle<uint32_t>										& vertexIndices								= vertexIndexList[renderCache.Triangle3dIndices[iTriangle]];
			::gpk::STriangle3<float>													triangle3DWorldNormals						=
				{ normals[vertexIndices.A]
				, normals[vertexIndices.B]
				, normals[vertexIndices.C]
				};
			::gpk::STriangle3<float>													& vertNormalsTri							= renderCache.TransformedNormalsVertex[iTriangle];
			vertNormalsTri.A														= xWorld.TransformDirection(normals[vertexIndices.A]).Normalize(); // gndNode.Normals[vertexIndices.A]; //
			vertNormalsTri.B														= xWorld.TransformDirection(normals[vertexIndices.B]).Normalize(); // gndNode.Normals[vertexIndices.B]; //
			vertNormalsTri.C														= xWorld.TransformDirection(normals[vertexIndices.C]).Normalize(); // gndNode.Normals[vertexIndices.C]; //
			//vertNormalsTri.A.y *= -1;
			//vertNormalsTri.B.y *= -1;
			//vertNormalsTri.C.y *= -1;
		}

	return 0;
}

static				::gpk::error_t										drawTriangles
	( const ::gpk::view_array<::gpk::STriangle<uint32_t>>			& vertexIndexList
	, const ::gpk::view_array<::gpk::n3<float>>				& vertices
	, const ::gpk::view_array<::gpk::n2<float>>				& uvs
	, const ::gpk::view_grid	<::gpk::SColorBGRA>					& textureView
	, const ::gpk::SNearFar											& nearFar
	, const ::gpk::n3<float>									& lightDir
	, ::gpk::SRenderCache											& renderCache
	, ::gpk::view_grid<uint32_t>									& targetDepthView
	, ::gpk::view_grid<::gpk::SColorBGRA>							& targetView
	, const ::gpk::SColorFloat										& diffuseColor
	, const ::gpk::SColorFloat										& ambientColor
	, const ::gpk::view_array<const ::gpk::SLightInfoRSW>			& lights
	, uint32_t														* pixelsDrawn
	, uint32_t														* pixelsSkipped
	, bool															wireframe
	) {	// ---
		//const ::gpk::n3<float>													& lightDir									= app.LightDirection;
		for(uint32_t iTriangle = 0, triCount = renderCache.Triangle3dIndices.size(); iTriangle < triCount; ++iTriangle) { //
			renderCache.TrianglePixelCoords.clear();
			renderCache.TrianglePixelWeights.clear();
			const ::gpk::STriangle3<float>												& tri3DToDraw								= renderCache.Triangle3dToDraw[iTriangle];
			gerror_if(errored(::gpk::drawTriangle(targetDepthView, nearFar, tri3DToDraw, renderCache.TrianglePixelCoords, renderCache.TrianglePixelWeights)), "Not sure if these functions could ever fail");
			++renderCache.TrianglesDrawn;
			const ::gpk::STriangle<uint32_t>										& vertexIndices								= vertexIndexList[renderCache.Triangle3dIndices[iTriangle]];
			const ::gpk::STriangle3<float>												triangle3DPositions							=
				{ vertices[vertexIndices.A]
				, vertices[vertexIndices.B]
				, vertices[vertexIndices.C]
				};
			const ::gpk::STriangle2<float>												triangle3DUVs								=
				{ uvs[vertexIndices.A]
				, uvs[vertexIndices.B]
				, uvs[vertexIndices.C]
				};

			for(uint32_t iPixel = 0, pixCount = renderCache.TrianglePixelCoords.size(); iPixel < pixCount; ++iPixel) {
				const ::gpk::n2<int16_t>												& pixelCoord								= renderCache.TrianglePixelCoords	[iPixel];
				const ::gpk::STriangle<float>												& pixelWeights								= renderCache.TrianglePixelWeights	[iPixel];
				if(false == wireframe) {
					if(0 == ::drawPixelGND(renderCache, targetView[pixelCoord.y][pixelCoord.x], pixelWeights, triangle3DPositions, triangle3DUVs, textureView, iTriangle, lightDir.Cast<double>(), diffuseColor, ambientColor, ::gpk::view_array<const ::gpk::SLightInfoRSW>{lights.begin(), ::gpk::min(lights.size(), 8U)}))
						++*pixelsDrawn;
					else
						++*pixelsSkipped;
				}
			}
			if(wireframe) {
				gerror_if(errored(::gpk::drawLine(targetView.metrics().Cast<uint16_t>(), ::gpk::SLine3<float>{renderCache.Triangle3dToDraw[iTriangle].A, renderCache.Triangle3dToDraw[iTriangle].B}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
				gerror_if(errored(::gpk::drawLine(targetView.metrics().Cast<uint16_t>(), ::gpk::SLine3<float>{renderCache.Triangle3dToDraw[iTriangle].B, renderCache.Triangle3dToDraw[iTriangle].C}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
				gerror_if(errored(::gpk::drawLine(targetView.metrics().Cast<uint16_t>(), ::gpk::SLine3<float>{renderCache.Triangle3dToDraw[iTriangle].C, renderCache.Triangle3dToDraw[iTriangle].A}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
			}
		}
	return 0;
}

					::gpk::error_t										updateTransforms							(::gpk::SSceneTransforms & transforms, ::gpk::SSceneCamera& camera, ::gpk::n2<uint32_t> targetMetrics)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SMatrix4<float>														& finalProjection							= transforms.FinalProjection	;
	::gpk::SMatrix4<float>														& fieldOfView								= transforms.FieldOfView		;
	::gpk::SMatrix4<float>														& mviewport									= transforms.Viewport			;
	fieldOfView.FieldOfView(camera.Angle * ::gpk::math_pi, targetMetrics.x / (double)targetMetrics.y, camera.NearFar);
	mviewport.ViewportRH(targetMetrics.Cast<uint16_t>());
	const ::gpk::n2<int32_t>												screenCenter								= {(int32_t)targetMetrics.x / 2, (int32_t)targetMetrics.y / 2};
	finalProjection															= fieldOfView * mviewport;
	transforms.FinalProjectionInverse										= finalProjection.GetInverse();

	::gpk::SMatrix4<float>														& viewMatrix								= transforms.View;
	::gpk::SCameraVectors														& cameraVectors								= camera.Vectors;
	cameraVectors.Up														= {0, 1, 0};
	cameraVectors.Front														= (camera.Points.Target - camera.Points.Position).Normalize();
	cameraVectors.Right														= cameraVectors.Up		.Cross(cameraVectors.Front).Normalize();
	cameraVectors.Up														= cameraVectors.Front	.Cross(cameraVectors.Right).Normalize();
	viewMatrix.View3D(camera.Points.Position, cameraVectors.Right, cameraVectors.Up, cameraVectors.Front);
	//viewMatrix.LookAt(camera.Points.Position, {(app.GNDData.Metrics.Size.x / 2.0f), 0, -(app.GNDData.Metrics.Size.y / 2.0f)}, {0, 1, 0});
	return 0;
}

					::gpk::error_t										drawGND
	( ::gpk::SRenderCache												& renderCache
	, ::gpk::SSceneTransforms											& transforms
	, ::gpk::SSceneCamera												& camera
	, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>					& target
	, const ::gpk::SModelPivot<float>									& modelPivot
	, const ::gpk::n3<float>										& lightDir
	, const ::gpk::SModelGND											& modelGND
	, const ::gpk::SRSWWorldLight										& directionalLight
	, const ::gpk::view_array<const ::gpk::SImage<::gpk::SColorBGRA>>	& textures
	, const ::gpk::view_array<const ::gpk::SLightInfoRSW>				& lights
	, bool																wireframe
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::view_grid<::gpk::SColorBGRA>											& offscreen									= target.Color.View;
	const ::gpk::n2<uint32_t>												& offscreenMetrics							= offscreen.metrics();
	::updateTransforms(transforms, camera, offscreenMetrics);

	const ::gpk::SMatrix4<float>												& projection								= transforms.FinalProjection	;
	const ::gpk::SMatrix4<float>												& viewMatrix								= transforms.View				;

	::gpk::SMatrix4<float>														xRotation									= {};
	xRotation.Identity();
	::gpk::SMatrix4<float>														xWorld										= {};
	xWorld.Identity();
	const ::gpk::SNearFar														& nearFar									= camera.NearFar;
	uint32_t																	& pixelsDrawn								= renderCache.PixelsDrawn	= 0;
	uint32_t																	& pixelsSkipped								= renderCache.PixelsSkipped	= 0;
	renderCache.WireframePixelCoords.clear();
	renderCache.TrianglesDrawn												= 0;
	const ::gpk::SColorFloat													ambient										= {directionalLight.Ambient.x, directionalLight.Ambient.y, directionalLight.Ambient.z, 1};
	const ::gpk::SColorFloat													diffuse										= {directionalLight.Diffuse.x, directionalLight.Diffuse.y, directionalLight.Diffuse.z, 1};
	xWorld		.Scale			(modelPivot.Scale, true);
	xRotation	.Identity();
	xRotation	.SetOrientation	(modelPivot.Orientation);
	xWorld																	= xWorld * xRotation;
	xWorld		.SetTranslation	(modelPivot.Position, false);
	for(uint32_t iGNDTexture = 0; iGNDTexture < textures.size(); ++iGNDTexture) {
		for(uint32_t iFacingDirection = 0; iFacingDirection < 6; ++iFacingDirection) {
			const ::gpk::view_grid<::gpk::SColorBGRA>									& gndNodeTexture							= textures[iGNDTexture].View;
			const ::gpk::SModelNodeGND													& gndNode									= modelGND.Nodes[textures.size() * iFacingDirection + iGNDTexture];
			::gpk::clear
				( renderCache.Triangle3dWorld
				, renderCache.Triangle3dToDraw
				, renderCache.Triangle3dIndices
				);
			const ::gpk::SMatrix4<float>												xWV											= xWorld * viewMatrix;
			transformTriangles	(gndNode.VertexIndices, gndNode.Vertices, nearFar, xWorld, xWV, projection, offscreenMetrics.Cast<int32_t>(), renderCache);
			gpk_necall(renderCache.TransformedNormalsVertex.resize(renderCache.Triangle3dIndices.size()), "Out of memory?");
			transformNormals	(gndNode.VertexIndices, gndNode.Normals, xWorld, renderCache);
			drawTriangles		(gndNode.VertexIndices, gndNode.Vertices, gndNode.UVs, gndNodeTexture, nearFar, lightDir, renderCache, target.DepthStencil.View, offscreen, diffuse, ambient, lights, &pixelsDrawn, &pixelsSkipped, wireframe);
		}
	}

	static	const ::gpk::SColorBGRA												color										= ::gpk::YELLOW;
	for(uint32_t iPixel = 0, pixCount = renderCache.WireframePixelCoords.size(); iPixel < pixCount; ++iPixel) {
		const ::gpk::n2<int16_t>												& pixelCoord								= renderCache.WireframePixelCoords[iPixel];
		if( offscreen[pixelCoord.y][pixelCoord.x] != color ) {
			offscreen[pixelCoord.y][pixelCoord.x]								= color;
			++pixelsDrawn;
		}
		else
			++pixelsSkipped;
	}
	return (::gpk::error_t)pixelsDrawn;
}
