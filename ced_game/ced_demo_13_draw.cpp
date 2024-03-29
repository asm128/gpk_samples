#include "ced_demo_13_game.h"

#include "gpk_raster_lh.h"

#include <algorithm>

static constexpr	const uint32_t					MAX_LIGHT_RANGE		= 10;

static	int											drawStars			(const ::ssg::SStars & stars, ::gpk::view_grid<::gpk::SColorBGRA> targetPixels)	{
	::gpk::SColorBGRA										colors[]			=
		{ {0xfF, 0xfF, 0xfF, }
		, {0xC0, 0xC0, 0xfF, }
		, {0xfF, 0xC0, 0xC0, }
		, {0xC0, 0xD0, 0xC0, }
		};
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::gpk::SCoord2<float>									starPos				= stars.Position[iStar];
		::gpk::SColorBGRA										starFinalColor		= colors[iStar % ::gpk::size(colors)] * stars.Brightness[iStar];
		::gpk::setPixel(targetPixels, starPos.Cast<int16_t>(), starFinalColor);
		const	int32_t											brightRadius		= 1 + (iStar % 3) + (rand() % 2);
		const	double											brightRadiusSquared	= brightRadius * (double)brightRadius;
		double													brightUnit			= 1.0 / brightRadiusSquared;
		for(int32_t y = -brightRadius; y < brightRadius; ++y)
		for(int32_t x = -brightRadius; x < brightRadius; ++x) {
			::gpk::SCoord2<float>									brightPos			= {(float)x, (float)y};
			const double											brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::gpk::SCoord2<int16_t>									pixelPos			= (starPos + brightPos).Cast<int16_t>();
				if( pixelPos.y >= 0 && pixelPos.y < (int32_t)targetPixels.metrics().y
				 && pixelPos.x >= 0 && pixelPos.x < (int32_t)targetPixels.metrics().x
 				)
					::gpk::setPixel(targetPixels, pixelPos, targetPixels[pixelPos.y][pixelPos.x] + starFinalColor * (1.0-(brightDistance * brightUnit * (1 + (rand() % 3)))));
			}
		}
	}
	return 0;
}

static	int											drawDebris
	( ::gpk::view_grid<::gpk::SColorBGRA>		targetPixels
	, const ::ssg::SDebris						& debris
	, const ::gpk::SMatrix4<float>				& matrixVPV
	, ::gpk::view_grid<uint32_t>				depthBuffer
	)	{
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		const ::gpk::SColorFloat								& colorShot			= debris.Colors[iParticle % ::gpk::size(debris.Colors)];
		::gpk::SCoord3<float>									starPos				= debris.Particles.Position[iParticle];
		starPos												= matrixVPV.Transform(starPos);
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		const ::gpk::SCoord2<int32_t>							pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		uint32_t												depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		::gpk::SColorFloat										starFinalColor	= colorShot * debris.Brightness[iParticle];
		starFinalColor.g									= ::gpk::max(0.0f, starFinalColor.g - (1.0f - ::gpk::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 2))));
		starFinalColor.b									= ::gpk::max(0.0f, starFinalColor.b - (1.0f - ::gpk::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 1))));
		//::gpk::setPixel(targetPixels, pixelCoord, starFinalColor);
		static constexpr	const double						brightRadius		= 1.5;
		static constexpr	const double						brightRadiusSquared	= brightRadius * brightRadius;
		static constexpr	const double						brightUnit			= 1.0 / brightRadiusSquared;

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
				if(depth > blendVal)
					continue;
				blendVal											= depth;
				double													finalBrightness					= 1.0-(brightDistance * brightUnit);
				::gpk::SColorBGRA										& pixelVal						= targetPixels[blendPos.y][blendPos.x];
				pixelVal											= (starFinalColor * finalBrightness + pixelVal).Clamp();
			}
		}
	}
	return 0;
}

