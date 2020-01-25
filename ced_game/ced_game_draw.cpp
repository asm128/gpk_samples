#include "ced_game.h"
#include "ced_draw.h"
#include <algorithm>

int													drawStars			(SStars & stars, ::gpk::view_grid<::gpk::SColorBGRA> targetPixels)	{
	::gpk::SColorBGRA											colors[]			=
		{ {0xfF, 0xfF, 0xfF, }
		, {0x40, 0x80, 0xfF, }
		, {0xfF, 0x80, 0x40, }
		, {0x80, 0x80, 0x40, }
		};
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::gpk::SCoord2<float>									starPos			= stars.Position[iStar];
		::gpk::SColorBGRA											starFinalColor	= colors[iStar % ::std::size(colors)] * stars.Brightness[iStar];
		::ced::setPixel(targetPixels, starPos.Cast<int32_t>(), starFinalColor);
		const	int32_t											brightRadius	= 1 + (iStar % 5) + (rand() % 4);
		double													brightUnit		= 1.0 / brightRadius;
		for(int32_t y = -brightRadius; y < brightRadius; ++y)
		for(int32_t x = -brightRadius; x < brightRadius; ++x) {
			::gpk::SCoord2<float>									brightPos		= {(float)x, (float)y};
			const double											brightDistance	= brightPos.Length();
			if(brightDistance <= brightRadius) {
				::gpk::SCoord2<int32_t>									pixelPos		= (starPos + brightPos).Cast<int32_t>();
				if( pixelPos.y >= 0 && pixelPos.y < (int32_t)targetPixels.metrics().y
				 && pixelPos.x >= 0 && pixelPos.x < (int32_t)targetPixels.metrics().x
 				)
					::ced::setPixel(targetPixels, pixelPos, targetPixels[pixelPos.y][pixelPos.x] + starFinalColor * (1.0-(brightDistance * brightUnit * (1 + (rand() % 3)))));
			}
		}
	}
	return 0;
}


int													drawDebris			(::gpk::view_grid<::gpk::SColorBGRA> targetPixels, SDebris & debris, const ::gpk::SMatrix4<float> & matrixVPV, ::gpk::view_grid<uint32_t> depthBuffer)	{
	::gpk::SColorBGRA										colors[]			=
		{ {0x80, 0xAF, 0xFF, }
		, {0x40, 0x80, 0xFF, }
		, {0x20, 0x80, 0xFF, }
		, {0x00, 0x00, 0xFF, }
		};
	::gpk::array_pod<::gpk::SCoord2<int32_t>>				pixelCoords;
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		::gpk::SColorBGRA										colorShot			= colors[iParticle % ::std::size(colors)];
		::gpk::SCoord3<float>									starPos				= debris.Position[iParticle];
		starPos												= matrixVPV.Transform(starPos);
		const ::gpk::SCoord2<int32_t>							pixelCoord		= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		uint32_t												depth				= uint32_t((1.0 - starPos.z) * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		::gpk::SColorFloat											starFinalColor	= colorShot * debris.Brightness[iParticle];
		starFinalColor.g										= ::std::max(0.0f, starFinalColor.g - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 2))));
		starFinalColor.b										= ::std::max(0.0f, starFinalColor.b - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 1))));
		::ced::setPixel(targetPixels, pixelCoord, starFinalColor);
		const	double											brightRadius		= 3.0;
		double													brightUnit			= 1.0 / brightRadius;
		for(int32_t y = (int32_t)-brightRadius; y < (int32_t)brightRadius; ++y)
		for(int32_t x = (int32_t)-brightRadius; x < (int32_t)brightRadius; ++x) {
			::gpk::SCoord2<float>									brightPos			= {(float)x, (float)y};
			const double											brightDistance		= brightPos.Length();
			if(brightDistance <= brightRadius) {
				::gpk::SCoord2<int32_t>									blendPos			= pixelCoord + (brightPos).Cast<int32_t>();
				if( blendPos.y >= 0 && blendPos.y < (int32_t)targetPixels.metrics().y
				 && blendPos.x >= 0 && blendPos.x < (int32_t)targetPixels.metrics().x
				 && depth <= depthBuffer[blendPos.y][blendPos.x]
				) {
					depthBuffer[blendPos.y][blendPos.x]					= depth;
					double													finalBrightness					= 1.0-(brightDistance * brightUnit);
					::gpk::SColorFloat										backgroundColor					= targetPixels[blendPos.y][blendPos.x];
					::ced::setPixel(targetPixels, blendPos, backgroundColor + starFinalColor * finalBrightness);
				}
			}
		}
	}
	return 0;
}

