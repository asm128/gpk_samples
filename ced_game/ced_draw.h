#include "ced_geometry.h"

#include "gpk_matrix.h"
#include "gpk_color.h"
#include "gpk_view_grid.h"

#include <algorithm>

#ifndef CED_DRAW_H_29837429837
#define CED_DRAW_H_29837429837

namespace ced
{

	int								setPixel			(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SCoord2		<int32_t>	position	, ::gpk::SColorBGRA color);
	int								drawRectangle		(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SRectangle2D	<int32_t>	rectangle	, ::gpk::SColorBGRA color);
	int								drawCircle			(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SCircle2D		<int32_t>	circle		, ::gpk::SColorBGRA color);
	int								drawLine			(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SLine2D		<int32_t>	line		, ::gpk::SColorBGRA color);
	int								drawLine			(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SLine2D		<int32_t>	line		, ::gpk::array_pod<::gpk::SCoord2<int32_t>> & pixelCoords);
	int								drawLine			(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::SLine3D		<int32_t>	line		, ::gpk::array_pod<::gpk::SCoord2<int32_t>> & pixelCoords, ::gpk::view_grid<uint32_t> & depthBuffer);
	int								drawTriangle		(::gpk::view_grid<::gpk::SColorBGRA> pixels	, ::gpk::STriangle2D	<int32_t>	triangle	, ::gpk::SColorBGRA color);
	int								drawTriangle		(::gpk::SCoord2<uint32_t> targetSize	, ::gpk::STriangle3D<float>		triangle	, ::gpk::array_pod<::gpk::SCoord2<int32_t>> & pixelCoords, ::gpk::array_pod<::gpk::STriangleWeights<double>> & proportions, ::gpk::view_grid<uint32_t> & depthBuffer);
	int								drawQuadTriangle
		( ::gpk::view_grid<::gpk::SColorBGRA>					targetPixels
		, ::ced::SGeometryQuads								& geometry
		, int												iTriangle
		, ::gpk::SMatrix4<float>							& matrixTransform
		, ::gpk::SMatrix4<float>							& matrixView
		, ::gpk::SMatrix4<float>							& matrixViewport
		, ::gpk::SCoord3<float>								& lightVector
		, ::gpk::SColorBGRA										color
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<uint32_t>						& depthBuffer
		);
		int								drawQuadTriangle
		( ::gpk::view_grid<::gpk::SColorBGRA>					targetPixels
		, ::ced::SGeometryQuads								& geometry
		, int												iTriangle
		, ::gpk::SMatrix4<float>							& matrixTransform
		, ::gpk::SMatrix4<float>							& matrixView
		, ::gpk::SMatrix4<float>							& matrixViewport
		, ::gpk::SCoord3<float>								& lightVector
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<::gpk::SColorBGRA>					textureImage
		, ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPoints
		, ::gpk::array_pod<::gpk::SColorBGRA>					& lightColors
		, ::gpk::view_grid<uint32_t>						& depthBuffer
		);
	int								drawTriangle
		( ::gpk::view_grid<::gpk::SColorBGRA>					targetPixels
		, ::ced::SGeometryTriangles							& geometry
		, int												iTriangle
		, ::gpk::SMatrix4<float>							& matrixTransform
		, ::gpk::SMatrix4<float>							& matrixView
		, ::gpk::SMatrix4<float>							& matrixViewport
		, ::gpk::SCoord3<float>								& lightVector
		, ::gpk::SColorBGRA										color
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<uint32_t>						& depthBuffer
		);

	int								drawTriangle
		( ::gpk::view_grid<::gpk::SColorBGRA>					targetPixels
		, ::ced::SGeometryTriangles							& geometry
		, int												iTriangle
		, ::gpk::SMatrix4<float>							& matrixTransform
		, ::gpk::SMatrix4<float>							& matrixView
		, ::gpk::SMatrix4<float>							& matrixViewport
		, ::gpk::SCoord3<float>								& lightVector
		, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
		, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
		, ::gpk::view_grid<::gpk::SColorBGRA>					textureImage
		, ::gpk::view_grid<uint32_t>						& depthBuffer
		);
} // namespace

#endif // CED_DRAW_H_29837429837
