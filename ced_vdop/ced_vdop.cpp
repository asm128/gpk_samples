#include "ced_vdop.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"

#include <time.h>

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "PNG Test");

::gpk::error_t										cleanup							(::SApplication & app)						{
	::klib::shutdownASCIIScreen();
	return ::gpk::mainWindowDestroy(app.Framework.MainDisplay);
}

::gpk::error_t										setup							(::SApplication & app)						{
	::gpk::SFramework										& framework						= app.Framework;
	::gpk::SDisplay											& mainWindow					= framework.MainDisplay;
	framework.Input.create();
	mainWindow.Size														= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?????!?!?!?!?");
	{ // Build the exit button
		::gpk::SGUI																& gui								= *framework.GUI;
		gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
		gui.ThemeDefault													= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
		app.IdExit															= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlExit						= gui.Controls.Controls[app.IdExit];
		controlExit.Area													= {{}, {64, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
		::gpk::SControlText														& controlText						= gui.Controls.Text[app.IdExit];
		controlText.Text													= "Exit";
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints												& controlConstraints				= gui.Controls.Constraints[app.IdExit];
		controlConstraints.AttachSizeToControl								= {app.IdExit, -1};
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}
	//framework.UseDoubleBuffer							= true;
	srand((uint32_t)time(0));

	const ::gpk::SCoord2<uint32_t>											metricsMap			= app.TextOverlay.MetricsMap;
	const ::gpk::SCoord2<uint32_t>											metricsLetter		= app.TextOverlay.MetricsLetter;
	//::gpk::SImage<::gpk::SColorBGRA>										fontImagePre;
	//::gpk::pngFileLoad(::gpk::view_const_string{"../gpk_data/images/dos_8x8_font_white.png"}, fontImagePre);//Codepage_437_24_12x12.png"}, fontImage);
	::gpk::SImage<::gpk::SColorBGRA>										fontImage;
	::gpk::pngFileLoad(::gpk::view_const_string{"../gpk_data/images/Codepage_437_24_12x12.png"}, fontImage);
	//fontImage.resize(::gpk::SCoord2<uint32_t>{app.TextOverlay.MetricsMap}.InPlaceScale(app.TextOverlay.MetricsLetter));
	//
	//for(uint32_t y = 0; y < metricsMap.y; ++y)
	//for(uint32_t x = 0; x < metricsMap.x; ++x) {
	//	::gpk::SRectangle2<uint32_t>											dstRect				= {{x * metricsLetter.x, y * metricsLetter.y}, metricsLetter};
	//	::gpk::SRectangle2<uint32_t>											srcRect				=
	//		{ {x * metricsLetter.x + 1 + x, y * metricsLetter.y + 1 + y}
	//		, metricsLetter
	//		};
	//	//::gpk::grid_copy_ex(fontImage.View, fontImagePre.View, metricsLetter.Cast<int32_t>(), dstRect.Offset.Cast<int32_t>(), srcRect.Offset.Cast<int32_t>());
	//	::gpk::grid_copy(fontImage.View, fontImagePre.View, dstRect.Offset, srcRect);
	//}
	::gpk::view_grid<::gpk::SGeometryQuads>									viewGeometries		= {app.TextOverlay.GeometryLetters, {16, 16}};
	const uint32_t															imagePitch			= metricsLetter.x * metricsMap.x;

	::gpk::array_pod<::gpk::STile>											tiles;
	for(uint32_t y = 0; y < metricsMap.y; ++y)
	for(uint32_t x = 0; x < metricsMap.x; ++x) {
		tiles.clear();
		const uint32_t											asciiCode			= y * app.TextOverlay.MetricsMap.x + x;
		const ::gpk::SCoord2<uint32_t>							asciiCoords			= {asciiCode %		metricsMap.x, asciiCode / app.TextOverlay.MetricsMap.x};
		const uint32_t											offsetPixelCoord	= (asciiCoords.y *	metricsLetter.y) * imagePitch + (asciiCoords.x * app.TextOverlay.MetricsLetter.x);
		::gpk::geometryBuildTileListFromImage({&fontImage.Texels[offsetPixelCoord], app.TextOverlay.MetricsLetter}, tiles, app.TextOverlay.MetricsLetter.x * app.TextOverlay.MetricsMap.x);
		::gpk::geometryBuildGridFromTileList(app.TextOverlay.GeometryLetters[asciiCode], ::gpk::view_grid<::gpk::STile>{tiles.begin(), app.TextOverlay.MetricsLetter}, {}, {1, 6.0f, 1});
	}

	::klib::initASCIIScreen(klib::SGlobalDisplay::Width, klib::SGlobalDisplay::Depth);

	::klib::SGame												& instanceGame		= app.Game;

	::klib::initGame(instanceGame);

	return 0;
}

int													update				(SApplication & app, bool exitSignal)	{
	::gpk::SFramework										& framework			= app.Framework;
	//::gpk::STimer															timer;
	(void)exitSignal;
	//retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	{
		::gpk::mutex_guard									lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen				= app.Offscreen;
	}
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");

	::gpk::SGUI												& gui						= *framework.GUI;
	::gpk::array_pod<uint32_t>								controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		uint32_t												idControl					= controlsToProcess		[iControl];
		const ::gpk::SControlState								& controlState				= gui.Controls.States	[idControl];
		if(controlState.Execute) {
			info_printf("Executed %u.", idControl);
			if(idControl == (uint32_t)app.IdExit)
				return 1;
		}
	}
	if(framework.MainDisplay.Resized) {
		::gpk::SMatrix4<float>									& matrixProjection	= app.TextOverlay.MatrixProjection;
		matrixProjection.FieldOfView(::gpk::math_pi * .25, framework.MainDisplay.Size.x / (double)framework.MainDisplay.Size.y, 0.01, 500);
		::gpk::SMatrix4<float>									matrixViewport		= {};
		matrixViewport.ViewportLH(framework.MainDisplay.Size);
		matrixProjection									*= matrixViewport;
	}
	::klib::pollInput(app.Game.FrameInput);
	::klib::SASCIITarget							target;
	::klib::getASCIIBackBuffer						(target);

	{
		::gpk::mutex_guard									lock						(app.LockRender);
		::klib::clearASCIIBackBuffer(' ', COLOR_WHITE);
 		::klib::drawAndPresentGame(app.Game, target);
	}
	::klib::presentASCIIBackBuffer();

	Sleep(1);
	return 0;
}

