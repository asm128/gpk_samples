#include "gpk_matrix.h"
#include "ced_geometry.h"
#include "gpk_color.h"

#include <algorithm>

#ifndef CED_DRAW_H_29837429837
#define CED_DRAW_H_29837429837

namespace ced
{
	struct SLight3 {
		::gpk::SCoord3<float>			Position;
		float							Range;
	};

	int								setPixel			(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SCoord2		<int32_t>	position	, ::gpk::SColorBGRA color);
	int								drawRectangle		(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SRectangle2D	<int32_t>	rectangle	, ::gpk::SColorBGRA color);
	int								drawCircle			(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SCircle2D		<int32_t>	circle		, ::gpk::SColorBGRA color);
	int								drawLine			(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SLine2			<int32_t>	line		, ::gpk::SColorBGRA color);
	int								drawLine			(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SLine2			<int32_t>	line		, ::gpk::array_pod<::gpk::SCoord2<int32_t>> & pixelCoords);
	int								drawLine
		( const ::gpk::view_grid<::gpk::SColorBGRA>		pixels
		, const ::gpk::SLine3<float>					& lineFloat
		, ::gpk::array_pod<::gpk::SCoord3<float>>		& pixelCoords
		, ::gpk::view_grid<uint32_t>					depthBuffer
		);
	int								drawTriangle		(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::STriangle2	<int32_t>	triangle	, ::gpk::SColorBGRA color);
	int								drawTriangle		(::gpk::SCoord2<uint32_t> targetSize	, ::gpk::STriangle3	<float>		triangle	, ::gpk::array_pod<::gpk::SCoord2<int32_t>> & pixelCoords, ::gpk::array_pod<::gpk::STriangleWeights<double>> & proportions, ::gpk::view_grid<uint32_t> depthBuffer);
	int								drawQuadTriangle
		( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
		, const ::ced::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::SCoord3<float>						& lightVector
		, const ::gpk::SColorBGRA							color
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<uint32_t>						depthBuffer
		);
	int								drawQuadTriangle
		( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
		, const ::ced::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::SCoord3<float>						& lightVector
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<::gpk::SColorBGRA>				textureImage
		, ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPoints
		, ::gpk::array_pod<::gpk::SColorBGRA>				& lightColors
		, ::gpk::view_grid<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
		, const ::ced::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::SCoord3<float>						& lightVector
		, ::gpk::SColorBGRA									color
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<uint32_t>						depthBuffer
		);
	int								drawQuadTriangle
		( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
		, ::gpk::STriangle3			<float>					triangle
		, ::gpk::SCoord3			<float>					normal
		, const ::gpk::STriangle2	<float>					& triangleTexCoords
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::SCoord3<float>						& lightVector
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<::gpk::SColorBGRA>				textureImage
		, ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPoints
		, ::gpk::array_pod<::gpk::SColorBGRA>				& lightColors
		, ::gpk::view_grid<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
		, const ::ced::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::SCoord3<float>						& lightVector
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<::gpk::SColorBGRA>				textureImage
		, ::gpk::view_grid<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
		, const ::ced::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::SCoord3<float>						& lightVector
		, const ::gpk::SColorFloat							& lightColor
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<::gpk::SColorBGRA>				textureImage
		, ::gpk::array_pod<::ced::SLight3>					& lightPoints
		, ::gpk::array_pod<::gpk::SColorBGRA>				& lightColors
		, ::gpk::view_grid<uint32_t>						depthBuffer
		);
} // namespace

#endif // CED_DRAW_H_29837429837