static	int											drawShots			(::gpk::view_grid<::gpk::SColorBGRA> targetPixels, const ::ssg::SShots & shots
	, const ::gpk::SMatrix4<float>				& matrixVPV
	, ::gpk::SColorFloat						colorShot
	, const	double								brightRadius
	, const	double								intensity
	, const	bool								line
	, ::gpk::view_grid<uint32_t>				depthBuffer
	, ::gpk::array_pod<::gpk::SCoord3<float>>	pixelCoordsCache
	) {
	const ::gpk::SCoord2<int32_t>						targetMetrics			= targetPixels.metrics().Cast<int32_t>();
	for(uint32_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		float													brightness			= shots.Brightness[iShot];
		pixelCoordsCache.clear();
		const ::gpk::SCoord3<float>								& starPosPrev		= shots.PositionDraw[iShot];
		const ::gpk::SCoord3<float>								& starPos			= shots.Particles.Position[iShot];
		//::gpk::SLine3<float>									raySegment			= {starPosPrev, starPos};

		::gpk::SLine3<float>									raySegment			= {starPos, starPosPrev}; //raySegmentWorld;
		raySegment.A										= matrixVPV.Transform(raySegment.A);
		raySegment.B										= matrixVPV.Transform(raySegment.B);
		if(raySegment.A.z < 0 || raySegment.A.z > 1) continue;
		if(raySegment.B.z < 0 || raySegment.B.z > 1) continue;
		if(line)
			::gpk::drawLine(targetPixels, raySegment, pixelCoordsCache, depthBuffer);
		else
			pixelCoordsCache.push_back(raySegment.B);
		const double											pixelCoordUnit		= 1.0 / pixelCoordsCache.size();
		for(uint32_t iPixelCoord = 0, countPixelCoords = pixelCoordsCache.size(); iPixelCoord < countPixelCoords; ++iPixelCoord) {
			const ::gpk::SCoord3<float>							& pixelCoord		= pixelCoordsCache[iPixelCoord];
			if(pixelCoord.z < 0 || pixelCoord.z > 1)
				continue;
			if( pixelCoord.y < 0 || pixelCoord.y >= targetMetrics.y
			 || pixelCoord.x < 0 || pixelCoord.x >= targetMetrics.x
			)
				continue;
			targetPixels[(uint32_t)pixelCoord.y][(uint32_t)pixelCoord.x]	= colorShot;
			const uint32_t											depth				= uint32_t(pixelCoord.z * 0xFFFFFFFFU);
			const	double											brightRadiusSquared	= brightRadius * brightRadius;
			double													brightUnit			= 1.0 / brightRadiusSquared * brightness;
			for(int32_t y = (int32_t)-brightRadius, brightCount = (int32_t)brightRadius; y < brightCount; ++y)
			for(int32_t x = (int32_t)-brightRadius; x < brightCount; ++x) {
				::gpk::SCoord2<float>									brightPos			= {(float)x, (float)y};
				const double											brightDistance		= brightPos.LengthSquared();
				if(brightDistance <= brightRadiusSquared) {
					::gpk::SCoord2<int32_t>									blendPos			= ::gpk::SCoord2<int32_t>{(int32_t)pixelCoord.x, (int32_t)pixelCoord.y} + (brightPos).Cast<int32_t>();
					if( blendPos.y < 0 || blendPos.y >= targetMetrics.y
					 || blendPos.x < 0 || blendPos.x >= targetMetrics.x
					)
						continue;
					uint32_t												& blendVal			= depthBuffer[blendPos.y][blendPos.x];
					if(depth > blendVal)
						continue;
					blendVal											= depth;
					::gpk::SColorBGRA										& pixelVal						= targetPixels[blendPos.y][blendPos.x];
					double													finalBrightness
						= line
						? (		 (brightDistance * brightUnit)) * (pixelCoordUnit * (countPixelCoords - 1 - iPixelCoord))
						: (1.0 - (brightDistance * brightUnit));
					::gpk::SColorFloat										pixelColor						= ::gpk::interpolate_linear(::gpk::SColorFloat{pixelVal}, colorShot, finalBrightness * intensity);
					pixelVal											= pixelColor.Clamp();
				}
			}
		}

	}
	return 0;
}

