// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"
#include "gpk_grid_copy.h"
#include "gpk_bitmap_target.h"

#define Y_PLUS -

static	::gpk::error_t						drawShipHealthBar			(::gpk::v2<::gpk::bgra> target, ::SApplication & app, const ::gpk::n2f & centerEnemy, const ::gpk::n2<int32_t> & halfMetrics, uint32_t health, int32_t yOffset, const ::gpk::bgra & finalColor)											{
	::gpk::line2<int32_t>							healthBar					= {};
	healthBar.A 								= {(int32_t)(centerEnemy.x  + .5f - halfMetrics.x), (int32_t)(centerEnemy.y Y_PLUS yOffset)};
	healthBar.B									= healthBar.A; //{(int32_t)(centerEnemy.x  + .5f + halfMetrics.x), (int32_t)(centerEnemy.y + yOffset)};
	healthBar.B.x								+= halfMetrics.x * 2;
	double											enemyHealthProportion		= health / 5000.0;
	healthBar.B.x								= ::gpk::interpolate_linear(healthBar.A.x, healthBar.B.x, enemyHealthProportion);
	app.CacheLinePoints.clear();
	::gpk::drawLine(target.metrics().Cast<uint16_t>(), healthBar, app.CacheLinePoints);
	for(uint32_t iLinePoint = 0, pointCount = app.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
		const ::gpk::n2f								& pointToDraw								= app.CacheLinePoints[iLinePoint].Cast<float>();
		::gpk::drawPixelLight(target, pointToDraw, finalColor, .2f, 1.5f);
	}
	return 0;
}
//
::gpk::error_t								drawShips								(::gpk::v2<::gpk::bgra> target, ::SApplication & app)											{
	::SGame											& gameInstance							= app.Game;
	::gpk::SFramework				& framework								= app.Framework;
	::gpk::view_grid<::gpk::bgra>				& viewOffscreen							= framework.RootWindow.BackBuffer->Color.View;
	// ---- Draw enemy ships
	const ::gpk::view_grid<::gpk::bgra>		& enemyView								= app.Processed[GAME_TEXTURE_ENEMY].View;
	char											indexPositionsX[]						= {0, 1, 2, 3, 2, 1, 0, -1, -2, -3, -2, -1};
	const ::gpk::n2<int32_t>						halfMetricsEnemy						= (enemyView.metrics() / 2).Cast<int32_t>();
	int32_t											halfMetricsEnemy2y						= halfMetricsEnemy.y;
	// ---- Draw ghosts.
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		stacxpr ::gpk::n2f						reference								= {1, 0};
		::gpk::n2f										vector;
		static double									beaconTimer								= 0;
		beaconTimer									+= framework.FrameInfo.Seconds.LastFrame * 8;
		int32_t											selectedPos								= ((int32_t)beaconTimer % ::gpk::size(indexPositionsX));
		for(uint32_t iGhost = 0; iGhost < 5; ++iGhost) {
			vector										= reference * (64 * sin(framework.FrameInfo.Seconds.Total));
			vector.Rotate(::gpk::math_2pi / 5 * iGhost + gameInstance.Enemies.TimeLived[iEnemy]);
			::gpk::n2<int32_t>								dstOffset								= (gameInstance.Enemies.Position[iEnemy] + vector).Cast<int32_t>() - app.TextureCenters[GAME_TEXTURE_ENEMY];
			//if constexpr((Y_PLUS 1) == -1)
			//	dstOffset.y *= -1;
			gerror_if(errored(::gpk::grid_copy_alpha(viewOffscreen, enemyView, dstOffset, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
			{ // Draw ghost light
				::gpk::n2f										centerPowerup							= gameInstance.Enemies.Position[iEnemy] + vector;
				::gpk::n2f										lightCrosshair							= centerPowerup + ::gpk::n2f{(float)indexPositionsX[selectedPos], 0.0f};
				::gpk::drawPixelLight(viewOffscreen, lightCrosshair.Cast<float>(), ::gpk::bgra(::gpk::YELLOW), .2f, 3.0f);
			}
		}
	}
	// --- Draw enemy ships.
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		const ::gpk::n2f								& centerEnemy								= gameInstance.Enemies.Position[iEnemy];
		const ::SHealthPoints							& enemyHealth								= gameInstance.Enemies.Health[iEnemy];
		gerror_if(errored(::gpk::grid_copy_alpha(viewOffscreen, enemyView, centerEnemy.Cast<int32_t>() - app.TextureCenters[GAME_TEXTURE_ENEMY], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		::drawShipHealthBar(target, app, centerEnemy, halfMetricsEnemy, enemyHealth.Health, (int32_t)(halfMetricsEnemy2y	), ::gpk::GREEN);
		::drawShipHealthBar(target, app, centerEnemy, halfMetricsEnemy, enemyHealth.Shield, (int32_t)(halfMetricsEnemy2y - 2), ::gpk::CYAN );
	}
	// --- Draw player ships.
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		const ::gpk::v2<::gpk::bgra>											& shipView									= app.Processed[GAME_TEXTURE_SHIP0 + iShip].View;
		const ::gpk::n2f														& centerShip								= gameInstance.Ships.Position	[iShip];
		const ::SHealthPoints														& enemyHealth								= gameInstance.Ships.Health	[iShip];
		::gpk::n2<int32_t>														halfMetricsShip								= (shipView.metrics() / 2).Cast<int32_t>();
		int32_t																		halfMetricsShip2y							= halfMetricsShip.y;
		::gpk::n2<int32_t>														dstOffset									= gameInstance.Ships.Position[iShip].Cast<int32_t>() - app.TextureCenters[GAME_TEXTURE_SHIP0 + iShip];
		gerror_if(errored(::gpk::grid_copy_alpha(viewOffscreen, shipView, dstOffset, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		::drawShipHealthBar(target, app, centerShip, halfMetricsShip, enemyHealth.Health, (int32_t)(halfMetricsShip2y		), ::gpk::GREEN);
		::drawShipHealthBar(target, app, centerShip, halfMetricsShip, enemyHealth.Shield, (int32_t)(halfMetricsShip2y - 2	), ::gpk::CYAN );
	}
	// --- Draw enemy ship lights.
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		const ::gpk::n2f													& centerEnemy								= gameInstance.Enemies.Position[iEnemy];
		static double																beaconTimer									= 0;
		beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 8;
		int32_t																		selectedPos									= ((int32_t)beaconTimer % ::gpk::size(indexPositionsX));
		::gpk::n2f														lightCrosshair								= centerEnemy + ::gpk::n2f{(float)indexPositionsX[selectedPos], 0.0f};
		::gpk::drawPixelLight(viewOffscreen, lightCrosshair.Cast<float>(), ::gpk::bgra(::gpk::RED), .2f, 3.0f);
	}
	return 0;
}

					::gpk::error_t										drawCollisions								(::gpk::v2<::gpk::bgra> target, ::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(uint32_t iRay = 0, rayCount = app.StuffToDraw.CollisionPoints.size(); iRay < rayCount; ++iRay) {
		const ::gpk::n2f													& pointToDraw									= app.StuffToDraw.CollisionPoints[iRay];
		::gpk::drawPixelLight(target, pointToDraw, ::gpk::bgra(::gpk::ORANGE), .15f, 3.0f);
	}
	for(uint32_t iRay = 0, rayCount = app.StuffToDraw.Debris.size(); iRay < rayCount; ++iRay) {
		const ::SParticleToDraw														& particleToDraw								= app.StuffToDraw.Debris[iRay];
		const ::gpk::n2f													& pointToDraw									= particleToDraw.Position.Cast<float>();
		if(app.ParticleSystemDebris.Instances[particleToDraw.IndexParticleInstance].Binding.Lit) {
			::gpk::bgra															finalColor
				= (0 == (particleToDraw.IndexParticlePhysics % 3)) ? ::gpk::bgra(::gpk::YELLOW)
				: (0 == (particleToDraw.IndexParticlePhysics % 2)) ? ::gpk::bgra(::gpk::RED)
				: ::gpk::bgra(::gpk::ORANGE)
				;
			finalColor																*= 1.0f - (particleToDraw.TimeLived);
			::gpk::drawPixelLight(target, pointToDraw, finalColor, .15f, ::gpk::max(0.0f, 1.0f - (particleToDraw.TimeLived)) * 4.0f);
		}
	}
	return 0;
}

static	const ::gpk::astatic<::gpk::bgra, WEAPON_TYPE_COUNT>	weaponTypeColorPalette			=
	{ ::gpk::bgra{::gpk::WHITE			} // WEAPON_TYPE_ARROW
	, ::gpk::bgra{::gpk::LIGHTGRAY		} // WEAPON_TYPE_BULLET
	, ::gpk::bgra{::gpk::RED			} // WEAPON_TYPE_LASER
	, ::gpk::bgra{::gpk::LIGHTCYAN		} // WEAPON_TYPE_PLASMA
	, ::gpk::bgra{::gpk::LIGHTCYAN		} // WEAPON_TYPE_BUBBLE
	, ::gpk::bgra{::gpk::LIGHTYELLOW	} // WEAPON_TYPE_SPARK
	, ::gpk::bgra{::gpk::LIGHTGREEN 	} // WEAPON_TYPE_POISON
	, ::gpk::bgra{::gpk::RED			} // WEAPON_TYPE_FIRE
	, ::gpk::bgra{::gpk::GRAY			} // WEAPON_TYPE_ROCK
	, ::gpk::bgra{::gpk::WHITE			} // WEAPON_TYPE_GRAVITY
	};

::gpk::error_t										drawShots									(::gpk::v2<::gpk::bgra> target, ::SApplication & app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(uint32_t iRay = 0, rayCount = app.StuffToDraw.ProjectilePaths.size(); iRay < rayCount; ++iRay) {
		const ::SLaserToDraw														& laserToDraw								= app.StuffToDraw.ProjectilePaths[iRay];
		app.CacheLinePoints.clear();
		::gpk::drawLine(target.metrics().Cast<uint16_t>(), laserToDraw.Segment, app.CacheLinePoints);
		const ::gpk::SParticleBinding<::SGameParticle>								& gameParticle								= app.ParticleSystemProjectiles.Instances[laserToDraw.IndexParticleInstance];
		const float																	lightRange									= (gameParticle.Binding.TypePlayer == PLAYER_TYPE_PLAYER) ? 3.0f : 2.5f;
		const float																	lightValue									= (gameParticle.Binding.TypePlayer == PLAYER_TYPE_PLAYER) ? .15f : 1.0f;
		const ::gpk::bgra														finalColor									= weaponTypeColorPalette[gameParticle.Binding.TypeWeapon];

		if(0 == app.CacheLinePoints.size()) {
			const ::gpk::n2f													& pointToDraw								= app.StuffToDraw.ProjectilePaths[iRay].Segment.A;
			::gpk::drawPixelLight(target, pointToDraw, finalColor, lightValue, lightRange);
		}
		else
			for(uint32_t iLinePoint = 0, pointCount = app.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
				const ::gpk::n2f													& pointToDraw								= app.CacheLinePoints[iLinePoint].Cast<float>();
				::gpk::drawPixelLight(target, pointToDraw, finalColor, lightValue, lightRange);
			}
	}
	return 0;
}
					::gpk::error_t										drawBackground								(::gpk::v2<::gpk::bgra> target, ::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::drawRectangle(target, app.ColorBackground, ::gpk::rect2<uint32_t>{{}, target.metrics()});
	for(uint32_t iRay = 0, rayCount = app.StuffToDraw.Stars.size(); iRay < rayCount; ++iRay) {
		::SParticleToDraw															& starToDraw								= app.StuffToDraw.Stars[iRay];
		if(false == app.ParticleSystemStars.Instances[starToDraw.IndexParticleInstance].Binding.Lit)
			continue;
		::gpk::n2<int32_t>														& particlePosition							= starToDraw.Position;
		target[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]
			= (0 == (starToDraw.IndexParticlePhysics % 7))	? ::gpk::DARKYELLOW	/ 2.0f
			: (0 == (starToDraw.IndexParticlePhysics % 6))	? ::gpk::GRAY
			: (0 == (starToDraw.IndexParticlePhysics % 5))	? ::gpk::WHITE
			: (0 == (starToDraw.IndexParticlePhysics % 4))	? ::gpk::DARKGRAY
			: (0 == (starToDraw.IndexParticlePhysics % 3))	? ::gpk::GRAY
			: (0 == (starToDraw.IndexParticlePhysics % 2))	? ::gpk::WHITE
			: ::gpk::DARKGRAY
			;
		float																		maxFactor	= .5f;
		float																		range		= 3.f;
		maxFactor																= (rand() % 3 + 1) * 0.10f;
		range																	= starToDraw.IndexParticlePhysics % 3 + 1.0f;
		::gpk::drawPixelLight(target, particlePosition.Cast<float>(), target[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], maxFactor, range);
	}
	return 0;
}
					::gpk::error_t										drawThrust								(::gpk::v2<::gpk::bgra> target, ::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::SGame																		& gameInstance								= app.Game;
	for(uint32_t iThrust = 0, particleCount = (uint32_t)app.StuffToDraw.Thrust.size(); iThrust < particleCount; ++iThrust) {
		::SParticleToDraw															& thrustToDraw								= app.StuffToDraw.Thrust[iThrust];
		::SApplication::TParticleInstance											& particleInstance							= app.ParticleSystemThrust.Instances[thrustToDraw.IndexParticleInstance];
		if(false == particleInstance.Binding.Lit)
			continue;
		::SShipState																& shipState									= (particleInstance.Binding.TypePlayer == PLAYER_TYPE_PLAYER)
			? gameInstance.Ships	.States[particleInstance.Binding.OwnerIndex]
			: gameInstance.Enemies	.States[particleInstance.Binding.OwnerIndex]
			;
		const int32_t																physicsId									= thrustToDraw.IndexParticlePhysics;
		const ::gpk::n2f													& particlePosition							= app.ParticleSystemThrust.Integrator.Particle[physicsId].Position;
		if(false == ::gpk::in_range(particlePosition, {{}, target.metrics().Cast<float>()}))
			continue;
		target[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]
			= (thrustToDraw.TimeLived > .075)		? (shipState.Thrust ? ::gpk::DARKGRAY	: ::gpk::DARKGRAY	)
			: (thrustToDraw.TimeLived > .03 )		? (shipState.Thrust ? ::gpk::GRAY		: ::gpk::GRAY 		)
			: (physicsId % 3)						? (shipState.Thrust ? ::gpk::CYAN		: ::gpk::RED 		)
			: (physicsId % 2)						? (shipState.Thrust ? ::gpk::WHITE		: ::gpk::ORANGE		)
			: ::gpk::YELLOW
			;
		float																		maxFactor	= .5f;
		float																		range		= 3.f;
		maxFactor																*= (1.0f - ::gpk::min(1.0f, thrustToDraw.TimeLived / 4));
		range																	= physicsId % 2 + (1.0f - ::gpk::min(1.0f, thrustToDraw.TimeLived / 4));
		::gpk::drawPixelLight(target, particlePosition, target[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], maxFactor, range);
	}
	return 0;
}

static	const ::gpk::bgra								powerupFamilyColorPalette []				=
	{ ::gpk::LIGHTYELLOW
	, ::gpk::LIGHTGREEN
	, ::gpk::LIGHTCYAN
	, ::gpk::RED
	};

static	::gpk::error_t										drawPowerup						(::gpk::v2<::gpk::bgra> target, POWERUP_FAMILY powFamily, const ::gpk::view<::gpk::view_grid<::gpk::bgra>>& texturePowerup, const ::gpk::n2<int32_t>& textureCenterPowerup, const ::gpk::n2f& powPosition, const ::gpk::view<const ::gpk::n2<int32_t>>& lightPos, double time)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::n2<int32_t>														position									= powPosition.Cast<int32_t>();
	for(uint32_t iTex = 0, textureCount = texturePowerup.size(); iTex < textureCount; ++iTex)
		gerror_if(errored(::gpk::grid_copy_alpha(target, texturePowerup[iTex], position - textureCenterPowerup, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	::gpk::n2<int32_t>															centerPowerup								= position;
	uint32_t																	lightIndex									= (uint32_t)time % (lightPos.size() / 2);
	const ::gpk::n2<int32_t>												& selectedLightPos0							= lightPos[lightIndex + 0]
		,																		& selectedLightPos2							= lightPos[lightIndex + 4]
		;
	const ::gpk::bgra														& selectedColor								= powerupFamilyColorPalette[powFamily];
	::gpk::drawPixelLight(target, selectedLightPos0.Cast<float>(), selectedColor, .3f, 3.0f);
	::gpk::drawPixelLight(target, selectedLightPos2.Cast<float>(), selectedColor, .3f, 3.0f);
	return 0;
}

stacxpr	int32_t												PWERUP_HALF_WIDTH							= 6;
stacxpr	const ::gpk::n2<int32_t>						squarePowerupLightPositions		[8]			=
	{ ::gpk::n2<int32_t>{-PWERUP_HALF_WIDTH    , -PWERUP_HALF_WIDTH - 1}
	, ::gpk::n2<int32_t>{-PWERUP_HALF_WIDTH - 1, -PWERUP_HALF_WIDTH}
	, ::gpk::n2<int32_t>{-PWERUP_HALF_WIDTH - 1,  PWERUP_HALF_WIDTH - 1}
	, ::gpk::n2<int32_t>{-PWERUP_HALF_WIDTH    ,  PWERUP_HALF_WIDTH}
	, ::gpk::n2<int32_t>{ PWERUP_HALF_WIDTH - 1,  PWERUP_HALF_WIDTH}
	, ::gpk::n2<int32_t>{ PWERUP_HALF_WIDTH    ,  PWERUP_HALF_WIDTH - 1}
	, ::gpk::n2<int32_t>{ PWERUP_HALF_WIDTH    , -PWERUP_HALF_WIDTH}
	, ::gpk::n2<int32_t>{ PWERUP_HALF_WIDTH - 1, -PWERUP_HALF_WIDTH - 1}
	};

stacxpr	const ::gpk::n2<int32_t>						diagonalPowerupLightPositions	[8]			=
	{ ::gpk::n2<int32_t>{-1, -PWERUP_HALF_WIDTH - 1}
	, ::gpk::n2<int32_t>{ 0, -PWERUP_HALF_WIDTH - 1}
	, ::gpk::n2<int32_t>{PWERUP_HALF_WIDTH, -1}
	, ::gpk::n2<int32_t>{PWERUP_HALF_WIDTH,  0}
	, ::gpk::n2<int32_t>{ 0, PWERUP_HALF_WIDTH}
	, ::gpk::n2<int32_t>{-1, PWERUP_HALF_WIDTH}
	, ::gpk::n2<int32_t>{-PWERUP_HALF_WIDTH - 1,  0}
	, ::gpk::n2<int32_t>{-PWERUP_HALF_WIDTH - 1, -1}
	};

					::gpk::error_t										drawPowerups								(::gpk::v2<::gpk::bgra> target, ::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework				& framework									= app.Framework;
	static double																timer										= 0;
	timer																	+= framework.FrameInfo.Seconds.LastFrame * 2;
	::SGame																		& gameInstance								= app.Game;

	for(uint32_t iPow = 0, powCount = gameInstance.Powerups.Alive.size(); iPow < powCount; ++iPow) {
		if(0 == gameInstance.Powerups.Alive[iPow])
			continue;
		POWERUP_FAMILY																powFamily									= gameInstance.Powerups.Family[iPow];
		::gpk::n2<int32_t>														position									= gameInstance.Powerups.Position[iPow].Cast<int32_t>();
		::gpk::n2<int32_t>														lightPos [8]								;
		::gpk::view<::gpk::view_grid<::gpk::bgra>>						texturePowerup								;
		::gpk::n2<int32_t>														textureCenterPowerup						;
		if(powFamily == POWERUP_FAMILY_HEALTH || powFamily == POWERUP_FAMILY_BUFF) { // draw square powerup box
			for(uint32_t i = 0; i < ::gpk::size(lightPos); ++i)
				lightPos[i]																= position + squarePowerupLightPositions[i];
			texturePowerup															= app.StuffToDraw.TexturesPowerup0;
			textureCenterPowerup													= app.TextureCenters[GAME_TEXTURE_POWCORESQUARE];
		}
		else { // draw diagonal powerup box
			for(uint32_t i = 0; i < ::gpk::size(lightPos); ++i)
				lightPos[i]																= position + diagonalPowerupLightPositions[i];
			texturePowerup															= app.StuffToDraw.TexturesPowerup1;
			textureCenterPowerup													= app.TextureCenters[GAME_TEXTURE_POWCOREDIAGONAL];
		}
		gpk_necall(::drawPowerup(target, powFamily, texturePowerup, textureCenterPowerup, position.Cast<float>(), lightPos, timer), "Why would this ever happen?");
	}
	return 0;
}


static				::gpk::error_t										drawCrosshairDiagonal						(::gpk::v2<::gpk::bgra> target, double beaconTimer, const ::gpk::n2<int32_t>	& centerCrosshair)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	int32_t																		halfWidth									= 10 - ((int32_t)beaconTimer % 11);
	::gpk::n2<int32_t>															lightCrosshair []							=
		{ centerCrosshair + ::gpk::n2<int32_t>{ halfWidth,  halfWidth }
		, centerCrosshair + ::gpk::n2<int32_t>{ halfWidth, -halfWidth - 1 }
		, centerCrosshair + ::gpk::n2<int32_t>{-halfWidth - 1, -halfWidth - 1 }
		, centerCrosshair + ::gpk::n2<int32_t>{-halfWidth - 1,  halfWidth }
		};
	for(uint32_t iPoint = 0, pointCount = ::gpk::size(lightCrosshair); iPoint < pointCount; ++iPoint) {
		::gpk::n2<int32_t>														& pointToTest								= lightCrosshair[iPoint];
		::gpk::drawPixelLight(target, pointToTest.Cast<float>(), ::gpk::bgra(::gpk::RED), .2f, 3.0f);
	}
	return 0;
}

static				::gpk::error_t										drawCrosshairAligned						(::gpk::v2<::gpk::bgra> target, double beaconTimer, const ::gpk::n2<int32_t>	& centerCrosshair)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const int32_t																halfWidth									= 10 - ((int32_t)beaconTimer % 11);
	const ::gpk::n2<int32_t>													lightCrosshair []							=
		{ centerCrosshair + ::gpk::n2<int32_t>{-1, -halfWidth - 1}
		, centerCrosshair + ::gpk::n2<int32_t>{ 0, -halfWidth - 1}
		, centerCrosshair + ::gpk::n2<int32_t>{halfWidth, -1}
		, centerCrosshair + ::gpk::n2<int32_t>{halfWidth,  0}
		, centerCrosshair + ::gpk::n2<int32_t>{ 0, halfWidth}
		, centerCrosshair + ::gpk::n2<int32_t>{-1, halfWidth}
		, centerCrosshair + ::gpk::n2<int32_t>{-halfWidth - 1,  0}
		, centerCrosshair + ::gpk::n2<int32_t>{-halfWidth - 1, -1}
		};

	for(uint32_t iPoint = 0, pointCount = ::gpk::size(lightCrosshair); iPoint < pointCount; ++iPoint) {
		const ::gpk::n2<int32_t>												& pointToTest								= lightCrosshair[iPoint];
		::gpk::drawPixelLight(target, pointToTest.Cast<float>()
			, (0 == (int32_t)beaconTimer % 5) ? ::gpk::bgra(::gpk::RED			)
			: (0 == (int32_t)beaconTimer % 3) ? ::gpk::bgra(::gpk::LIGHTGREEN		)
			: (0 == (int32_t)beaconTimer % 2) ? ::gpk::bgra(::gpk::LIGHTYELLOW	)
			: ::gpk::bgra(::gpk::LIGHTCYAN)
			, .2f, 3.0f
			);
	}
	return 0;
}
					::gpk::error_t										drawCrosshair								(::gpk::v2<::gpk::bgra> target, ::SApplication & app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework				& framework									= app.Framework;
	static double																beaconTimer									= 0;
	beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 10;
	::SGame																		& gameInstance								= app.Game;
	for(uint32_t iShip = 0, shipCount = gameInstance.Ships.Alive.size(); iShip < shipCount; ++iShip) {
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		::gpk::n2<int32_t>														posXHair									= gameInstance.PositionCrosshair[iShip].Cast<int32_t>();
		if(false == gameInstance.Ships.LineOfFire[iShip])
			::drawCrosshairDiagonal(target, beaconTimer, posXHair);
		gerror_if(errored(::gpk::grid_copy_alpha(framework.RootWindow.BackBuffer->Color.View, app.Processed[GAME_TEXTURE_CROSSHAIR].View, posXHair - app.TextureCenters[GAME_TEXTURE_CROSSHAIR], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		if(gameInstance.Ships.LineOfFire[iShip])
			::drawCrosshairAligned(target, beaconTimer, posXHair);
	}
	return 0;
}
