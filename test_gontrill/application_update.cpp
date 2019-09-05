// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"
#include "gpk_collision.h"

					::gpk::error_t										updateInput									(::SApplication& applicationInstance)											{
	::gpk::SInput																& inputSystem								= *applicationInstance.Framework.Input;
	::SGame																		& gameInstance								= applicationInstance.Game;
	::SShipState																& ship0State								= gameInstance.Ships.States[0];
	ship0State.Firing														= inputSystem.KeyboardCurrent.KeyState['T'] != 0;
	ship0State.Thrust														= inputSystem.KeyboardCurrent.KeyState['Y'] != 0;
	ship0State.Brakes														= inputSystem.KeyboardCurrent.KeyState['U'] != 0;

	::SShipState																& ship1State								= gameInstance.Ships.States[1];
	ship1State.Firing														= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD1] != 0;
	ship1State.Thrust														= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD2] != 0;
	ship1State.Brakes														= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD3] != 0;
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip)
		gameInstance.Ships.Direction[iShip]										= {};

	::gpk::SCoord2<float>														& ship0Direction							= gameInstance.Ships.Direction[0];
	::gpk::SCoord2<float>														& ship1Direction							= gameInstance.Ships.Direction[1];
	//if(inputSystem.KeyboardCurrent.KeyState['W'			]) ship0Direction.y	+= 1;
	//if(inputSystem.KeyboardCurrent.KeyState['S'			]) ship0Direction.y	-= 1;
	if(inputSystem.KeyboardCurrent.KeyState['W'			]) ship0Direction.y	-= 1;
	if(inputSystem.KeyboardCurrent.KeyState['S'			]) ship0Direction.y	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState['D'			]) ship0Direction.x	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState['A'			]) ship0Direction.x	-= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_UP		]) ship1Direction.y	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_DOWN		]) ship1Direction.y	-= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_RIGHT	]) ship1Direction.x	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_LEFT		]) ship1Direction.x	-= 1;
	for(uint32_t iWeaponSelect = 0, weaponCount = ::gpk::min(::gpk::size(::weaponProperties), 11U); iWeaponSelect < weaponCount; ++iWeaponSelect) {
		if(inputSystem.KeyboardCurrent.KeyState['0' + iWeaponSelect]) {
			gameInstance.Ships.Weapon[0].IndexProperties							= iWeaponSelect;
			break;
		}
	}

	if(inputSystem.KeyDown('P'))
		applicationInstance.Paused												= !applicationInstance.Paused;
	if(inputSystem.KeyDown(VK_F2))
		applicationInstance.Debugging											= !applicationInstance.Debugging;

	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip)
		gameInstance.Ships.Direction[iShip].InPlaceNormalize();
	return 0;
}

struct SAABBCache {
						::gpk::SLine2D<float>								RectangleSegments	[4]						= {};
						::gpk::SCoord2<float>								CollisionPoints		[4]						= {};
						bool												Collision			[4]						= {};
};

template<typename _tCoord>
static				::gpk::error_t										checkLaserCollision
	( const ::gpk::SLine2D<_tCoord>				& projectilePath
	, ::SAABBCache								& aabbCache
	, const ::gpk::SCoord2<_tCoord>				& posXHair
	, float										halfSizeBox
	, ::gpk::array_pod<::gpk::SCoord2<_tCoord>>	& collisionPoints
	)
{ // Check powerup
	::gpk::buildAABBSegments(posXHair, halfSizeBox
		, aabbCache.RectangleSegments[0]
		, aabbCache.RectangleSegments[1]
		, aabbCache.RectangleSegments[2]
		, aabbCache.RectangleSegments[3]
		);
	::gpk::error_t																result										= 0;
	for(uint32_t iSeg = 0; iSeg < 4; ++iSeg) {
		::gpk::SCoord2<_tCoord>														& collision									= aabbCache.CollisionPoints[iSeg];
		if(1 == ::gpk::segment_segment_intersect(projectilePath, aabbCache.RectangleSegments[iSeg], collision)) {
			bool																		bFound										= false;
			for(uint32_t iS2 = 0; iS2 < iSeg; ++iS2) {
				if(collision == aabbCache.CollisionPoints[iS2]) {
					bFound																	= true;
					info_printf("Discarded collision point.");
					break;
				}
			}
			if(false == bFound) {
				result																	= 1;
				aabbCache.Collision	[iSeg]												= true;
 				collisionPoints.push_back(collision);
			}
		}
	}
	return result;
}

