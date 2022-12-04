#include "solarsystem.h"
#include "gpk_json.h"
#include "gpk_stdstring.h"
#include "gpk_label.h"

struct SPlanet {
	float	Mass				;
	float	Scale				;
	float	Day					;
	float	AxialTilt			;
	float	Distance			;
	float	OrbitalPeriod		;
	float	OrbitalVelocity		;
	float	OrbitalInclination	;
	float	OrbitalEccentricity	;
	float	Images				;
};															

																						//	- Mercury			- Venus				- Earth				- Mars				- Jupiter				- Saturn				- Uranus				- Neptune				- Pluto
static constexpr const double	PLANET_MASSES				[::ssg::PLANET_COUNT]	=	{	0.330f				, 4.87f				, 5.97f				, 0.642f			, 1899					, 568					, 86.8f					, 102					, 0.0125f			};
static constexpr const double	PLANET_SCALES				[::ssg::PLANET_COUNT]	=	{	4879				, 12104				, 12756				, 6792				, 142984				, 120536				, 51118					, 49528					, 2390				};
static constexpr const double	PLANET_DAY					[::ssg::PLANET_COUNT]	=	{	4222.6f				, 2802.0f			, 23.9f				, 24.7f				, 9.9f					, 10.7f					, 17.2f					, 16.1f					, 153.3f			};
static constexpr const double	PLANET_AXIALTILT			[::ssg::PLANET_COUNT]	=	{	0.01f				, 177.4f			, 23.4f				, 25.2f				, 3.1f					, 26.7f					, 97.8f					, 28.3f					, 122.5f			};
static constexpr const double	PLANET_DISTANCE				[::ssg::PLANET_COUNT]	=	{	57.9f				, 108.2f			, 149.6f			, 227.9f			, 778.6f				, 1433.5f				, 2872.5f				, 4495.1f				, 5870.0f			};

static constexpr const double	PLANET_ORBITALPERIOD		[::ssg::PLANET_COUNT]	=	{	88.0f				, 224.7f			, 365.2f			, 687.0f			, 4331					, 10747					, 30589					, 59800					, 90588				};
static constexpr const double	PLANET_ORBITALVELOCITY		[::ssg::PLANET_COUNT]	=	{	47.9f				, 35.0f				, 29.8f				, 24.1f				, 13.1f					, 9.7f					, 6.8f					, 5.4f					, 4.7f				};
static constexpr const double	PLANET_ORBITALINCLINATION	[::ssg::PLANET_COUNT]	=	{	7.0					, 3.4f				, 0.0f				, 1.9f				, 1.3f					, 2.5f					, 0.8f					, 1.8f					, 17.2f				};
static constexpr const double	PLANET_ORBITALECCENTRICITY	[::ssg::PLANET_COUNT]	=	{	0.205f				, 0.007f			, 0.017f			, 0.094f			, 0.049f				, 0.057f				, 0.046f				, 0.011f				, 0.244f			};
static constexpr const char*	PLANET_IMAGE				[::ssg::PLANET_COUNT]	=	{	"mercury_color.png"	, "venus_color.png"	, "earth_color.png"	, "mars_color.png"	, "jupiter_color.png"	, "saturn_color.png"	, "uranus_color.png"	, "neptune_color.png"	, "pluto_color.png"	};

