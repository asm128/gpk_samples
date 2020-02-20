#include "solarsystem.h"
#include "gpk_png.h"
#include "gpk_image.h"
#include "gpk_ptr.h"

#include <cstring>
#include <cstdint>
#include <algorithm>

#include <Windows.h>

																				//	- Mercury	- Venus		- Earth		- Mars		- Jupiter	- Saturn	- Uranus	- Neptune	- Pluto
static constexpr const double	PLANET_MASSES				[PLANET_COUNT]	=	{	0.330f		, 4.87f		, 5.97f		, 0.642f	, 1899		, 568		, 86.8f		, 102		, 0.0125f	};
static constexpr const double	PLANET_SCALES				[PLANET_COUNT]	=	{	4879		, 12104		, 12756		, 6792		, 142984	, 120536	, 51118		, 49528		, 2390		};
static constexpr const double	PLANET_DAY					[PLANET_COUNT]	=	{	4222.6f		, 2802.0f	, 23.9f		, 24.7f		, 9.9f		, 10.7f		, 17.2f		, 16.1f		, 153.3f	};
static constexpr const double	PLANET_AXIALTILT			[PLANET_COUNT]	=	{	0.01f		, 177.4f	, 23.4f		, 25.2f		, 3.1f		, 26.7f		, 97.8f		, 28.3f		, 122.5f	};
static constexpr const double	PLANET_DISTANCE				[PLANET_COUNT]	=	{	57.9f		, 108.2f	, 149.6f	, 227.9f	, 778.6f	, 1433.5f	, 2872.5f	, 4495.1f	, 5870.0f	};

static constexpr const double	PLANET_ORBITALPERIOD		[PLANET_COUNT]	=	{	88.0f	, 224.7f	, 365.2f	, 687.0f	, 4331		, 10747		, 30589		, 59800		, 90588		};
static constexpr const double	PLANET_ORBITALVELOCITY		[PLANET_COUNT]	=	{	47.9f	, 35.0f		, 29.8f		, 24.1f		, 13.1f		, 9.7f		, 6.8f		, 5.4f		, 4.7f		};
static constexpr const double	PLANET_ORBITALINCLINATION	[PLANET_COUNT]	=	{	7.0		, 3.4f		, 0.0f		, 1.9f		, 1.3f		, 2.5f		, 0.8f		, 1.8f		, 17.2f		};
static constexpr const double	PLANET_ORBITALECCENTRICITY	[PLANET_COUNT]	=	{	0.205f	, 0.007f	, 0.017f	, 0.094f	, 0.049f	, 0.057f	, 0.046f	, 0.011f	, 0.244f	};
static constexpr const char*	PLANET_IMAGE				[PLANET_COUNT]	=	{	"mercury_color.bmp"	, "venus_color.bmp"	, "earth_color.bmp"	, "mars_color.bmp"	, "jupiter_color.bmp"	, "saturn_color.bmp"	, "uranus_color.bmp"	, "neptune_color.bmp"	, "pluto_color.bmp"	};

static	int											drawDebris			(::gpk::view_grid<::gpk::SColorBGRA> targetPixels, SDebris & debris, const ::gpk::SMatrix4<float> & matrixVPV, ::gpk::view_grid<uint32_t> depthBuffer)	{
	::gpk::array_pod<::gpk::SCoord2<int32_t>>				pixelCoords;
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		::gpk::SColorFloat										colorShot			= debris.Colors[iParticle % ::std::size(debris.Colors)];
		::gpk::SCoord3<float>									starPos				= debris.Particles.Position[iParticle];
		starPos												= matrixVPV.Transform(starPos);
		const ::gpk::SCoord2<int32_t>							pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		uint32_t												depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		::gpk::SColorFloat											starFinalColor	= colorShot * debris.Brightness[iParticle];
		starFinalColor.g										= ::std::max(0.0f, starFinalColor.g - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 2))));
		starFinalColor.b										= ::std::max(0.0f, starFinalColor.b - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 1))));
		//::gpk::setPixel(targetPixels, pixelCoord, starFinalColor);
		const	double											brightRadius		= 2.0;
		const	double											brightRadiusSquared	= brightRadius * brightRadius;
		double													brightUnit			= 1.0 / brightRadiusSquared;
		for(int32_t y = (int32_t)-brightRadius - 1; y < (int32_t)brightRadius + 1; ++y)
		for(int32_t x = (int32_t)-brightRadius - 1; x < (int32_t)brightRadius + 1; ++x) {
			::gpk::SCoord2<float>									brightPos			= {(float)x, (float)y};
			const double											brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::gpk::SCoord2<int32_t>									blendPos			= pixelCoord + (brightPos).Cast<int32_t>();
				if( blendPos.y < 0 || blendPos.y >= (int32_t)targetPixels.metrics().y
				 || blendPos.x < 0 || blendPos.x >= (int32_t)targetPixels.metrics().x
				)
					continue;
				uint32_t												& blendVal			= depthBuffer[blendPos.y][blendPos.x];
				//if(depth > blendVal)
				//	continue;
				blendVal											= depth;
				double													finalBrightness					= 1.0-(brightDistance * brightUnit);
				::gpk::SColorBGRA										& pixelVal						= targetPixels[blendPos.y][blendPos.x];
				::gpk::SColorFloat										pixelColor						= starFinalColor * finalBrightness + pixelVal;
				pixelVal											= pixelColor;
			}
		}
	}
	return 0;
}

