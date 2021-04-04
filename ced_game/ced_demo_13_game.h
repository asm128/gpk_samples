#include "gpk_array.h"
#include "gpk_color.h"
#include "gpk_image.h"
#include "gpk_matrix.h"
#include "gpk_model.h"
#include "gpk_rigidbody.h"
#include "gpk_geometry_lh.h"

#include <mutex>

#ifndef CED_DEMO_13_GAME_H_293874239874
#define CED_DEMO_13_GAME_H_293874239874

namespace ssg
{
	struct SStars	{
		::gpk::array_pod<::gpk::SCoord2<float>>		Position			= {};
		::gpk::array_pod<float>						Speed				= {};
		::gpk::array_pod<float>						Brightness			= {};

		int											Update				(uint32_t yMax, float secondsLastFrame)	{
			static constexpr const double					randUnit			= 1.0 / RAND_MAX;
			for(uint32_t iStar = 0; iStar < Brightness.size(); ++iStar) {
				::gpk::SCoord2<float>							& starPos			= Position[iStar];
				float											& starSpeed			= Speed[iStar];
				starPos.y									+= starSpeed * secondsLastFrame;
				Brightness[iStar]							= float(randUnit * rand());
				if(starPos.y >= yMax) {
					starSpeed									= float(16 + (rand() % 64)) * .1f;
					starPos.y									= 0;
				}
			}
			return 0;
		}
	};

	struct SParticles3 {
		::gpk::array_pod<::gpk::SCoord3<float>>		Position			= {};
		::gpk::array_pod<::gpk::SCoord3<float>>		Direction			= {};
		::gpk::array_pod<float>						Speed				= {};