template <size_t _sizeAlive>
					::gpk::error_t										updateLineOfFire							(::SApplication & applicationInstance, ::SAABBCache& aabbCache, uint32_t iShip, float halfSizeBox, const ::gpk::SLine2D<float>& projectilePath, const ::gpk::array_static<::gpk::SCoord2<float>, _sizeAlive>& positions, const ::SArrayElementState<_sizeAlive>& alive)	{
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iEnemy = 0; iEnemy < alive.size(); ++iEnemy) {
		if(0 == alive[iEnemy])
			continue;
		const ::gpk::SCoord2<float>													& posEnemy									= positions[iEnemy];
		if(1 == ::checkLaserCollision(projectilePath, aabbCache, posEnemy, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints))
			gameInstance.Ships.LineOfFire[iShip]										= true;
	}
	return 0;
}
					::gpk::error_t										updateShips									(::SApplication & applicationInstance)			{
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::SGame																		& gameInstance			 					= applicationInstance.Game;
	const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= framework.MainDisplayOffscreen->Color.View.metrics();
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) { // Update ship positions
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		::gpk::SCoord2<float>														& shipPosition								= gameInstance.Ships.Position[iShip];
		const ::SShipState															& shipState									= gameInstance.Ships.States[iShip];
		shipPosition															+= gameInstance.Ships.Direction[iShip] * (float)(framework.FrameInfo.Seconds.LastFrame * 100) *
			(shipState.Brakes ? .25f : (shipState.Thrust ? 2 : 1));
		shipPosition.x															= ::gpk::clamp(shipPosition.x, .1f, (float)offscreenMetrics.x - 1);
		shipPosition.y															= ::gpk::clamp(shipPosition.y, .1f, (float)offscreenMetrics.y - 1);

		::gpk::SCoord2<float>														& crosshairPosition							= gameInstance.PositionCrosshair[iShip];
		crosshairPosition														= shipPosition + ::gpk::SCoord2<float>{96,};
		crosshairPosition.x														= ::gpk::min(crosshairPosition.x, (float)offscreenMetrics.x);
	}

	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {	// --- Calculate line of fire and set state accordingly. This causes Draw() to draw the crosshair in the right mode.
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		::gpk::SCoord2<float>														& shipPosition								= gameInstance.Ships.Position[iShip];
		::SAABBCache																aabbCache;
		::gpk::SLine2D<float>														projectilePath								= {shipPosition, shipPosition + ::gpk::SCoord2<float>{10000, }};
		projectilePath.A.y														-= 1;
		projectilePath.B.y														-= 1;
		gameInstance.Ships.LineOfFire[iShip]										= false;
		::updateLineOfFire(applicationInstance, aabbCache, iShip, gameInstance.HalfWidthEnemy	, projectilePath, gameInstance.Enemies	.Position, gameInstance.Enemies	.Alive);
		::updateLineOfFire(applicationInstance, aabbCache, iShip, gameInstance.HalfWidthPowerup	, projectilePath, gameInstance.Powerups	.Position, gameInstance.Powerups.Alive);
	}

	// Pick up powerups
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {	// --- Calculate line of fire and set state accordingly. This causes Draw() to draw the crosshair in the right mode.
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		for(uint32_t iPow = 0, powCount = gameInstance.Powerups.Alive.size(); iPow < powCount; ++iPow) {	// --- Calculate line of fire and set state accordingly. This causes Draw() to draw the crosshair in the right mode.
			if(0 == gameInstance.Powerups.Alive[iPow])
				continue;

			float																		collisionDistance							= (applicationInstance.Game.HalfWidthShip + applicationInstance.Game.HalfWidthPowerup);
			if((gameInstance.Powerups.Position[iPow] - gameInstance.Ships.Position[iShip]).LengthSquared() > (collisionDistance * collisionDistance))
				continue;
			::SPowerup																	& powerup									= gameInstance.Powerups.Type[iPow];
			if(powerup.TypeBuff		!= BUFF_TYPE_INVALID)	{
				switch (powerup.TypeBuff) {
				case BUFF_TYPE_FIRE_RATIO:
					gameInstance.Ships.Health[iShip].Health									+= 2000;
					gameInstance.Ships.Health[iShip].Shield									+= 2000;
					break;
				case BUFF_TYPE_FORCE_FIELD:
					gameInstance.Ships.Health[iShip].Health									+= 2000;
					gameInstance.Ships.Health[iShip].Shield									+= 2000;
					break;
				}
			}
			if(powerup.TypeWeapon	!= WEAPON_TYPE_INVALID)	{
				gameInstance.Ships.Weapon[iShip].IndexProperties	= powerup.TypeWeapon;
			}
			if(powerup.TypeHealth	!= HEALTH_TYPE_INVALID)	{
				switch (powerup.TypeHealth) {
				case HEALTH_TYPE_HEALTH		: gameInstance.Ships.Health[iShip].Health += 1000; break;
				case HEALTH_TYPE_SHIELD		: gameInstance.Ships.Health[iShip].Shield += 1000; break;
				}
			}
			gameInstance.Powerups.Alive[iPow]										= 0;

		}
		::gpk::SCoord2<float>														& shipPosition								= gameInstance.Ships.Position[iShip];
		::SAABBCache																aabbCache;
		::gpk::SLine2D<float>														projectilePath								= {shipPosition, shipPosition + ::gpk::SCoord2<float>{10000, }};
		projectilePath.A.y														-= 1;
		projectilePath.B.y														-= 1;
		gameInstance.Ships.LineOfFire[iShip]										= false;
		::updateLineOfFire(applicationInstance, aabbCache, iShip, gameInstance.HalfWidthEnemy	, projectilePath, gameInstance.Enemies	.Position, gameInstance.Enemies	.Alive);
		::updateLineOfFire(applicationInstance, aabbCache, iShip, gameInstance.HalfWidthPowerup	, projectilePath, gameInstance.Powerups	.Position, gameInstance.Powerups.Alive);
	}
	return 0;
}


					::gpk::error_t										removeDeadParticles							(::SApplication::TIntegrator & particleIntegrator, ::gpk::array_pod<::SApplication::TParticleInstance> & particleInstances, const ::gpk::SCoord2<uint32_t> & offscreenViewMetrics)											{
	typedef	::SApplication::TParticleInstance									TParticleInstance;
	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		typedef	::SApplication::TParticle											TParticle;
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		TParticle																	& particleNext								= particleIntegrator.ParticleNext	[particleInstance.IndexParticlePhysics];
		TParticle																	& particleCurrent							= particleIntegrator.Particle		[particleInstance.IndexParticlePhysics];
		const bool																	nextPosOutOfRange
			= (	((uint32_t)particleNext.Position.x) >= offscreenViewMetrics.x
			||	((uint32_t)particleNext.Position.y) >= offscreenViewMetrics.y
			);
		const bool																	currentPosOutOfRange
			= (	((uint32_t)particleCurrent.Position.x) >= offscreenViewMetrics.x
			||	((uint32_t)particleCurrent.Position.y) >= offscreenViewMetrics.y
			);
		const bool																	instanceTimeout
			=  (particleInstance.Binding.TimeLived >=	  .125 && particleInstance.Binding.Type == PARTICLE_TYPE_SHIP_THRUST)
			|| (particleInstance.Binding.TimeLived >=		1. && particleInstance.Binding.Type == PARTICLE_TYPE_DEBRIS		)
			|| (particleInstance.Binding.TimeLived >=	99999. && particleInstance.Binding.Type == PARTICLE_TYPE_PROJECTILE	)
			;
		if((currentPosOutOfRange && nextPosOutOfRange) || instanceTimeout) { // Remove the particle instance and related information.
			particleIntegrator.ParticleState[particleInstance.IndexParticlePhysics].Unused	= true;
			ree_if(errored(particleInstances.remove(iParticle)), "Not sure why would this fail.");
			--iParticle;
		}
	}
	return 0;
}

