#include "gpk_engine.h"

#include "gpk_view_grid.h"

#ifndef GPK_MESH_H_324234234
#define GPK_MESH_H_324234234


namespace gpk
{
	GDEFINE_ENUM_TYPE(SHAPE_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Custom		, 0);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Rectangle	, 1);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Circle		, 2);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Ring			, 3);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cube			, 4);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Sphere		, 5);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cylinder		, 6);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Torus		, 7);

	//::gpk::error_t		meshBuildSquare		(::gpk::SMesh & out_mesh, gpk::AXIS axis) {
	//	out_mesh.Mode					= ::ghg::MESH_MODE_Fan;
	//	out_mesh.Type					= ::ghg::MESH_TYPE_Triangle;
	//	out_mesh.NormalMode				= ::ghg::NORMAL_MODE_Triangle;
	//	if(axis & (gpk::AXIS_XPOSITIVE | gpk::AXIS_XNEGATIVE)) { 
	//		out_mesh.Positions.Values	= {{0, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 1, 1}}; 
	//		out_mesh.UV.Values			= {{0, 0}, {1, 0}, {0, 1}, {1, 1}}; 
	//		out_mesh.Normals.Indices.resize(out_mesh.Normals.Indices.size() + 4, 0); 
	//		if(axis & (gpk::AXIS_XPOSITIVE | gpk::AXIS_XNEGATIVE)) {
	//			out_mesh.Positions.Indices = out_mesh.UV.Indices = {0, 1, 2, 3, 0, 1}; 
	//		}
	//		else if(axis & gpk::AXIS_XNEGATIVE) {
	//			out_mesh.Normals.Values.push_back({-1, 0, 0}); 
	//			out_mesh.Positions.Indices = out_mesh.UV.Indices = {0, 3, 2, 1}; 
	//		}
	//		else {
	//			out_mesh.Normals.Values.push_back({1, 0, 0}); 
	//			out_mesh.Positions.Indices = out_mesh.UV.Indices = {0, 1, 2, 3}; 
	//		}
	//	}
	//	else if(axis & (gpk::AXIS_YPOSITIVE | gpk::AXIS_YNEGATIVE)) { out_mesh.Positions.Values = {{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}}; out_mesh.UV.Values = {{0, 0}, {1, 0}, {0, 1}, {1, 1}}; out_mesh.Normals.Values.push_back({0, 1, 0}); out_mesh.Normals.Indices.resize(out_mesh.Normals.Indices.size() + 4, 0); if(axis & (gpk::AXIS_YPOSITIVE | gpk::AXIS_YNEGATIVE)) out_mesh.Positions.Indices = out_mesh.UV.Indices = {0, 1, 2, 3, 2, 1}; else if(axis & gpk::AXIS_YNEGATIVE) out_mesh.Positions.Indices = out_mesh.UV.Indices = {0, 3, 2, 1}; else out_mesh.Positions.Indices = out_mesh.UV.Indices = {0, 1, 2, 3}; }
	//	else if(axis & (gpk::AXIS_ZPOSITIVE | gpk::AXIS_ZNEGATIVE)) { out_mesh.Positions.Values = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}; out_mesh.UV.Values = {{0, 0}, {1, 0}, {0, 1}, {1, 1}}; out_mesh.Normals.Values.push_back({0, 0, 1}); out_mesh.Normals.Indices.resize(out_mesh.Normals.Indices.size() + 4, 0); if(axis & (gpk::AXIS_ZPOSITIVE | gpk::AXIS_ZNEGATIVE)) out_mesh.Positions.Indices = out_mesh.UV.Indices = {0, 1, 2, 3, 2, 1}; else if(axis & gpk::AXIS_ZNEGATIVE) out_mesh.Positions.Indices = out_mesh.UV.Indices = {0, 3, 2, 1}; else out_mesh.Positions.Indices = out_mesh.UV.Indices = {0, 1, 2, 3}; }
	//	return 0; 
	//}

	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildCube		(::ghg::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh) {
	//	out_mesh.Mode					= ::ghg::MESH_MODE_List;
	//	out_mesh.Type					= ::ghg::MESH_TYPE_Triangle;
	//	out_mesh.NormalMode				= ::ghg::NORMAL_MODE_Face;
	//	constexpr ::gpk::SCoord3<int8_t>	positionIndices	[] = 
	//		{ 0, 1, 2, 0, 2, 3	// Left
	//		, 0, 1, 2, 0, 2, 3	// Bottom
	//		, 0, 1, 2, 0, 2, 3	// Back
	//		, 0, 1, 2, 0, 2, 3	// Right
	//		, 0, 1, 2, 0, 2, 3	// Top
	//		, 0, 1, 2, 0, 2, 3	// Front
	//		}; 
	//
	//	constexpr ::gpk::SCoord3<int8_t>	positions	[] = 
	//		{ {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}
	//		, {0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}
	//		}; 
	//	constexpr ::gpk::SCoord2<int8_t>	uv			[] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}}; 
	//	constexpr ::gpk::SCoord3<int8_t>	normals		[] = 
	//		{ {0, 0, 1}		// Face 1
	//		, {0, 1, 0}		// Face 2
	//		, {1, 0, 0}		// Face 3
	//		, {0, 0, -1}	// Face 4
	//		, {0, -1, 0}	// Face 5
	//		, {-1, 0, 0}	// Face 6
	//		}; 
	//
	//	for(uint32_t i = 0; i < gpk::size(positions	); ++i) out_mesh.Positions	.Values.push_back(positions[i]);
	//	for(uint32_t i = 0; i < gpk::size(uv		); ++i) out_mesh.UV			.Values.push_back(uv[i]);
	//	for(uint32_t i = 0; i < gpk::size(normals	); ++i) out_mesh.Normals	.Values.push_back(normals[i]);
	//	out_mesh.Positions.Indices		= 
	//	out_mesh.Positions.Indices = out_mesh.UV.Indices = (axis & (gpk::AXIS_XPOSITIVE | gpk::AXIS_XNEGATIVE)) ? {0, 1, 2, 3, 2, 1} : (axis & gpk::AXIS_XNEGATIVE) ? {0, 3, 2, 1} : {0, 1, 2, 3}; }
	//}

	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildCircle		(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, gpk::AXIS axis);
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildRing		(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, gpk::AXIS axis, float inset, float outset);
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildSphere		(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, uint8_t stacks, uint8_t slices);
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildCylinder	(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, uint8_t stacks, uint8_t slices, float radiusYMin, float radiusYMax);
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildTorus		(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, uint8_t stacks, uint8_t slices, float radiusCircle, float radiusCylinder);
	//
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildRectangle	(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, const ::gpk::SRectangle2 <float> & rect);
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildCube		(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, const ::gpk::SRectangle3D<float> & rect);
	//
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildCircle		(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, const ::gpk::SCircle		<float> & circle);
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildRing		(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, const ::gpk::SSphere		<float> & circle, float width);
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildSphere		(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, uint8_t stacks, uint8_t slices, const ::gpk::SSphere<float> & sphere);
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildCylinder	(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, uint8_t stacks, uint8_t slices, float radiusYMin, float radiusYMax);
	//template<typename _tAxisPosition, typename _tAxisNormal>	::gpk::error_t		meshBuildTorus		(::gpk::SMesh<_tAxisPosition, _tAxisNormal> & out_mesh, uint8_t stacks, uint8_t slices, float radiusCircle, float radiusCylinder);
} // namespace


