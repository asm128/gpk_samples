#include "gpk_the_one.h"
#include "gpk_noise.h"

::gpk::error_t							the1::poolGameReset		(::the1::SPoolGame & pool) {
	pool.StartState.Seed					= ::gpk::timeCurrentInUs();
	for(uint32_t iBall = 0; iBall < ::the1::MAX_BALLS; ++iBall)
		pool.PositionDeltas[iBall].clear();

	::gpk::array_pod<uint32_t>					ballPool				= {};
	uint32_t									ball1					= 1 + ::gpk::noise1DBase(pool.StartState.Seed + 1) % 7;
	uint32_t									ball5					= 9 + ::gpk::noise1DBase(pool.StartState.Seed + 2) % 7;
	for(uint32_t iBall = 0; ballPool.size() < 12; ++iBall) {
		if(iBall == 8)
			continue;
		if(iBall == 0)
			continue;
		if(iBall == ball1)
			continue;
		if(iBall == ball5)
			continue;
		ballPool.push_back(iBall);
	}
	pool.StartState.BallOrder[0] = 0;
	pool.StartState.BallOrder[1] = ball1;
	pool.StartState.BallOrder[2] = ballPool[0];
	pool.StartState.BallOrder[3] = ballPool[ballPool.size() - 1];
	pool.StartState.BallOrder[4] = ballPool[1];
	pool.StartState.BallOrder[5] = ball5;
	pool.StartState.BallOrder[6] = ballPool[ballPool.size() - 2];
	pool.StartState.BallOrder[7] = ballPool[2];
	pool.StartState.BallOrder[8] = ballPool[ballPool.size() - 3];
	pool.StartState.BallOrder[9] = ballPool[3];
	pool.StartState.BallOrder[10] = ballPool[ballPool.size() - 4];
	pool.StartState.BallOrder[11] = 8;
	pool.StartState.BallOrder[12] = ballPool[4];
	pool.StartState.BallOrder[13] = ballPool[ballPool.size() - 5];
	pool.StartState.BallOrder[14] = ballPool[5];
	pool.StartState.BallOrder[15] = ballPool[ballPool.size() - 6];

	pool.Engine.Integrator.ZeroForces();
	for(uint32_t iBall = 0; iBall < ::the1::MAX_BALLS; ++iBall) {
		pool.Engine.SetDampingLinear(pool.StartState.Balls[iBall].Entity, 1.0);
		const ::gpk::SVirtualEntity					& entity				= pool.Engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity];
		const ::gpk::SRenderNode					& renderNode			= pool.Engine.Scene->ManagedRenderNodes.RenderNodes[entity.RenderNode];
		const ::gpk::SRenderMesh					& mesh					= *pool.Engine.Scene->ManagedMeshes.Meshes[renderNode.Mesh];
		const ::gpk::SGeometrySlice					& slice					= mesh.GeometrySlices[renderNode.Slice];
		::gpk::SSkin								& skin					= *pool.Engine.Scene->ManagedMeshes.Skins[slice.Skin];
		::gpk::SRenderMaterial						& material				= skin.Material;
		material.Color.Specular	= material.Color.Diffuse = pool.StartState.BallColors[pool.StartState.BallOrder[iBall]];
  		material.Color.Ambient	= material.Color.Diffuse *.1f;
	}

	pool.Engine.SetPosition(pool.StartState.Balls[0].Entity, {-5, 0.5f, 0});
	::gpk::SCoord3<float>					velocity		= {30.f, 0, 0};
	velocity.RotateY(::gpk::noiseNormal1D(pool.StartState.Seed + 2) / 10 * ((rand() % 2) ? -1 : 1));
	pool.Engine.SetVelocity(pool.StartState.Balls[0].Entity, velocity);
	uint8_t									rowLen			= 5;
	::gpk::SCoord3<float>					diagonal		= {1, 0, 1};
	//diagonal.Normalize();
	for(uint32_t iRow = 0, iBall = 1; iRow < 5; ++iRow, --rowLen) {
		::gpk::SCoord3<float>					offsetZ			= {0, 0, -(rowLen / 2.0f) + .5f};
		for(uint32_t iColumn = 0; iColumn < rowLen; ++iColumn) {
			::gpk::SCoord3<float> position = offsetZ + ::gpk::SCoord3<float>{(+5.f + diagonal.x * 5) - iRow * diagonal.x, 0.5f, iColumn * 1.f};
			uint32_t iEntity = pool.StartState.Balls[iBall++].Entity;
			pool.Engine.SetPosition(iEntity, position);
		}
	}
	return 0;
}

::gpk::error_t							the1::poolGameSetup			(::the1::SPoolGame & pool) {
	for(uint32_t iBall = 0; iBall < ::the1::MAX_BALLS; ++iBall) {
		gpk_necs(pool.StartState.Balls[iBall].Entity = pool.Engine.CreateSphere());
	}
	::the1::poolGameReset(pool);
	return 0;
}
