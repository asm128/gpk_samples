#include "gpk_engine_entity.h"
#include "gpk_engine_rendermesh.h"
#include "gpk_engine_renderbuffer.h"
#include "gpk_engine_renderimage.h"
#include "gpk_engine_rendercolor.h"
#include "gpk_engine_rendernode.h"
#include "gpk_rigidbody.h"

#ifndef GPK_ENGINE_H
#define GPK_ENGINE_H

namespace gpk 
{
	struct SEngineScene {
		::gpk::SMeshManager				ManagedMeshes		;
		::gpk::SRenderBufferManager		ManagedBuffers		;
		::gpk::SRenderNodeManager		ManagedRenderNodes	;
	};

	struct SEngine {
		::gpk::SEngineScene				Scene;
		::gpk::SVirtualEntityManager	ManagedEntities		;
		::gpk::SRigidBodyIntegrator		Integrator			;

		::gpk::error_t					CreateSphere		()	{ return 0; }
		::gpk::error_t					CreateCylinder		()	{ return 0; }
		::gpk::error_t					CreateBox			()	{ return 0; }
		::gpk::error_t					CreateCircle		()	{ return 0; }
		::gpk::error_t					CreateRing			()	{ return 0; }
		::gpk::error_t					CreateSquare		()	{ return 0; }
		::gpk::error_t					CreateTriangle		()	{ return 0; }
	};

} // namespace

#endif