static				::gpk::error_t										updateEffectParticles						(float lastFrameSeconds, ::SApplication::TIntegrator & particleIntegrator, ::gpk::array_pod<::SApplication::TParticleInstance> & particleInstances, ::gpk::array_pod<::SParticleToDraw> & particlesToDraw)											{
	typedef	::SApplication::TParticleInstance									TParticleInstance;
	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		int32_t																		physicsId									= particleInstance.IndexParticlePhysics;
		const ::SParticleToDraw														particleToDraw								= {physicsId, (int32_t)iParticle, particleInstance.Binding.TimeLived, particleIntegrator.Particle[physicsId].Position.Cast<int32_t>()};
		particlesToDraw.push_back(particleToDraw);
		particleInstance.Binding.TimeLived										+= lastFrameSeconds;
	}
	return 0;
}

static				::gpk::error_t										integrateParticleVelocity					(::SApplication& applicationInstance)											{
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	const float																	lastFrameSeconds							= (float)framework.FrameInfo.Seconds.LastFrame;
	ree_if(errored(applicationInstance.ParticleSystemThrust			.Integrator.Integrate(lastFrameSeconds, framework.FrameInfo.Seconds.LastFrameHalfSquared)), "Not sure why would this fail.");
	ree_if(errored(applicationInstance.ParticleSystemDebris			.Integrator.Integrate(lastFrameSeconds, framework.FrameInfo.Seconds.LastFrameHalfSquared)), "Not sure why would this fail.");
	ree_if(errored(applicationInstance.ParticleSystemProjectiles	.Integrator.Integrate(lastFrameSeconds, framework.FrameInfo.Seconds.LastFrameHalfSquared)), "Not sure why would this fail.");
	ree_if(errored(applicationInstance.ParticleSystemStars			.Integrator.Integrate(lastFrameSeconds, framework.FrameInfo.Seconds.LastFrameHalfSquared)), "Not sure why would this fail.");
	return 0;
}

					::gpk::error_t										updateParticles								(::SApplication& applicationInstance)											{
	gpk_necall(::integrateParticleVelocity(applicationInstance), "???");
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::clear
		( applicationInstance.StuffToDraw.ProjectilePaths
		, applicationInstance.StuffToDraw.CollisionPoints
		, applicationInstance.StuffToDraw.Debris
		, applicationInstance.StuffToDraw.Thrust
		, applicationInstance.StuffToDraw.Stars
		);
	const ::gpk::SCoord2<uint32_t>												& offscreenViewMetrics						= framework.MainDisplayOffscreen->Color.View.metrics();
	gpk_necall(::removeDeadParticles(applicationInstance.ParticleSystemThrust		.Integrator, applicationInstance.ParticleSystemThrust		.Instances, offscreenViewMetrics), "???");
	gpk_necall(::removeDeadParticles(applicationInstance.ParticleSystemDebris		.Integrator, applicationInstance.ParticleSystemDebris		.Instances, offscreenViewMetrics), "???");
	gpk_necall(::removeDeadParticles(applicationInstance.ParticleSystemProjectiles	.Integrator, applicationInstance.ParticleSystemProjectiles	.Instances, offscreenViewMetrics), "???");
	gpk_necall(::removeDeadParticles(applicationInstance.ParticleSystemStars		.Integrator, applicationInstance.ParticleSystemStars		.Instances, offscreenViewMetrics), "???");
	const float																	lastFrameSeconds							= (float)framework.FrameInfo.Seconds.LastFrame;
	::updateEffectParticles(lastFrameSeconds, applicationInstance.ParticleSystemThrust.Integrator, applicationInstance.ParticleSystemThrust.Instances, applicationInstance.StuffToDraw.Thrust);
	::updateEffectParticles(lastFrameSeconds, applicationInstance.ParticleSystemDebris.Integrator, applicationInstance.ParticleSystemDebris.Instances, applicationInstance.StuffToDraw.Debris);
	::updateEffectParticles(lastFrameSeconds, applicationInstance.ParticleSystemStars .Integrator, applicationInstance.ParticleSystemStars .Instances, applicationInstance.StuffToDraw.Stars );
	{ // Projectiles
		typedef	::SApplication::TParticleInstance									TParticleInstance;
		::SApplication::TIntegrator													& particleIntegrator						= applicationInstance.ParticleSystemProjectiles.Integrator;
		::gpk::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystemProjectiles.Instances;
		for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
			TParticleInstance															& particleInstance							= particleInstances[iParticle];
			int32_t																		physicsId									= particleInstance.IndexParticlePhysics;
			typedef	::SApplication::TParticle											TParticle;
			TParticle																	& particleNext								= particleIntegrator.ParticleNext	[physicsId];
			TParticle																	& particleCurrent							= particleIntegrator.Particle		[physicsId];
			const ::SLaserToDraw														laserToDraw									= {physicsId, (int32_t)iParticle, ::gpk::SLine2D<float>{particleCurrent.Position, particleNext.Position}};
			applicationInstance.StuffToDraw.ProjectilePaths.push_back(laserToDraw);
			particleInstance.Binding.TimeLived										+= lastFrameSeconds;
		}
	}
	memcpy(applicationInstance.ParticleSystemThrust			.Integrator.Particle.begin(), applicationInstance.ParticleSystemThrust		.Integrator.ParticleNext.begin(), sizeof(::SApplication::TParticle) * applicationInstance.ParticleSystemThrust		.Integrator.ParticleState.size());
	memcpy(applicationInstance.ParticleSystemDebris			.Integrator.Particle.begin(), applicationInstance.ParticleSystemDebris		.Integrator.ParticleNext.begin(), sizeof(::SApplication::TParticle) * applicationInstance.ParticleSystemDebris		.Integrator.ParticleState.size());
	memcpy(applicationInstance.ParticleSystemProjectiles	.Integrator.Particle.begin(), applicationInstance.ParticleSystemProjectiles	.Integrator.ParticleNext.begin(), sizeof(::SApplication::TParticle) * applicationInstance.ParticleSystemProjectiles	.Integrator.ParticleState.size());
	memcpy(applicationInstance.ParticleSystemStars			.Integrator.Particle.begin(), applicationInstance.ParticleSystemStars		.Integrator.ParticleNext.begin(), sizeof(::SApplication::TParticle) * applicationInstance.ParticleSystemStars		.Integrator.ParticleState.size());
	return 0;
}

