#include "gpk_engine.h"
#include "gpk_voxel.h"
#include "gpk_ascii_color.h"

::gpk::error_t						gpk::SEngine::CreateLight			(::gpk::LIGHT_TYPE type) {
	int32_t									iEntity								= this->ManagedEntities.Create();
	::gpk::SVirtualEntity					& entity							= ManagedEntities.Entities[iEntity];
	entity.Parent						= -1;
	entity.RenderNode					= Scene.ManagedRenderNodes.Create();

	uint16_t								indexLight							= (uint16_t)-1;
	switch(type) {
	case ::gpk::LIGHT_TYPE_Directional	: indexLight = (uint16_t)Scene.ManagedRenderNodes.LightsDirectional		.push_back({}); break;
	case ::gpk::LIGHT_TYPE_Point		: indexLight = (uint16_t)Scene.ManagedRenderNodes.LightsPoint			.push_back({}); break;
	case ::gpk::LIGHT_TYPE_Spot			: indexLight = (uint16_t)Scene.ManagedRenderNodes.LightsSpot			.push_back({}); break;
	default:
		error_printf("Invalid light type: %i", (int)type);
		return -1;
	}
	Scene.ManagedRenderNodes.RenderNodeLights		[entity.RenderNode]->push_back({type, indexLight});
	Scene.ManagedRenderNodes.RenderNodeTransforms	[entity.RenderNode].SetIdentity();
	return iEntity;
}

::gpk::error_t						gpk::SEngine::CreateCamera			() {
	int32_t									iEntity								= this->ManagedEntities.Create();
	::gpk::SVirtualEntity					& entity							= ManagedEntities.Entities[iEntity];
	entity.Parent						= -1;
	entity.RenderNode					= Scene.ManagedRenderNodes.Create();
	entity.RigidBody					= this->Integrator.Create();

	::gpk::SCamera							camera;
	camera.Angle						= .25 * ::gpk::math_pi;
	camera.Front						= {1, 0, 0};
	camera.Up							= {0, 1, 0};
	camera.Right						= {0, 0, 1};

	Scene.ManagedRenderNodes.RenderNodeTransforms	[entity.RenderNode].SetIdentity();
	Scene.ManagedRenderNodes.RenderNodeCameras		[entity.RenderNode]->push_back(camera);
	return iEntity;
}

::gpk::error_t						gpk::SEngine::CreateBox				()	{
	int32_t									iEntity								= this->ManagedEntities.Create();
	::gpk::SVirtualEntity					& entity							= ManagedEntities.Entities[iEntity];
	entity.Parent						= -1;
	entity.RenderNode					= -1;
	entity.RigidBody					= this->Integrator.Create();

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

	uint32_t							iVertices				= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pVertices);
	uint32_t							iNormals				= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pNormals);
	uint32_t							iUV						= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pUV);
	uint32_t							iIndicesVertex			= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pIndicesVertex);
	//uint32_t							iIndicesNormals			= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pIndicesNormal);
	//uint32_t							iIndicesUV				= (uint32_t)Scene.ManagedBuffers.Buffers.push_back(pIndicesUV);

	uint32_t							iMesh					= (uint32_t)Scene.ManagedMeshes.CreateMesh();
	::gpk::ptr_obj<::gpk::SRenderMesh>	& mesh					= Scene.ManagedMeshes.Meshes[iMesh];

	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});
	//mesh->ConstantBuffers.append({iNormals, iUV});

	mesh->Desc.Mode					= ::gpk::MESH_MODE_List;
	mesh->Desc.Type					= ::gpk::GEOMETRY_TYPE_Triangle;
	mesh->Desc.NormalMode			= ::gpk::NORMAL_MODE_Point;
	mesh->GeometrySlices.resize(6);	// one per face

	pVertices		->Data.resize(24 * sizeof(::gpk::SCoord3<float>));
	pNormals		->Data.resize(24 * sizeof(::gpk::SCoord3<float>));
	pUV				->Data.resize(24 * sizeof(::gpk::SCoord2<float>));
	pIndicesVertex	->Data.resize(12 * sizeof(::gpk::STriangle<uint16_t>));
	memcpy(&pVertices		->Data[0], ::gpk::VOXEL_FACE_VERTICES	, pVertices			->Data.size());
	memcpy(&pIndicesVertex	->Data[0], ::gpk::VOXEL_FACE_INDICES_16	, pIndicesVertex	->Data.size());
	memcpy(&pNormals		->Data[0], ::gpk::VOXEL_FACE_NORMALS	, pNormals			->Data.size());
	memcpy(&pUV				->Data[0], ::gpk::VOXEL_FACE_UV			, pUV				->Data.size());

	uint32_t							offsetIndex				= 0;
	for(uint32_t iFace = 0; iFace < 6; ++iFace) {
		uint32_t							iSkin					= (uint32_t)Scene.ManagedMeshes.CreateSkin();
		uint32_t							iSurface				= (uint32_t)Scene.ManagedSurfaces.Create();
		::gpk::ptr_obj<::gpk::SSkin>		& skin					= Scene.ManagedMeshes.Skins[iSkin];
		skin->Textures.push_back(iSurface);
		skin->Material.Color.Ambient	= ::gpk::SColorBGRA(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.Color.Diffuse	= ::gpk::SColorBGRA(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.Color.Specular	= ::gpk::SColorBGRA(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.SpecularPower	= 0.5f;

		skin->Material.Color.Ambient	*= .1f;

		::gpk::ptr_obj<::gpk::SSurface>		& surface				= Scene.ManagedSurfaces.Surfaces[iSurface];
		surface->Desc.ColorType			= ::gpk::COLOR_TYPE_BGRA;
		surface->Desc.MethodCompression	= 0;
		surface->Desc.MethodFilter		= 0;
		surface->Desc.MethodInterlace	= 0;
		surface->Desc.Dimensions		= {1, 1};
		surface->Data.resize(1 * sizeof(::gpk::SColorBGRA));
		*(::gpk::SColorBGRA*)&surface->Data[0]	= ::gpk::SColorRGBA{::gpk::VOXEL_PALETTE[iFace]};

		::gpk::SGeometrySlice				& slice					= mesh->GeometrySlices[iFace];
		slice.Skin						= iSkin;
		slice.Slice						= {offsetIndex, offsetIndex += 6};

		int32_t								iFaceEntity						= this->ManagedEntities.Create();
		uint32_t							iFaceRenderNode					= Scene.ManagedRenderNodes.Create();
		::gpk::SVirtualEntity				& faceEntity					= this->ManagedEntities.Entities[iFaceEntity];
		::gpk::SRenderNode					& faceRenderNode				= this->Scene.ManagedRenderNodes.RenderNodes[iFaceRenderNode];

		faceRenderNode.Mesh				= iMesh;
		faceRenderNode.Slice			= iFace;

		faceEntity.RigidBody			= -1;
		faceEntity.Parent				= iEntity;
		faceEntity.RenderNode			= iFaceRenderNode;
		this->ManagedEntities.EntityChildren[iFaceEntity]->push_back(iFaceEntity);
		this->ManagedEntities.EntityNames	[iFaceEntity]	= ::gpk::get_value_label((VOXEL_FACE)iFace);
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