#include "ced_geometry.h"

// Vertex coordinates for cube faces
static constexpr const ::gpk::STriangle3D<int8_t>	geometryCube	[12]						=
	{ {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}}	// Right	- first
	, {{1, 0, 0}, {0, 1, 0}, {1, 1, 0}}	// Right	- second

	, {{0, 0, 1}, {0, 1, 0}, {0, 0, 0}}	// Back		- first
	, {{0, 0, 1}, {0, 1, 1}, {0, 1, 0}}	// Back		- second

	, {{1, 0, 0}, {0, 0, 1}, {0, 0, 0}}	// Bottom	- first
	, {{1, 0, 0}, {1, 0, 1}, {0, 0, 1}}	// Bottom	- second

	, {{1, 0, 1}, {0, 1, 1}, {0, 0, 1}}	// Left		- first
	, {{1, 0, 1}, {1, 1, 1}, {0, 1, 1}}	// Left		- second

	, {{1, 0, 1}, {1, 1, 0}, {1, 1, 1}}	// Front	- first
	, {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}}	// Front	- second

	, {{1, 1, 0}, {0, 1, 1}, {1, 1, 1}}	// Top		- first
	, {{1, 1, 0}, {0, 1, 0}, {0, 1, 1}}	// Top		- second
	};

static constexpr const ::gpk::SCoord3<int8_t>		geometryNormals	[6]		=
	{ { 0, 0, 1} // Right
	, {-1, 0, 0} // Back
	, { 0,-1, 0} // Bottom
	, { 0, 0,-1} // Left
	, { 1, 0, 0} // Front
	, { 0, 1, 0} // Top
	};

int													ced::geometryBuildCube	(SGeometryQuads & geometry)	{
	geometry.Triangles	.resize((uint32_t)::gpk::size(geometryCube));
	geometry.Normals	.resize((uint32_t)::gpk::size(geometryNormals));

	for(uint32_t iTriangle = 0; iTriangle < geometry.Triangles.size(); ++iTriangle) {
		::gpk::STriangle3D<float>								& newTriangle		= geometry.Triangles[iTriangle];
		newTriangle											= geometryCube[iTriangle].Cast<float>();
		newTriangle.A										-= {.5, .5, .5};
		newTriangle.B										-= {.5, .5, .5};
		newTriangle.C										-= {.5, .5, .5};

		::gpk::SCoord3<float>									& newNormal			= geometry.Normals[iTriangle / 2];
		newNormal											= geometryNormals[iTriangle / 2].Cast<float>();
	}
	return 0;
}

int													ced::geometryBuildGrid	(SGeometryQuads & geometry, ::gpk::SCoord2<uint32_t> gridSize, ::gpk::SCoord2<float> gridCenter)	{
	for(uint32_t z = 0; z < gridSize.y; ++z)
	for(uint32_t x = 0; x < gridSize.x; ++x)  {
		::gpk::SCoord3<double>									coords	[4]			=
			{ {0, 0, 0}
			, {0, 0, 1}
			, {1, 0, 0}
			, {1, 0, 1}
			};
		::gpk::STriangle3D<float>								triangleA			= {coords[0].Cast<float>(), coords[1].Cast<float>(), coords[2].Cast<float>()};
		::gpk::STriangle3D<float>								triangleB			= {coords[1].Cast<float>(), coords[3].Cast<float>(), coords[2].Cast<float>()};
		//::gpk::STriangle3<float>								triangleA			= {{1, 0, 0}, {0, 0, 1}, {1, 0, 1}};
		//::gpk::STriangle3<float>								triangleB			= {{1, 0, 0}, {0, 0, 0}, {0, 0, 1}};
		triangleA.A											+= {(float)x, 0, (float)z};
		triangleA.B											+= {(float)x, 0, (float)z};
		triangleA.C											+= {(float)x, 0, (float)z};
		triangleB.A											+= {(float)x, 0, (float)z};
		triangleB.B											+= {(float)x, 0, (float)z};
		triangleB.C											+= {(float)x, 0, (float)z};

		triangleA.A											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleA.B											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleA.C											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleB.A											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleB.B											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleB.C											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		geometry.Triangles	.push_back(triangleA);
		geometry.Triangles	.push_back(triangleB);
		geometry.Normals	.push_back({0, 1, 0});
	}
	return 0;
}

//
int													ced::geometryBuildFigure0	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::gpk::SCoord3<float> gridCenter)	{
	(void)radius;
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::gpk::SCoord3<double>									coords	[4]				=
			{ {sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * x			/ slices), cos(::gpk::math_pi * x		/slices)}
			, {sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), cos(::gpk::math_pi * (x + 1) /slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * x			/ slices), cos(::gpk::math_pi * x		/slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), cos(::gpk::math_pi * (x + 1)	/slices)}
			};
		::gpk::STriangle3D<float>								triangleA			= {coords[0].Cast<float>() * radius, coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::gpk::STriangle3D<float>								triangleB			= {coords[1].Cast<float>() * radius, coords[3].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		triangleA.A											-= gridCenter;
		triangleA.B											-= gridCenter;
		triangleA.C											-= gridCenter;
		triangleB.A											-= gridCenter;
		triangleB.B											-= gridCenter;
		triangleB.C											-= gridCenter;
		geometry.Triangles	.push_back(triangleA);
		geometry.Triangles	.push_back(triangleB);
		geometry.Normals	.push_back({0, 1, 0});
	}
	return 0;
}

