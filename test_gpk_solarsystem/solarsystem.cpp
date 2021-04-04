#include "solarsystem.h"
#include "gpk_png.h"
#include "gpk_image.h"
#include "gpk_ptr.h"
#include "gpk_stl.h"

#include <cstring>
#include <cstdint>
#include <algorithm>

#include <Windows.h>


::gpk::error_t							geometryBuildFromSTL			(const ::gpk::SSTLFile & stlFile, ::gpk::SGeometryTriangles & geometry) {
	geometry.Triangles		.resize(stlFile.Triangles.size());
	geometry.Normals		.resize(stlFile.Triangles.size());
	geometry.TextureCoords	.resize(stlFile.Triangles.size());
	for(uint32_t iTriangle = 0; iTriangle < stlFile.Triangles.size(); ++iTriangle) {
		geometry.Normals		[iTriangle] = {stlFile.Triangles[iTriangle].Normal, stlFile.Triangles[iTriangle].Normal, stlFile.Triangles[iTriangle].Normal};
		geometry.Triangles		[iTriangle] = stlFile.Triangles[iTriangle].Triangle;
		geometry.TextureCoords	[iTriangle] = {};
	}
	return 0;
}
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
		::gpk::SColorFloat										colorShot			= debris.Colors[iParticle % ::gpk::size(debris.Colors)];
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
		starFinalColor.g										= ::gpk::max(0.0f, starFinalColor.g - (1.0f - ::gpk::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 2))));
		starFinalColor.b										= ::gpk::max(0.0f, starFinalColor.b - (1.0f - ::gpk::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 1))));
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

int													setupGame						(SSolarSystem & solarSystem)	{
	solarSystem.Scene.Geometry.resize(1);
	::gpk::geometryBuildSphere(solarSystem.Scene.Geometry[0], 20U, 16U, 1, {});
//	::gpk::geometryBuildFromSTL();

	solarSystem.Scene.Pivot.resize(PLANET_COUNT + 1); //
	::gpk::SIntegrator3										& bodies						= solarSystem.Bodies;
	::SScene												& scene							= solarSystem.Scene;
	for(uint32_t iModel = 0; iModel < solarSystem.Scene.Pivot.size(); ++iModel) {
		int32_t													iPlanet							= iModel - 1;
		{ // Set up rigid body
			::SModelPivot											& model							= scene.Pivot[iModel];
			const float												scale							= (0 == iModel) ? 10.0f : float(1.0 / PLANET_SCALES[PLANET_EARTH] * PLANET_SCALES[iPlanet]) * 2;
			model.Scale											= {scale, scale, scale};
			model.Position										= {0, 0.5f};
		}
		if(iModel) { // Set up rigid body
			::gpk::createOrbiter(bodies
				, PLANET_MASSES				[iPlanet]
				, PLANET_DISTANCE			[iPlanet]
				, PLANET_AXIALTILT			[iPlanet]
				, PLANET_DAY				[iPlanet]
				, PLANET_DAY				[PLANET_EARTH]
				, PLANET_ORBITALPERIOD		[iPlanet]
				, PLANET_ORBITALINCLINATION	[iPlanet]
				, 1.0 / PLANET_DISTANCE		[PLANET_COUNT - 1] * 2500
				);
		}
	}
	solarSystem.Entities.push_back({-1, 0, 0, 0, -1});
	for(uint32_t iPlanet = 0; iPlanet < PLANET_COUNT; ++iPlanet) {
		const uint32_t											iBodyOrbit					= iPlanet * 2;
		const uint32_t											iBodyPlanet					= iBodyOrbit + 1;
		int32_t													iEntityOrbit				= solarSystem.Entities.push_back({0, -1, -1, -1, (int32_t)iBodyOrbit});
		solarSystem.Entities[0].IndexChild.push_back(iEntityOrbit);
		int32_t													iEntityPlanet				= solarSystem.Entities.push_back({iEntityOrbit, 0, (int32_t)iPlanet + 1, (int32_t)iPlanet + 1, (int32_t)iBodyPlanet});
		solarSystem.Entities[iEntityOrbit].IndexChild.push_back(iEntityPlanet);
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
		, ::gpk::YELLOW
		};

	solarSystem.Image.resize(PLANET_COUNT + 1);
	//::gpk::bmpFileLoad("../gpk_data/sun_color.bmp", solarSystem.Image[0], true);
	//for(uint32_t iPlanet = 0; iPlanet < PLANET_COUNT; ++iPlanet) {
	//	char														finalPath[256] = {};
	//	sprintf_s(finalPath, "../gpk_data/%s", PLANET_IMAGE[iPlanet]);
	//	::gpk::bmpFileLoad(finalPath, solarSystem.Image[iPlanet + 1], true);
	//}

	for(uint32_t iImage = 0; iImage < solarSystem.Image.size(); ++iImage) {
		if(solarSystem.Image[iImage].Texels.size())
			continue;
		solarSystem.Image[iImage].resize({512, 512});
		for(uint32_t y = 0; y < solarSystem.Image[iImage].metrics().y; ++y) { // Generate noise color for planet texture
			const double															ecuatorialShade			= cos(y * (1.0 / solarSystem.Image[iImage].metrics().y * ::gpk::math_2pi)) + 1.5;
			uint32_t																rowOffset				= y * solarSystem.Image[iImage].metrics().x;
			for(uint32_t x = 0; x < solarSystem.Image[iImage].metrics().x; ++x) {
				solarSystem.Image[iImage].Texels[rowOffset + x]	= colors[iImage % ::gpk::size(colors)] * ecuatorialShade * (1.0 - (rand() / 3.0 / (double)RAND_MAX));
			}
		}
	}
	bodies.Integrate((365 * 4 + 1) * 10);	// Update physics

	solarSystem.Scene.Camera.Target					= {};
	solarSystem.Scene.Camera.Position				= {-0.000001f, 500, -1000};
	solarSystem.Scene.Camera.Up						= {0, 1, 0};
	return 0;
}