static	int											drawPixels
	( ::gpk::view_grid<::gpk::SColorBGRA>				targetPixels
	, const ::gpk::STriangle3	<float>					& triangleWorld
	, const ::gpk::SCoord3		<float>					& normal
	, const ::gpk::SCoord3		<float>					& lightVector
	, const ::gpk::SColorFloat							& texelColor
	, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, double											timeAnimation
	) {
	double													lightFactorDirectional		= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::SCoord2<int32_t>									pixelCoord					= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<float>					& vertexWeights				= pixelVertexWeights[iPixelCoord];
		const ::gpk::SCoord3<float>								position					= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		double													factorWave					= (::gpk::max(0.0, sin(- timeAnimation * 4 + position.y * .75))) * .6;
		double													factorWave2					= (::gpk::max(0.0, sin(- timeAnimation + position.x * .0125 + position.z * .125))) * .5;
		::gpk::setPixel(targetPixels, pixelCoord, targetPixels[pixelCoord.y][pixelCoord.x] * .25 + (texelColor * (lightFactorDirectional * 2) + texelColor * factorWave + texelColor * factorWave2));
	}
	return 0;
}

int													draw3DCharacter
	( const ::gpk::view_grid<::gpk::SColorBGRA>				& targetPixels
	, const ::gpk::SCoord2<uint32_t>						& metricsCharacter
	, const ::gpk::SCoord2<uint32_t>						& metricsMap
	, const uint8_t											asciiCode
	, const ::gpk::SCoord3<float>							& position
	, const ::gpk::SCoord3<float>							& lightVector
	, const ::gpk::SMatrix4<float>							& matrixView
	, const ::gpk::view_grid<const ::gpk::SGeometryQuads>	& viewGeometries
	, ::SDrawCache											& drawCache
	, ::gpk::view_grid<uint32_t>							& depthBuffer
	, double												timeAnimation
	, const ::gpk::SColorBGRA								& color
	)	{
	::gpk::SMatrix4<float>									matrixScale				;
	::gpk::SMatrix4<float>									matrixPosition			;
	::gpk::SMatrix4<float>									matrixRotation			;
	matrixRotation.Identity();
	if(asciiCode == 5)
		matrixRotation.RotationX(-::gpk::math_pi_2);
	if(asciiCode == 0xB)
		matrixRotation.RotationX(-::gpk::math_pi_2);
	if(asciiCode == 0xC)
		matrixRotation.RotationX(-::gpk::math_pi_2);
	if(asciiCode == 0xE8)
		matrixRotation.RotationX(-::gpk::math_pi_2);
	::gpk::SCoord3<float>									translation				= {};
	translation.x										= float(position.x * metricsCharacter.x);
	translation.z										= float(position.z * metricsCharacter.y);
	matrixPosition	.SetTranslation	({translation.x, 0, translation.z}, true);
	matrixScale		.Scale			({1, 1, 1}, true);
	::gpk::SMatrix4<float>									matrixTransform										= matrixScale * matrixRotation * matrixPosition;
	::gpk::SMatrix4<float>									matrixTransformView									= matrixTransform * matrixView;
	const ::gpk::SCoord2<uint32_t>							asciiCoords				= {asciiCode % metricsMap.x, asciiCode / metricsMap.x};
	const ::gpk::SGeometryQuads								& geometry				= viewGeometries[asciiCoords.y][asciiCoords.x];
	for(uint32_t iTriangle = 0; iTriangle < geometry.Triangles.size(); ++iTriangle) {
		drawCache.PixelCoords			.clear();
		drawCache.PixelVertexWeights	.clear();
		::gpk::STriangle3		<float>							triangle			= geometry.Triangles	[iTriangle];;
		const ::gpk::SCoord3	<float>							& normal			= geometry.Normals		[iTriangle / 2];
		::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
		::gpk::SCoord3	<float>									xnormal				= matrixTransform.TransformDirection(normal).Normalize();
		::gpk::STriangle3		<float>							triangleWorld		= triangle;
		::gpk::transform(triangleWorld, matrixTransform);
		::drawPixels(targetPixels, triangleWorld, xnormal, lightVector, color * .75, drawCache.PixelCoords, drawCache.PixelVertexWeights, timeAnimation);
	}
	return 0;
}

