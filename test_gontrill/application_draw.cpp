// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"
#include "gpk_grid_copy.h"
#include "gpk_bitmap_target.h"

#define Y_PLUS -

static				::gpk::error_t										drawShipHealthBar							(::SApplication& applicationInstance, const ::gpk::SCoord2<float> & centerEnemy, const ::gpk::SCoord2<int32_t> & halfMetrics, uint32_t health, int32_t yOffset, const ::gpk::SColorBGRA & finalColor)											{
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::view_grid<::gpk::SColorBGRA>											& viewOffscreen								= framework.MainDisplayOffscreen->Color.View;
	::gpk::SLine2D<int32_t>														healthBar									= {};
	healthBar.A 															= {(int32_t)(centerEnemy.x  + .5f - halfMetrics.x), (int32_t)(centerEnemy.y Y_PLUS yOffset)};
	healthBar.B																= healthBar.A; //{(int32_t)(centerEnemy.x  + .5f + halfMetrics.x), (int32_t)(centerEnemy.y + yOffset)};
	healthBar.B.x															+= halfMetrics.x * 2;
	double																		enemyHealthProportion						= health / 5000.0;
	healthBar.B.x															= ::gpk::interpolate_linear(healthBar.A.x, healthBar.B.x, enemyHealthProportion);
	applicationInstance.CacheLinePoints.clear();
	::gpk::drawLine(viewOffscreen.metrics(), healthBar, applicationInstance.CacheLinePoints);
	for(uint32_t iLinePoint = 0, pointCount = applicationInstance.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
		const ::gpk::SCoord2<float>													& pointToDraw								= applicationInstance.CacheLinePoints[iLinePoint].Cast<float>();
		::gpk::drawPixelLight(viewOffscreen, pointToDraw, finalColor, .2f, 1.5f);
	}
	return 0;
}
//
					::gpk::error_t										drawShips									(::SApplication& applicationInstance)											{
	::SGame																		& gameInstance								= applicationInstance.Game;
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::view_grid<::gpk::SColorBGRA>											& viewOffscreen								= framework.MainDisplayOffscreen->Color.View;
	// ---- Draw enemy ships
	const ::gpk::view_grid<::gpk::SColorBGRA>									& enemyView									= applicationInstance.Textures[GAME_TEXTURE_ENEMY].Processed.View;
	char																		indexPositionsX[]							= {0, 1, 2, 3, 2, 1, 0, -1, -2, -3, -2, -1};
	const ::gpk::SCoord2<int32_t>												halfMetricsEnemy							= (enemyView.metrics() / 2).Cast<int32_t>();
	int32_t																		halfMetricsEnemy2y							= halfMetricsEnemy.y;
	// ---- Draw ghosts.
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		static constexpr const ::gpk::SCoord2<float>								reference	= {1, 0};
		::gpk::SCoord2<float>														vector;
		static double																beaconTimer								= 0;
		beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 8;
		int32_t																		selectedPos								= ((int32_t)beaconTimer % ::gpk::size(indexPositionsX));
		for(uint32_t iGhost = 0; iGhost < 5; ++iGhost) {
			vector																	= reference * (64 * sin(framework.FrameInfo.Seconds.Total));
			vector.Rotate(::gpk::math_2pi / 5 * iGhost + gameInstance.Enemies.TimeLived[iEnemy]);
			::gpk::SCoord2<int32_t>														dstOffset								= (gameInstance.Enemies.Position[iEnemy] + vector).Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY];
			//if constexpr((Y_PLUS 1) == -1)
			//	dstOffset.y *= -1;
			error_if(errored(::gpk::grid_copy_alpha(viewOffscreen, enemyView, dstOffset, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
			{ // Draw ghost light
				::gpk::SCoord2<float>														centerPowerup							= gameInstance.Enemies.Position[iEnemy] + vector;
				::gpk::SCoord2<float>														lightCrosshair							= centerPowerup + ::gpk::SCoord2<float>{(float)indexPositionsX[selectedPos], 0.0f};
				::gpk::drawPixelLight(viewOffscreen, lightCrosshair.Cast<float>(), ::gpk::SColorBGRA(::gpk::YELLOW), .2f, 3.0f);
			}
		}
	}
	// --- Draw enemy ships.
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		const ::gpk::SCoord2<float>													& centerEnemy								= gameInstance.Enemies.Position[iEnemy];
		const ::SHealthPoints														& enemyHealth								= gameInstance.Enemies.Health[iEnemy];
		error_if(errored(::gpk::grid_copy_alpha(viewOffscreen, enemyView, centerEnemy.Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		::drawShipHealthBar(applicationInstance, centerEnemy, halfMetricsEnemy, enemyHealth.Health, (int32_t)(halfMetricsEnemy2y	), ::gpk::GREEN);
		::drawShipHealthBar(applicationInstance, centerEnemy, halfMetricsEnemy, enemyHealth.Shield, (int32_t)(halfMetricsEnemy2y - 2), ::gpk::CYAN );
	}
	// --- Draw player ships.
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		const ::gpk::view_grid<::gpk::SColorBGRA>									& shipView									= applicationInstance.Textures[GAME_TEXTURE_SHIP0 + iShip].Processed.View;
		const ::gpk::SCoord2<float>													& centerShip								= gameInstance.Ships.Position	[iShip];
		const ::SHealthPoints														& enemyHealth								= gameInstance.Ships.Health	[iShip];
		::gpk::SCoord2<int32_t>														halfMetricsShip								= (shipView.metrics() / 2).Cast<int32_t>();
		int32_t																		halfMetricsShip2y							= halfMetricsShip.y;
		::gpk::SCoord2<int32_t>														dstOffset									= gameInstance.Ships.Position[iShip].Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_SHIP0 + iShip];
		error_if(errored(::gpk::grid_copy_alpha(viewOffscreen, shipView, dstOffset, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		::drawShipHealthBar(applicationInstance, centerShip, halfMetricsShip, enemyHealth.Health, (int32_t)(halfMetricsShip2y		), ::gpk::GREEN);
		::drawShipHealthBar(applicationInstance, centerShip, halfMetricsShip, enemyHealth.Shield, (int32_t)(halfMetricsShip2y - 2	), ::gpk::CYAN );
	}
	// --- Draw enemy ship lights.
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		const ::gpk::SCoord2<float>													& centerEnemy								= gameInstance.Enemies.Position[iEnemy];
		static double																beaconTimer									= 0;
		beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 8;
		int32_t																		selectedPos									= ((int32_t)beaconTimer % ::gpk::size(indexPositionsX));
		::gpk::SCoord2<float>														lightCrosshair								= centerEnemy + ::gpk::SCoord2<float>{(float)indexPositionsX[selectedPos], 0.0f};
		::gpk::drawPixelLight(viewOffscreen, lightCrosshair.Cast<float>(), ::gpk::SColorBGRA(::gpk::RED), .2f, 3.0f);
	}
	return 0;
}

					::gpk::error_t										drawCollisions								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::view_grid<::gpk::SColorBGRA>											& viewOffscreen								= framework.MainDisplayOffscreen->Color.View;
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.CollisionPoints.size(); iRay < rayCount; ++iRay) {
		const ::gpk::SCoord2<float>													& pointToDraw									= applicationInstance.StuffToDraw.CollisionPoints[iRay];
		::gpk::drawPixelLight(viewOffscreen, pointToDraw, ::gpk::SColorBGRA(::gpk::ORANGE), .15f, 3.0f);
	}
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.Debris.size(); iRay < rayCount; ++iRay) {
		const ::SParticleToDraw														& particleToDraw								= applicationInstance.StuffToDraw.Debris[iRay];
		const ::gpk::SCoord2<float>													& pointToDraw									= particleToDraw.Position.Cast<float>();
		if(applicationInstance.ParticleSystemDebris.Instances[particleToDraw.IndexParticleInstance].Binding.Lit) {
			::gpk::SColorBGRA															finalColor										
				= (0 == (particleToDraw.IndexParticlePhysics % 3)) ? ::gpk::SColorBGRA(::gpk::YELLOW) 
				: (0 == (particleToDraw.IndexParticlePhysics % 2)) ? ::gpk::SColorBGRA(::gpk::RED) 
				: ::gpk::SColorBGRA(::gpk::ORANGE)
				;
			finalColor																*= 1.0f - (particleToDraw.TimeLived);
			::gpk::drawPixelLight(viewOffscreen, pointToDraw, finalColor, .15f, ::gpk::max(0.0f, 1.0f - (particleToDraw.TimeLived)) * 4.0f);
		}
	}
	return 0;
}