int													updateEntityTransforms		(uint32_t iEntity, ::gpk::array_obj<::SEntity> & entities, SScene & scene, ::gpk::SIntegrator3 & bodies)	{
	const SEntity											& entity					= entities[iEntity];
	::gpk::SModelMatrices									matrices					= {};
	if(-1 == entity.IndexBody)
		scene.Transform[iEntity]							= (-1 == entity.IndexParent) ? bodies.MatrixIdentity4 : scene.Transform[entity.IndexParent];
	else {
		if(-1 == entity.IndexParent)
			bodies.GetTransform(entity.IndexBody, scene.Transform[iEntity]);
		else {
			::gpk::SMatrix4<float>									matrixBody					= {};
			bodies.GetTransform(entity.IndexBody, matrixBody);
			scene.Transform[iEntity]							= matrixBody * scene.Transform[entity.IndexParent];
		}
	}
	for(uint32_t iChild = 0; iChild < entity.IndexChild.size(); ++iChild) {
		const uint32_t											iChildEntity				= entity.IndexChild[iChild];
		::updateEntityTransforms(iChildEntity, entities, scene, bodies);
	}
	return 0;
}

int													updateGame						(SSolarSystem & solarSystem, double secondsLastFrame, ::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>> & target)	{
	// ------------------------------------------- Handle input
	::gpk::SIntegrator3										& bodies						= solarSystem.Bodies;
	::SScene												& scene							= solarSystem.Scene;

	::gpk::SMatrix4<float>									matrixBody						= {};
	scene.Transform.resize(solarSystem.Entities.size());
	for(uint32_t iEntity = 0; iEntity < solarSystem.Entities.size(); ++iEntity) {
		const ::SEntity											& entity					= solarSystem.Entities[iEntity];
		if(-1 != entity.IndexParent)	// process root entities
			::updateEntityTransforms(iEntity, solarSystem.Entities, scene, bodies);
		else
			solarSystem.Scene.Transform[iEntity].Identity();
	}


	// ------------------------------------------- Handle input
	::gpk::SCamera											& camera					= solarSystem.Scene.Camera;
	if(GetAsyncKeyState('Q')) camera.Position.z				-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 100 : 10);
	if(GetAsyncKeyState('E')) camera.Position.z				+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 100 : 10);
	if(GetAsyncKeyState('S')) camera.Position				+= camera.Position / camera.Position.Length() * (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame;
	if(GetAsyncKeyState('W')) camera.Position				-= camera.Position / camera.Position.Length() * (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame;
	if(GetAsyncKeyState('A')) camera.Position.RotateY( (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame);
	if(GetAsyncKeyState('D')) camera.Position.RotateY(-(GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame);
	if(GetAsyncKeyState('0')) { ; camera.Target = scene.Transform[0 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[0 * 2].IndexModel].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('1')) { ; camera.Target = scene.Transform[1 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[1 * 2].IndexModel].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('2')) { ; camera.Target = scene.Transform[2 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[2 * 2].IndexModel].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('3')) { ; camera.Target = scene.Transform[3 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[3 * 2].IndexModel].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('4')) { ; camera.Target = scene.Transform[4 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[4 * 2].IndexModel].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('5')) { ; camera.Target = scene.Transform[5 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[5 * 2].IndexModel].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('6')) { ; camera.Target = scene.Transform[6 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[6 * 2].IndexModel].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('7')) { ; camera.Target = scene.Transform[7 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[7 * 2].IndexModel].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('8')) { ; camera.Target = scene.Transform[8 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[8 * 2].IndexModel].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('9')) { ; camera.Target = scene.Transform[9 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::SCoord3<float>{scene.Pivot[solarSystem.Entities[9 * 2].IndexModel].Scale.x * 10, 0, 0}; }

	solarSystem.SunFire.SpawnSpherical(100, {}, 5, 1, 10);

	// Update physics
	solarSystem.SunFire.Update(secondsLastFrame * 2);
	bodies.Integrate(secondsLastFrame);

	//------------------------------------------- Transform and Draw
	::gpk::view_grid<::gpk::SColorBGRA>						targetPixels				= target->Color.View;
	memset(targetPixels.begin(), 0, sizeof(::gpk::SColorBGRA) * targetPixels.size());
		::gpk::SColorBGRA										colorBackground		= {0x20, 0x8, 0x4};
	//colorBackground									+= (colorBackground * (0.5 + (0.5 / 65535 * rand())) * ((rand() % 2) ? -1 : 1)) ;
	for(uint32_t y = 0; y < targetPixels.metrics().y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < targetPixels.metrics().x; ++x)
		targetPixels.begin()[y * targetPixels.metrics().x + x]	= colorBackground;

	::gpk::SCoord3<float>									lightVector					= camera.Position;
	lightVector.Normalize();

	::gpk::SMatrix4<float>									matrixView					= {};
	::gpk::SMatrix4<float>									matrixProjection			= {};
	::gpk::SMatrix4<float>									matrixViewport				= {};
	matrixView.LookAt(camera.Position, camera.Target, camera.Up);
	matrixProjection.FieldOfView(::gpk::math_pi * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.1, 10000);
	matrixViewport.ViewportLH(targetPixels.metrics());
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	::gpk::array_pod<::gpk::SCoord2<int16_t>>				pixelCoords					= {};
	::gpk::array_pod<::gpk::STriangleWeights<float>>		pixelVertexWeights			= {};
	::gpk::SModelMatrices									matrices					= {};
	::gpk::view_grid<uint32_t>								depthBuffer					= target->DepthStencil.View;
	memset(depthBuffer.begin(), -1, sizeof(uint32_t) * depthBuffer.size());
	::gpk::array_pod<::gpk::SLight3>						lightPoints;
	::gpk::array_pod<::gpk::SColorBGRA>						lightColors;
	lightPoints.push_back({{0,0,0}, 10000});
	lightColors.push_back(::gpk::WHITE);

	for(uint32_t iEntity = 0; iEntity < solarSystem.Entities.size(); ++iEntity) {
		::gpk::SMatrix4<float>									matrixTransform				= scene.Transform[iEntity];
		const ::SEntity											& entity					= solarSystem.Entities[iEntity];
		if(-1 == entity.IndexModel)
			continue;
		if(-1 == entity.IndexImage)
			continue;

		matrices.Scale		.Scale			(scene.Pivot[entity.IndexModel].Scale		, true);
		matrices.Position	.SetTranslation	(scene.Pivot[entity.IndexModel].Position	, true);
		::gpk::view_grid<::gpk::SColorBGRA>						entityImage					= solarSystem.Image[entity.IndexImage];
		::gpk::SGeometryTriangles								& entityGeometry			= scene.Geometry[entity.IndexGeometry];
		matrixTransform										= matrices.Scale * matrices.Position * matrixTransform;
		::gpk::SMatrix4<float>									matrixTransformView			= matrixTransform * matrixView;
		for(uint32_t iTriangle = 0; iTriangle < entityGeometry.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::gpk::drawTriangle(targetPixels, entityGeometry, iTriangle, matrixTransform, matrixTransformView, lightVector, iEntity ? ::gpk::BLACK : ::gpk::WHITE, pixelCoords, pixelVertexWeights, entityImage, lightPoints, lightColors, depthBuffer);
		}
	}
	::drawDebris(targetPixels, solarSystem.SunFire, matrixView, depthBuffer);

	return ::drawDebris(targetPixels, solarSystem.SunFire, matrixView, depthBuffer);
}

