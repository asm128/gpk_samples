// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "gpk_particle.h"
#include "gpk_framework.h"
#include "game.h"
#include "spawner.h"
#include "track.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

enum PARTICLE_TYPE : int8_t
	{	PARTICLE_TYPE_SNOW			= 0
	,	PARTICLE_TYPE_SHIP_THRUST
	,	PARTICLE_TYPE_STAR
	,	PARTICLE_TYPE_PROJECTILE
	,	PARTICLE_TYPE_BOMB
	,	PARTICLE_TYPE_DEBRIS
	,	PARTICLE_TYPE_COUNT
	,	PARTICLE_TYPE_INVALID		= -1
	};

#pragma pack(push, 1)
struct SGameParticle {
						PARTICLE_TYPE																				Type										= (PARTICLE_TYPE)-1;
						PLAYER_TYPE																					TypePlayer									= (PLAYER_TYPE	)-1;
						WEAPON_TYPE																					TypeWeapon									= (WEAPON_TYPE	)-1;
						int8_t																						IndexWeapon									= -1;
						int8_t																						Padding										= 0;
						uint32_t																					OwnerIndex									= (uint32_t		)-1;
						float																						TimeLived									= 0;
						bool																						Lit											= true;
};

struct SParticleToDraw {
						int32_t																						IndexParticlePhysics;
						int32_t																						IndexParticleInstance;
						float																						TimeLived;
						::gpk::SCoord2<int32_t>																		Position;
};

struct SLaserToDraw {
						int32_t																						IndexParticlePhysics;
						int32_t																						IndexParticleInstance;
						::gpk::SLine2<float>																		Segment;
};
#pragma pack(pop)

struct SStuffToDraw {
						::gpk::array_pod<::SLaserToDraw>															ProjectilePaths								= {};
						::gpk::array_pod<::SParticleToDraw>															Stars										= {};
						::gpk::array_pod<::SParticleToDraw>															Thrust										= {};
						::gpk::array_pod<::SParticleToDraw>															Debris										= {};
						::gpk::array_pod<::gpk::SCoord2<float>>														CollisionPoints								= {};

						::gpk::array_obj<::gpk::view_grid<::gpk::SColorBGRA>>										TexturesPowerup0							= {};
						::gpk::array_obj<::gpk::view_grid<::gpk::SColorBGRA>>										TexturesPowerup1							= {};
};

enum GAME_TEXTURE : int8_t
	{ GAME_TEXTURE_FONT_ATLAS		= 0
	, GAME_TEXTURE_SHIP0
	, GAME_TEXTURE_SHIP1
	, GAME_TEXTURE_POWCORESQUARE
	, GAME_TEXTURE_POWCOREDIAGONAL
	, GAME_TEXTURE_CROSSHAIR
	, GAME_TEXTURE_POWICON
	, GAME_TEXTURE_ENEMY
	, GAME_TEXTURE_COUNT
	, GAME_TEXTURE_INVALID			= -1
	};

struct SEffectsDelay {
						double																						Thrust;
						double																						Star;
};

static constexpr	const ::gpk::n2<uint16_t>																	GAME_SCREEN_SIZE							= {640 / 2 * 3, 360 / 2 * 3};

struct SLevelState {
	int32_t							Number;
	float							Time;
};

struct SApplication {
	typedef				::gpk::SParticleSystem<::SGameParticle, float>												TParticleSystem;
	typedef				TParticleSystem::TParticleInstance															TParticleInstance;
	typedef				TParticleSystem::TIntegrator																TIntegrator;
	typedef				TIntegrator::TParticle																		TParticle;

						::gpk::SFramework																			Framework									;

						::SGame																						Game										= {};
						::SLevelState																				Level										= {0, 0};
						bool																						Playing										= true;
						bool																						Paused										= true;
						bool																						Debugging									= true;

						TParticleSystem																				ParticleSystemThrust						= {};
						TParticleSystem																				ParticleSystemProjectiles					= {};
						TParticleSystem																				ParticleSystemDebris						= {};
						TParticleSystem																				ParticleSystemStars							= {};

						::gpk::SColorBGRA																			ColorBackground								= ::gpk::SColorFloat(.15f, .15f, .15f, 1.0f);

						::gpk::array_static<::gpk::img<::gpk::SColorBGRA>				, GAME_TEXTURE_COUNT>		Original									= {};
						::gpk::array_static<::gpk::img<::gpk::SColorBGRA>				, GAME_TEXTURE_COUNT>		Processed									= {};
						::gpk::array_static<::gpk::SCoord2<int32_t>						, GAME_TEXTURE_COUNT>		TextureCenters								= {};
						::gpk::SCoord2<int32_t>																		PSOffsetFromShipCenter						= {};
						::gpk::SImageMonochrome<uint32_t>															TextureFontMonochrome						= {};

						::SStuffToDraw																				StuffToDraw									= {};
						::gpk::array_pod<::gpk::SCoord2<int16_t>>													CacheLinePoints								= {};
						::SEffectsDelay																				EffectsDelay								= {};

																													SApplication								(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