int													setupGame						(SSolarSystem & sunsystem)	{
	sunsystem.Scene.Geometry.resize(1);
	::ced::geometryBuildSphere(sunsystem.Scene.Geometry[0], 20U, 16U, 1, {});

	sunsystem.Scene.Pivot.resize	(PLANET_COUNT + 1);
	sunsystem.World.Spawn			(PLANET_COUNT * 2);
	::ced::SIntegrator3										& bodies						= sunsystem.World;
	::SScene												& scene							= sunsystem.Scene;
	::gpk::SQuaternion<float>								axialTilt, orbitalInclination;

	for(uint32_t iModel = 0; iModel < sunsystem.Scene.Pivot.size(); ++iModel) {
		int32_t													iPlanet							= iModel - 1;
		const float												scale							= float(1.0 / PLANET_SCALES[PLANET_EARTH] * PLANET_SCALES[iPlanet]) * 2;
		{ // Set up rigid body
			::SModelPivot										& model							= scene.Pivot[iModel];
			if(0 == iModel)
				model.Scale											= {10.f, 10.f, 10.f};
			else {
				model.Scale											= {scale, scale, scale};
			}
			model.Position										= {0, 0.5f};
		}
		if(iModel) { // Set up rigid body
			axialTilt.Identity();
			orbitalInclination.Identity();

			//::gpk::SMass3											& orbitMass						= bodies.Masses		[iPlanet * 2]		= {};
			::ced::STransform3										& orbitTransform				= bodies.Transforms	[iPlanet * 2]		= {};
			::ced::SForce3											& orbitForces					= bodies.Forces		[iPlanet * 2]		= {};
			::ced::SMass3											& planetMass					= bodies.Masses		[iPlanet * 2 + 1]	= {};
			::ced::STransform3										& planetTransform				= bodies.Transforms	[iPlanet * 2 + 1]	= {};
			::ced::SForce3											& planetForces					= bodies.Forces		[iPlanet * 2 + 1]	= {};
			planetMass.InverseMass								= float(1.0 / PLANET_MASSES[iPlanet]);
			planetTransform.Position.x							= float(1.0 / PLANET_DISTANCE[PLANET_COUNT - 1] * PLANET_DISTANCE[iPlanet] * 2500);
			planetForces										= {};

			axialTilt.MakeFromEulerTaitBryan((float)(::gpk::math_2pi / 360.0 * PLANET_AXIALTILT[iPlanet]), 0, 0);					// Calculate the axial inclination of the planet IN RADIANS
			planetTransform.Orientation							= axialTilt;										// Set the calculated axial tilt to the planet

			::gpk::SCoord3<float>									rotatedRotation					= { 0.0f, -(float)(::gpk::math_2pi / PLANET_DAY[iPlanet] * PLANET_DAY[PLANET_EARTH]), 0.0f };	// Calculate the rotation velocity of the planet IN EARTH DAYS
			rotatedRotation										= axialTilt.RotateVector( rotatedRotation );		// Rotate our calculated torque in relation to the planetary axis
			planetForces.Rotation								= rotatedRotation;		// Set the rotation velocity of the planet IN EARTH DAYS

			orbitForces.Rotation								= {0.0f, (float)(::gpk::math_2pi / PLANET_ORBITALPERIOD[iPlanet]), 0.0f};			// Set the orbital rotation velocity IN EARTH DAYS
			orbitalInclination.MakeFromEulerTaitBryan( (float)(::gpk::math_2pi / 360.0 * PLANET_ORBITALINCLINATION[iPlanet]), 0.0f, 0.0f );	// Calculate the orbital tilt IN RADIANS
			orbitTransform.Orientation							= orbitalInclination;								// Set the calculated inclination to the orbit
		}
	}

	::gpk::SColorFloat										colors []						=
		{ ::gpk::YELLOW
		, ::gpk::DARKRED
		, ::gpk::ORANGE
		, ::gpk::BLUE
		, ::gpk::LIGHTRED
		, ::gpk::LIGHTGRAY
		, ::gpk::LIGHTYELLOW
		, ::gpk::GREEN
		, ::gpk::DARKGRAY
		};

	sunsystem.Image.resize(PLANET_COUNT + 1);
	::gpk::pngFileLoad(::gpk::view_const_string{"../gpk_data/sun_color.png"}, sunsystem.Image[0]);
	for(uint32_t iPlanet = 0; iPlanet < PLANET_COUNT; ++iPlanet) {
		char														finalPath[256] = {};
		sprintf_s(finalPath, "../gpk_data/%s", PLANET_IMAGE[iPlanet]);
		::gpk::pngFileLoad(::gpk::view_const_string{"../gpk_data/sun_color.png"}, sunsystem.Image[iPlanet + 1]);
	}

	for(uint32_t iImage = 0; iImage < sunsystem.Image.size(); ++iImage) {
		if(sunsystem.Image[iImage].Texels.size())
			continue;
		sunsystem.Image[iImage].resize(24, 12);
		for(uint32_t y = 0; y < sunsystem.Image[iImage].metrics().y; ++y) // Generate noise color for planet texture
		for(uint32_t x = 0; x < sunsystem.Image[iImage].metrics().x; ++x) {
			sunsystem.Image[iImage][y][x]		= colors[iImage % ::std::size(colors)];
		}
	}
	sunsystem.Entities.push_back({-1, -1, 0, 0, -1});
	for(uint32_t iPlanet = 0; iPlanet < PLANET_COUNT; ++iPlanet) {
		const uint32_t											iBodyOrbit					= iPlanet * 2;
		const uint32_t											iBodyPlanet					= iBodyOrbit + 1;
		int32_t													iEntityOrbit				= sunsystem.Entities.push_back({0, -1, -1, -1, (int32_t)iBodyOrbit});
		sunsystem.Entities[0].IndexChild.push_back(iEntityOrbit);
		int32_t													iEntityPlanet				= sunsystem.Entities.push_back({iEntityOrbit, 0, (int32_t)iPlanet + 1, (int32_t)iPlanet + 1, (int32_t)iBodyPlanet});
		sunsystem.Entities[iEntityOrbit].IndexChild.push_back(iEntityPlanet);
	}

	// Update physics
	bodies.Integrate((365 * 4 + 1) * 10);

	sunsystem.Scene.Camera.Target						= {};
	sunsystem.Scene.Camera.Position						= {-0.000001f, 500, -1000};
	sunsystem.Scene.Camera.Up							= {0, 1, 0};
	return 0;
}

