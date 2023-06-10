#include "gpk_geometry.h"
#include "gpk_model.h"
#include "gpk_camera.h"
#include "gpk_rigidbody.h"
#include "gpk_particle.h"

#include "gpk_image.h"
#include "gpk_ptr_obj.h"

#ifndef SOLARSYSTEM_H_92837423798
#define SOLARSYSTEM_H_92837423798

namespace ssg 
{
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
		::gpk::n3f32						Scale							= {1, 1, 1};
		::gpk::n3f32						Position						= {};
	};

	struct SRenderNode {
	};

	struct SScene {
		::gpk::apod<::ssg::SModelPivot>		Pivot							= {};
		::gpk::apod<::gpk::m4f32>			Transform						= {};

		::gpk::SCameraPoints				Camera							= {};
	};

	struct SEntity {
		int32_t								Parent						;
		int32_t								Geometry					;
		int32_t								Model						;
		int32_t								Images						;
		int32_t								Body						;
		::gpk::au32							Children					;
	};

	struct SDebris	{
		::gpk::bgra					Colors[4]			=
			{ {0x80, 0xAF, 0xFF, }
			, {0x40, 0xAF, 0xFF, }
			, {0x80, 0xCF, 0xFF, }
			//, {0x00, 0x00, 0xFF, }
			};
		::gpk::af32					Brightness			= {};
		::gpk::SParticles3			Particles			= {};

		int							Create				(const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speed, float brightness)	{
			Particles.Create(position, direction, speed);
			return Brightness.push_back(brightness);
		}
		int							SpawnSpherical		(uint32_t countDebris, const ::gpk::n3f32 & position, float speedDebris, float brightness, float offset)	{
			for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
				::gpk::n3f32										direction				= {0, 1 * offset, 0};
				direction.RotateX(rand() * (::gpk::math_2pi / RAND_MAX));
				direction.RotateY(rand() * (::gpk::math_2pi / RAND_MAX));
				direction.RotateZ(rand() * (::gpk::math_2pi / RAND_MAX));
				const ::gpk::n3f32	newPosition		= position + direction;
				direction.Normalize();
				Create(newPosition, direction, speedDebris, brightness);
			}
			return 0;
		}
		int							Update				(double secondsLastFrame)	{
			Particles.IntegrateSpeed(secondsLastFrame);
			const float											fLastFrame			= (float)secondsLastFrame;
			for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
				float														& speed				= Particles.Speed		[iShot];
				float														& brightness 		= Brightness			[iShot];
				brightness												-= fLastFrame;
				speed													-= fLastFrame * (rand() % 16);
				if(brightness < 0) {
					Particles.Remove(iShot);
					Brightness.remove_unordered(iShot--);
				}
			}
			return 0;
		}
	};

#pragma pack(push, 1)
	struct SStarDetail {
		uint32_t					Mass					; // 1988500	// (10^24 kg)		
		uint32_t					Gm						; // 132712		// (x 106 km3/s2)	
		uint32_t					Volume					; // 1412000	// (1012 km3)		
		uint32_t					VolumetricMeanRadius	; // 695700		// (km)			
		uint32_t					MeanDensity				; // 1408		// (kg/m3)			
		float						SurfaceGravity			; // 274.0		// (eq.) (m/s2)	
		float						EscapeVelocity			; // 617.6		// (km/s)			
		float						Ellipticity				; // 0.00005	
		float						MomentOfInertia			; // 0.070		// (I/MR2)	
		float						VisualMagnitude			; // -26.74		// V(1,0)	
		float						AbsoluteMagnitude		; // 4.83		
		float						Luminosity				; // 382.8		// (1024 J/s)
		uint32_t					MassConversionRate		; // 4260		// (106 kg/s)
		float						MeanEnergyProduction	; // 0.1925		// (10-3 J/kg s)
		float						SurfaceEmission			; // 62.94		// (106 J/m2s)
		float						CentralPressure			; // 2.477		// x 1011	bar
		float						CentralTemperature		; // 1.571		// x 107	K
		float						CentralDensity			; // 1.622		// x 105	kg/m3
	};

	struct SPlanetDetail {
		float						Mass					; // 0.330		// (10^24 kg)	
		uint32_t					Diameter				; // 4879		// (km)
		uint32_t					Density					; // 5427		// (kg/m3)
		float						Gravity					; // 3.7		// (m/s^2)
		float						EscapeVelocity			; // 4.3		// (km/s)
		float						RotationPeriod			; // 1407.6		// (hours)	
		float						LengthOfDay				; // 4222.6		// (hours)
		float						DistanceFromSun			; // 57.9		// (10^6 km)
		float						Perihelion				; // 46.0		// (10^6 km)
		float						Aphelion				; // 69.8		// (10^6 km)
		float						OrbitalPeriod			; // 88.0		// (days)	
		float						OrbitalVelocity			; // 47.4		// (km/s)
		float						OrbitalInclination		; // 7.0		// (degrees)	
		float						OrbitalEccentricity		; // 0.205
		float						ObliquityToOrbit		; // 0.034		// (degrees)
		uint32_t					MeanTemperature			; // 167		// (Celsius)
		uint32_t					SurfacePressure			; // 0			// (Bars)
		uint32_t					NumberOfMoons			; // 0			// 
		bool						RingSystem				; // false
		bool						GlobalMagneticField		; // true
	};
