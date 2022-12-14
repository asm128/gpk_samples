#include "gpk_engine_entity.h"
#include "gpk_engine_scene.h"
#include "gpk_rigidbody.h"
#include "gpk_view_grid.h"

#ifndef GPK_ENGINE_H
#define GPK_ENGINE_H

namespace gpk 
{
	::gpk::error_t						updateEntityTransforms			
		( uint32_t								iEntity
		, const ::gpk::SVirtualEntity			& entity
		, const ::gpk::SVirtualEntityManager	& managedEntities	
		, ::gpk::SRigidBodyIntegrator			& integrator		
		, ::gpk::SRenderNodeManager				& renderNodes
		);

	struct SEngine {
		::gpk::ptr_obj<::gpk::SEngineScene>	Scene				;
		::gpk::SVirtualEntityManager		ManagedEntities		;
		::gpk::SRigidBodyIntegrator			Integrator			;

		::gpk::error_t						Clone				(uint32_t iEntitySource, bool cloneSkin, bool cloneSurfaces) {
			const ::gpk::SVirtualEntity				entitySource		= ManagedEntities.Entities[iEntitySource];
			int32_t									iEntityNew			= ManagedEntities.Create();
			::gpk::SVirtualEntity					& entityNew			= ManagedEntities.Entities[iEntityNew];
			entityNew.RenderNode				= Scene->ManagedRenderNodes	.Clone(entitySource.RenderNode);
			entityNew.RigidBody					= ((uint32_t)entitySource.RigidBody < Integrator.BodyFlags.size()) ? Integrator.Clone(entitySource.RigidBody) : (uint32_t)-1;
			entityNew.Parent					= entitySource.Parent;

			uint32_t								idSkinSource		= Scene->ManagedRenderNodes.RenderNodes[entityNew.RenderNode].Skin;
			if(cloneSkin && idSkinSource < Scene->ManagedRenderNodes.Skins.size()) {
				uint32_t								idSkin						= Scene->ManagedRenderNodes.CloneSkin(idSkinSource);
				Scene->ManagedRenderNodes.RenderNodes[entityNew.RenderNode].Skin	= idSkin;
				if(cloneSurfaces) {
					if(Scene->ManagedRenderNodes.Skins[idSkin]) {
						::gpk::SSkin							& newSkin					= *Scene->ManagedRenderNodes.Skins[idSkin];
						for(uint32_t iTexture = 0; iTexture < newSkin.Textures.size(); ++iTexture) {
							newSkin.Textures[iTexture]			= Scene->ManagedSurfaces.Clone(newSkin.Textures[iTexture]);
						}
					}
				}
			}

			const ::gpk::ptr_obj<::gpk::array_pod<uint32_t>>	childrenSource	= ManagedEntities.EntityChildren[iEntitySource];
			if(childrenSource && childrenSource->size()) {
				::gpk::ptr_obj<::gpk::array_pod<uint32_t>>			childrenNew		= ManagedEntities.EntityChildren[iEntityNew];
				for(uint32_t iChild = 0; iChild < childrenSource->size(); ++iChild) {
					uint32_t entityChild = Clone((*childrenSource)[iChild], cloneSkin, cloneSurfaces);
					ManagedEntities.Entities[entityChild].Parent	= iEntityNew;
					childrenNew->push_back(entityChild);
				}
				ManagedEntities.EntityChildren[iEntityNew]		= childrenNew;
			}
			return iEntityNew;
		}

		::gpk::error_t						SetScale			(uint32_t iEntity, const ::gpk::SCoord3<float> & scale) {
			Scene->ManagedRenderNodes.RenderNodeBaseTransforms[ManagedEntities.Entities[iEntity].RenderNode].World.Scale(scale, true);
			return 0;
		}

		::gpk::error_t						SetPosition			(uint32_t iEntity, const ::gpk::SCoord3<float> & position) {
			Integrator.SetPosition(ManagedEntities.Entities[iEntity].RigidBody, position);
			return 0;
		}

		::gpk::error_t						SetVelocity			(uint32_t iEntity, const ::gpk::SCoord3<float> & velocity) {
			Integrator.SetVelocity(ManagedEntities.Entities[iEntity].RigidBody, velocity);
			return 0;
		}

		::gpk::error_t						SetRotation			(uint32_t iEntity, const ::gpk::SCoord3<float> & velocity) {
			Integrator.SetRotation(ManagedEntities.Entities[iEntity].RigidBody, velocity);
			return 0;
		}

		::gpk::error_t						SetDampingLinear		(uint32_t iEntity, float damping) {
			Integrator.Masses[ManagedEntities.Entities[iEntity].RigidBody].LinearDamping = damping;
			return 0;
		}

		::gpk::error_t						SetDampingAngular		(uint32_t iEntity, float damping) {
			Integrator.Masses[ManagedEntities.Entities[iEntity].RigidBody].AngularDamping = damping;
			return 0;
		}

		::gpk::error_t						SetOrientation			(uint32_t iEntity, const ::gpk::SQuaternion<float> & orientation) {
			Integrator.SetOrientation(ManagedEntities.Entities[iEntity].RigidBody, orientation);
			return 0;
		}

		::gpk::error_t						SetHidden				(uint32_t iEntity, bool hidden) {
			Scene->ManagedRenderNodes.RenderNodeFlags[ManagedEntities.Entities[iEntity].RenderNode].NoDraw = hidden;
			return 0;
		}

		::gpk::error_t						ToggleHidden			(uint32_t iEntity) {
			::gpk::SRenderNodeFlags					& flags					= Scene->ManagedRenderNodes.RenderNodeFlags[ManagedEntities.Entities[iEntity].RenderNode];
			flags.NoDraw						= !flags.NoDraw;
			return 0;
		}

		::gpk::error_t						CreateLight			(::gpk::LIGHT_TYPE type);
		::gpk::error_t						CreateCamera		();

		::gpk::error_t						CreateSphere		();
		::gpk::error_t						CreateCylinder		();
		::gpk::error_t						CreateBox			();
		::gpk::error_t						CreateCircle		();
		::gpk::error_t						CreateRing			();
		::gpk::error_t						CreateSquare		();
		::gpk::error_t						CreateTriangle		();
		::gpk::error_t						Update				(double secondsLastFrame)			{
			Integrator.Integrate(secondsLastFrame);
			for(uint32_t iEntity = 0; iEntity < ManagedEntities.Entities.size(); ++iEntity) {
				::gpk::SVirtualEntity					& entity			= ManagedEntities.Entities[iEntity];
				if(entity.Parent != -1)
					continue;

				::gpk::updateEntityTransforms(iEntity, entity, ManagedEntities, Integrator, Scene->ManagedRenderNodes);
			}
			return 0;
		}
	};

} // namespace

#endif
