#include "gpk_pool_game.h"
#include "gpk_noise.h"
#include "gpk_gui_text.h"
#include "gpk_pool_shader.h"

static	::gpk::error_t					poolGameResetTest2Balls		(::the1::SPoolGame & pool) { 
	pool.StartState.BallCount				= 2;
	pool.Engine.SetPosition(pool.StartState.Balls[0].Entity, {0, pool.StartState.Balls[0].BallRadius,-5});
	pool.Engine.SetPosition(pool.StartState.Balls[1].Entity, {0, pool.StartState.Balls[1].BallRadius, 5});
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		pool.Engine.SetDampingLinear(pool.StartState.Balls[iBall].Entity, pool.StartState.DampingClothDisplacement);
		pool.Engine.SetDampingAngular(pool.StartState.Balls[iBall].Entity, pool.StartState.DampingClothRotation);
		pool.Engine.SetHidden(pool.StartState.Balls[iBall].Entity, false);
	}
	::gpk::SCoord3<float>						velocity					= {0, 0, -15.f};
	velocity.RotateY(::gpk::noiseNormal1D(pool.StartState.Seed + 2) / 20 * ((rand() % 2) ? -1 : 1));
	pool.Engine.SetVelocity(pool.StartState.Balls[1].Entity, velocity);
	return 0; 
}

static	::gpk::error_t					textureBallNumber			(::gpk::view_grid<::gpk::SColorBGRA> view, uint32_t number, const ::gpk::SRasterFont & font) { 
	char										strNumber[4]				= {};
	sprintf_s(strNumber, "%i", number);
	const ::gpk::SRectangle2<int16_t>			targetRect					= 
		{ {int16_t(view.metrics().x / 2 - (font.CharSize.x * strlen(strNumber)) / 2), int16_t(view.metrics().y / 2 - font.CharSize.y / 2)}
		, font.CharSize.Cast<int16_t>()
		};
	::gpk::array_pod<::gpk::SCoord2<uint16_t>>	coords;
	::gpk::textLineRaster(view.metrics().Cast<uint16_t>(), font.CharSize, targetRect, font.Texture, strNumber, coords);
	for(uint32_t iCoord = 0; iCoord < coords.size(); ++iCoord) {
		const ::gpk::SCoord2<uint16_t>				coord						= coords[iCoord];
		view[coord.y][coord.x]					= ::gpk::BLACK;
	}
	return 0; 
}

