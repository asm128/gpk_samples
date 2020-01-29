#include "gpk_array.h"
#include "gpk_view_grid.h"

#ifndef CED_GEOMETRY_H_29234234
#define CED_GEOMETRY_H_29234234

namespace ced
{
	struct SGeometryQuads {
		::gpk::array_pod<::gpk::STriangle3	<float>>		Triangles;
		::gpk::array_pod<::gpk::SCoord3		<float>>		Normals;
		::gpk::array_pod<::gpk::STriangle2<float>>			TextureCoords;
	};

	struct SGeometryTriangles {
		::gpk::array_pod<::gpk::STriangle3<float>>			Triangles;
		::gpk::array_pod<::gpk::STriangle3<float>>			Normals;
		::gpk::array_pod<::gpk::STriangle2<float>>			TextureCoords;
	};

	int													geometryBuildCube		(SGeometryQuads & geometry);
	int													geometryBuildGrid		(SGeometryQuads & geometry, ::gpk::SCoord2<uint32_t> gridSize, ::gpk::SCoord2<float> gridCenter);
	int													geometryBuildGrid		(SGeometryQuads & geometry, ::gpk::SCoord2<uint32_t> gridSize, ::gpk::SCoord2<float> gridCenter);
	int													geometryBuildFigure0	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::gpk::SCoord3<float> gridCenter);
	int													geometryBuildSphere		(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::gpk::SCoord3<float> gridCenter);

	int													geometryBuildSphere		(SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, ::gpk::SCoord3<float> gridCenter);
} // namespace

#endif // CED_GEOMETRY_H_29234234