static	int											getLightArrays
	( const ::ssg::SSolarSystem					& solarSystem
	, ::gpk::array_pod<::gpk::SCoord3<float>>	& lightPoints
	, ::gpk::array_pod<::gpk::SColorBGRA>		& lightColors
	)						{
	::gpk::SColorBGRA										colorLightPlayer		= ::gpk::SColorBGRA{0xFF, 0xFF, 0xFF};
	::gpk::SColorBGRA										colorLightEnemy			= ::gpk::SColorBGRA{0xFF, 0xFF, 0xFF};
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		const ::ssg::SShip										& ship					= solarSystem.Ships[iShip];
		lightPoints.push_back(solarSystem.ShipPhysics.Transforms[solarSystem.Entities[ship.Entity].Body].Position);
		lightColors.push_back((0 == solarSystem.Ships[iShip].Team) ? colorLightPlayer : colorLightEnemy);
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
			const ::ssg::SShipPart									& shipPart				= ship.Parts[iPart];
			const ::gpk::SColorFloat								colorShot
				= (::ssg::MUNITION_TYPE_RAY		== ship.Parts[iPart].Type) ? ::gpk::SColorFloat{1.0f, 0.1f, 0.0f}
				: (::ssg::MUNITION_TYPE_SHELL	== ship.Parts[iPart].Type) ? ship.Team ? ::gpk::SColorFloat{1.0f, 0.125f, 0.25f} : ::gpk::TURQUOISE
				: (::ssg::MUNITION_TYPE_BULLET	== ship.Parts[iPart].Type) ? ::gpk::GRAY
				: ::gpk::SColorFloat{::gpk::SColorBGRA{0xFF, 0xFF, 0xFF}}
				;
			for(uint32_t iShot = 0; iShot < shipPart.Shots.Particles.Position.size(); ++iShot) {
				lightPoints.push_back(shipPart.Shots.Particles.Position[iShot]);
				lightColors.push_back(colorShot);
			}
		}
	}
	for(uint32_t iParticle = 0; iParticle < solarSystem.Debris.Particles.Position.size(); ++iParticle) {
		lightPoints.push_back(solarSystem.Debris.Particles.Position[iParticle]);
		::gpk::SColorFloat										colorShot			= solarSystem.Debris.Colors[iParticle % ::gpk::size(solarSystem.Debris.Colors)];
		lightColors.push_back(colorShot * solarSystem.Debris.Brightness[iParticle]);
	}
	return 0;
}

static	int											getLightArrays
	( const ::gpk::SCoord3<float>							& modelPosition
	, const ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPointsWorld
	, const ::gpk::array_pod<::gpk::SColorBGRA>				& lightColorsWorld
	, ::gpk::array_pod<::gpk::SCoord3<float>>				& lightPointsModel
	, ::gpk::array_pod<::gpk::SColorBGRA>					& lightColorsModel
	) {
	::gpk::clear(lightPointsModel, lightColorsModel);
	for(uint32_t iLightPoint = 0; iLightPoint < lightPointsWorld.size(); ++iLightPoint) {
		const ::gpk::SCoord3<float>								& lightPoint		=	lightPointsWorld[iLightPoint];
		if((lightPoint - modelPosition).LengthSquared() < (MAX_LIGHT_RANGE * MAX_LIGHT_RANGE)) {
			lightPointsModel.push_back(lightPoint);
			lightColorsModel.push_back(lightColorsWorld[iLightPoint]);
		}
	}
	return 0;
}
//
//static	int											getLightArrays
//	( const ::gpk::SCoord3<float>							& modelPosition
//	, const ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPointsWorld
//	, ::gpk::array_pod<uint16_t>							& indicesPointLights
//	) {
//	indicesPointLights.clear();
//	for(uint32_t iLightPoint = 0; iLightPoint < lightPointsWorld.size(); ++iLightPoint) {
//		const ::gpk::SCoord3<float>								& lightPoint		=	lightPointsWorld[iLightPoint];
//		if((lightPoint - modelPosition).LengthSquared() < (MAX_LIGHT_RANGE * MAX_LIGHT_RANGE))
//			indicesPointLights.push_back((uint16_t)iLightPoint);
//	}
//	return 0;
//}