int													drawShots			(::gpk::view_grid<::gpk::SColorBGRA> targetPixels, SShots & shots
	, const ::gpk::SMatrix4<float> & matrixVPV
	, ::gpk::SColorFloat colorShot, ::gpk::view_grid<uint32_t> depthBuffer)	{
	::gpk::array_pod<::gpk::SCoord3<float>>					pixelCoords;
	const ::gpk::SColorFloat								starFinalColor	= colorShot;// * shots.Brightness[iShot];
	for(uint32_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		pixelCoords.clear();
		::gpk::SCoord3<float>									starPosPrev		= shots.PositionPrev[iShot];
		::gpk::SCoord3<float>									starPos			= shots.Position[iShot];
		::gpk::SLine3<float>									raySegmentWorld	= {starPosPrev, starPos};

		::gpk::SLine3<float>									raySegment		= raySegmentWorld;
		raySegment.A										= matrixVPV.Transform(raySegment.A);
		raySegment.B										= matrixVPV.Transform(raySegment.B);

		::ced::drawLine(targetPixels, raySegment, pixelCoords, depthBuffer);
		//::ced::drawLine(targetPixels, {{int32_t(raySegment.A.x), int32_t(raySegment.A.y)}, {int32_t(raySegment.B.x), int32_t(raySegment.B.y)}}, pixelCoords);//, depthBuffer);{int32_t(raySegment.A.x), int32_t(raySegment.A.y + 1)}
		//if(2 > pixelCoords.size()) {
		//	pixelCoords.push_back({int32_t(raySegment.A.x), int32_t(raySegment.A.y + 1)});
		//}
		for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
			const ::gpk::SCoord3<float>							& pixelCoord		= pixelCoords[iPixelCoord];
			if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
			 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
			)
				continue;
			uint32_t												depth				= uint32_t(pixelCoord.z * 0xFFFFFFFFU);
			::ced::setPixel(targetPixels, {(int32_t)pixelCoord.x, (int32_t)pixelCoord.y}, starFinalColor);

			const	double											brightRadius		= 7.5;
			double													brightUnit			= 1.0 / brightRadius;
			for(int32_t y = (int32_t)-brightRadius; y < (int32_t)brightRadius; ++y)
			for(int32_t x = (int32_t)-brightRadius; x < (int32_t)brightRadius; ++x) {
				::gpk::SCoord2<float>									brightPos			= {(float)x, (float)y};
				const double											brightDistance		= brightPos.Length();
				if(brightDistance <= brightRadius) {
					::gpk::SCoord2<int32_t>									blendPos			= ::gpk::SCoord2<int32_t>{(int32_t)pixelCoord.x, (int32_t)pixelCoord.y} + (brightPos).Cast<int32_t>();
					if( blendPos.y >= 0 && blendPos.y < (int32_t)targetPixels.metrics().y
					 && blendPos.x >= 0 && blendPos.x < (int32_t)targetPixels.metrics().x
					 && depth <= depthBuffer[blendPos.y][blendPos.x]
					) {
						depthBuffer[blendPos.y][blendPos.x]					= depth;
						double													finalBrightness					= (1.0 - (brightDistance * brightUnit)) * (1.0 / pixelCoords.size() * iPixelCoord);
						::gpk::SColorFloat										backgroundColor					= targetPixels[blendPos.y][blendPos.x];
						::gpk::SColorFloat										pixelColor						= backgroundColor + starFinalColor * finalBrightness;
						::ced::setPixel(targetPixels, blendPos, pixelColor);
					}
				}
			}
		}

	}
	return 0;
}

