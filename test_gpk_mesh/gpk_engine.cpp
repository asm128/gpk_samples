#include "gpk_engine.h"
#include "gpk_voxel.h"
#include "gpk_ascii_color.h"
#include "gpk_geometry_lh.h"

int												gpk::updateEntityTransforms			
	( uint32_t								iEntity
	, const ::gpk::SVirtualEntity			& entity
	, const ::gpk::SVirtualEntityManager	& managedEntities	
	, ::gpk::SRigidBodyIntegrator			& integrator		
	, ::gpk::SRenderNodeManager				& renderNodes
	)
{
	const ::gpk::ptr_obj<::gpk::array_pod<uint32_t>>	& children						= managedEntities.EntityChildren[iEntity];
	if(-1 != entity.RenderNode) {
		::gpk::SMatrix4<float>								& worldTransform				= renderNodes.RenderNodeTransforms[entity.RenderNode];
		if(-1 == entity.RigidBody)
			worldTransform									= ::gpk::SMatrix4<float>::GetIdentity();
		else
			integrator.GetTransform(entity.RigidBody, worldTransform);
	
		if(-1 != entity.Parent) {
			const ::gpk::SVirtualEntity			& entityParent					= managedEntities.Entities[entity.Parent];
			if(-1 != entityParent.RenderNode) 
				worldTransform					= renderNodes.RenderNodeTransforms[entityParent.RenderNode] * worldTransform;
			else if(-1 != entityParent.RigidBody) {
				worldTransform					= integrator.TransformsLocal[entityParent.RigidBody] * worldTransform;
			}
		}
	}
	if(children) {
		for(uint32_t iChild = 0; iChild < children->size(); ++iChild) {
			const uint32_t iChildEntity = (*children)[iChild];
			::gpk::updateEntityTransforms(iChildEntity, managedEntities.Entities[iChildEntity], managedEntities, integrator, renderNodes);
		}
	}

	return 0;
}

::gpk::error_t						gpk::SEngine::CreateLight			(::gpk::LIGHT_TYPE type) {
	int32_t									iEntity								= this->ManagedEntities.Create();
	::gpk::SVirtualEntity					& entity							= ManagedEntities.Entities[iEntity];
	entity.Parent						= -1;
	entity.RenderNode					= Scene->ManagedRenderNodes.Create();

	uint16_t								indexLight							= (uint16_t)-1;
	switch(type) {
	case ::gpk::LIGHT_TYPE_Directional	: indexLight = (uint16_t)Scene->ManagedRenderNodes.LightsDirectional	.push_back({}); break;
	case ::gpk::LIGHT_TYPE_Point		: indexLight = (uint16_t)Scene->ManagedRenderNodes.LightsPoint			.push_back({}); break;
	case ::gpk::LIGHT_TYPE_Spot			: indexLight = (uint16_t)Scene->ManagedRenderNodes.LightsSpot			.push_back({}); break;
	default:
		error_printf("Invalid light type: %i", (int)type);
		return -1;
	}
	Scene->ManagedRenderNodes.RenderNodeLights		[entity.RenderNode]->push_back({type, indexLight});
	Scene->ManagedRenderNodes.RenderNodeTransforms	[entity.RenderNode].SetIdentity();
	return iEntity;
}

::gpk::error_t						gpk::SEngine::CreateCamera			() {
	int32_t									iEntity								= this->ManagedEntities.Create();
	::gpk::SVirtualEntity					& entity							= ManagedEntities.Entities[iEntity];
	entity.Parent						= -1;
	entity.RenderNode					= Scene->ManagedRenderNodes.Create();
	entity.RigidBody					= this->Integrator.Create();

	::gpk::SCamera							camera;
	camera.Angle						= .25 * ::gpk::math_pi;
	camera.Front						= {1, 0, 0};
	camera.Up							= {0, 1, 0};
	camera.Right						= {0, 0, 1};

	Scene->ManagedRenderNodes.RenderNodeTransforms	[entity.RenderNode].SetIdentity();
	Scene->ManagedRenderNodes.RenderNodeCameras		[entity.RenderNode]->push_back(camera);
	return iEntity;
}