int													updateEntityTransforms		(uint32_t iEntity, const ::gpk::view_array<const ::SEntity> & entities, SScene & scene, ::ced::SIntegrator3 & bodies)	{
	const SEntity											& entity					= entities[iEntity];
	::ced::SModelMatrices									matrices					= {};
	::gpk::SMatrix4<float>									matrixBody					= {};
	bodies.GetTransform(entity.IndexBody, matrixBody);
	::gpk::SMatrix4<float>									matrixTransform				= matrixBody;
	scene.Transform[iEntity]							= (-1 == entity.IndexParent) ? matrixTransform : matrixTransform * scene.Transform[entity.IndexParent];
	for(uint32_t iChild = 0; iChild < entity.IndexChild.size(); ++iChild) {
		const uint32_t											iChildEntity				= entity.IndexChild[iChild];
		updateEntityTransforms(iChildEntity, entities, scene, bodies);
	}
	return 0;
}
int													updateGame						(SSolarSystem & sunsystem, double secondsLastFrame, ::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>> & target)	{
	// ------------------------------------------- Handle input
	if(GetAsyncKeyState('Q')) sunsystem.Scene.Camera.Position.z				-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 100 : 10);
	if(GetAsyncKeyState('E')) sunsystem.Scene.Camera.Position.z				+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 100 : 10);
	if(GetAsyncKeyState('S')) sunsystem.Scene.Camera.Position					+= sunsystem.Scene.Camera.Position / sunsystem.Scene.Camera.Position.Length() * (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame;
	if(GetAsyncKeyState('W')) sunsystem.Scene.Camera.Position					-= sunsystem.Scene.Camera.Position / sunsystem.Scene.Camera.Position.Length() * (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame;
	if(GetAsyncKeyState('A')) sunsystem.Scene.Camera.Position.RotateY( (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame);
	if(GetAsyncKeyState('D')) sunsystem.Scene.Camera.Position.RotateY(-(GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame);

	sunsystem.SunFire.SpawnSpherical(2000, {}, 15, 1, 5);

	// Update physics
	sunsystem.SunFire.Update(secondsLastFrame * 2);
	::ced::SIntegrator3										& bodies						= sunsystem.World;
	bodies.Integrate(secondsLastFrame);

	::gpk::view_grid<::gpk::SColorBGRA>						targetPixels					= target->Color.View;
	::gpk::view_grid<uint32_t>								depthBuffer						= target->DepthStencil.View;
	//------------------------------------------- Transform and Draw
	const ::ced::SCamera									& camera						= sunsystem.Scene.Camera;
	memset(targetPixels.begin(), 0, sizeof(::gpk::SColorBGRA) * targetPixels.size());
	::gpk::SCoord3<float>									lightVector						= camera.Position;
	lightVector.Normalize();

	::gpk::SMatrix4<float>									matrixView						= {};
	::gpk::SMatrix4<float>									matrixProjection				= {};
	::gpk::SMatrix4<float>									matrixViewport					= {};
	matrixView.LookAt(camera.Position, camera.Target, camera.Up);
	matrixProjection.FieldOfView(::gpk::math_pi * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.1, 10000);
	matrixViewport.ViewportLH(targetPixels.metrics());
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	::gpk::array_pod<::gpk::SCoord2<int32_t>>				pixelCoords						= {};
	::gpk::array_pod<::gpk::STriangleWeights<double>>		pixelVertexWeights				= {};
	::ced::SModelMatrices									matrices						= {};
	::SScene												& scene							= sunsystem.Scene;

	::gpk::SMatrix4<float>									matrixBody						= {};
	scene.Transform.resize(sunsystem.Entities.size());
	for(uint32_t iEntity = 0; iEntity < sunsystem.Entities.size(); ++iEntity) {
		const ::SEntity											& entity						= sunsystem.Entities[iEntity];
		if(-1 != entity.IndexParent)	// process root entities
			updateEntityTransforms(iEntity, sunsystem.Entities, scene, bodies);
		else
			sunsystem.Scene.Transform[iEntity].Identity();
	}

	::gpk::array_pod<::ced::SLight3>						lightPoints;
	::gpk::array_pod<::gpk::SColorBGRA>						lightColors;
	lightPoints.push_back({{0,0,0}, 10000});
	lightColors.push_back(::gpk::WHITE);

	for(uint32_t iEntity = 0; iEntity < sunsystem.Entities.size(); ++iEntity) {
		::gpk::SMatrix4<float>									matrixTransform					= scene.Transform[iEntity];
		const ::SEntity											& entity						= sunsystem.Entities[iEntity];
		if(-1 == entity.IndexModel)
			continue;
		if(-1 == entity.IndexImage)
			continue;
		if(-1 == entity.IndexGeometry)
			continue;

		matrices.Scale		.Scale			(scene.Pivot[entity.IndexModel].Scale		, true);
		matrices.Position	.SetTranslation	(scene.Pivot[entity.IndexModel].Position	, true);
		const ::gpk::view_grid<::gpk::SColorBGRA>				entityImage						= sunsystem.Image[entity.IndexImage].View;
		::ced::SGeometryTriangles								& entityGeometry				= scene.Geometry[entity.IndexGeometry];
		matrixTransform										= matrices.Scale * matrices.Position * matrixTransform;
		::gpk::SMatrix4<float>									matrixTransformView				= matrixTransform * matrixView;
		for(uint32_t iTriangle = 0; iTriangle < entityGeometry.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawTriangle(targetPixels, entityGeometry, iTriangle, matrixTransform, matrixTransformView, lightVector, iEntity ? ::gpk::BLACK : ::gpk::WHITE, pixelCoords, pixelVertexWeights, entityImage, lightPoints, lightColors, depthBuffer);
		}
	}
	return ::drawDebris(targetPixels, sunsystem.SunFire, matrixView, depthBuffer);
}