		int											IntegrateSpeed		(double secondsLastFrame)	{
			for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
				::gpk::SCoord3<float>							& direction			= Direction	[iShot];
				::gpk::SCoord3<float>							& position			= Position	[iShot];
				float											& speed				= Speed		[iShot];
				position									+= direction * speed * secondsLastFrame;
			}
			return 0;
		}
		int											Remove				(int32_t iParticle)	{
			Position	.remove_unordered(iParticle);
			Direction	.remove_unordered(iParticle);
			return Speed.remove_unordered(iParticle);
		}
		int											Spawn				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed)	{
			Position	.push_back(position);
			Direction	.push_back(direction);
			return Speed.push_back(speed);
		}
	};

	struct SDebris	{
		::gpk::SColorBGRA							Colors[4]			=
			{ {0x80, 0xAF, 0xFF, }
			, {0x40, 0x80, 0xFF, }
			, {0x20, 0x80, 0xFF, }
			, {0x00, 0x00, 0xFF, }
			};
		::gpk::array_pod<float>						Brightness			= {};
		::ssg::SParticles3							Particles			= {};

		int											Spawn				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed, float brightness)	{
			Particles.Spawn(position, direction, speed);
			return Brightness.push_back(brightness);
		}
		int											SpawnSpherical		(uint32_t countDebris, const ::gpk::SCoord3<float> & position, float speedDebris, float brightness)	{
			static constexpr const double					randUnit			= ::gpk::math_2pi / RAND_MAX;
			for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
				::gpk::SCoord3<float>							direction			= {0, 1, 0};
				direction.RotateX(rand() * randUnit);
				direction.RotateY(rand() * randUnit);
				direction.Normalize();
				Spawn(position, direction, speedDebris, brightness);
			}
			return 0;
		}
		int											SpawnDirected		(uint32_t countDebris, double noiseFactor, const ::gpk::SCoord3<float> & direction, const ::gpk::SCoord3<float> & position, float speedDebris, float brightness)	{
			static constexpr const double					randUnit			= ::gpk::math_2pi / RAND_MAX;
			for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
				::gpk::SCoord3<float>							finalDirection		= {0, 1, 0};
				finalDirection.RotateX(rand() * randUnit);
				finalDirection.RotateY(rand() * randUnit);
				finalDirection.Normalize();
				Spawn(position, ::gpk::interpolate_linear(direction, finalDirection, noiseFactor), speedDebris, brightness);
			}
			return 0;
		}
		int											Update				(float secondsLastFrame)	{
			Particles.IntegrateSpeed(secondsLastFrame);
			for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
				float											& speed				= Particles.Speed		[iShot];
				float											& brightness 		= Brightness			[iShot];
				brightness									-= secondsLastFrame;
				speed										-= secondsLastFrame * ((0 > speed) ? (rand() % 16) * 5 : (rand() % 16));

				if(0 > brightness) {
					Particles.Remove(iShot);
					Brightness.remove_unordered(iShot--);
				}
			}
			return 0;
		}
	};

	enum WEAPON_TYPE
		{ WEAPON_TYPE_GUN		= 0
		, WEAPON_TYPE_CANNON
		, WEAPON_TYPE_ROCKET
		, WEAPON_TYPE_SHOTGUN
		, WEAPON_TYPE_SHIELD
		, WEAPON_TYPE_COUNT
		};

	enum MUNITION_TYPE
		{ MUNITION_TYPE_BULLET		= 0
		, MUNITION_TYPE_SHELL
		, MUNITION_TYPE_RAY
		, MUNITION_TYPE_ROCKET
		, MUNITION_TYPE_MISSILE
		, MUNITION_TYPE_WAVE
		, MUNITION_TYPE_FLARE
		, MUNITION_TYPE_COUNT
		};

	struct SShots	{
		int32_t										Weapon				= WEAPON_TYPE_GUN;
		double										Delay				= 0;
		int32_t										Damage				= 1;
		double										MaxDelay			= .1;
		MUNITION_TYPE								Type				= MUNITION_TYPE_BULLET;

		::gpk::array_pod<float>						Brightness			= {};
		::gpk::array_pod<::gpk::SCoord3<float>>		PositionPrev		= {};
		::gpk::array_pod<::gpk::SCoord3<float>>		PositionDraw		= {};
		::ssg::SParticles3							Particles;
		::gpk::SIntegrator3							Bodies;

		int											SpawnForgpk			(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed, float brightness)	{
			Particles.Spawn(position, direction, speed);
			PositionPrev.push_back(position);
			PositionDraw.push_back(position);
			return Brightness.push_back(brightness);
		}
		int											SpawnForgpkDirected	(double noiseFactor, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speedDebris, float brightness)	{
			static constexpr const double					randUnit			= ::gpk::math_2pi / RAND_MAX;
			::gpk::SCoord3<float>							finalDirection		= {0, 1, 0};
			finalDirection.RotateX(rand() * randUnit);
			finalDirection.RotateY(rand() * randUnit);
			finalDirection.Normalize();
			return SpawnForgpk(position, ::gpk::interpolate_linear(direction, finalDirection, noiseFactor), speedDebris, brightness);
		}
		int											Spawn				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed, float brightness)	{
			if(Delay < MaxDelay)
				return 0;
			Delay										= 0;
			return SpawnForgpk(position, direction, speed, brightness);
		}
		int											SpawnDirected		(double noiseFactor, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speedDebris, float brightness)	{
			if(Delay < MaxDelay)
				return 0;
			Delay										= 0;
			return SpawnForgpkDirected(noiseFactor, position, direction, speedDebris, brightness);
		}
		int											SpawnDirected		(uint32_t countShots, double noiseFactor, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speedDebris, float brightness)	{
			if(Delay < MaxDelay || 0 == countShots)
				return 0;
			int32_t											indexFirst			= -1;
			if(countShots)
				indexFirst									= SpawnForgpkDirected(noiseFactor, position, direction, speedDebris, brightness);
			for(uint32_t iDebris = 0; iDebris < (countShots - 1); ++iDebris)
				SpawnForgpkDirected(noiseFactor, position, direction, speedDebris, brightness);
			Delay										= 0;
			return indexFirst;
		}
		int											Update				(float secondsLastFrame)	{
			static constexpr	const uint32_t				maxRange			= 200;
			static constexpr	const uint32_t				maxRangeSquared		= maxRange * maxRange;
			memcpy(PositionPrev.begin(), Particles.Position.begin(), Particles.Position.size() * sizeof(::gpk::SCoord3<float>));
			Particles.IntegrateSpeed(secondsLastFrame);
			for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
				if (Particles.Position[iShot].LengthSquared() > maxRangeSquared)
					Remove(iShot--);
			}
			return 0;
		}
		int											Remove				(uint32_t iShot)			{
			Particles.Remove(iShot);
			PositionPrev		.remove_unordered(iShot);
			PositionDraw		.remove_unordered(iShot);
			return Brightness	.remove_unordered(iShot);
		}
	};

	struct SExplosion {
		int32_t										IndexMesh;
		::gpk::array_pod<::gpk::SRange<uint16_t>>	Slices;
		::ssg::SParticles3							Particles;

		int											Update				(float secondsLastFrame)	{
			Particles.IntegrateSpeed(secondsLastFrame);
			for(uint32_t iShot = 0; iShot < Particles.Speed.size(); ++iShot) {
				float											& speed				= Particles.Speed[iShot];
				//speed										-= secondsLastFrame * (rand() % 16);
				speed										-= secondsLastFrame * ((0 > speed) ? (rand() % 16) * 5 : (rand() % 16));
				if (speed < -30)
					Remove(iShot--);
			}
			return 0;
		}
		int											Remove				(uint32_t iSlice)			{
			Particles.Remove(iSlice);
			return Slices.remove_unordered(iSlice);
		}
	};

	#pragma pack(push, 1)
	struct SEntityFlags {
		int32_t												Padding						:1;
	};
	#pragma pack(pop)

	struct SEntity {
		int32_t												Parent						;
		int32_t												Geometry					;
		int32_t												Transform					;
		int32_t												Image						;
		int32_t												Body						;
		SEntityFlags										Flags						;
		::gpk::array_pod<uint32_t>							Children					;
	};

	enum SHIP_PART_TYPE
		{ SHIP_PART_TYPE_CANNON		= 0
		, SHIP_PART_TYPE_WAFER
		, SHIP_PART_TYPE_GUN
		, SHIP_PART_TYPE_COIL
		, SHIP_PART_TYPE_SHIELD
		, SHIP_PART_TYPE_SILO
		, SHIP_PART_TYPE_TRACTOR
		, SHIP_PART_TYPE_COUNT
		};

	struct SShipPart {
		int32_t												Entity	;
		int32_t												Type	;
		int32_t												Health	;
		::ssg::SShots										Shots	;
	};

	struct SShip {
		int32_t												Entity	;
		int32_t												Team	;
		int32_t												Health	;
		::gpk::array_obj<::ssg::SShipPart>					Parts	;
	};

	enum CAMERA_MODE
		{ CAMERA_MODE_SKY			= 0
		, CAMERA_MODE_PERSPECTIVE
		, CAMERA_MODE_FOLLOW
		//, CAMERA_MODE_FRONT
		, CAMERA_MODE_COUNT
		};

	struct SShipScene	{
		::gpk::array_obj<::gpk::SGeometryQuads>				Geometry						= {};
		::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>	Image							= {};
		//::gpk::array_pod<::gpk::SMatrix4<float>>			ModelMatricesLocal				= {};
		::gpk::array_pod<::gpk::SMatrix4<float>>			ModelMatricesGlobal				= {};
		//::gpk::array_pod<::gpk::SModel3>					Transforms						= {};
		::gpk::SCamera										Camera[CAMERA_MODE_COUNT]		= {};
		::gpk::SCoord3	<float>								LightVector						= {0, -12, 0};
		::gpk::SMatrix4	<float>								MatrixProjection				= {};
		CAMERA_MODE											CameraMode						= CAMERA_MODE_PERSPECTIVE;
	};

	struct SSolarSystemDrawCache {
		::gpk::array_pod<::gpk::SCoord2<int16_t>>			PixelCoords				;
		::gpk::array_pod<::gpk::STriangleWeights<float>>	PixelVertexWeights		;
		::gpk::array_pod<::gpk::SCoord3<float>>				LightPointsWorld		;
		::gpk::array_pod<::gpk::SColorBGRA>					LightColorsWorld		;
		::gpk::array_pod<::gpk::SCoord3<float>>				LightPointsModel		;
		::gpk::array_pod<::gpk::SColorBGRA>					LightColorsModel		;
	};

	struct SSolarSystem {
		::ssg::SShipScene									Scene							= {};
		::gpk::array_obj<::ssg::SEntity>					Entities						= {};
		::gpk::array_obj<::ssg::SShip>						Ships							= {};
		::gpk::SIntegrator3									ShipPhysics						= {};

		::ssg::SStars										Stars							= {};
		::ssg::SDebris										Debris							= {};
		::gpk::array_obj<::ssg::SExplosion>					Explosions						= {};

		double												AnimationTime					= 0;
		double												TimeScale						= 1;

		static constexpr const double						RelativeSpeedTarget				= 20;
		double												RelativeSpeedCurrent			= -50;

		::gpk::SImage<::gpk::SColorBGRA>					BackgroundImage					= {};
		bool												Slowing							= true;
		int													AccelerationControl				= 0;

		uint32_t											Stage							= 0;
		uint32_t											Score							= 0;
		double												CameraSwitchDelay				= 0;

		SSolarSystemDrawCache								DrawCache;
		::std::mutex										LockUpdate;
	};

	int													stageSetup						(::ssg::SSolarSystem & solarSystem);
	int													solarSystemSetup				(::ssg::SSolarSystem & solarSystem, ::gpk::SCoord2<uint32_t> windowSize);
	int													solarSystemDraw					(const ::ssg::SSolarSystem & solarSystem, ::ssg::SSolarSystemDrawCache & drawCache, ::std::mutex & lockUpdate, ::gpk::view_grid<::gpk::SColorBGRA> & targetPixels, ::gpk::view_grid<uint32_t> depthBuffer);
	int													solarSystemUpdate				(::ssg::SSolarSystem & solarSystem, double secondsLastFrame, ::gpk::SCoord2<uint32_t> screenSize);
	int													solarSystemSetupBackgroundImage	(::gpk::SImage<::gpk::SColorBGRA> & backgroundImage, ::gpk::SCoord2<uint32_t> windowSize);
	int													setupStars						(SStars & stars, ::gpk::SCoord2<uint32_t> targetSize);
}

#endif // CED_DEMO_13_GAME_H_293874239874