static				const ::gpk::array_static<::gpk::SColorBGRA, WEAPON_TYPE_COUNT>	weaponTypeColorPalette			= 
	{ ::gpk::SColorBGRA{::gpk::WHITE		} // WEAPON_TYPE_ARROW
	, ::gpk::SColorBGRA{::gpk::LIGHTGRAY	} // WEAPON_TYPE_BULLET
	, ::gpk::SColorBGRA{::gpk::RED			} // WEAPON_TYPE_LASER				
	, ::gpk::SColorBGRA{::gpk::LIGHTCYAN	} // WEAPON_TYPE_PLASMA
	, ::gpk::SColorBGRA{::gpk::LIGHTCYAN	} // WEAPON_TYPE_BUBBLE
	, ::gpk::SColorBGRA{::gpk::LIGHTYELLOW	} // WEAPON_TYPE_SPARK
	, ::gpk::SColorBGRA{::gpk::LIGHTGREEN 	} // WEAPON_TYPE_POISON
	, ::gpk::SColorBGRA{::gpk::RED			} // WEAPON_TYPE_FIRE
	, ::gpk::SColorBGRA{::gpk::GRAY			} // WEAPON_TYPE_ROCK
	, ::gpk::SColorBGRA{::gpk::WHITE		} // WEAPON_TYPE_GRAVITY
	};

					::gpk::error_t										drawShots									(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::view_grid<::gpk::SColorBGRA>											& viewOffscreen								= applicationInstance.Framework.MainDisplayOffscreen->Color.View;
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iRay < rayCount; ++iRay) {
		const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iRay];
		applicationInstance.CacheLinePoints.clear();
		::gpk::drawLine(viewOffscreen.metrics(), laserToDraw.Segment, applicationInstance.CacheLinePoints);
		const ::gpk::SParticleBinding<::SGameParticle>								& gameParticle								= applicationInstance.ParticleSystemProjectiles.Instances[laserToDraw.IndexParticleInstance];
		const float																	lightRange									= (gameParticle.Binding.TypePlayer == PLAYER_TYPE_PLAYER) ? 3.0f : 2.5f;
		const float																	lightValue									= (gameParticle.Binding.TypePlayer == PLAYER_TYPE_PLAYER) ? .15f : 1.0f;
		const ::gpk::SColorBGRA														finalColor									= weaponTypeColorPalette[gameParticle.Binding.TypeWeapon];

		if(0 == applicationInstance.CacheLinePoints.size()) {
			const ::gpk::SCoord2<float>													& pointToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iRay].Segment.A;
			::gpk::drawPixelLight(viewOffscreen, pointToDraw, finalColor, lightValue, lightRange);
		}
		else
			for(uint32_t iLinePoint = 0, pointCount = applicationInstance.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
				const ::gpk::SCoord2<float>													& pointToDraw								= applicationInstance.CacheLinePoints[iLinePoint].Cast<float>();
				::gpk::drawPixelLight(viewOffscreen, pointToDraw, finalColor, lightValue, lightRange);
			}
	}
	return 0;
}
					::gpk::error_t										drawBackground								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::SImage<::gpk::SColorBGRA>											& offscreen									= framework.MainDisplayOffscreen->Color;
	::gpk::view_grid<::gpk::SColorBGRA>											& viewOffscreen								= offscreen.View;
	::gpk::drawRectangle(offscreen.View, applicationInstance.ColorBackground, ::gpk::SRectangle2D<uint32_t>{{}, offscreen.View.metrics()});
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.Stars.size(); iRay < rayCount; ++iRay) {
		::SParticleToDraw															& starToDraw								= applicationInstance.StuffToDraw.Stars[iRay];
		if(false == applicationInstance.ParticleSystemStars.Instances[starToDraw.IndexParticleInstance].Binding.Lit)
			continue;
		::gpk::SCoord2<int32_t>														& particlePosition							= starToDraw.Position;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
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
		::gpk::drawPixelLight(viewOffscreen, particlePosition.Cast<float>(), viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], maxFactor, range);
	}
	return 0;
}
					::gpk::error_t										drawThrust								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::SImage<::gpk::SColorBGRA>											& offscreen									= framework.MainDisplayOffscreen->Color;
	::gpk::view_grid<::gpk::SColorBGRA>											& viewOffscreen								= offscreen.View;
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iThrust = 0, particleCount = (uint32_t)applicationInstance.StuffToDraw.Thrust.size(); iThrust < particleCount; ++iThrust) {
		::SParticleToDraw															& thrustToDraw								= applicationInstance.StuffToDraw.Thrust[iThrust];
		::SApplication::TParticleInstance											& particleInstance							= applicationInstance.ParticleSystemThrust.Instances[thrustToDraw.IndexParticleInstance];
		if(false == particleInstance.Binding.Lit)
			continue;
		::SShipState																& shipState									= (particleInstance.Binding.TypePlayer == PLAYER_TYPE_PLAYER) 
			? gameInstance.Ships	.States[particleInstance.Binding.OwnerIndex] 
			: gameInstance.Enemies	.States[particleInstance.Binding.OwnerIndex]
			;
		const int32_t																physicsId									= thrustToDraw.IndexParticlePhysics;
		const ::gpk::SCoord2<float>													& particlePosition							= applicationInstance.ParticleSystemThrust.Integrator.Particle[physicsId].Position;
		if(false == ::gpk::in_range(particlePosition, {{}, offscreen.View.metrics().Cast<float>()}))
			continue;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
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
		::gpk::drawPixelLight(viewOffscreen, particlePosition, viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], maxFactor, range);
	}
	return 0;
}