static	::gpk::error_t					poolGameResetBall10		(::the1::SPoolGame & pool) { (void)pool; return 0; }
static	::gpk::error_t					poolGameResetBall9		(::the1::SPoolGame & pool) { (void)pool; return 0; }
static	::gpk::error_t					poolGameResetBall8		(::the1::SPoolGame & pool) {
	pool.StartState.BallCount				= 16;
	const ::gpk::SRasterFont					& font					= *pool.Engine.Scene->ManagerFonts.Fonts[9];
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		//const bool									stripped				= iBall && iBall > 8;
		pool.Engine.SetDampingLinear	(pool.StartState.Balls[iBall].Entity, pool.StartState.DampingClothDisplacement);
		pool.Engine.SetDampingAngular	(pool.StartState.Balls[iBall].Entity, pool.StartState.DampingClothRotation);
		pool.Engine.SetHidden			(pool.StartState.Balls[iBall].Entity, false);
		pool.Engine.SetOrientation		(pool.StartState.Balls[iBall].Entity, {0, 0, 1, -1});
		const ::gpk::SVirtualEntity					& entity				= pool.Engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity];
		const ::gpk::SRenderNode					& renderNode			= pool.Engine.Scene->ManagedRenderNodes.RenderNodes[entity.RenderNode];
		::gpk::SSkin								& skin					= *pool.Engine.Scene->ManagedRenderNodes.Skins[renderNode.Skin];
		::gpk::SSurface								& surface				= *pool.Engine.Scene->ManagedSurfaces.Surfaces[skin.Textures[0]];
		::gpk::SRenderMaterial						& material				= skin.Material;
		::gpk::SColorFloat							color					= pool.StartState.BallColors[iBall];
		material.Color.Specular					= ::gpk::WHITE;
		material.Color.Diffuse					= color;
  		material.Color.Ambient					= material.Color.Diffuse * .1f;
		if(iBall) {
			::gpk::view_grid<::gpk::SColorBGRA>			view					= {(::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()};
			textureBallNumber(view, iBall, font);
			//if(0 == iBall)
			//	textureBallCue(view, ::gpk::RED);
			//else if(stripped)
			//	textureBallStripped(view, font, color, iBall);
			//else
			//	textureBallSolid(view, font, color, iBall);
		}
	}

	const uint32_t								ball1					= 1 + ::gpk::noise1DBase(pool.StartState.Seed + 1) % 7;
	const uint32_t								ball5					= 9 + ::gpk::noise1DBase(pool.StartState.Seed + 5) % 7;
	pool.StartState.BallOrder[0]			= 0;
	pool.StartState.BallOrder[1]			= ball1;
	pool.StartState.BallOrder[5]			= ball5;
	pool.StartState.BallOrder[11]			= 8;

	::gpk::array_pod<uint32_t>					ballPool				= {};
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

	constexpr char								ballsToSet	[12]		= {2, 3, 4, 6, 7, 8, 9, 10, 12, 13, 14, 15};
	for(uint32_t iBallToSet = 0; iBallToSet < 12; ++iBallToSet) { 
		uint32_t index = ::gpk::noise1DBase32((uint32_t)pool.StartState.Seed + iBallToSet) % ballPool.size(); 
		pool.StartState.BallOrder[ballsToSet[iBallToSet]] = ballPool[index]; 
		ballPool.remove_unordered(index); 
	}

	const float									distanceFromCenter		= pool.StartState.Table.Size.x / 4;

	pool.Engine.SetPosition(pool.StartState.Balls[0].Entity, {-distanceFromCenter / 4, pool.StartState.Balls[0].BallRadius, 0});
	::gpk::SCoord3<float>						velocity				= {10.0f + (rand() % 150), 0, 0}; //{70.0f + (rand() % 90), 0, 0};
	float										reverse					= (rand() % 2) ? -1.0f : 1.0f;
	velocity.RotateY(::gpk::noiseNormal1D(pool.StartState.Seed + 2) / 50 + .1f * reverse);
	pool.Engine.SetVelocity(pool.StartState.Balls[0].Entity, velocity);
	pool.Engine.SetRotation(pool.StartState.Balls[0].Entity, {0, (30.0f + (rand() % 50) * 2) * -reverse, 0});
	uint8_t										rowLen					= 5;
	::gpk::SCoord3<float>						diagonal				= {1, 0, 1};
	diagonal								= diagonal.Normalize() * 1.22f; 

	for(uint32_t iRow = 0, iBall = 1; iRow < 5; ++iRow, --rowLen) {
		float										offsetZ					= -(rowLen / 2.0f) + .5f;
		for(uint32_t iColumn = 0; iColumn < rowLen; ++iColumn) {
			::gpk::SCoord3<float>						position				= ::gpk::SCoord3<float>{(distanceFromCenter + diagonal.x * 5) - iRow * diagonal.x, pool.StartState.Balls[iBall].BallRadius, offsetZ + (float)iColumn};
			uint32_t									iEntity					= pool.StartState.Balls[pool.StartState.BallOrder[iBall++]].Entity;
			pool.Engine.SetPosition(iEntity, position);
			pool.Engine.SetRotation(iEntity, {0, 0, 0});
		}
	}
	return 0;
}

