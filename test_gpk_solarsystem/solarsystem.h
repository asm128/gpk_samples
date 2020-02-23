#include "gpk_raster_lh.h"
#include "gpk_model.h"
#include "gpk_rigidbody.h"
#include "ced_particles.h"

#include "gpk_image.h"

#ifndef SOLARSYSTEM_H_92837423798
#define SOLARSYSTEM_H_92837423798

enum PLANET
	{ PLANET_MERCURY
	, PLANET_VENUS
	, PLANET_EARTH
	, PLANET_MARS
	, PLANET_JUPITER
	, PLANET_SATURN
	, PLANET_URANUS
	, PLANET_NEPTUNE
	, PLANET_PLUTO

	, PLANET_COUNT
	};

struct SModelPivot {
	::gpk::SCoord3<float>								Scale							= {1, 1, 1};
	::gpk::SCoord3<float>								Position						= {};
};

struct SScene {
	::gpk::array_pod<::SModelPivot>						Pivot							= {};
	::gpk::array_obj<::gpk::SGeometryTriangles>			Geometry						= {};
	::gpk::array_pod<::gpk::SMatrix4<float>>			Transform						= {};

	::gpk::SCamera										Camera							= {};
};

struct SEntity {
	int32_t												IndexParent						;
	int32_t												IndexGeometry					;
	int32_t												IndexModel						;
	int32_t												IndexImage						;
	int32_t												IndexBody						;
	::gpk::array_pod<uint32_t>							IndexChild						;
};

struct SDebris	{
	::gpk::SColorBGRA							Colors[4]			=
		{ {0x80, 0xAF, 0xFF, }
		, {0x40, 0xAF, 0xFF, }
		, {0x80, 0xCF, 0xFF, }
		//, {0x00, 0x00, 0xFF, }
		};
	::gpk::array_pod<float>						Brightness			= {};
	::ced::SParticles3							Particles			= {};

	int											Spawn				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed, float brightness)	{
		Particles.Spawn(position, direction, speed);
		return Brightness.push_back(brightness);
	}
	int											SpawnSpherical		(uint32_t countDebris, const ::gpk::SCoord3<float> & position, float speedDebris, float brightness, float offset)	{
		for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
			::gpk::SCoord3<float>									direction				= {0, 1 * offset, 0};
			direction.RotateX(rand() * (::gpk::math_2pi / RAND_MAX));
			direction.RotateY(rand() * (::gpk::math_2pi / RAND_MAX));
			direction.RotateZ(rand() * (::gpk::math_2pi / RAND_MAX));
			const ::gpk::SCoord3<float>newPosition		= position + direction;
			direction.Normalize();
			Spawn(newPosition, direction, speedDebris, brightness);
		}
		return 0;
	}
	int											Update				(double secondsLastFrame)	{
		Particles.IntegrateSpeed(secondsLastFrame);
		const float										fLastFrame			= (float)secondsLastFrame;
		for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
			float											& speed				= Particles.Speed		[iShot];
			float											& brightness 		= Brightness			[iShot];
			brightness									-= fLastFrame;
			speed										-= fLastFrame * (rand() % 16);
			if(brightness < 0) {
				Particles.Remove(iShot);
				Brightness.remove_unordered(iShot--);
			}
		}
		return 0;
	}
};

struct SSolarSystem {
	::SDebris											SunFire							= {};
	::gpk::SIntegrator3									Bodies							= {};
	::SScene											Scene							= {};
	::gpk::array_obj<::SEntity>							Entities						= {};
	::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>	Image							= {};
};

#endif // SOLARSYSTEM_H_92837423798