#include "gpk_framework.h"
#include "gpk_gui.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SVSCache {
	::gpk::array_pod<::gpk::STriangle<float>>		TriangleWeights			= {};
	::gpk::array_pod<::gpk::SCoord2<int16_t>>		SolidPixelCoords		= {};
	::gpk::array_pod<::gpk::SCoord2<int16_t>>		WireframePixelCoords	= {};
};

struct SVSOutput {
	::gpk::array_pod<::gpk::STriangle3<float>>		Positions				= {};
	::gpk::array_pod<::gpk::STriangle3<float>>		Normals					= {};
};

struct SApplication {
	::gpk::SFramework								Framework				;

	::gpk::SGUI										GUI						= {};
	int32_t											IdViewport				= -1;

	::gpk::SEngine									Engine					= {};
	int32_t											EntityCamera			= -1;
	int32_t											EntityLightDirectional	= -1;
	int32_t											EntityLightPoint		= -1;
	int32_t											EntityLightSpot			= -1;
	int32_t											EntityBox				= -1;
	int32_t											EntitySphere			= -1;
	int32_t											Balls[16]				= {};

	::gpk::SImage<::gpk::SColorBGR>					Image;
	::gpk::SImage<::gpk::SColorBGRA>				ImageAlpha;

	::SVSOutput										OutputVertexShader		= {};
	::SVSCache										CacheVertexShader		= {};

													SApplication			(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423

#endif // GPK_MESH_H_324234234