template<typename _tParticleType>
static				::gpk::error_t										addParticle
	(	::SGameParticle												particleType
	,	::gpk::array_pod<::gpk::SParticleBinding<_tParticleType>>	& particleInstances
	,	::SApplication::TIntegrator									& particleIntegrator
	,	const ::gpk::SCoord2<float>									& particlePosition
	,	const ::gpk::SCoord2<float>									& particleDirection
	,	float														speed
	,	const ::gpk::view_array<::SApplication::TParticle>			& particleDefinitions
	)
{
	int32_t																		indexParticleInstance						= ::gpk::addParticle(particleType, particleInstances, particleIntegrator, particleDefinitions[particleType.Type]);
	ree_if(errored(indexParticleInstance), "Cannot create particle instance.");
	::gpk::SParticleBinding<_tParticleType>									& newInstance								= particleInstances[indexParticleInstance];
	::SApplication::TParticle													& newParticle								= particleIntegrator.Particle[newInstance.IndexParticlePhysics];
	newParticle.Position													= particlePosition;
	::gpk::SCoord2<float>														newDirection								= particleDirection;
	const float																	value										= .5;
	switch(particleType.Type) {
	default							: break;
	case ::PARTICLE_TYPE_SHIP_THRUST:
		newParticle.Position.y													+= rand() % 3 - 1;
		newDirection.Rotate(((rand() % 32767) / 32766.0f) * value - value / 2);
		break;
	}
	newParticle.Forces.Velocity												= newDirection * speed;	//{ -, (float)((rand() % 31 * 4) - 15 * 4)};
	return indexParticleInstance;
}

static				::gpk::error_t										addProjectile								(::SGame & gameInstance, int32_t iShip, PLAYER_TYPE playerType, WEAPON_TYPE weaponType, float projectileSpeed)					{
	::gpk::view_bit<uint64_t>												& projectilesAlive							= gameInstance.Projectiles.Alive;
	uint32_t																	iProjectile									= ::firstUnused(gameInstance.Projectiles.Alive);
	rew_if(iProjectile == -1, "Not enough space for storing new projectile.");
	projectilesAlive[iProjectile]											= 1;
	::SProjectile																& projectile							= gameInstance.Projectiles.Projectiles[iProjectile] = {};
	projectile.TypeWeapon													= weaponType;
	projectile.TypePlayer													= playerType;
	projectile.ShipIndex													= iShip;
	projectile.TimeLived													= 0;
	projectile.Speed														= projectileSpeed;
	++gameInstance.CountProjectiles;
	return iProjectile;
}