::gpk::error_t						gpk::SEngine::CreateBox				()	{
	int32_t									iEntity								= this->ManagedEntities.Create();
	ManagedEntities.EntityNames[iEntity]	= ::gpk::vcs{"Box"};
	::gpk::SVirtualEntity					& entity							= ManagedEntities.Entities[iEntity];
	entity.Parent						= -1;
	entity.RenderNode					= Scene->ManagedRenderNodes.Create();
	entity.RigidBody					= this->Integrator.Create();

	::gpk::ptr_obj<::gpk::SRenderBuffer>	pIndicesVertex;
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pVertices;
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pNormals;
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pUV;
	pIndicesVertex	->Desc.Format	= ::gpk::DATA_TYPE_UINT16;
	pIndicesVertex	->Desc.Usage	= ::gpk::BUFFER_USAGE_Index;

	pVertices		->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_3;
	pVertices		->Desc.Usage	= ::gpk::BUFFER_USAGE_Position;

	pNormals		->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_3;
	pNormals		->Desc.Usage	= ::gpk::BUFFER_USAGE_Normal;

	pUV				->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_2;
	pUV				->Desc.Usage	= ::gpk::BUFFER_USAGE_UV;


	uint32_t							iVertices				= (uint32_t)Scene->ManagedBuffers.Buffers.push_back(pVertices);
	uint32_t							iNormals				= (uint32_t)Scene->ManagedBuffers.Buffers.push_back(pNormals);
	uint32_t							iUV						= (uint32_t)Scene->ManagedBuffers.Buffers.push_back(pUV);
	uint32_t							iIndicesVertex			= (uint32_t)Scene->ManagedBuffers.Buffers.push_back(pIndicesVertex);

	uint32_t							iMesh					= (uint32_t)Scene->ManagedMeshes.CreateMesh();
	Scene->ManagedMeshes.MeshNames[iMesh]	= ::gpk::vcs{"Box"};
	::gpk::ptr_obj<::gpk::SRenderMesh>	& mesh					= Scene->ManagedMeshes.Meshes[iMesh];

	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});

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

	::gpk::view_array<::gpk::SCoord3<float>> viewPositions = {(::gpk::SCoord3<float>*)pVertices->Data.begin(), pVertices->Data.size() / sizeof(::gpk::SCoord3<float>)};
	for(uint32_t index = 0; index < viewPositions.size(); ++index) {
		viewPositions[index] -= {.5f, .5f, .5f};
	}

	uint32_t							offsetIndex				= 0;
	for(uint32_t iFace = 0; iFace < 6; ++iFace) {
		uint32_t							iSkin					= (uint32_t)Scene->ManagedRenderNodes.CreateSkin();
		uint32_t							iSurface				= (uint32_t)Scene->ManagedSurfaces.Create();
		::gpk::ptr_obj<::gpk::SSkin>		& skin					= Scene->ManagedRenderNodes.Skins[iSkin];
		skin->Textures.push_back(iSurface);
		skin->Material.Color.Ambient	= ::gpk::SColorBGRA(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.Color.Diffuse	= ::gpk::SColorBGRA(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.Color.Specular	= ::gpk::SColorBGRA(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.SpecularPower	= 0.5f;

		skin->Material.Color.Ambient	*= .1f;

		::gpk::ptr_obj<::gpk::SSurface>		& surface				= Scene->ManagedSurfaces.Surfaces[iSurface];
		surface->Desc.ColorType			= ::gpk::COLOR_TYPE_BGRA;
		surface->Desc.MethodCompression	= 0;
		surface->Desc.MethodFilter		= 0;
		surface->Desc.MethodInterlace	= 0;
		surface->Desc.Dimensions		= {1, 1};
		surface->Data.resize(1 * sizeof(::gpk::SColorBGRA));
		*(::gpk::SColorBGRA*)&surface->Data[0]	= ::gpk::SColorRGBA{::gpk::VOXEL_PALETTE[iFace]};

		::gpk::SGeometrySlice				& slice					= mesh->GeometrySlices[iFace];
		slice.Slice							= {offsetIndex, 6};
		offsetIndex						+= slice.Slice.Count;

		int32_t								iFaceEntity						= this->ManagedEntities.Create();
		uint32_t							iFaceRenderNode					= Scene->ManagedRenderNodes.Create();
		::gpk::SVirtualEntity				& faceEntity					= this->ManagedEntities.Entities[iFaceEntity];
		::gpk::SRenderNode					& faceRenderNode				= this->Scene->ManagedRenderNodes.RenderNodes[iFaceRenderNode];

		faceRenderNode.Mesh				= iMesh;
		faceRenderNode.Slice			= iFace;
		faceRenderNode.Skin				= iSkin;

		faceEntity.RigidBody			= -1;
		faceEntity.Parent				= iEntity;
		faceEntity.RenderNode			= iFaceRenderNode;
		this->ManagedEntities.EntityNames	[iFaceEntity]	= ::gpk::get_value_label((VOXEL_FACE)iFace);

		this->ManagedEntities.EntityChildren[iEntity]->push_back(iFaceEntity);
	}

	return iEntity;
}

::gpk::error_t						gpk::SEngine::CreateSphere			()	{ 
	SGeometryIndexedTriangles				geometry;
	::gpk::geometryBuildSphere(geometry, 24, 24, .5f, {});

	int32_t									iEntity								= this->ManagedEntities.Create();
	ManagedEntities.EntityNames[iEntity]	= ::gpk::vcs{"Sphere"};
	::gpk::SVirtualEntity					& entity							= ManagedEntities.Entities[iEntity];
	entity.Parent						= -1;
	entity.RenderNode					= Scene->ManagedRenderNodes.Create();;
	entity.RigidBody					= this->Integrator.Create();

	::gpk::ptr_obj<::gpk::SRenderBuffer>	pIndicesVertex;
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pVertices;
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pNormals;
	::gpk::ptr_obj<::gpk::SRenderBuffer>	pUV;

	pIndicesVertex	->Desc.Format		= ::gpk::DATA_TYPE_UINT16;
	pIndicesVertex	->Desc.Usage		= ::gpk::BUFFER_USAGE_Index;

	pVertices		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pVertices		->Desc.Usage		= ::gpk::BUFFER_USAGE_Position;

	pNormals		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pNormals		->Desc.Usage		= ::gpk::BUFFER_USAGE_Normal;

	pUV				->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_2;
	pUV				->Desc.Usage		= ::gpk::BUFFER_USAGE_UV;

	pIndicesVertex	->Data.resize(geometry.PositionIndices	.byte_count() / 2);
	::gpk::view_array<uint16_t> viewIndices = {(uint16_t*)pIndicesVertex->Data.begin(), geometry.PositionIndices.size()};
	for(uint32_t index = 0; index < geometry.PositionIndices.size(); ++index) {
		viewIndices[index] = (uint16_t)geometry.PositionIndices[index];
	}
	//memcpy(&pIndicesVertex	->Data[0], geometry.PositionIndices	.begin(), pIndicesVertex	->Data.size());

	pVertices		->Data.resize(geometry.Positions		.byte_count());
	pNormals		->Data.resize(geometry.Normals			.byte_count());
	pUV				->Data.resize(geometry.TextureCoords	.byte_count());
	memcpy(&pVertices		->Data[0], geometry.Positions		.begin(), pVertices			->Data.size());
	memcpy(&pNormals		->Data[0], geometry.Normals			.begin(), pNormals			->Data.size());
	memcpy(&pUV				->Data[0], geometry.TextureCoords	.begin(), pUV				->Data.size());

	uint32_t									iVertices				= (uint32_t)Scene->ManagedBuffers.Buffers.push_back(pVertices);
	uint32_t									iNormals				= (uint32_t)Scene->ManagedBuffers.Buffers.push_back(pNormals);
	uint32_t									iUV						= (uint32_t)Scene->ManagedBuffers.Buffers.push_back(pUV);
	uint32_t									iIndicesVertex			= (uint32_t)Scene->ManagedBuffers.Buffers.push_back(pIndicesVertex);

	uint32_t									iMesh					= (uint32_t)Scene->ManagedMeshes.CreateMesh();
	::gpk::ptr_obj<::gpk::SRenderMesh>			& mesh					= Scene->ManagedMeshes.Meshes[iMesh];
	Scene->ManagedMeshes.MeshNames[iMesh]	= ::gpk::vcs{"Sphere"};
	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});

	mesh->Desc.Mode							= ::gpk::MESH_MODE_List;
	mesh->Desc.Type							= ::gpk::GEOMETRY_TYPE_Triangle;
	mesh->Desc.NormalMode					= ::gpk::NORMAL_MODE_Point;
	uint32_t									iSkin					= (uint32_t)Scene->ManagedRenderNodes.CreateSkin();
	::gpk::ptr_obj<::gpk::SSkin>				& skin					= Scene->ManagedRenderNodes.Skins[iSkin];
	skin->Material.Color.Ambient			= ::gpk::SColorBGRA(::gpk::ASCII_PALETTE[3]);
	skin->Material.Color.Diffuse			= ::gpk::SColorBGRA(::gpk::ASCII_PALETTE[3]);
	skin->Material.Color.Specular			= ::gpk::SColorBGRA(::gpk::ASCII_PALETTE[3]);
	skin->Material.SpecularPower			= 0.5f;

	uint32_t									iSurface				= (uint32_t)Scene->ManagedSurfaces.Create();
	skin->Textures.push_back(iSurface);

	skin->Material.Color.Ambient			*= .1f;

	::gpk::ptr_obj<::gpk::SSurface>				& surface				= Scene->ManagedSurfaces.Surfaces[iSurface];
	surface->Desc.ColorType					= ::gpk::COLOR_TYPE_BGRA;
	surface->Desc.MethodCompression			= 0;
	surface->Desc.MethodFilter				= 0;
	surface->Desc.MethodInterlace			= 0;
	surface->Desc.Dimensions				= {16, 16};
	surface->Data.resize(surface->Desc.Dimensions.Area() * sizeof(::gpk::SColorBGRA));
	memset(surface->Data.begin(), 0xFF, surface->Data.size());
	::gpk::view_grid<::gpk::SColorBGRA>			view	= {(::gpk::SColorBGRA*)surface->Data.begin(), surface->Desc.Dimensions.Cast<uint32_t>()};
	::gpk::SColorRGBA							color	= {::gpk::ASCII_PALETTE[rand() % 16]};
	for(uint32_t y = 6; y < 10; ++y)
	for(uint32_t x = 0; x < 16; ++x)
		view[y][x]	= color;

	mesh->GeometrySlices.resize(1);	// one per face
	::gpk::SGeometrySlice						& slice					= mesh->GeometrySlices[0];
	slice.Slice								= {0, geometry.PositionIndices.size()};

	Scene->ManagedRenderNodes.RenderNodes[entity.RenderNode].Skin	= iSkin;
	Scene->ManagedRenderNodes.RenderNodes[entity.RenderNode].Mesh	= iMesh;
	Scene->ManagedRenderNodes.RenderNodes[entity.RenderNode].Slice	= 0;

	return iEntity;
}

::gpk::error_t			gpk::SEngine::CreateCylinder		()	{ 
	int32_t									iEntity								= this->ManagedEntities.Create();
	return iEntity;
}

::gpk::error_t			gpk::SEngine::CreateCircle			()	{ 	
	int32_t									iEntity								= this->ManagedEntities.Create();
	return iEntity;
 }
::gpk::error_t			gpk::SEngine::CreateRing			()	{ 	
	int32_t									iEntity								= this->ManagedEntities.Create();
	return iEntity;
 }
::gpk::error_t			gpk::SEngine::CreateSquare			()	{ 	
	int32_t									iEntity								= this->ManagedEntities.Create();
	return iEntity;
 }
::gpk::error_t			gpk::SEngine::CreateTriangle		()	{ 	
	int32_t									iEntity								= this->ManagedEntities.Create();
	return iEntity;
 }