int													ssg::solarSystemSetup				(::ssg::SSolarSystemGame & solarSystem, const ::gpk::vcc & filename)	{
	solarSystem.Geometries.resize(1);
	::gpk::geometryBuildSphere(solarSystem.Geometries[0], 16U, 16U, 1, {});
//	::gpk::geometryBuildFromSTL();

	SPlanet													planet								= {};
	SPlanetDetail											planetdetail						= {};
	SStar													star								= {};
	SStarDetail												stardetail							= {};

	::gpk::SRigidBodyIntegrator								& bodies							= solarSystem.Bodies;
	::ssg::SScene											& scene								= solarSystem.Scene;
	//if constexpr(false && filename.size()) {
	if (filename.size() && 0 == filename.size()) {
		::gpk::SJSONFile					solarSystemFile			= {};
		::gpk::jsonFileRead(solarSystemFile, filename);
		::gpk::array_pod<int32_t>			stellarBodyIndices;
		const uint32_t						stellarBodyCount		= ::gpk::jsonObjectKeyList(solarSystemFile.Reader, 0, stellarBodyIndices);
		::gpk::view_array<const ::gpk::vcc>	jsonView				= solarSystemFile.Reader.View;
		for(uint32_t iPlanet = 0; iPlanet < stellarBodyCount; ++iPlanet) {
			const int32_t						jsonIndexStellarBody	= stellarBodyIndices[iPlanet];

			planet.Name	= star.Name			= jsonView[jsonIndexStellarBody];
			::gpk::array_pod<int32_t>			propertyIndices;
			//int32_t								iBodyValue				= ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody, planet.Name);
			const uint32_t						propertyCount			= ::gpk::jsonObjectKeyList(solarSystemFile.Reader, jsonIndexStellarBody + 2, propertyIndices);
			for(uint32_t iProperty = 0; iProperty < propertyCount; ++iProperty) 
				::gpk::tolower(::gpk::view_char{(char*)jsonView[propertyIndices[iProperty]].begin(), star.Name.size()});

			int32_t								iTypeValue				= ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "type");
			STELLAR_BODY						bodyType				= (0 == strncmp(jsonView[iTypeValue].begin(), "star", 4)) ? STELLAR_BODY_STAR : STELLAR_BODY_PLANET;
			switch(bodyType) {
			case STELLAR_BODY_STAR	: { 
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "mass"					); stardetail.Mass					= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "gm"						); stardetail.Gm					= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "volume"					); stardetail.Volume				= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "volumetric_mean_radius"	); stardetail.VolumetricMeanRadius	= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "mean_density"			); stardetail.MeanDensity			= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "surface_gravity"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.SurfaceGravity			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "escape_velocity"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.EscapeVelocity			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "ellipticity"			); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.Ellipticity				= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "moment_of_inertia"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.MomentOfInertia			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "visual_magnitude"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.VisualMagnitude			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "absolute_magnitude"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.AbsoluteMagnitude		= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "luminosity"				); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.Luminosity				= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "mass_conversion_rate"	); stardetail.MassConversionRate	= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "mean_energy_production"	); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.MeanEnergyProduction		= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "surface_emission"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.SurfaceEmission			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "central_pressure"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.CentralPressure			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "central_temperature"	); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.CentralTemperature		= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "central_density"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); stardetail.CentralDensity			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "spectral_type"			); star.SpectralType				= ::gpk::label(jsonView[iValue]); }
																											   	
			} break;
			case STELLAR_BODY_PLANET: { 
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "mass"					); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.Mass					= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "diameter"				); planetdetail.Diameter			= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "density"				); planetdetail.Density				= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "gravity"				); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.Gravity				= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "escape_velocity"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.EscapeVelocity			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "rotation_period"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.RotationPeriod			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "length_of_day"			); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.LengthOfDay			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "distance_from_sun"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.DistanceFromSun		= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "perihelion"				); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.Perihelion				= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "aphelion"				); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.Aphelion				= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "orbital_period"			); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.OrbitalPeriod			= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "orbital_velocity"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.OrbitalVelocity		= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "orbital_inclination"	); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.OrbitalInclination		= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "orbital_eccentricity"	); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.OrbitalEccentricity	= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "obliquity_to_orbit"		); double temp = 0; memcpy(&temp, &solarSystemFile.Reader.Token[iValue].Value, sizeof(double)); planetdetail.ObliquityToOrbit		= float		(temp); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "mean_temperature"		); planetdetail.MeanTemperature		= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "surface_pressure"		); planetdetail.SurfacePressure		= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "number_of_moons"		); planetdetail.NumberOfMoons		= uint32_t(solarSystemFile.Reader.Token[iValue].Value); }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "ring_system"			); planetdetail.RingSystem			= jsonView[iValue] == ::gpk::vcs{"true"}; }
				{ int32_t iValue = ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "global_magnetic_field"	); planetdetail.GlobalMagneticField	= jsonView[iValue] == ::gpk::vcs{"true"}; }

			} break;
			}

			{ 
				int32_t iValue	= ::gpk::jsonObjectValueGet(solarSystemFile.Reader, jsonIndexStellarBody + 2, "parent"); 
				int32_t iBody	= -1;
				if(iValue >= 0) {
					iBody	= ::gpk::createOrbiter(bodies
						, planetdetail.Mass						
						, planetdetail.DistanceFromSun			
						, planetdetail.ObliquityToOrbit			
						, planetdetail.LengthOfDay				
						, PLANET_DAY							[ssg::PLANET_EARTH]
						, planetdetail.OrbitalPeriod			
						, planetdetail.OrbitalInclination		
						, 1.0// / planetdetail.DistanceFromSun	[ssg::PLANET_COUNT - 1] * 2500
						);
				}
			}
			{ // Set up rigid body
				::ssg::SModelPivot										model							= {};
				const float												scale							= (bodyType == STELLAR_BODY_STAR) ? 10.0f : float(1.0 / PLANET_SCALES[ssg::PLANET_EARTH] * planetdetail.Diameter);
				model.Scale											= {scale, scale, scale};
				model.Position										= {0, 0.5f};
				scene.Pivot.push_back(model);
			}

			solarSystem.SolarSystem.CreateStellarBody(bodyType);
		}
	}
	else {
		solarSystem.Scene.Pivot.resize(::ssg::PLANET_COUNT + 1); //
		for(uint32_t iModel = 0; iModel < solarSystem.Scene.Pivot.size(); ++iModel) {
			int32_t													iPlanet							= iModel - 1;
			{ // Set up rigid body
				::ssg::SModelPivot										& model							= scene.Pivot[iModel];
				const float												scale							= (0 == iModel) ? 10.0f : float(1.0 / PLANET_SCALES[ssg::PLANET_EARTH] * PLANET_SCALES[iPlanet]);
				model.Scale											= {scale, scale, scale};
				model.Position										= {0, 0.5f};
			}
			if(iModel) { // Set up rigid body
				/*int32_t iBody = */::gpk::createOrbiter(bodies
					, PLANET_MASSES				[iPlanet]
					, PLANET_DISTANCE			[iPlanet]
					, PLANET_AXIALTILT			[iPlanet]
					, PLANET_DAY				[iPlanet]
					, PLANET_DAY				[ssg::PLANET_EARTH]
					, PLANET_ORBITALPERIOD		[iPlanet]
					, PLANET_ORBITALINCLINATION	[iPlanet]
					, 1.0 / PLANET_DISTANCE		[ssg::PLANET_COUNT - 1] * 2500
					);
			}
		}
		solarSystem.Entities.push_back({-1, 0, 0, 0, -1});
		for(uint32_t iPlanet = 0; iPlanet < ssg::PLANET_COUNT; ++iPlanet) {
			const uint32_t											iBodyOrbit					= iPlanet * 2;
			const uint32_t											iBodyPlanet					= iBodyOrbit + 1;
			int32_t													iEntityOrbit				= solarSystem.Entities.push_back({0, -1, -1, -1, (int32_t)iBodyOrbit});
			solarSystem.Entities[0].Children.push_back(iEntityOrbit);
			int32_t													iEntityPlanet				= solarSystem.Entities.push_back({iEntityOrbit, 0, (int32_t)iPlanet + 1, (int32_t)iPlanet + 1, (int32_t)iBodyPlanet});
			solarSystem.Entities[iEntityOrbit].Children.push_back(iEntityPlanet);
		}

		::gpk::SColorBGRA										colors []						=
			{ ::gpk::YELLOW
			, ::gpk::DARKRED
			, ::gpk::ORANGE
			, ::gpk::BLUE
			, ::gpk::LIGHTRED
			, ::gpk::LIGHTGRAY
			, ::gpk::LIGHTYELLOW
			, ::gpk::GREEN
			, ::gpk::DARKGRAY
			, ::gpk::YELLOW
			};

		solarSystem.Images.resize(ssg::PLANET_COUNT + 1);
		::gpk::SPNGData		pngCache;
		::gpk::pngFileLoad(pngCache, "../gpk_data/images/sun_color.png", solarSystem.Images[0]);
		for(uint32_t iPlanet = 0; iPlanet < ::ssg::PLANET_COUNT; ++iPlanet) {
			char														finalPath[256] = {};
			sprintf_s(finalPath, "../gpk_data/images/%s", PLANET_IMAGE[iPlanet]);
			::gpk::pngFileLoad(pngCache, finalPath, solarSystem.Images[iPlanet + 1]);
		}

		//for(uint32_t iImage = 0; iImage < solarSystem.Image.size(); ++iImage) {
		//	if(solarSystem.Image[iImage].Texels.size())
		//		continue;
		//	solarSystem.Image[iImage].resize({512, 512});
		//	for(uint32_t y = 0; y < solarSystem.Image[iImage].metrics().y; ++y) { // Generate noise color for planet texture
		//		const double															ecuatorialShade			= cos(y * (1.0 / solarSystem.Image[iImage].metrics().y * ::gpk::math_2pi)) + 1.5;
		//		uint32_t																rowOffset				= y * solarSystem.Image[iImage].metrics().x;
		//		for(uint32_t x = 0; x < solarSystem.Image[iImage].metrics().x; ++x) {
		//			solarSystem.Image[iImage].Texels[rowOffset + x]	= (colors[iImage % ::gpk::size(colors)] * ecuatorialShade * (1.0 - (rand() / 3.0 / (double)RAND_MAX))).Clamp();
		//		}
		//	}
		//}
		bodies.Integrate((365 * 4 + 1) * 10);	// Update physics
	}
	solarSystem.Scene.Camera.Target					= {};
	solarSystem.Scene.Camera.Position				= {-0.000001f, 500, -1000};
	solarSystem.Scene.Camera.Up						= {0, 1, 0};
	return 0;
}
