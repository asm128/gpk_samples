#include "gpk_engine_rendercolor.h"
#include "gpk_engine_renderbuffer.h"
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

#pragma pack(push, 1)
	struct SMeshDescription {
		GEOMETRY_TYPE								Type;
		MESH_MODE									Mode;
		NORMAL_MODE									NormalMode;
	};
#pragma pack(pop)

	struct SMesh {
		SMeshDescription							Desc;
		::gpk::array_pod<uint32_t>					GeometryBuffers;
		::gpk::array_pod<SGeometrySlice>			GeometrySlices;
	};	
	
	struct SSkin {
		::gpk::SMaterial							Material;
		::gpk::array_pod<uint32_t>					Textures;
	};

	struct SMeshManager {
		::gpk::array_pobj<::gpk::SRenderBuffer>		Buffers;
		::gpk::array_pobj<::gpk::SMesh>				Meshes;
		::gpk::array_pobj<::gpk::SSkin>				Skins;
	};
} // namespace

#endif