::gpk::error_t							the1::poolGameReset		(::the1::SPoolGame & pool, POOL_GAME_MODE mode) {
	pool.StartState.Mode					= mode;
	pool.StartState.Seed					= ::gpk::timeCurrentInUs() ^ ::gpk::noise1DBase(::gpk::timeCurrentInUs());
	pool.Engine.Integrator.ZeroForces();
	for(uint32_t iBall = 0; iBall < ::the1::MAX_BALLS; ++iBall) {
		pool.PositionDeltas[iBall].clear();
		pool.Engine.SetHidden	(pool.StartState.Balls[iBall].Entity, true);
		pool.Engine.SetPosition	(pool.StartState.Balls[iBall].Entity, {});
	}

	pool.Engine.SetPosition	(pool.StartState.Table.Entity, {0, -3});
	const ::gpk::SCoord2<float>					tableCenter				= {pool.StartState.Table.Size.x * .5f, pool.StartState.Table.Size.y * .5f};
	for(uint32_t iPocket = 0; iPocket < 6; ++iPocket) {
		const uint32_t								row						= iPocket / 3;
		const uint32_t								column					= iPocket % 3;
		const ::gpk::SCoord3<float>					pocketPosition			= {tableCenter.x * column - tableCenter.x, -1.35f, pool.StartState.Table.Size.y * row - tableCenter.y};
		pool.Engine.SetHidden	(pool.StartState.Table.Pockets[iPocket].Entity, false);
		pool.Engine.SetPosition	(pool.StartState.Table.Pockets[iPocket].Entity, pocketPosition);
	}

	const ::gpk::SCoord2<float>					pocketWidth					= {.5, .5};
	const ::gpk::SCoord2<float>					pocketCenter				= pocketWidth * .5f;
	for(uint32_t iPocket = 0; iPocket < 6; ++iPocket) {
		const uint32_t								row						= iPocket / 3;
		const uint32_t								column					= iPocket % 3;
		const ::gpk::SCoord3<float>					pocketPosition			= {-(pocketCenter.x * column - pocketCenter.x), -.15f, -(pocketWidth.y * row - pocketCenter.y)};
		pool.Engine.SetPosition	((*pool.Engine.ManagedEntities.EntityChildren[pool.StartState.Table.Pockets[iPocket].Entity])[6], pocketPosition);
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
	gpk_necs(::gpk::rasterFontDefaults(pool.Engine.Scene->ManagerFonts));

	// balls
	gpk_necs(pool.StartState.Balls[0].Entity = pool.Engine.CreateSphere());
	pool.Engine.Integrator.BodyFlags[pool.Engine.ManagedEntities.Entities[pool.StartState.Balls[0].Entity].RigidBody].Collides	= true;;
	pool.Engine.Scene->ManagedShaders.Shaders[pool.Engine.Scene->ManagedRenderNodes.RenderNodes[pool.Engine.ManagedEntities.Entities[pool.StartState.Balls[0].Entity].RenderNode].Shader] = ::the1::shaderBall;
	for(uint32_t iBall = 1; iBall < ::the1::MAX_BALLS; ++iBall) {
		gpk_necs(pool.StartState.Balls[iBall].Entity = pool.Engine.Clone(pool.StartState.Balls[0].Entity, true, true));
	}

	// table
	gpk_necs(pool.StartState.Table.Entity = pool.Engine.CreateBox());
	pool.Engine.SetScale(pool.StartState.Table.Entity, {44, 6, 22});

	// pockets
	gpk_necs(pool.StartState.Table.Pockets[0].Entity = pool.Engine.CreateBox());
	uint32_t iEntity = pool.Engine.CreateCylinder();
	pool.Engine.ManagedEntities.EntityChildren[pool.StartState.Table.Pockets[0].Entity]->push_back(iEntity);
	pool.Engine.ManagedEntities.Entities[iEntity].Parent	= pool.StartState.Table.Pockets[0].Entity;
	pool.Engine.SetScale(pool.StartState.Table.Pockets[0].Entity, {3, 3, 3});
	pool.Engine.SetScale(iEntity, {.5, .5, .5});
	for(uint32_t iPocket = 1; iPocket < 6; ++iPocket) {
		gpk_necs(pool.StartState.Table.Pockets[iPocket].Entity = pool.Engine.Clone(pool.StartState.Table.Pockets[0].Entity, false, false));
	}

	// sticks
	gpk_necs(pool.StartState.Players[0].Stick.Entity = pool.Engine.CreateCylinder());
	for(uint32_t iPlayer = 1; iPlayer < ::gpk::size(pool.StartState.Players); ++iPlayer) {
		gpk_necs(pool.StartState.Players[iPlayer].Stick.Entity = pool.Engine.Clone(pool.StartState.Players[0].Stick.Entity, true, true));
	}
	::the1::poolGameReset(pool, mode);
	return 0;
}

//
//static	::gpk::error_t					textureBallStripped		(::gpk::view_grid<::gpk::SColorBGRA> view, const ::gpk::SRasterFont & font, ::gpk::SColorBGRA color, uint32_t number) { 
//	memset(view.begin(), 0xFF, view.byte_count());
//
//	::gpk::SCoord2<uint32_t>					viewCenter				= view.metrics() / 2;
//	::gpk::SSlice<uint16_t>						colorBand				= {uint16_t(view.metrics().y / 3), uint16_t(view.metrics().y / 3 * 2)};
//	for(uint32_t y = colorBand.Begin; y < colorBand.End; ++y)
//	for(uint32_t x = 0; x < view.metrics().x; ++x) {
//		if((viewCenter - ::gpk::SCoord2<uint32_t>{x, y}).Length() < view.metrics().y / 7)
//			view[y][x]								= ::gpk::WHITE;
//		else
//			view[y][x]								= color;
//	}
//
//	::textureBallNumber(view, number, font);
//	return 0; 
//}
//
//static	::gpk::error_t					textureBallSolid		(::gpk::view_grid<::gpk::SColorBGRA> view, const ::gpk::SRasterFont & font, ::gpk::SColorBGRA color, uint32_t number) { 
//	::gpk::SCoord2<uint32_t>					viewCenter				= view.metrics() / 2;
//
//	for(uint32_t y = 0; y < view.metrics().y; ++y)
//	for(uint32_t x = 0; x < view.metrics().x; ++x) {
//		if((viewCenter - ::gpk::SCoord2<uint32_t>{x, y}).Length() < view.metrics().y / 7)
//			view[y][x]								= ::gpk::WHITE;
//		else
//			view[y][x]								= color;
//	}
//
//	::textureBallNumber(view, number, font);
//	return 0; 
//}
//
//static	::gpk::error_t					textureBallCue			(::gpk::view_grid<::gpk::SColorBGRA> view, ::gpk::SColorBGRA color) {
//	::gpk::SCoord2<float>						viewCenter				= view.metrics().Cast<float>() / 2;
//	::gpk::SCoord2<float>						pointCenters[]			= 
//		{ {0, viewCenter.y}
//		, {view.metrics().x / 4.0f * 1, viewCenter.y}
//		, {view.metrics().x / 4.0f * 2, viewCenter.y}
//		, {view.metrics().x / 4.0f * 3, viewCenter.y}
//		, {view.metrics().x * 1.0f, viewCenter.y}
//		};
//	float										pointRadius				= view.metrics().y / 16.0f;
//	if(0 == pointRadius)
//		pointRadius = 3;
//
//	memset(view.begin(), 0xFF, view.byte_count());
//	for(uint32_t y = 0; y < view.metrics().y; ++y)
//	for(uint32_t x = 0; x < view.metrics().x; ++x) {
//		if(y <= pointRadius)
//			view[y][x]								= color;
//		else if(y >= (view.metrics().y - pointRadius - 1.0f))
//			view[y][x]								= color;
//		else {
//			for(uint32_t iPoint = 0; iPoint < ::gpk::size(pointCenters); ++iPoint) {
//				::gpk::SCoord2<float>					pointCenter			= pointCenters[iPoint];
//				if((pointCenter - ::gpk::SCoord2<float>{x + .0f, y + .0f}).Length() <= pointRadius + 1.0f)
//					view[y][x]								= color;
//			}
//		}
//	}
//
//	return 0; 
//}