static constexpr	const ::gpk::SColorBGRA								powerupFamilyColorPalette []				= 
	{ ::gpk::LIGHTYELLOW
	, ::gpk::LIGHTGREEN
	, ::gpk::LIGHTCYAN
	, ::gpk::RED
	};

static				::gpk::error_t										drawPowerup						(::SApplication& applicationInstance, POWERUP_FAMILY powFamily, const ::gpk::view_array<::gpk::view_grid<::gpk::SColorBGRA>>& texturePowerup, const ::gpk::SCoord2<int32_t>& textureCenterPowerup, const ::gpk::SCoord2<float>& powPosition, const ::gpk::view_array<const ::gpk::SCoord2<int32_t>>& lightPos, double time)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::view_grid<::gpk::SColorBGRA>											& viewOffscreen								= framework.MainDisplayOffscreen->Color.View;
	::gpk::SCoord2<int32_t>														position									= powPosition.Cast<int32_t>();
	for(uint32_t iTex = 0, textureCount = texturePowerup.size(); iTex < textureCount; ++iTex)
		error_if(errored(::gpk::grid_copy_alpha(viewOffscreen, texturePowerup[iTex], position - textureCenterPowerup, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	::gpk::SCoord2<int32_t>															centerPowerup								= position;
	uint32_t																	lightIndex									= (uint32_t)time % (lightPos.size() / 2); 
	const ::gpk::SCoord2<int32_t>												& selectedLightPos0							= lightPos[lightIndex + 0]
		,																		& selectedLightPos2							= lightPos[lightIndex + 4]
		;
	const ::gpk::SColorBGRA														& selectedColor								= powerupFamilyColorPalette[powFamily];
	::gpk::drawPixelLight(viewOffscreen, selectedLightPos0.Cast<float>(), selectedColor, .3f, 3.0f);
	::gpk::drawPixelLight(viewOffscreen, selectedLightPos2.Cast<float>(), selectedColor, .3f, 3.0f);
	return 0;
}

static constexpr	int32_t												PWERUP_HALF_WIDTH							= 6;
static constexpr	const ::gpk::SCoord2<int32_t>						squarePowerupLightPositions		[8]			= 
	{ ::gpk::SCoord2<int32_t>{-PWERUP_HALF_WIDTH    , -PWERUP_HALF_WIDTH - 1}	
	, ::gpk::SCoord2<int32_t>{-PWERUP_HALF_WIDTH - 1, -PWERUP_HALF_WIDTH}		
	, ::gpk::SCoord2<int32_t>{-PWERUP_HALF_WIDTH - 1,  PWERUP_HALF_WIDTH - 1}	
	, ::gpk::SCoord2<int32_t>{-PWERUP_HALF_WIDTH    ,  PWERUP_HALF_WIDTH}		
	, ::gpk::SCoord2<int32_t>{ PWERUP_HALF_WIDTH - 1,  PWERUP_HALF_WIDTH}		
	, ::gpk::SCoord2<int32_t>{ PWERUP_HALF_WIDTH    ,  PWERUP_HALF_WIDTH - 1}	
	, ::gpk::SCoord2<int32_t>{ PWERUP_HALF_WIDTH    , -PWERUP_HALF_WIDTH}		
	, ::gpk::SCoord2<int32_t>{ PWERUP_HALF_WIDTH - 1, -PWERUP_HALF_WIDTH - 1}	
	};

static constexpr	const ::gpk::SCoord2<int32_t>						diagonalPowerupLightPositions	[8]			= 
	{ ::gpk::SCoord2<int32_t>{-1, -PWERUP_HALF_WIDTH - 1}
	, ::gpk::SCoord2<int32_t>{ 0, -PWERUP_HALF_WIDTH - 1}
	, ::gpk::SCoord2<int32_t>{PWERUP_HALF_WIDTH, -1}		
	, ::gpk::SCoord2<int32_t>{PWERUP_HALF_WIDTH,  0}		
	, ::gpk::SCoord2<int32_t>{ 0, PWERUP_HALF_WIDTH}		
	, ::gpk::SCoord2<int32_t>{-1, PWERUP_HALF_WIDTH}		
	, ::gpk::SCoord2<int32_t>{-PWERUP_HALF_WIDTH - 1,  0}
	, ::gpk::SCoord2<int32_t>{-PWERUP_HALF_WIDTH - 1, -1}
	};

					::gpk::error_t										drawPowerups								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	static double																timer										= 0;
	timer																	+= framework.FrameInfo.Seconds.LastFrame * 2;
	::SGame																		& gameInstance								= applicationInstance.Game;

	for(uint32_t iPow = 0, powCount = gameInstance.Powerups.Alive.size(); iPow < powCount; ++iPow) {
		if(0 == gameInstance.Powerups.Alive[iPow])
			continue;
		POWERUP_FAMILY																powFamily									= gameInstance.Powerups.Family[iPow];
		::gpk::SCoord2<int32_t>														position									= gameInstance.Powerups.Position[iPow].Cast<int32_t>();
		::gpk::SCoord2<int32_t>														lightPos [8]								;
		::gpk::view_array<::gpk::view_grid<::gpk::SColorBGRA>>						texturePowerup								;
		::gpk::SCoord2<int32_t>														textureCenterPowerup						;
		if(powFamily == POWERUP_FAMILY_HEALTH || powFamily == POWERUP_FAMILY_BUFF) { // draw square powerup box
			for(uint32_t i = 0; i < ::gpk::size(lightPos); ++i)
				lightPos[i]																= position + squarePowerupLightPositions[i];
			texturePowerup															= applicationInstance.StuffToDraw.TexturesPowerup0;
			textureCenterPowerup													= applicationInstance.TextureCenters[GAME_TEXTURE_POWCORESQUARE];
		}
		else { // draw diagonal powerup box
			for(uint32_t i = 0; i < ::gpk::size(lightPos); ++i)
				lightPos[i]																= position + diagonalPowerupLightPositions[i];
			texturePowerup															= applicationInstance.StuffToDraw.TexturesPowerup1;
			textureCenterPowerup													= applicationInstance.TextureCenters[GAME_TEXTURE_POWCOREDIAGONAL];
		}
		gpk_necall(::drawPowerup(applicationInstance, powFamily, texturePowerup, textureCenterPowerup, position.Cast<float>(), lightPos, timer), "Why would this ever happen?");
	}
	return 0;
}


static				::gpk::error_t										drawCrosshairDiagonal						(::SApplication& applicationInstance, double beaconTimer, const ::gpk::SCoord2<int32_t>	& centerCrosshair)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::view_grid<::gpk::SColorBGRA>											& viewOffscreen								= framework.MainDisplayOffscreen->Color.View;
	int32_t																		halfWidth									= 10 - ((int32_t)beaconTimer % 11);
	::gpk::SCoord2<int32_t>														lightCrosshair []							= 
		{ centerCrosshair + ::gpk::SCoord2<int32_t>{ halfWidth,  halfWidth }
		, centerCrosshair + ::gpk::SCoord2<int32_t>{ halfWidth, -halfWidth - 1 }
		, centerCrosshair + ::gpk::SCoord2<int32_t>{-halfWidth - 1, -halfWidth - 1 }
		, centerCrosshair + ::gpk::SCoord2<int32_t>{-halfWidth - 1,  halfWidth }
		};
	for(uint32_t iPoint = 0, pointCount = ::gpk::size(lightCrosshair); iPoint < pointCount; ++iPoint) {
		::gpk::SCoord2<int32_t>														& pointToTest								= lightCrosshair[iPoint];
		::gpk::drawPixelLight(viewOffscreen, pointToTest.Cast<float>(), ::gpk::SColorBGRA(::gpk::RED), .2f, 3.0f);
	}
	return 0;
}