static	int											drawShip
	( const ::ssg::SSolarSystem							& solarSystem
	, const ::ssg::SShip								& ship
	, const ::gpk::SMatrix4<float>						& matrixVP
	, ::gpk::view_grid<::gpk::SColorBGRA>				& targetPixels
	, ::gpk::view_grid<uint32_t>						depthBuffer
	, ::ssg::SSolarSystemDrawCache						& drawCache
	) {
	for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
		const ::ssg::SShipPart									& shipPart				= ship.Parts[iPart];
		if(shipPart.Health <= 0)
			continue;
		const ::ssg::SEntity									& entity					= solarSystem.Entities[shipPart.Entity];
		for(uint32_t iEntity = 0; iEntity < entity.Children.size(); ++iEntity) {
			const ::ssg::SEntity									& entityChild				= solarSystem.Entities[entity.Children[iEntity]];
			if(-1 == entityChild.Parent)
				continue;
			if(-1 == entityChild.Geometry)
				continue;
			::gpk::SMatrix4<float>									matrixTransform				= solarSystem.Scene.ModelMatricesGlobal[entityChild.Transform];
			::gpk::SMatrix4<float>									matrixTransformVP			= matrixTransform * matrixVP;
			::getLightArrays(matrixTransform.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
			const ::gpk::SGeometryQuads								& mesh						= solarSystem.Scene.Geometry[entityChild.Geometry];
			const ::gpk::view_grid<const ::gpk::SColorBGRA>			image						= solarSystem.Scene.Image	[entityChild.Image].View;
			for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
				::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
				::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, solarSystem.Scene.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer);
			}
		}
	}
	return 0;
}

