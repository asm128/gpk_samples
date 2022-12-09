#include "gpk_pool_game.h"
#include "gpk_noise.h"

static	::gpk::error_t					poolGameResetTest2Balls	(::the1::SPoolGame & pool) { 
	pool.StartState.BallCount				= 2;
	pool.Engine.SetPosition(pool.StartState.Balls[0].Entity, {0, pool.StartState.BallRadius,-5});
	pool.Engine.SetPosition(pool.StartState.Balls[1].Entity, {0, pool.StartState.BallRadius, 5});
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		pool.Engine.SetDampingLinear(pool.StartState.Balls[iBall].Entity, pool.StartState.DampingRollDisplacement);
		pool.Engine.SetDampingAngular(pool.StartState.Balls[iBall].Entity, pool.StartState.DampingRollRotation);
		pool.Engine.SetHidden(pool.StartState.Balls[iBall].Entity, false);
	}
	::gpk::SCoord3<float>						velocity				= {0, 0, -25.f};
	velocity.RotateY(::gpk::noiseNormal1D(pool.StartState.Seed + 2) / 20 * ((rand() % 2) ? -1 : 1));
	pool.Engine.SetVelocity(pool.StartState.Balls[1].Entity, velocity);
	return 0; 
}

static	::gpk::error_t					poolGameResetBall10		(::the1::SPoolGame & pool) { (void)pool; return 0; }
static	::gpk::error_t					poolGameResetBall9		(::the1::SPoolGame & pool) { (void)pool; return 0; }
static	::gpk::error_t					poolGameResetBall8		(::the1::SPoolGame & pool) {
	pool.StartState.BallCount				= 16;

	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		const bool								stripped			= iBall > 0 && iBall < 8;
		pool.Engine.SetDampingLinear	(pool.StartState.Balls[iBall].Entity, pool.StartState.DampingRollDisplacement);
		pool.Engine.SetDampingAngular	(pool.StartState.Balls[iBall].Entity, pool.StartState.DampingRollRotation);
		pool.Engine.SetHidden			(pool.StartState.Balls[iBall].Entity, false);
		pool.Engine.SetOrientation		(pool.StartState.Balls[iBall].Entity, {0, 0, 1, 1});
		const ::gpk::SVirtualEntity				& entity			= pool.Engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity];
		const ::gpk::SRenderNode				& renderNode		= pool.Engine.Scene->ManagedRenderNodes.RenderNodes[entity.RenderNode];
		::gpk::SSkin							& skin				= *pool.Engine.Scene->ManagedRenderNodes.Skins[renderNode.Skin];
		::gpk::SSurface							& surface			= *pool.Engine.Scene->ManagedSurfaces.Surfaces[skin.Textures[0]];
		::gpk::SRenderMaterial					& material			= skin.Material;
		::gpk::SColorFloat						color				= pool.StartState.BallColors[iBall];
		material.Color.Specular	= material.Color.Diffuse = color;
  		material.Color.Ambient	= material.Color.Diffuse *.1f;
		::gpk::view_grid<::gpk::SColorBGRA>		view				= {(::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()};
		if(stripped) {
			memset(surface.Data.begin(), 0xFF, surface.Data.size());
			for(uint32_t y = 5; y < 11; ++y)
			for(uint32_t x = 0; x < 16; ++x)
				view[y][x]	= color;
		}
		else {
			for(uint32_t y = 0; y < surface.Desc.Dimensions.x; ++y)
			for(uint32_t x = 0; x < surface.Desc.Dimensions.y; ++x)
				view[y][x]	= color;
		}
	}

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
	pool.StartState.BallOrder[0]	= 0;
	pool.StartState.BallOrder[1]	= ball1;
	pool.StartState.BallOrder[5]	= ball5;
	pool.StartState.BallOrder[11]	= 8;

	{ uint32_t index = ::gpk::noise1DBase32( 1, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[ 2] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32( 2, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[ 3] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32( 3, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[ 4] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32( 4, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[ 6] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32( 5, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[ 7] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32( 6, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[ 8] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32( 7, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[ 9] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32( 8, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[10] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32( 9, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[12] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32(10, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[13] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32(11, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[14] = ballPool[index]; ballPool.remove_unordered(index); }
	{ uint32_t index = ::gpk::noise1DBase32(12, (uint32_t)pool.StartState.Seed) % ballPool.size(); pool.StartState.BallOrder[15] = ballPool[index]; ballPool.remove_unordered(index); }

	pool.Engine.SetPosition(pool.StartState.Balls[0].Entity, {-10, pool.StartState.BallRadius, 0});
	::gpk::SCoord3<float>					velocity			= {40.0f + (rand() % 90), 0, 0};
	velocity.RotateY(::gpk::noiseNormal1D(pool.StartState.Seed + 2) / 10 * ((rand() % 2) ? -1 : 1));
	pool.Engine.SetVelocity(pool.StartState.Balls[0].Entity, velocity);
	//pool.Engine.SetRotation(pool.StartState.Balls[0].Entity, {0, 0, 10});
	uint8_t									rowLen				= 5;
	::gpk::SCoord3<float>					diagonal			= {1, 0, 1};
	diagonal = diagonal.Normalize() * 1.22f; 
	for(uint32_t iRow = 0, iBall = 1; iRow < 5; ++iRow, --rowLen) {
		::gpk::SCoord3<float>					offsetZ				= {0, 0, -(rowLen / 2.0f) + .5f};
		for(uint32_t iColumn = 0; iColumn < rowLen; ++iColumn) {
			::gpk::SCoord3<float>					position			= offsetZ + ::gpk::SCoord3<float>{(10.f + diagonal.x * 5) - iRow * diagonal.x, pool.StartState.BallRadius, (float)iColumn};
			uint32_t								iEntity				= pool.StartState.Balls[pool.StartState.BallOrder[iBall++]].Entity;
			pool.Engine.SetPosition(iEntity, position);
		}
	}
	return 0;
}

::gpk::error_t							the1::poolGameReset		(::the1::SPoolGame & pool, POOL_GAME_MODE mode) {
	pool.StartState.Mode					= mode;
	pool.StartState.Seed					= ::gpk::timeCurrentInUs();
	pool.Engine.Integrator.ZeroForces();
	for(uint32_t iBall = 0; iBall < ::the1::MAX_BALLS; ++iBall) {
		pool.PositionDeltas[iBall].clear();
		pool.Engine.SetHidden	(pool.StartState.Balls[iBall].Entity, true);
		pool.Engine.SetPosition	(pool.StartState.Balls[iBall].Entity, {});
	}

	switch(mode) {
	default:
	case POOL_GAME_MODE_8Ball		: gpk_necs(::poolGameResetBall8		(pool)); break;
	case POOL_GAME_MODE_9Ball		: gpk_necs(::poolGameResetBall9		(pool)); break;
	case POOL_GAME_MODE_10Ball		: gpk_necs(::poolGameResetBall10	(pool)); break;
	case POOL_GAME_MODE_Test2Balls	: gpk_necs(::poolGameResetTest2Balls(pool)); break;
	}
	return 0;
}

::gpk::error_t							the1::poolGameSetup			(::the1::SPoolGame & pool, POOL_GAME_MODE mode) {
	gpk_necs(pool.StartState.Balls[0].Entity = pool.Engine.CreateSphere());
	for(uint32_t iBall = 1; iBall < ::the1::MAX_BALLS; ++iBall) {
		gpk_necs(pool.StartState.Balls[iBall].Entity = pool.Engine.Clone(pool.StartState.Balls[0].Entity, true, true));
	}
	::the1::poolGameReset(pool, mode);
	return 0;
}