static				::gpk::error_t										drawCrosshairAligned						(::SApplication& applicationInstance, double beaconTimer, const ::gpk::SCoord2<int32_t>	& centerCrosshair)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::view_grid<::gpk::SColorBGRA>											& viewOffscreen								= framework.MainDisplayOffscreen->Color.View;
	const int32_t																halfWidth									= 10 - ((int32_t)beaconTimer % 11);
	const ::gpk::SCoord2<int32_t>												lightCrosshair []							= 
		{ centerCrosshair + ::gpk::SCoord2<int32_t>{-1, -halfWidth - 1}
		, centerCrosshair + ::gpk::SCoord2<int32_t>{ 0, -halfWidth - 1}
		, centerCrosshair + ::gpk::SCoord2<int32_t>{halfWidth, -1}
		, centerCrosshair + ::gpk::SCoord2<int32_t>{halfWidth,  0}
		, centerCrosshair + ::gpk::SCoord2<int32_t>{ 0, halfWidth}
		, centerCrosshair + ::gpk::SCoord2<int32_t>{-1, halfWidth}
		, centerCrosshair + ::gpk::SCoord2<int32_t>{-halfWidth - 1,  0}
		, centerCrosshair + ::gpk::SCoord2<int32_t>{-halfWidth - 1, -1}
		};
	
	for(uint32_t iPoint = 0, pointCount = ::gpk::size(lightCrosshair); iPoint < pointCount; ++iPoint) {
		const ::gpk::SCoord2<int32_t>												& pointToTest								= lightCrosshair[iPoint];
		::gpk::drawPixelLight(viewOffscreen, pointToTest.Cast<float>()
			, (0 == (int32_t)beaconTimer % 5) ? ::gpk::SColorBGRA(::gpk::RED			) 
			: (0 == (int32_t)beaconTimer % 3) ? ::gpk::SColorBGRA(::gpk::LIGHTGREEN		)
			: (0 == (int32_t)beaconTimer % 2) ? ::gpk::SColorBGRA(::gpk::LIGHTYELLOW	) 
			: ::gpk::SColorBGRA(::gpk::LIGHTCYAN)
			, .2f, 3.0f
			);
	}
	return 0;
}
					::gpk::error_t										drawCrosshair								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	static double																beaconTimer									= 0;
	beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 10;
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = gameInstance.Ships.Alive.size(); iShip < shipCount; ++iShip) {
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		::gpk::SCoord2<int32_t>														posXHair									= gameInstance.PositionCrosshair[iShip].Cast<int32_t>();
		if(false == gameInstance.Ships.LineOfFire[iShip]) 
			::drawCrosshairDiagonal(applicationInstance, beaconTimer, posXHair);
		error_if(errored(::gpk::grid_copy_alpha(framework.MainDisplayOffscreen->Color.View, applicationInstance.Textures[GAME_TEXTURE_CROSSHAIR].Processed.View, posXHair - applicationInstance.TextureCenters[GAME_TEXTURE_CROSSHAIR], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		if(gameInstance.Ships.LineOfFire[iShip]) 
			::drawCrosshairAligned(applicationInstance, beaconTimer, posXHair);
	}
	return 0;
}