static	int											drawExplosion
	( const ::ssg::SSolarSystem							& solarSystem
	, const ::ssg::SExplosion							& explosion
	, const ::gpk::SMatrix4<float>						& matrixView
	, ::gpk::view_grid<::gpk::SColorBGRA>				& targetPixels
	, ::gpk::view_grid<uint32_t>						depthBuffer
	, ::ssg::SSolarSystemDrawCache						& drawCache
	) {
	::gpk::view_grid<const ::gpk::SColorBGRA>				image					= solarSystem.Scene.Image	[explosion.IndexMesh].View;
	const ::gpk::SGeometryQuads								& mesh					= solarSystem.Scene.Geometry[explosion.IndexMesh];
	for(uint32_t iExplosionPart = 0; iExplosionPart < explosion.Particles.Position.size(); ++iExplosionPart) {
		const ::gpk::SRange<uint16_t>							& sliceMesh				= explosion.Slices[iExplosionPart];
		::gpk::SMatrix4<float>									matrixPart				= {};
		matrixPart.Identity();
		if(iExplosionPart % 5) matrixPart.RotationY(solarSystem.AnimationTime * 2);
		if(iExplosionPart % 3) matrixPart.RotationX(solarSystem.AnimationTime * 2);
		if(iExplosionPart % 2) matrixPart.RotationZ(solarSystem.AnimationTime * 2);
		matrixPart.SetTranslation(explosion.Particles.Position[iExplosionPart], false);
		::gpk::SMatrix4<float>									matrixTransformView		= matrixPart * matrixView;
		::getLightArrays(matrixPart.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		for(uint32_t iTriangle = 0, countTriangles = sliceMesh.Count; iTriangle < countTriangles; ++iTriangle) {
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			const uint32_t											iActualTriangle		= sliceMesh.Offset + iTriangle;
			::gpk::STriangle3	<float>								triangle			= mesh.Triangles	[iActualTriangle];
			::gpk::STriangle3	<float>								triangleWorld		= mesh.Triangles	[iActualTriangle];
			::gpk::SCoord3		<float>								normal				= mesh.Normals		[iActualTriangle / 2];
			::gpk::STriangle2	<float>								triangleTexCoords	= mesh.TextureCoords[iActualTriangle];
			::gpk::STriangle3	<float>								triangleScreen		= triangleWorld;
			::gpk::transform(triangleScreen, matrixTransformView);
			if(triangleScreen.ClipZ())
				continue;

			::gpk::transform(triangleWorld, matrixPart);
			normal												= matrixPart.TransformDirection(normal).Normalize();
 			::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
 			::gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.Scene.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			triangle											= {triangle.A, triangle.C, triangle.B};
			triangleWorld										= {triangleWorld.A, triangleWorld.C, triangleWorld.B};
			triangleTexCoords									= {triangleTexCoords.A, triangleTexCoords.C, triangleTexCoords.B};
			normal												*= -1;
			::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
  			::gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.Scene.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
		}
	}
	return 0;
}

int													ssg::solarSystemDraw		(const ::ssg::SSolarSystem & solarSystem, ::ssg::SSolarSystemDrawCache & drawCache, ::std::mutex & mutexUpdate, ::gpk::view_grid<::gpk::SColorBGRA> & targetPixels, ::gpk::view_grid<uint32_t> depthBuffer)	{
	//------------------------------------------- Transform and Draw
	if(0 == targetPixels.size())
		return 1;

	memset(depthBuffer.begin(), -1, sizeof(uint32_t) * depthBuffer.size());
	{
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		memcpy(targetPixels.begin(), solarSystem.BackgroundImage.Texels.begin(), sizeof(::gpk::SColorBGRA) * ::gpk::min(targetPixels.size(), solarSystem.BackgroundImage.Texels.size()));
		::drawStars(solarSystem.Stars, targetPixels);
	}
	::gpk::SMatrix4<float>									matrixView			= {};
	const ::gpk::SCamera									& camera			= solarSystem.Scene.Camera[solarSystem.Scene.CameraMode];
	matrixView.LookAt(camera.Position, camera.Target, camera.Up);
	matrixView											*= solarSystem.Scene.MatrixProjection;
	drawCache.LightPointsWorld.clear();
	drawCache.LightColorsWorld.clear();
	{
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		::getLightArrays(solarSystem, drawCache.LightPointsWorld, drawCache.LightColorsWorld);
	}
	drawCache.LightPointsModel.reserve(drawCache.LightPointsWorld.size());
	drawCache.LightColorsModel.reserve(drawCache.LightColorsWorld.size());

	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		const ::ssg::SShip										& ship					= solarSystem.Ships[iShip];
		if(ship.Health <= 0)
			continue;
		::drawShip(solarSystem, ship, matrixView, targetPixels, depthBuffer, drawCache);
	}
#pragma pack(push, 1)
	struct SRenderNode	{
		uint32_t					Mesh				;
		int32_t						Image				;
		::gpk::SSlice<uint16_t>		Slice				;
		::gpk::array_pod<uint16_t>	IndicesPointLight	;
	};
#pragma pack(pop)

	::gpk::array_pod<uint16_t>								indicesPointLight;
	for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion) {
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		const ::ssg::SExplosion									& explosion				= solarSystem.Explosions[iExplosion];
		if(0 == explosion.Slices.size())
			continue;
		::drawExplosion(solarSystem, explosion, matrixView, targetPixels, depthBuffer, drawCache);
	}

	::gpk::array_pod<::gpk::SCoord3<float>>					pixelCoordsCache;
	pixelCoordsCache.reserve(512);
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		const ::ssg::SShip										& ship					= solarSystem.Ships[iShip];
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
			::gpk::SColorFloat									colorShot				= ::gpk::WHITE;
			double												brightRadius			= 1;
			double												intensity				= 1;
			bool												line					= true;
				 if(::ssg::MUNITION_TYPE_RAY	== ship.Parts[iPart].Shots.Type) { colorShot = ::gpk::SColorFloat{1.0f, 0.1f, 0.0f}		; brightRadius =  2.0; intensity =  1; line = true ;}
			else if(::ssg::MUNITION_TYPE_BULLET	== ship.Parts[iPart].Shots.Type) { colorShot = ::gpk::DARKGRAY							; brightRadius =  2.0; intensity =  1; line = true ;}
			else if(::ssg::MUNITION_TYPE_SHELL	== ship.Parts[iPart].Shots.Type) {
				colorShot										= ship.Team ? ::gpk::SColorFloat{1.0f, 0.125f, 0.25f} : ::gpk::TURQUOISE;
				brightRadius									= 7;
				intensity										= 9;
				line											= false;
			}
			::drawShots(targetPixels, ship.Parts[iPart].Shots, matrixView, colorShot, brightRadius, intensity, line, depthBuffer, drawCache.LightPointsModel);
		}
	}
	{
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		::drawDebris(targetPixels, solarSystem.Debris, matrixView, depthBuffer);
	}
	return 0;
}
