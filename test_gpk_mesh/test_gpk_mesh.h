#include "gpk_enum.h"
#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_datatype.h"
#include "gpk_ptr.h"

#ifndef GPK_MESH_H_324234234
#define GPK_MESH_H_324234234

namespace gpk
{
	GDEFINE_ENUM_TYPE(BUFFER_USAGE, uint8_t);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Position		,  0);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Index			,  1);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Color			,  2);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, ColorIndex		,  3);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Normal			,  4);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, UV				,  5);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Shadow			,  6);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Light			,  7);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, BlendIndex		,  8);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, BlendWeight	,  9);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, ImageColors	, 10);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, ImageIndices	, 11);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Palette		, 12);

#pragma pack(push, 1)
	struct SRenderBufferDesc {
		::gpk::DATA_TYPE							Format	= {};
		BUFFER_USAGE								Usage	= {};
		uint8_t										Stride	= {};
		uint16_t									Width	= {};
	};
#pragma pack(pop)

	struct SRenderBuffer {
		SRenderBufferDesc							Desc	= {};
		::gpk::array_pod<uint8_t>					Data	= {};

		::gpk::error_t								Save	(::gpk::array_pod<byte_t> & output)		{ 			
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const ::gpk::SRenderBufferDesc>{&Desc, 1}, output));
			gpk_necs(::gpk::viewWrite(::gpk::view_const_uint8{Data}, output));
			return 0; 
		}

		::gpk::error_t								Load	(::gpk::view_array<const byte_t> & input)	{ 
			gpk_necs(::gpk::loadPOD (input, Desc));
			gpk_necs(::gpk::loadView(input, Data));
			return 0;
		}
	};

#pragma pack(push, 1)
	struct SGeometrySlice {
		::gpk::SRange<uint32_t>						Slice;
		uint32_t									Skin;
	};
#pragma pack(pop)

	GDEFINE_ENUM_TYPE(NORMAL_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Point			, 0);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Line			, 1);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Triangle		, 2);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Quad			, 3);

	GDEFINE_ENUM_TYPE(MESH_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(MESH_MODE, List				, 0);
	GDEFINE_ENUM_VALUE(MESH_MODE, Strip				, 1);
	GDEFINE_ENUM_VALUE(MESH_MODE, Fan				, 2);

	GDEFINE_ENUM_TYPE(GEOMETRY_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Point			, 0);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Line			, 1);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Triangle		, 2);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Quad			, 3);

	struct SMesh {
		GEOMETRY_TYPE								Type;
		MESH_MODE									Mode;
		NORMAL_MODE									NormalMode;
		::gpk::array_pod<uint32_t>					GeometryBuffers;
		::gpk::array_pod<uint32_t>					GeometrySlices;
	};	
	
	struct SMeshManager {
		::gpk::array_pobj<::gpk::SRenderBuffer>		Buffers;
		::gpk::array_pobj<::gpk::SMesh>				Meshes;
		::gpk::array_pod<::gpk::SGeometrySlice>		Geometries;

		::gpk::error_t								CreateTriangle	()	{ return 0; }
		::gpk::error_t								CreateRectangle	()	{ return 0; }
		::gpk::error_t								CreateCircle	()	{ return 0; }
		::gpk::error_t								CreateRing		()	{ return 0; }
		::gpk::error_t								CreateBox		()	{ return 0; }
		::gpk::error_t								CreateGrid		()	{ return 0; }
		::gpk::error_t								CreateCylinder	()	{ return 0; }
	};


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

struct SApplication {
	::gpk::SFramework						Framework									;

	::gpk::SImage<::gpk::SColorBGRA>		TextureFont									= {};
	::gpk::SImageMonochrome<uint32_t>		TextureFontMonochrome						= {};
	::gpk::SGUI								GUI											= {};
	::gpk::STriangle3	<float>				CubePositions[12]							= {};
	int32_t									IdViewport									= -1;

											SApplication								(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423

#endif // GPK_MESH_H_324234234