#pragma pack(pop)

	struct SStar {
		::gpk::vcc					Name					; // "Sun" :
		::gpk::vcc					SpectralType			; // "G2 V"
		int32_t						Parent					; // "Sun"
	};

	struct SPlanet {
		::gpk::vcc					Name					; // "Mercury" :
		int32_t						Parent					; // "Sun"
	};

	GDEFINE_ENUM_TYPE(STELLAR_BODY, uint8_t);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, STAR		, 0);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, PLANET		, 1);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, MOON		, 2);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, ASTEROID	, 3);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, COMET		, 4);

	struct SStellarBody {
		STELLAR_BODY						Type;
		uint16_t							Index;
	};

	struct SSolarSystem {
		::gpk::aobj<::ssg::SStar>			NamesStars						= {};
		::gpk::aobj<::ssg::SPlanet>			NamesPlanets					= {};
		::gpk::apod<::ssg::SStarDetail>		DetailStars						= {};
		::gpk::apod<::ssg::SPlanetDetail>	DetailPlanets					= {};
		::gpk::apod<::ssg::SStellarBody>	StellarBodies					= {};

		::gpk::error_t						CreateStellarBody				(STELLAR_BODY type) {
			int32_t									index							= -1;
			switch(type) {
			case STELLAR_BODY_STAR	: index = StellarBodies.push_back({STELLAR_BODY_STAR  , (uint16_t)CreateStar	()}); break;
			case STELLAR_BODY_PLANET: index = StellarBodies.push_back({STELLAR_BODY_PLANET, (uint16_t)CreatePlanet	()}); break;
			}
			return index;
		}

		::gpk::error_t						RemoveStellarBody				(int32_t iStellarBody) {
			int32_t									index							= StellarBodies[iStellarBody].Index;
			switch(StellarBodies[iStellarBody].Type) {
			case STELLAR_BODY_STAR	: RemoveStar	(index);	
			case STELLAR_BODY_PLANET: RemovePlanet	(index);	
			}
			return StellarBodies.remove(iStellarBody);
		}
	protected:
		::gpk::error_t						CreateStar						(::gpk::vcs name = {}) {
			NamesStars.push_back({name});
			return DetailStars.push_back({});
		}

		::gpk::error_t						CreatePlanet					() {
			NamesPlanets.push_back({});
			return DetailPlanets.push_back({});
		}

		::gpk::error_t						RemovePlanet					(int32_t iPlanet) {
			NamesPlanets.remove(iPlanet);
			return DetailPlanets.remove(iPlanet);
		}

		::gpk::error_t						RemoveStar						(int32_t iStar) {
			NamesStars.remove(iStar);
			return DetailStars.remove(iStar);
		}
	};

	struct SSolarSystemGame {
		::ssg::SSolarSystem								SolarSystem						= {};
		::ssg::SDebris									SunFire							= {};
		::ssg::SScene									Scene							= {};
		::gpk::SRigidBodyIntegrator						Bodies							= {};
		::gpk::aobj<::ssg::SEntity>						Entities						= {};
		::gpk::aobj<::gpk::img<::gpk::bgra>>			Images							= {};
		::gpk::aobj<::gpk::STrianglesIndexed>	Geometries						= {};
	};

	::gpk::error_t									solarSystemSetup				(::ssg::SSolarSystemGame & sunsystem, const ::gpk::vcc & filename);
	::gpk::error_t									solarSystemUpdate				(::ssg::SSolarSystemGame & sunsystem, double secondsLastFrame, ::gpk::pobj<::gpk::rtbgra8d32> & target);

} // namespace

#endif // SOLARSYSTEM_H_92837423798

// 08003451058 galicia