template <size_t _sizeAlive>
static				::gpk::error_t										updateSpawnShots
	( ::SApplication												& applicationInstance
	, const ::gpk::view_array<::SApplication::TParticle>			& particleDefinitions
	, uint32_t														maxShips
	, const ::gpk::array_static<::gpk::SCoord2<float>, _sizeAlive>	& positions
	, ::gpk::array_static<double, _sizeAlive>						& weaponDelay
	, const ::gpk::array_static<::SWeapon, _sizeAlive>				& weapons
	, const ::gpk::array_static<::SShipState, _sizeAlive>			& shipState
	, const ::SArrayElementState<_sizeAlive>						& alive
	, PLAYER_TYPE													playerType
	)
{
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::SGame																		& gameInstance								= applicationInstance.Game;
	::gpk::array_pod<::SApplication::TParticleInstance>							& particleInstances							= applicationInstance.ParticleSystemProjectiles.Instances;
	::SApplication::TIntegrator													& particleIntegrator						= applicationInstance.ParticleSystemProjectiles.Integrator;
	for(uint32_t iShip = 0, shipCount = maxShips; iShip < shipCount; ++iShip) {
		if(0 == alive[iShip])
			continue;
		uint32_t																	textureIndex								= (playerType == PLAYER_TYPE_PLAYER) ? GAME_TEXTURE_SHIP0 + iShip : GAME_TEXTURE_ENEMY;
		weaponDelay[iShip]														+= framework.FrameInfo.Seconds.LastFrame;
		const ::SWeapon																& weapon										= weapons[iShip];
		if(shipState[iShip].Firing && (weapon.IndexProperties != -1)) { // Add lasers / bullets.
			if( weaponDelay[iShip] >= weaponProperties[weapon.IndexProperties].Delay ) {
				const ::SWeaponProperties													& weaponProp								= ::weaponProperties[weapons[iShip].IndexProperties];
				weaponDelay[iShip]														= 0;
				::SGameParticle																gameParticle;
				gameParticle.OwnerIndex													= iShip;
				gameParticle.TimeLived													= 0;
				gameParticle.Type														= PARTICLE_TYPE_PROJECTILE;
				gameParticle.Lit														= true;
				gameParticle.TypePlayer													= playerType;
				gameParticle.IndexWeapon												= (int8_t)weapons[iShip].IndexProperties;
				gameParticle.TypeWeapon													= ::weaponProperties[gameParticle.IndexWeapon].TypeWeapon;
				const ::gpk::SCoord2<float>													textureShipMetrics							= applicationInstance.Textures[textureIndex].Processed.View.metrics().Cast<float>();
				const ::gpk::SCoord2<float>													weaponParticleOffset						= {textureShipMetrics.x - (textureShipMetrics.x - applicationInstance.TextureCenters[textureIndex].x), -1};
				const ::gpk::SCoord2<float>													shotDirection								= (playerType == PLAYER_TYPE_PLAYER) ? ::gpk::SCoord2<float>{1.0f, 0.0f} :
					(gameInstance.Ships.Position[rand() % gameInstance.ShipsPlaying] - gameInstance.Enemies.Position[iShip]).InPlaceNormalize();
				int32_t																		projectileIndex								= ::addProjectile(gameInstance, iShip, gameParticle.TypePlayer, gameParticle.TypeWeapon, weaponProp.Speed);
				ce_if(errored(projectileIndex), "Projectile storage is full. Cannot add projectile.");
				int32_t																		particleIndex								= ::addParticle(gameParticle, particleInstances, particleIntegrator, positions[iShip] + weaponParticleOffset, shotDirection, weaponProp.Speed, particleDefinitions);
				e_if(errored(particleIndex), "Particle storage is full. Cannot add projectile particle.");
			}
		}
	}
	return 0;
}

					::gpk::error_t										updateSpawn
	( ::SApplication										& applicationInstance
	, const ::gpk::view_array<::SApplication::TParticle>	& particleDefinitions
	)
{
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::SFramework::TOffscreen												& offscreen									= framework.MainDisplayOffscreen->Color;
	// Add some effect particles
	applicationInstance.EffectsDelay.Thrust									+= framework.FrameInfo.Seconds.LastFrame;
	::SGame																		& gameInstance								= applicationInstance.Game;
	{
		::gpk::array_pod<::SApplication::TParticleInstance>							& particleInstances							= applicationInstance.ParticleSystemThrust.Instances;
		::SApplication::TIntegrator													& particleIntegrator						= applicationInstance.ParticleSystemThrust.Integrator;
		for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
			if(0 == gameInstance.Ships.Alive[iShip])
				continue;
			if(applicationInstance.EffectsDelay.Thrust > .01) { // Add thrust particles.
				for(int32_t i = 0, particleCountToSpawn = 1 + rand() % 4; i < particleCountToSpawn; ++i) {
					::SGameParticle																gameParticle;
					gameParticle.OwnerIndex													= iShip;
					gameParticle.TimeLived													= 0;
					gameParticle.Type														= PARTICLE_TYPE_SHIP_THRUST;
					gameParticle.TypePlayer													= PLAYER_TYPE_PLAYER;
					gameParticle.Lit														= 0 == (rand() % 2);
					::addParticle(gameParticle, particleInstances, particleIntegrator, gameInstance.Ships.Position[iShip] + applicationInstance.PSOffsetFromShipCenter.Cast<float>(), gameInstance.Ships.Direction[iShip] * -1.0, (float)(rand() % 400) + (gameInstance.Ships.States[iShip].Thrust ? 400 : 0), particleDefinitions);
				}
			}
		}
	}
	{
		::gpk::array_pod<::SApplication::TParticleInstance>							& particleInstances							= applicationInstance.ParticleSystemStars.Instances;
		::SApplication::TIntegrator													& particleIntegrator						= applicationInstance.ParticleSystemStars.Integrator;
		applicationInstance.EffectsDelay.Star									+= framework.FrameInfo.Seconds.LastFrame;
		if( applicationInstance.EffectsDelay.Star > .1 ) {
			applicationInstance.EffectsDelay.Star									= 0;
			bool																		bFastStarFromThrust							= false;
			for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
				if(0 == gameInstance.Ships.Alive[iShip])
					continue;
				if (gameInstance.Ships.States[iShip].Thrust) {
					bFastStarFromThrust														= true;
					break;
				}
			}
			::SGameParticle																gameParticle;
			gameParticle.TimeLived													= 0;
			gameParticle.Type														= PARTICLE_TYPE_STAR;
			gameParticle.Lit														= 0 == rand() % 3;
			::addParticle(gameParticle, particleInstances, particleIntegrator, {offscreen.View.metrics().x - 1.0f, (float)(rand() % offscreen.View.metrics().y)}, {-1, 0}, (float)(rand() % (bFastStarFromThrust ? 400 : 75)) + 25, particleDefinitions);
		}
	}
	::updateSpawnShots(applicationInstance, particleDefinitions, gameInstance.ShipsPlaying			, gameInstance.Ships	.Position, gameInstance.Ships	.WeaponDelay, gameInstance.Ships	.Weapon, gameInstance.Ships		.States, gameInstance.Ships		.Alive, PLAYER_TYPE_PLAYER	);
	::updateSpawnShots(applicationInstance, particleDefinitions, gameInstance.Enemies.Alive.size()	, gameInstance.Enemies	.Position, gameInstance.Enemies	.WeaponDelay, gameInstance.Enemies	.Weapon, gameInstance.Enemies	.States, gameInstance.Enemies	.Alive, PLAYER_TYPE_ENEMY	);
	return 0;
}

					::gpk::error_t										removeDeadStuff								(::SApplication& applicationInstance)					{
	::SGame																		& gameInstance								= applicationInstance.Game;
	::gpk::view_bit<uint64_t>												& projectilesAlive							= gameInstance.Projectiles.Alive;
	for(uint32_t iProjectile = 0, projectileCount = projectilesAlive.size(); iProjectile < projectileCount; ++iProjectile) {
		if(projectilesAlive[iProjectile]) {
			::SProjectile																& projectile								= gameInstance.Projectiles.Projectiles[iProjectile];
			projectile.TimeLived													+= applicationInstance.Framework.FrameInfo.Seconds.LastFrame;
			if(projectile.TimeLived > 0.1) {
				projectilesAlive[iProjectile]											= 0;
				--gameInstance.CountProjectiles;
			}
		}
	}
	return 0;
}