int													draw					(SApplication & app) {
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, ::gpk::DARKGREEN, 0xFFFFFFFFU);
	//for(uint32_t y = 0; y < target->Color.View.metrics().y; ++y)
	//for(uint32_t x = 0; x < target->Color.View.metrics().x; ++x) {
	//	target->Color.View[y][x]											= rand();
	//	target->Color.View[y][x].a											= 255;
	//}


	//s::gpk::view_grid<::gpk::SColorBGRA>						targetPixels			= {app.Framework.DoubleBuffer[InterlockedIncrement64(&app.Framework.CurrentRenderBuffer) % 2].begin(), app.Framework.Window.Size};
	//s::gpk::view_grid<uint32_t>								depthBuffer				= {app.Framework.DepthBuffer.begin(), app.Framework.Window.Size};
	::gpk::view_grid<::gpk::SColorBGRA>						targetPixels			= target->Color.View;
	::gpk::view_grid<uint32_t>								depthBuffer				= target->DepthStencil.View;
	//::gpk::clearTarget(*target);

	app.TextOverlay.DrawCache							= {};


	app.TextOverlay.LightVector0.Normalize();
	::gpk::view_grid<::gpk::SGeometryQuads>					viewGeometries		= {app.TextOverlay.GeometryLetters, {16, 16}};
	uint32_t												colorIndex			= 0;

	//uint32_t												timeHours			= (int)app.Framework.FrameInfo.Seconds.Total / 3600;
	//uint32_t												timeMinutes			= (int)app.Framework.FrameInfo.Seconds.Total / 60 % 60;
	//uint32_t												timeSeconds			= (int)app.Framework.FrameInfo.Seconds.Total % 60;
	//uint32_t												timeCents			= int (app.Framework.FrameInfo.Seconds.Total * 10) % 10;
	//char													strStage		[17]	= {};
	//char													strScore		[17]	= {};
	//char													strTimeHours	[3]	= {};
	//char													strTimeMinutes	[3]	= {};
	//char													strTimeSeconds	[3]	= {};
	//char													strTimeCents	[2]	= {};
	//memset(&mapToDraw[1][1], 0, mapToDraw.metrics().x - 2);
	//memset(depthBuffer.begin(), -1, sizeof(uint32_t) * depthBuffer.size());
	//sprintf_s(strStage		, "Stage:%u", (uint32_t)(0)); memcpy(&mapToDraw[1][1], strStage, strlen(strStage));
	//sprintf_s(strScore		, "Score:%u", (uint32_t)(0)); memcpy(&mapToDraw[1][mapToDraw.metrics().x - (uint32_t)strlen(strScore) - 1], strScore, strlen(strScore));
	//sprintf_s(strTimeHours	, "%.2u" , timeHours	); memcpy(&mapToDraw[3][ 6], strTimeHours	, 2);
	//sprintf_s(strTimeMinutes, "%.2u" , timeMinutes	); memcpy(&mapToDraw[3][ 9], strTimeMinutes	, 2);
	//sprintf_s(strTimeSeconds, "%.2u" , timeSeconds	); memcpy(&mapToDraw[3][12], strTimeSeconds	, 2);
	//sprintf_s(strTimeCents	, "%.1u" , timeCents	); memcpy(&mapToDraw[3][15], strTimeCents	, 1);

	::gpk::SMatrix4<float>									matrixView					= {};
	::gpk::SMatrix4<float>									matrixProjection			= {};
	::gpk::SMatrix4<float>									matrixViewport				= {};
	matrixView		.Identity();
	matrixProjection.Identity();
	matrixViewport	.Identity();

	if(0 > app.Game.TacticalInfo.CurrentPlayer)
		matrixView.LookAt(app.TextOverlay.CameraPosition, app.TextOverlay.CameraTarget, app.TextOverlay.CameraUp);
	else {
		::klib::SPlayer		& player = app.Game.Players[app.Game.TacticalInfo.Setup.Players[app.Game.TacticalInfo.CurrentPlayer]];
		matrixView.LookAt(app.TextOverlay.CameraPosition, player.Army[player.Squad.Agents[player.Selection.PlayerUnit]]->Position.Cast<float>().Scale(12), app.TextOverlay.CameraUp);
		for(uint32_t iPlayer = 0; iPlayer < ::gpk::size(app.Game.Players); ++iPlayer) {

		}
	}
	matrixProjection.FieldOfView(::gpk::math_pi * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 5000);
	matrixViewport.ViewportLH(targetPixels.metrics());
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	{
		::gpk::mutex_guard										lock						(app.LockRender);
		::gpk::view_grid<char>									mapToDraw			= app.Game.TacticalDisplay.Screen.Cells;

		if(0 > app.Game.TacticalInfo.CurrentPlayer)
			matrixView.LookAt(app.TextOverlay.CameraPosition, app.TextOverlay.CameraTarget, app.TextOverlay.CameraUp);
		else {
			::klib::SPlayer											& player			= app.Game.Players[app.Game.TacticalInfo.Setup.Players[app.Game.TacticalInfo.CurrentPlayer]];
			::gpk::SCoord3<float>									agentPosition		= player.Army[player.Squad.Agents[player.Selection.PlayerUnit]]->Position.Cast<float>();
			agentPosition.Scale({1, 1, -1});
			agentPosition										-= ::gpk::SCoord3<float>{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
			app.TextOverlay.CameraPosition						= agentPosition;
			agentPosition.Scale(12);
			app.TextOverlay.CameraPosition						+= {-24, 6, -24};
			app.TextOverlay.CameraPosition.Scale(12);
			matrixView.LookAt(app.TextOverlay.CameraPosition, agentPosition, app.TextOverlay.CameraUp);
			for(uint32_t iPlayer = 0; iPlayer < ::gpk::size(app.Game.Players); ++iPlayer) {

			}
		}
		matrixView											*= matrixProjection;
		matrixView											*= matrixViewport;

		::gpk::SCoord3<float>									offset			= {};//app.TextOverlay.ControlTranslation;
		offset												-= ::gpk::SCoord3<float>{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
		::gpk::view_grid<uint16_t>								mapColors		= app.Game.TacticalDisplay.TextAttributes.Cells;
		for(uint32_t y = 0; y < mapToDraw.metrics().y; ++y)
		for(uint32_t x = 0; x < mapToDraw.metrics().x; ++x) {
			const uint8_t											asciiCode			= mapToDraw[y][x];
			if(0 == asciiCode)
				continue;
			const uint16_t											asciiColor			= mapColors[y][x];
			::gpk::SColorFloat										color					= app.Framework.GUI->Palette[(asciiColor & 0xF)];//::gpk::COLOR_TABLE[((int)timeAnimation) % ::gpk::size(::gpk::COLOR_TABLE)];
			::gpk::SCoord3<float>									position			= offset;
			position.x											+= x;
			position.z											-= y;
			draw3DCharacter(targetPixels, app.TextOverlay.MetricsLetter, app.TextOverlay.MetricsMap, asciiCode, position, app.TextOverlay.LightVector0, matrixView, viewGeometries, app.TextOverlay.DrawCache, depthBuffer, app.Framework.FrameInfo.Seconds.Total, color);
			++colorIndex;
		}
	}

	{
		::gpk::mutex_guard										lock						(app.LockRender);
		::gpk::view_grid<char>									mapToDraw					= app.Game.PostEffectDisplay.Screen.Cells;
		::gpk::view_grid<uint16_t>								mapColors					= app.Game.PostEffectDisplay.TextAttributes.Cells;
		::gpk::SCoord3<float>									offset						= {};//app.TextOverlay.ControlTranslation;
		offset												-= ::gpk::SCoord3<float>{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
	//::gpk::view_grid<uint8_t>								mapToDraw			= app.MapToDraw;
		for(uint32_t y = 0; y < mapToDraw.metrics().y; ++y)
		for(uint32_t x = 0; x < mapToDraw.metrics().x; ++x) {
			const uint8_t											asciiCode			= mapToDraw[y][x];
			if(0 == asciiCode)
				continue;
			const uint16_t											asciiColor			= mapColors[y][x];
			::gpk::SColorFloat										color				= app.Framework.GUI->Palette[(asciiColor & 0xF0)];//::gpk::COLOR_TABLE[((int)timeAnimation) % ::gpk::size(::gpk::COLOR_TABLE)];
			::gpk::SCoord3<float>									position			= offset;
			position.x											+= x;
			position.z											-= y;
			draw3DCharacter(targetPixels, app.TextOverlay.MetricsLetter, app.TextOverlay.MetricsMap, asciiCode, position, app.TextOverlay.LightVector0, matrixView, viewGeometries, app.TextOverlay.DrawCache, depthBuffer, app.Framework.FrameInfo.Seconds.Total, color);
			++colorIndex;
		}
	}
	{
		::gpk::mutex_guard														lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}

	return 0;
}