int													drawGame				(::gme::SApplication & app, ::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>> target)	{
	//------------------------------------------- Transform and Draw
	::gpk::view_grid<::gpk::SColorBGRA>						targetPixels		= target->Color.View;
	if(0 == targetPixels.size())
		return 1;
	::gpk::SColorBGRA											colorBackground		= {0x20, 0x8, 0x4};
	//colorBackground										+= (colorBackground * (0.5 + (0.5 / 65535 * rand())) * ((rand() % 2) ? -1 : 1)) ;
	for(uint32_t y = 0; y < targetPixels.metrics().y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < targetPixels.metrics().x; ++x)
		targetPixels[y][x]										= colorBackground;

	drawStars(app.Stars, targetPixels);

	app.Scene.LightVector.Normalize();

	::gpk::SMatrix4<float>									matrixView			= {};
	matrixView.LookAt(app.Scene.Camera.Position, app.Scene.Camera.Target, app.Scene.Camera.Up);
	{
		::gpk::SMatrix4<float>									matrixProjection	= {};
		matrixProjection.FieldOfView(::gpk::math_pi * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.1, 200);
		matrixView											= matrixView * matrixProjection;
	}
	{
		::gpk::SMatrix4<float>									matrixViewport		= {};
		matrixViewport.ViewportLH(targetPixels.metrics(), 0.01, 1000);
		matrixViewport										= matrixViewport.GetInverse();
		matrixViewport._41									+= targetPixels.metrics().x / 2;
		matrixViewport._42									+= targetPixels.metrics().y / 2;
		matrixView											*= matrixViewport;
	}

	::gpk::SColorBGRA										colorShotPlayer			= ::gpk::SColorBGRA{0x40, 0xfF, 0x80};// *.2;
	::gpk::SColorBGRA										colorShotEnemy			= ::gpk::SColorBGRA{0x00, 0x00, 0xfF};// *.2;
	::gpk::array_pod<::gpk::SCoord2<int32_t>>				pixelCoords;
	::gpk::array_pod<::gpk::STriangleWeights<double>>		pixelVertexWeights;
	::ced::SModelTransform									matrices;
	::ced::SModelTransform									matricesParent;
	::gpk::SColorBGRA										colorLightPlayer		= ::gpk::SColorBGRA{0xFF, 0xFF, 0xFF};// *.2;
	::gpk::SColorBGRA										colorLightEnemy			= ::gpk::SColorBGRA{0xFF, 0xFF, 0xFF};// *.2;
	for(uint32_t iModel = 0; iModel < app.Scene.Models.size(); ++iModel) {
		if(app.Health[iModel] <= 0)
			continue;
		::ced::SModel3D											& model			= app.Scene.Models[iModel];
		matrices.Scale		.Scale			(model.Scale	, true);
		matrices.Rotation	.Rotation		(model.Rotation);
		matrices.Position	.SetTranslation	(model.Position, true);

		::ced::SEntity											& entity				= app.Scene.Entities[iModel];
		if(-1 == entity.Parent)
			continue;
		::ced::SModel3D											& modelParent			= app.Scene.Models[entity.Parent];
		matricesParent.Scale	.Scale			(modelParent.Scale, true);
		matricesParent.Rotation	.Rotation		(modelParent.Rotation);
		matricesParent.Position	.SetTranslation	(modelParent.Position, true);

		::gpk::SMatrix4<float>									matrixTransform			= matrices.Scale * matrices.Rotation * matrices.Position;
		::gpk::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
		matrixTransform										= matrixTransform  * matrixTransformParent ;
		::gpk::array_pod<::gpk::SCoord3<float>>					lightPoints				= {};
		::gpk::array_pod<::gpk::SColorBGRA>						lightColors				= {};
		lightPoints.resize(app.ShotsEnemy.Position.size() + app.ShotsPlayer.Position.size() + app.Debris.Position.size() + 4);
		lightColors.resize(app.ShotsEnemy.Position.size() + app.ShotsPlayer.Position.size() + app.Debris.Position.size() + 4);
		lightPoints[0]									= app.Scene.Models[0].Position;
		lightColors[0]									= colorLightPlayer;
		for(uint32_t iEnemy = 1; iEnemy < 4; ++iEnemy) {
			uint32_t iModelEnemy = 7 * iEnemy;
			if(iModelEnemy >= app.Scene.Models.size())
				continue;
			lightPoints[iEnemy]								= app.Scene.Models[iModelEnemy].Position;
			lightColors[iEnemy]								= colorLightEnemy;
		}
		uint32_t												iOffset					= 4;
		for(uint32_t iShot = 0; iShot < app.ShotsEnemy.Position.size(); ++iShot) {
			lightPoints[iOffset + iShot]						= app.ShotsEnemy.Position[iShot];
			lightColors[iOffset + iShot]						= colorShotEnemy;
		}
		iOffset												+= app.ShotsEnemy.Position.size();
		for(uint32_t iShot = 0; iShot < app.ShotsPlayer.Position.size(); ++iShot) {
			lightPoints[iOffset + iShot]						= app.ShotsPlayer.Position[iShot];
			lightColors[iOffset + iShot]						= colorShotPlayer;
		}
		iOffset												+= app.ShotsPlayer.Position.size();
		for(uint32_t iShot = 0; iShot < app.Debris.Position.size(); ++iShot) {
			lightPoints[iOffset + iShot]						= app.Debris.Position[iShot];
			lightColors[iOffset + iShot]						= {0x2F,0xAF, 0xFF};
		}
		for(uint32_t iTriangle = 0; iTriangle < app.Scene.Geometry[iModel / 7].Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawQuadTriangle(targetPixels, app.Scene.Geometry[iModel / 7], iTriangle, matrixTransform, matrixView, app.Scene.LightVector, pixelCoords, pixelVertexWeights, app.Scene.Image[iModel / 7].View, lightPoints, lightColors, target->DepthStencil.View);
		}
	}

	::drawShots(targetPixels, app.ShotsPlayer	, matrixView, colorShotPlayer	, target->DepthStencil.View);
	::drawShots(targetPixels, app.ShotsEnemy	, matrixView, colorShotEnemy	, target->DepthStencil.View);
	::drawDebris(targetPixels, app.Debris		, matrixView, target->DepthStencil.View);
	return 0;
}