static				::gpk::error_t										spawnPowOfRandomType						(::SGame & gameInstance, ::gpk::SCoord2<float> powPosition)			{
	int32_t																		indexToSpawnPow								= firstUnused(gameInstance.Powerups.Alive);
	ree_if(indexToSpawnPow == -1, "Not enough space to spawn more pows!");
	gameInstance.Powerups.Alive		[indexToSpawnPow]						= 1;
	gameInstance.Powerups.Position	[indexToSpawnPow]						= powPosition;
	gameInstance.Powerups.Family	[indexToSpawnPow]						= (POWERUP_FAMILY)(rand() % POWERUP_FAMILY_COUNT);
	::SPowerup																	& powerup									= gameInstance.Powerups.Type[indexToSpawnPow];
	powerup.TypeBuff														= BUFF_TYPE_INVALID;
	powerup.TypeHealth														= HEALTH_TYPE_INVALID;
	powerup.TypeWeapon														= WEAPON_TYPE_INVALID;
	switch(rand() % 3) {
	case 0: powerup.TypeBuff													= (BUFF_TYPE	)(rand() % BUFF_TYPE_COUNT	); break;
	case 1:	powerup.TypeHealth													= (HEALTH_TYPE	)(rand() % HEALTH_TYPE_COUNT); break;
	case 2:	powerup.TypeWeapon													= (WEAPON_TYPE	)(rand() % WEAPON_TYPE_COUNT); break;
	}
	++gameInstance.CountPowerups;
	return 0;
}

					::gpk::error_t										updateShots
	( ::SApplication										& applicationInstance
	, const ::gpk::view_array<::SApplication::TParticle>	& particleDefinitions
	)
{
	::SGame																		& gameInstance								= applicationInstance.Game;
	applicationInstance.StuffToDraw.CollisionPoints.clear();
	::SAABBCache																aabbCache;
	::gpk::array_pod<::SApplication::TParticleInstance>							& particleInstances							= applicationInstance.ParticleSystemProjectiles.Instances;
	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		const ::gpk::SLine2D<float>													& projectilePath							= laserToDraw.Segment;
		for(uint32_t iPow = 0; iPow < gameInstance.Powerups.Alive.size(); ++iPow) { // Check powerup
			if(0 == gameInstance.Powerups.Alive[iPow])
				continue;
			const ::gpk::SCoord2<float>													& posPowerup								= gameInstance.Powerups.Position[iPow];
			float																		halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_POWCORESQUARE].x;
			if(1 == ::checkLaserCollision(projectilePath, aabbCache, posPowerup, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints)) {

			}
		}
		::SApplication::TParticleInstance											& particleInstance							= particleInstances[laserToDraw.IndexParticleInstance];
		const ::SWeapon																& weapon									= (particleInstance.Binding.TypePlayer == PLAYER_TYPE_PLAYER)
			? gameInstance.Ships	.Weapon[particleInstance.Binding.OwnerIndex]
			: gameInstance.Enemies	.Weapon[particleInstance.Binding.OwnerIndex]
			;
		if(particleInstance.Binding.TypePlayer == PLAYER_TYPE_PLAYER) {
			for(uint32_t iEnemy = 0; iEnemy < gameInstance.Enemies.Alive.size(); ++iEnemy) { // Check enemy
				if(0 == gameInstance.Enemies.Alive[iEnemy])
					continue;
				const ::gpk::SCoord2<float>													& posEnemy									= gameInstance.Enemies.Position[iEnemy];
				const float																	halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY].x;
				::SHealthPoints																& enemyHealth								= gameInstance.Enemies.Health[iEnemy];
				if(1 == ::checkLaserCollision(projectilePath, aabbCache, posEnemy, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints)) {
					float																		damegePorportion							= ::gpk::max(.5f, (rand() % 0x7FFF) / (65535.0f/2));
					enemyHealth.Health														-= (int32_t)(::weaponProperties[weapon.IndexProperties].Damage * (1.0f - damegePorportion));
					enemyHealth.Shield														-= (int32_t)(::weaponProperties[weapon.IndexProperties].Damage * damegePorportion);
					if(enemyHealth.Health < 0) enemyHealth.Health								= 0;
					if(enemyHealth.Shield < 0) enemyHealth.Shield								= 0;
					particleInstance.Binding.TimeLived										= 9999999.f;
				}
				if(0 >= enemyHealth.Health) {
					gameInstance.Enemies.Alive[iEnemy]										= 0;
					--gameInstance.CountEnemies;
					if(0 == (rand() % 2))
						::spawnPowOfRandomType(gameInstance, gameInstance.Enemies.Position[iEnemy]);
					continue;
				}
				static constexpr const ::gpk::SCoord2<float>								reference									= {1, 0};
				::gpk::SCoord2<float>														vector;
				for(uint32_t iGhost = 0; iGhost < 5; ++iGhost) {
					vector																	= reference * (64 * sin(applicationInstance.Framework.FrameInfo.Seconds.Total));
					vector.Rotate(::gpk::math_2pi / 5 * iGhost + gameInstance.Enemies.TimeLived[iEnemy]);
					if(1 == ::checkLaserCollision(projectilePath, aabbCache, posEnemy + vector, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints)) {
						particleInstance.Binding.TimeLived										= 9999999.f;
					}
				}
			}
		}
		else {
			for(uint32_t iShip = 0; iShip < gameInstance.Ships.Alive.size(); ++iShip) { // Check enemy
				if(0 == gameInstance.Ships.Alive[iShip])
					continue;
				const ::gpk::SCoord2<float>													& posEnemy									= gameInstance.Ships.Position[iShip];
				const float																	halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_SHIP0 + iShip].x;
				::SHealthPoints																& enemyHealth								= gameInstance.Ships.Health[iShip];
				if(1 == ::checkLaserCollision(projectilePath, aabbCache, posEnemy, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints)) {
					float																		damegePorportion							= ::gpk::max(.5f, (rand() % 5001) / 5000.0f);
					enemyHealth.Health														-= (int32_t)(weaponProperties[weapon.IndexProperties].Speed * (1.0f - damegePorportion));
					enemyHealth.Shield														-= (int32_t)(weaponProperties[weapon.IndexProperties].Speed * damegePorportion);
					if(enemyHealth.Health < 0) enemyHealth.Health								= 0;
					if(enemyHealth.Shield < 0) enemyHealth.Shield								= 0;
				}
				if(0 >= enemyHealth.Health) {
					gameInstance.Ships.Alive[iShip]											= 0;
					continue;
				}
			}
		}
	}

	for(uint32_t iCollision = 0, collisionCount = applicationInstance.StuffToDraw.CollisionPoints.size(); iCollision < collisionCount; ++iCollision)
		for(uint32_t i=0; i < 10; ++i) {
			::gpk::SCoord2<float>	angle	= {(float)-(rand() % 20) - 10, (float)(rand() % 20 - 1 - 10)};
			angle.InPlaceNormalize();
			::SGameParticle																gameParticle;
			gameParticle.TimeLived													= 0;
			gameParticle.Type														= PARTICLE_TYPE_DEBRIS;
			gameParticle.Lit														= 0 == (rand()% 2);
			gameParticle.TypePlayer													= PLAYER_TYPE_PLAYER;
			::addParticle(gameParticle, applicationInstance.ParticleSystemDebris.Instances, applicationInstance.ParticleSystemDebris.Integrator, applicationInstance.StuffToDraw.CollisionPoints[iCollision], angle, (float)(rand() % 400) + 100, particleDefinitions);
		}
	return 0;
}

					::gpk::error_t										updateGUI									(::SApplication& applicationInstance)					{
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {	//  ------ update crosshair collision points with lasers
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		const ::gpk::SCoord2<float>													& posXHair									= gameInstance.PositionCrosshair[iShip];
		for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
			const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
			const ::SApplication::TParticleInstance										& particleInstance							= applicationInstance.ParticleSystemProjectiles.Instances[laserToDraw.IndexParticleInstance];
			if(particleInstance.Binding.OwnerIndex != iShip || particleInstance.Binding.TypePlayer != PLAYER_TYPE_PLAYER)
				continue;
			float																		halfSizeBox									= gameInstance.HalfWidthCrosshair;
			const ::gpk::SLine2D<float>													verticalSegments[]							=
				{ {posXHair + ::gpk::SCoord2<float>{ halfSizeBox - 1, halfSizeBox - 1}, posXHair + ::gpk::SCoord2<float>{ halfSizeBox - 1	,-halfSizeBox}}
				, {posXHair + ::gpk::SCoord2<float>{-halfSizeBox	, halfSizeBox - 1}, posXHair + ::gpk::SCoord2<float>{-halfSizeBox		,-halfSizeBox}}
				};
			const ::gpk::SLine2D<float>													& projectilePath							= laserToDraw.Segment;
			::gpk::SCoord2<float>														collisions	[::gpk::size(verticalSegments)]	= {};
			for(uint32_t iSeg = 0; iSeg < ::gpk::size(verticalSegments); ++iSeg) {
				::gpk::SCoord2<float>														& collision									= collisions		[iSeg];
				const ::gpk::SLine2D<float>													& segSelected								= verticalSegments	[iSeg];
				if(1 == ::gpk::line_line_intersect(projectilePath, segSelected, collision)) {
					bool																		bFound										= false;
					for(uint32_t iS2 = 0; iS2 < iSeg; ++iS2) {
						if(collision == collisions[iS2]) {
							bFound																	= true;
							info_printf("Discarded collision point.");
							break;
						}
					}
					if(false == bFound)
 						applicationInstance.StuffToDraw.CollisionPoints.push_back(collision);
				}
			}
		}
	}
	return 0;
}
//         3     4      5
//   --------------------------
// 2 -------------------------- 8
//   --------------------------
// 1 -------------------------- 7
//   --------------------------
// 0 -------------------------- 6
//   --------------------------
//         9    10     11
static				const ::gpk::array_static<::gpk::SCoord2<int32_t>, 12>		spawnerPositions								= {
	{ {(int32_t)0 - 8, (int32_t)GAME_SCREEN_SIZE.y / 4 * 1}						// -- Left
	, {(int32_t)0 - 8, (int32_t)GAME_SCREEN_SIZE.y / 4 * 2}
	, {(int32_t)0 - 8, (int32_t)GAME_SCREEN_SIZE.y / 4 * 3}

	, {(int32_t)GAME_SCREEN_SIZE.x / 4 * 1, (int32_t)GAME_SCREEN_SIZE.y	+ 8}	// Top
	, {(int32_t)GAME_SCREEN_SIZE.x / 4 * 2, (int32_t)GAME_SCREEN_SIZE.y	+ 8}
	, {(int32_t)GAME_SCREEN_SIZE.x / 4 * 3, (int32_t)GAME_SCREEN_SIZE.y	+ 8}

	, {(int32_t)GAME_SCREEN_SIZE.x + 8, (int32_t)GAME_SCREEN_SIZE.y / 4 * 1}	// Right
	, {(int32_t)GAME_SCREEN_SIZE.x + 8, (int32_t)GAME_SCREEN_SIZE.y / 4 * 2}
	, {(int32_t)GAME_SCREEN_SIZE.x + 8, (int32_t)GAME_SCREEN_SIZE.y / 4 * 3}

	, {(int32_t)GAME_SCREEN_SIZE.x / 4 * 1	, (int32_t)0 - 8}					// Bottom
	, {(int32_t)GAME_SCREEN_SIZE.x / 4 * 2	, (int32_t)0 - 8}
	, {(int32_t)GAME_SCREEN_SIZE.x / 4 * 3	, (int32_t)0 - 8}
	}
};

