#include "gpk_engine_entity.h"
#include "gpk_engine_rendermesh.h"
#include "gpk_engine_renderbuffer.h"
#include "gpk_engine_rendersurface.h"
#include "gpk_engine_rendernode.h"
#include "gpk_rigidbody.h"

#ifndef GPK_ENGINE_H
#define GPK_ENGINE_H

namespace gpk 
{
	struct SEngineRenderCache {
		::gpk::SVSOutput					OutputVertexShader		= {};
		::gpk::SVSCache						CacheVertexShader		= {};
	};

	struct SEngineScene {
		::gpk::SRenderBufferManager			ManagedBuffers			= {};
		::gpk::SRenderNodeManager			ManagedRenderNodes		= {};
		::gpk::SSurfaceManager				ManagedSurfaces			= {};
		::gpk::SMeshManager					ManagedMeshes			= {};
		::gpk::SEngineRenderCache			RenderCache				= {};
	};

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

		::gpk::error_t						SetPosition			(uint32_t iEntity, const ::gpk::SCoord3<float> & position) {
			Integrator.SetPosition(ManagedEntities.Entities[iEntity].RigidBody, position);
			return 0;
		}

		::gpk::error_t						SetVelocity			(uint32_t iEntity, const ::gpk::SCoord3<float> & velocity) {
			Integrator.SetVelocity(ManagedEntities.Entities[iEntity].RigidBody, velocity);
			return 0;
		}

		::gpk::error_t						SetDampingLinear		(uint32_t iEntity, float damping) {
			Integrator.Masses[ManagedEntities.Entities[iEntity].RigidBody].LinearDamping = damping;
			return 0;
		}

		::gpk::error_t						SetOrientation			(uint32_t iEntity, const ::gpk::SQuaternion<float> & orientation) {
			Integrator.SetOrientation(ManagedEntities.Entities[iEntity].RigidBody, orientation);
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
