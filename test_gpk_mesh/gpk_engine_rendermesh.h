#include "gpk_engine_rendercolor.h"
#include "gpk_matrix.h"
#include "gpk_ptr.h"

#ifndef GPK_ENGINE_RENDERMESH_H
#define GPK_ENGINE_RENDERMESH_H

namespace gpk 
{
	// ------------------ Geometry Mesh
#pragma pack(push, 1)
	struct SGeometrySlice {
		::gpk::SRange<uint32_t>						Slice;
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

#pragma pack(push, 1)
	struct SMeshDescription {
		::gpk::GEOMETRY_TYPE						Type;
		::gpk::MESH_MODE							Mode;
		::gpk::NORMAL_MODE							NormalMode;
	};
#pragma pack(pop)

	struct SRenderMesh {
		::gpk::SMeshDescription						Desc;
		::gpk::array_pod<uint32_t>					GeometryBuffers;
		::gpk::array_pod<uint32_t>					ConstantBuffers;
		::gpk::array_pod<::gpk::SGeometrySlice>		GeometrySlices;
	};	

	struct SMeshManager {
		::gpk::array_pobj<::gpk::SRenderMesh>		Meshes			= {};

		::gpk::array_obj<::gpk::vcc>				MeshNames		= {};

		::gpk::error_t								CloneMesh		(uint32_t index)	{ Meshes	.push_back(::gpk::ptr_obj<::gpk::SRenderMesh	>{Meshes[index]}); return MeshNames.push_back(::gpk::vcc{MeshNames[index]}); }

		::gpk::error_t								CreateMesh		()					{ MeshNames.push_back({});				return Meshes .push_back({}); }
		::gpk::error_t								DeleteMesh		(uint32_t index)	{ MeshNames.remove_unordered(index);	return Meshes .remove_unordered(index); }
	};
} // namespace

#endif
