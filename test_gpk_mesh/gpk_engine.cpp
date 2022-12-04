#include "gpk_engine.h"
#include "gpk_voxel.h"

::gpk::error_t			gpk::SEngine::CreateBox				()	{
	int32_t						iEntity								= this->ManagedEntities.Create();
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pVertices;
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pNormals;
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pUV;
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pIndicesVertex;
	//::gpk::ptr_obj<::gpk::SRenderBuffer>	pIndicesNormal;
	//::gpk::ptr_obj<::gpk::SRenderBuffer>	pIndicesUV;
	pVertices		->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_3;
	pVertices		->Desc.Usage	= ::gpk::BUFFER_USAGE_Position;

	pNormals		->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_3;
	pNormals		->Desc.Usage	= ::gpk::BUFFER_USAGE_Normal;

	pUV				->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_2;
	pUV				->Desc.Usage	= ::gpk::BUFFER_USAGE_UV;

	pIndicesVertex	->Desc.Format	= ::gpk::DATA_TYPE_UINT8;
	pIndicesVertex	->Desc.Usage	= ::gpk::BUFFER_USAGE_Index;

	//pIndicesNormal	->Desc.Format	= ::gpk::DATA_TYPE_UINT8;
	//pIndicesNormal	->Desc.Usage	= ::gpk::BUFFER_USAGE_Normal;
	//
	//pIndicesUV		->Desc.Format	= ::gpk::DATA_TYPE_UINT8;
	//pIndicesUV		->Desc.Usage	= ::gpk::BUFFER_USAGE_UV;

	uint32_t					iVertices		= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pVertices);
	uint32_t					iNormals		= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pNormals);
	uint32_t					iUV				= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pUV);
	uint32_t					iIndicesVertex	= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pIndicesVertex);
	//uint32_t					iIndicesNormals	= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pIndicesNormal);
	//uint32_t					iIndicesUV		= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pIndicesUV);

	uint32_t					iSkin			= (uint32_t)Scene.ManagedMeshes.CreateSkin();
	uint32_t					iSurface		= (uint32_t)Scene.ManagedSurfaces.Create();
	Scene.ManagedMeshes.Skins[iSkin]->Textures.push_back(iSurface);

	Scene.ManagedSurfaces.Surfaces[iSurface]->Desc.BitDepth				= 8;
	Scene.ManagedSurfaces.Surfaces[iSurface]->Desc.ColorType			= ::gpk::COLOR_TYPE_BGRA;
	Scene.ManagedSurfaces.Surfaces[iSurface]->Desc.MethodCompression	= 0;
	Scene.ManagedSurfaces.Surfaces[iSurface]->Desc.MethodFilter			= 0;
	Scene.ManagedSurfaces.Surfaces[iSurface]->Desc.MethodInterlace		= 0;
	Scene.ManagedSurfaces.Surfaces[iSurface]->Desc.Dimensions			= {1, 1};
	Scene.ManagedSurfaces.Surfaces[iSurface]->Data.resize(1 * sizeof(::gpk::SColorBGRA));

	uint32_t					iMesh			= (uint32_t)Scene.ManagedMeshes.CreateMesh();

	Scene.ManagedMeshes.Meshes[iMesh]->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});
	//Scene.ManagedMeshes.Meshes[iMesh]->ConstantBuffers.append({iNormals, iUV});

	Scene.ManagedMeshes.Meshes[iMesh]->Desc.Mode		= ::gpk::MESH_MODE_List;
	Scene.ManagedMeshes.Meshes[iMesh]->Desc.Type		= ::gpk::GEOMETRY_TYPE_Triangle;
	Scene.ManagedMeshes.Meshes[iMesh]->Desc.NormalMode	= ::gpk::NORMAL_MODE_Point;
	Scene.ManagedMeshes.Meshes[iMesh]->GeometrySlices.resize(6);	// one per face

	pVertices		->Data.resize(24 * sizeof(::gpk::SCoord3<float>));
	pNormals		->Data.resize(24 * sizeof(::gpk::SCoord3<float>));
	pUV				->Data.resize(24 * sizeof(::gpk::SCoord2<float>));
	pIndicesVertex	->Data.resize(12 * sizeof(::gpk::STriangle<uint16_t>));
	memcpy(&pVertices		->Data[0], ::gpk::VOXEL_FACE_VERTICES	, pVertices			->Data.size());
	memcpy(&pIndicesVertex	->Data[0], ::gpk::VOXEL_FACE_INDICES_16	, pIndicesVertex	->Data.size());
	memcpy(&pNormals		->Data[0], ::gpk::VOXEL_FACE_NORMALS	, pNormals			->Data.size());
	memcpy(&pUV				->Data[0], ::gpk::VOXEL_FACE_UV			, pUV				->Data.size());

	uint32_t					offsetVertex			= 0;
	for(uint32_t iFace = 0; iFace < 6; ++iFace) {
		Scene.ManagedMeshes.Meshes[iMesh]->GeometrySlices[iFace].Skin	= iSkin;
		Scene.ManagedMeshes.Meshes[iMesh]->GeometrySlices[iFace].Slice	= {offsetVertex, offsetVertex += 4};
	}

	return iEntity;
}

::gpk::error_t			gpk::SEngine::CreateSphere			()	{ 
	return 0;
}

::gpk::error_t			gpk::SEngine::CreateCylinder		()	{ 
	return 0; 
}

::gpk::error_t			gpk::SEngine::CreateCircle			()	{ return 0; }
::gpk::error_t			gpk::SEngine::CreateRing			()	{ return 0; }
::gpk::error_t			gpk::SEngine::CreateSquare			()	{ return 0; }
::gpk::error_t			gpk::SEngine::CreateTriangle		()	{ return 0; }