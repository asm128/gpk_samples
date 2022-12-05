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
	struct SEngineScene {
		::gpk::SRenderBufferManager			ManagedBuffers		;
		::gpk::SRenderNodeManager			ManagedRenderNodes	;
		::gpk::SSurfaceManager				ManagedSurfaces		;
		::gpk::SMeshManager					ManagedMeshes		;
	};

	struct SEngine {
		::gpk::ptr_obj<::gpk::SEngineScene>	Scene				;
		::gpk::SVirtualEntityManager		ManagedEntities		;
		::gpk::SRigidBodyIntegrator			Integrator			;

		::gpk::error_t						CreateLight			(::gpk::LIGHT_TYPE type);
		::gpk::error_t						CreateCamera		();

		::gpk::error_t						CreateSphere		();
		::gpk::error_t						CreateCylinder		();
		::gpk::error_t						CreateBox			();
		::gpk::error_t						CreateCircle		();
		::gpk::error_t						CreateRing			();
		::gpk::error_t						CreateSquare		();
		::gpk::error_t						CreateTriangle		();
	};

} // namespace

#endif