static				::gpk::error_t												spawnEnemy										(::SGame & gameInstance, int32_t spawnerIndex, const ::gpk::SCoord2<uint32_t> & offscreenMetrics)			{
	int32_t																				indexToSpawnEnemy								= firstUnused(gameInstance.Enemies.Alive);
	ree_if(indexToSpawnEnemy == -1, "Not enough space in enemy container to spawn more enemies!");
	gameInstance.Enemies.Alive			[indexToSpawnEnemy]							= 1;
	gameInstance.Enemies.Position		[indexToSpawnEnemy]							= spawnerPositions[spawnerIndex % 12].Cast<float>();//{offscreenMetrics.x - 1.0f, (float)(rand() % offscreenMetrics.y)};
	gameInstance.Enemies.Health			[indexToSpawnEnemy]							= {5000, 5000};
	gameInstance.Enemies.Weapon			[indexToSpawnEnemy]							= {(int32_t)(MAX_PLAYER_WEAPONS + (rand() % (::gpk::size(::weaponProperties) - MAX_PLAYER_WEAPONS)))};
	gameInstance.Enemies.WeaponDelay	[indexToSpawnEnemy]							= 0;
	gameInstance.Enemies.States			[indexToSpawnEnemy].Firing					= true;
	++gameInstance.CountEnemies;
	offscreenMetrics;
	return 0;
}

					::gpk::error_t												updateEnemies									(::SApplication & applicationInstance)			{
	::gpk::SFramework																	& framework										= applicationInstance.Framework;
	const ::gpk::SCoord2<uint32_t>														& offscreenMetrics								= framework.MainDisplayOffscreen->Color.View.metrics();
	::SGame																				& gameInstance									= applicationInstance.Game;
	gameInstance.GhostTimer															+= framework.FrameInfo.Seconds.LastFrame;
	static float																		timerSpawn										= 0;
	timerSpawn																		+= (float)framework.FrameInfo.Seconds.LastFrame;
	if(timerSpawn > 5.5) {
		timerSpawn																		= 0;
		gpk_necall(::spawnEnemy(gameInstance, rand() % ::gpk::size(spawners), offscreenMetrics), "Something prevented to add more enemies. Probably a static limit on the container.");
	}
	for(uint32_t iEnemy = 0; iEnemy < gameInstance.Enemies.Alive.size(); ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		float																		& timerPath									= gameInstance.Enemies.TimerPath[iEnemy];
		gameInstance.Enemies.SkillTimer[iEnemy]									+= framework.FrameInfo.Seconds.LastFrame;
		timerPath																+= (float)framework.Timer.LastTimeSeconds;
		uint32_t																	& enemyPathStep								= gameInstance.Enemies.PathStep[iEnemy];
		if(timerPath > 10.0f) {
			timerPath																= 0;
			++enemyPathStep;
			if( enemyPathStep >= ::gpk::size(gameInstance.PathEnemy) )
				enemyPathStep															= 0;
		}
		{
			::gpk::SCoord2<float>														& enemyPosition								= gameInstance.Enemies.Position[iEnemy];
			const ::gpk::SCoord2<float>													& pathTarget								= gameInstance.PathEnemy[enemyPathStep];
			::gpk::SCoord2<float>														directionEnemy								= (pathTarget - enemyPosition);
			if(directionEnemy.LengthSquared() < 0.5) {
				timerPath																= 0;
				++enemyPathStep;
				if( enemyPathStep >= ::gpk::size(gameInstance.PathEnemy) )
					enemyPathStep															= 0;
			}
			else {
				directionEnemy.InPlaceNormalize();
				enemyPosition															+= directionEnemy * (float)(framework.FrameInfo.Seconds.LastFrame * 100);// * (applicationInstance.ShipState.Brakes ? .25f : (applicationInstance.ShipState.Thrust ? 2 : 1));
			}
		}
	}
	return 0;
}