//
int													ced::geometryBuildSphere	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::gpk::SCoord3<float> gridCenter)	{
	::gpk::SCoord2<float>									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		{
			::gpk::SCoord2<float>									texcoords	[4]				=
				{ {(z		) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z		) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				};
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		::gpk::SCoord3<double>									coords	[4]				=
			{ {sin(::gpk::math_pi * x		/slices) * cos(::gpk::math_2pi * z			/ stacks), cos(::gpk::math_pi * x		/slices), sin(::gpk::math_pi * x		/ slices) * sin(::gpk::math_2pi * z			/ stacks)}
			, {sin(::gpk::math_pi * (x + 1)	/slices) * cos(::gpk::math_2pi * z			/ stacks), cos(::gpk::math_pi * (x + 1) /slices), sin(::gpk::math_pi * (x + 1)	/ slices) * sin(::gpk::math_2pi * z			/ stacks)}
			, {sin(::gpk::math_pi * x		/slices) * cos(::gpk::math_2pi * (z + 1)	/ stacks), cos(::gpk::math_pi * x		/slices), sin(::gpk::math_pi * x		/ slices) * sin(::gpk::math_2pi * (z + 1)	/ stacks)}
			, {sin(::gpk::math_pi * (x + 1)	/slices) * cos(::gpk::math_2pi * (z + 1)	/ stacks), cos(::gpk::math_pi * (x + 1)	/slices), sin(::gpk::math_pi * (x + 1)	/ slices) * sin(::gpk::math_2pi * (z + 1)	/ stacks)}
			};
		{
			::gpk::STriangle3D<float>								triangleA			= {coords[0].Cast<float>() * radius, coords[2].Cast<float>() * radius, coords[1].Cast<float>() * radius};
			::gpk::STriangle3D<float>								triangleB			= {coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius, coords[3].Cast<float>() * radius};
			triangleA.A											-= gridCenter;
			triangleA.B											-= gridCenter;
			triangleA.C											-= gridCenter;
			triangleB.A											-= gridCenter;
			triangleB.B											-= gridCenter;
			triangleB.C											-= gridCenter;
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
		}
		{
			::gpk::SCoord3<float>									normal				= coords[0].Cast<float>();
			normal												+= coords[1].Cast<float>();
			normal												+= coords[2].Cast<float>();
			normal												+= coords[3].Cast<float>();
			normal												/= 4;
			normal.Normalize();
			geometry.Normals.push_back(normal);
		}
	}
	return 0;
}

int													ced::geometryBuildSphere	(SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, ::gpk::SCoord3<float> gridCenter)	{
	::gpk::SCoord2<float>									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		{
			::gpk::SCoord2<float>									texcoords	[4]				=
				{ {(z		) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z		) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				};
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		::gpk::SCoord3<double>									coords		[4]				=
			{ {sin(::gpk::math_pi * x		/slices) * cos(::gpk::math_2pi * z			/ stacks), cos(::gpk::math_pi * x		/slices), sin(::gpk::math_pi * x		/ slices) * sin(::gpk::math_2pi * z			/ stacks)}
			, {sin(::gpk::math_pi * (x + 1)	/slices) * cos(::gpk::math_2pi * z			/ stacks), cos(::gpk::math_pi * (x + 1) /slices), sin(::gpk::math_pi * (x + 1)	/ slices) * sin(::gpk::math_2pi * z			/ stacks)}
			, {sin(::gpk::math_pi * x		/slices) * cos(::gpk::math_2pi * (z + 1)	/ stacks), cos(::gpk::math_pi * x		/slices), sin(::gpk::math_pi * x		/ slices) * sin(::gpk::math_2pi * (z + 1)	/ stacks)}
			, {sin(::gpk::math_pi * (x + 1)	/slices) * cos(::gpk::math_2pi * (z + 1)	/ stacks), cos(::gpk::math_pi * (x + 1)	/slices), sin(::gpk::math_pi * (x + 1)	/ slices) * sin(::gpk::math_2pi * (z + 1)	/ stacks)}
			};
		{
			geometry.Normals.push_back(
				{ coords[0].Normalize().Cast<float>()
				, coords[2].Normalize().Cast<float>()
				, coords[1].Normalize().Cast<float>()
				});
			geometry.Normals.push_back(
				{ coords[1].Normalize().Cast<float>()
				, coords[2].Normalize().Cast<float>()
				, coords[3].Normalize().Cast<float>()
				});
		}
		{
			::gpk::STriangle3D<float>								triangleA			= {(coords[0] * radius).Cast<float>(), (coords[2] * radius).Cast<float>(), (coords[1] * radius).Cast<float>()};
			::gpk::STriangle3D<float>								triangleB			= {(coords[1] * radius).Cast<float>(), (coords[2] * radius).Cast<float>(), (coords[3] * radius).Cast<float>()};
			triangleA.A											-= gridCenter;
			triangleA.B											-= gridCenter;
			triangleA.C											-= gridCenter;
			triangleB.A											-= gridCenter;
			triangleB.B											-= gridCenter;
			triangleB.C											-= gridCenter;
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
		}
	}
	return 0;
}
