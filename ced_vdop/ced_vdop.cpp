#include "ced_vdop.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"

#include <time.h>

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "VDoP Server");

::gpk::error_t			cleanup		(::SApplication & app)						{
	::klib::shutdownASCIIScreen();
	return ::gpk::mainWindowDestroy(app.Framework.RootWindow);
}

::gpk::error_t			setup		(::SApplication & app)						{
	::gpk::SFramework				& framework						= app.Framework;
	::gpk::SWindow					& mainWindow					= framework.RootWindow;
	mainWindow.Size										= {1280, 720};
	es_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	{ // Build the exit button
		::gpk::SGUI											& gui								= *framework.GUI;
		gui.ColorModeDefault							= ::gpk::GUI_COLOR_MODE_3D;
		gui.ThemeDefault								= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
		app.IdExit										= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement										& controlExit						= gui.Controls.Placement[app.IdExit];
		controlExit.Area								= {{}, {64, 20}};
		controlExit.Border								= {10, 10, 10, 10};
		controlExit.Margin								= {1, 1, 1, 1};
		controlExit.Align								= ::gpk::ALIGN_BOTTOM_RIGHT;
		::gpk::SControlText									& controlText						= gui.Controls.Text[app.IdExit];
		controlText.Text								= "Exit";
		controlText.Align								= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints							& controlConstraints				= gui.Controls.Constraints[app.IdExit];
		controlConstraints.AttachSizeToControl			= {app.IdExit, -1};
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}
	srand((uint32_t)time(0));

	const ::gpk::n2<uint32_t>		metricsMap			= app.TextOverlay.MetricsMap;
	const ::gpk::n2<uint32_t>		metricsLetter		= app.TextOverlay.MetricsLetter;
	::gpk::img<::gpk::bgra>			fontImage;
	::gpk::pngFileLoad(::gpk::vcs{"../gpk_data/images/Codepage_437_24_12x12.png"}, fontImage);
	::gpk::grid<::gpk::SGeometryQuads>viewGeometries		= {app.TextOverlay.GeometryLetters, {16, 16}};
	const uint32_t					imagePitch			= metricsLetter.x * metricsMap.x;

	::gpk::apod<::gpk::STile>		tiles;
	for(uint32_t y = 0; y < metricsMap.y; ++y)
	for(uint32_t x = 0; x < metricsMap.x; ++x) {
		tiles.clear();
		const uint32_t					asciiCode			= y * app.TextOverlay.MetricsMap.x + x;
		const ::gpk::n2<uint32_t>		asciiCoords			= {asciiCode %		metricsMap.x, asciiCode / app.TextOverlay.MetricsMap.x};
		const uint32_t					offsetPixelCoord	= (asciiCoords.y *	metricsLetter.y) * imagePitch + (asciiCoords.x * app.TextOverlay.MetricsLetter.x);
		::gpk::geometryBuildTileListFromImage({&fontImage.Texels[offsetPixelCoord], app.TextOverlay.MetricsLetter}, tiles, app.TextOverlay.MetricsLetter.x * app.TextOverlay.MetricsMap.x);
		::gpk::geometryBuildGridFromTileList(app.TextOverlay.GeometryLetters[asciiCode], ::gpk::grid<::gpk::STile>{tiles.begin(), app.TextOverlay.MetricsLetter}, {}, {1, 6.0f, 1});
	}


#define DEFAULT_ASCII_DISPLAY_HEIGHT	70
#define DEFAULT_ASCII_DISPLAY_WIDTH		((uint32_t)(DEFAULT_ASCII_DISPLAY_HEIGHT * 2.666666f))
	::klib::initASCIIScreen(DEFAULT_ASCII_DISPLAY_WIDTH, DEFAULT_ASCII_DISPLAY_HEIGHT);

	::klib::SGame												& instanceGame		= app.Game;
	instanceGame.GlobalDisplay	.Resize({DEFAULT_ASCII_DISPLAY_WIDTH, DEFAULT_ASCII_DISPLAY_HEIGHT});
	instanceGame.TacticalDisplay.Resize({::klib::GAME_MAP_WIDTH, ::klib::GAME_MAP_DEPTH});

	::klib::initGame(instanceGame);


	//::gpk::tcpipInitialize();
	//::gpk::tcpipAddress("192.168.0.3", "51515", app.TacticalClient.AddressConnect);
	//::gpk::clientConnect(app.TacticalClient);

	return 0;
}

::gpk::error_t			update		(SApplication & app, bool exitSignal)	{
	rvis_if(::gpk::APPLICATION_STATE_EXIT, exitSignal);

	::gpk::SFramework				& framework	= app.Framework;
	//::gpk::STimer															timer;
	(void)exitSignal;
	{
		::std::lock_guard									lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer				= app.Offscreen;
	}
	rvis_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui			= *framework.GUI;
	::gpk::acid					toProcess		= {};
	if(1 == ::gpk::guiProcessControls(gui, [&app](::gpk::cid_t iControl) { return one_if(iControl == app.IdExit); }))
		return 1;

	if(framework.RootWindow.Resized) {
		::gpk::m4<float>									& matrixProjection			= app.TextOverlay.MatrixProjection;
		matrixProjection.FieldOfView(::gpk::math_pi * .25, framework.RootWindow.Size.x / (double)framework.RootWindow.Size.y, 0.01, 500.0);
		::gpk::m4<float>									matrixViewport				= {};
		matrixViewport.ViewportLH(framework.RootWindow.Size.Cast<uint16_t>());
		matrixProjection									*= matrixViewport;
	}
	::klib::pollInput(app.Game.FrameInput);
	::klib::SASCIITarget							target;
	::klib::getASCIIBackBuffer						(target);

	{
		::std::lock_guard									lock						(app.LockRender);
		::klib::clearASCIIBackBuffer(' ', ::klib::ASCII_COLOR_INDEX_WHITE);
 		::klib::drawAndPresentGame(app.Game, target);
	}
	::klib::presentASCIIBackBuffer();

	//::gpk::connectionPushData(app.TacticalClient, app.TacticalClient.Queue, "Message arrived! 4", false, false, 0);
	//::gpk::clientUpdate(app.TacticalClient);

	::gpk::sleep(10);


	Sleep(1);
	return 0;
}

static	int											drawPixels
	( ::gpk::g8bgra		targetPixels
	, const ::gpk::tri3f32			& triangleWorld
	, const ::gpk::n3f32			& normal
	, const ::gpk::n3f32			& lightVector
	, const ::gpk::rgbaf			& texelColor
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
	, double											timeAnimation
	) {
	double													lightFactorDirectional		= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::n2i16									pixelCoord					= pixelCoords		[iPixelCoord];
		const ::gpk::trif32					& vertexWeights				= pixelVertexWeights[iPixelCoord];
		const ::gpk::n3f32								position					= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		double													factorWave					= (::gpk::max(0.0, sin(- timeAnimation * 4 + position.y * .75))) * .6;
		double													factorWave2					= (::gpk::max(0.0, sin(- timeAnimation + position.x * .0125 + position.z * .125))) * .5;
		::gpk::setPixel(targetPixels, pixelCoord, (targetPixels[pixelCoord.Cast<uint32_t>()] * .25) + (texelColor * (lightFactorDirectional * 2) + texelColor * factorWave + texelColor * factorWave2));
	}
	return 0;
}

int													draw3DCharacter
	( const ::gpk::g8bgra	& targetPixels
	, const ::gpk::n2u32			& metricsCharacter
	, const ::gpk::n2u32			& metricsMap
	, const uint8_t					asciiCode
	, const ::gpk::n3f32			& position
	, const ::gpk::n3f32			& lightVector
	, const ::gpk::m4f32			& matrixView
	, const ::gpk::grid<const ::gpk::SGeometryQuads>	& viewGeometries
	, ::SDrawCache					& drawCache
	, ::gpk::gu32					& depthBuffer
	, double						timeAnimation
	, const ::gpk::bgra				& color
	)	{
	::gpk::m4f32					matrixScale				;
	::gpk::m4f32					matrixPosition			;
	::gpk::m4f32					matrixRotation			;
	matrixRotation.Identity();
	::gpk::n3f32					translation				= {};
	translation.x				= float(position.x * metricsCharacter.x);
	translation.z				= float(position.z * metricsCharacter.y);
	if(asciiCode == 0x05) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0x0B) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0x0C) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0xE8) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0xAB) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0xAC) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	matrixPosition	.SetTranslation	(translation, true);
	matrixScale		.Scale			({1, 1, 1}, true);
	::gpk::m4<float>									matrixTransform										= matrixScale * matrixRotation * matrixPosition;
	::gpk::m4<float>									matrixTransformView									= matrixTransform * matrixView;
	const ::gpk::n2<uint32_t>							asciiCoords				= {asciiCode % metricsMap.x, asciiCode / metricsMap.x};
	const ::gpk::SGeometryQuads								& geometry				= viewGeometries[asciiCoords.y][asciiCoords.x];
	for(uint32_t iTriangle = 0; iTriangle < geometry.Triangles.size(); ++iTriangle) {
		drawCache.PixelCoords			.clear();
		drawCache.PixelVertexWeights	.clear();
		::gpk::tri3		<float>							triangle			= geometry.Triangles	[iTriangle];;
		const ::gpk::n3	<float>							& normal			= geometry.Normals		[iTriangle / 2];
		::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
		::gpk::n3	<float>									xnormal				= matrixTransform.TransformDirection(normal).Normalize();
		::gpk::tri3		<float>							triangleWorld		= triangle;
		::gpk::transform(triangleWorld, matrixTransform);
		::drawPixels(targetPixels, triangleWorld, xnormal, lightVector, color * .75, drawCache.PixelCoords, drawCache.PixelVertexWeights, timeAnimation);
	}
	return 0;
}

int													draw					(SApplication & app) {
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::DARKGREEN, 0xFFFFFFFFU);
	::gpk::grid<::gpk::bgra>						targetPixels			= target->Color;
	::gpk::grid<uint32_t>								depthBuffer				= target->DepthStencil;
	app.TextOverlay.DrawCache							= {};

	app.TextOverlay.LightVector0.Normalize();
	::gpk::grid<::gpk::SGeometryQuads>					viewGeometries		= {app.TextOverlay.GeometryLetters, {16, 16}};
	uint32_t												colorIndex			= 0;

	::gpk::m4<float>									matrixView					= {};
	::gpk::m4<float>									matrixProjection			= {};
	::gpk::m4<float>									matrixViewport				= {};
	matrixView		.Identity();
	matrixProjection.Identity();
	matrixViewport	.Identity();

	matrixProjection.FieldOfView(::gpk::math_pi * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 5000.0);
	matrixViewport.ViewportLH(targetPixels.metrics().Cast<uint16_t>());
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	{
		::std::lock_guard										lock						(app.LockRender);
		::gpk::grid<char>									mapToDraw					= app.Game.GlobalDisplay.Screen.Color;
		::gpk::grid<uint16_t>								mapColors					= app.Game.GlobalDisplay.Screen.DepthStencil;
		matrixView.LookAt(app.TextOverlay.CameraPosition, app.TextOverlay.CameraTarget, app.TextOverlay.CameraUp);
		matrixView											*= matrixProjection;
		matrixView											*= matrixViewport;
		::gpk::n3f32									offset			= {};//app.TextOverlay.ControlTranslation;
		offset												-= ::gpk::n3f32{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
		for(uint32_t y = 0; y < mapToDraw.metrics().y; ++y)
		for(uint32_t x = 0; x < mapToDraw.metrics().x; ++x) {
			const uint8_t											asciiCode			= mapToDraw[y][x];
			if(0 == asciiCode)
				continue;
			const uint16_t											asciiColor			= mapColors[y][x];
			::gpk::rgbaf										color					= (*app.Framework.GUI->Colors->Palette)[(asciiColor & 0xF)];//::gpk::COLOR_TABLE[((int)timeAnimation) % ::gpk::size(::gpk::COLOR_TABLE)];
			::gpk::n3f32									position			= offset;
			position.x											+= x;
			position.z											-= y;
			draw3DCharacter(targetPixels, app.TextOverlay.MetricsLetter, app.TextOverlay.MetricsMap, asciiCode, position, app.TextOverlay.LightVector0, matrixView, viewGeometries, app.TextOverlay.DrawCache, depthBuffer, app.Framework.FrameInfo.Seconds.Total, color);
			++colorIndex;
		}
	}

	{
		::std::lock_guard										lock						(app.LockRender);
		::gpk::grid<char>									mapToDraw					= app.Game.TacticalDisplay.Screen.Color;
		::gpk::grid<uint16_t>								mapColors					= app.Game.TacticalDisplay.Screen.DepthStencil;

		if((app.Game.State.State != ::klib::GAME_STATE_START_MISSION && app.Game.State.State != ::klib::GAME_STATE_TACTICAL_CONTROL) || 0 > app.Game.TacticalInfo.CurrentPlayer)
			matrixView.LookAt(app.TextOverlay.CameraPosition, app.TextOverlay.CameraTarget, app.TextOverlay.CameraUp);
		else {
			::klib::SGamePlayer											& player			= app.Game.Players[app.Game.TacticalInfo.Setup.Players[app.Game.TacticalInfo.CurrentPlayer]];
			::gpk::n3f32									agentPosition		= player.Tactical.Army[player.Tactical.Squad.Agents[player.Tactical.Selection.PlayerUnit]]->Position.Cast<float>();
			agentPosition.Scale({1, 1, -1});
			agentPosition										-= ::gpk::n3f32{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
			::gpk::n3f32									cameraPosition		= agentPosition;
			agentPosition.Scale(12);
			cameraPosition										+= {-24, 6, -24};
			cameraPosition.Scale(12);
			matrixView.LookAt(cameraPosition, agentPosition, {0, 1, 0});//app.TextOverlay.CameraUp);
		}
		matrixView											*= matrixProjection;
		matrixView											*= matrixViewport;

		::gpk::n3f32									offset			= {};//app.TextOverlay.ControlTranslation;
		offset												-= ::gpk::n3f32{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
		for(uint32_t y = 0; y < mapToDraw.metrics().y; ++y)
		for(uint32_t x = 0; x < mapToDraw.metrics().x; ++x) {
			const uint8_t											asciiCode			= mapToDraw[y][x];
			if(0 == asciiCode)
				continue;
			const uint16_t											asciiColor			= mapColors[y][x];
			::gpk::rgbaf										color					= (*app.Framework.GUI->Colors->Palette)[(asciiColor & 0xF)];//::gpk::COLOR_TABLE[((int)timeAnimation) % ::gpk::size(::gpk::COLOR_TABLE)];
			::gpk::n3f32									position			= offset;
			position.x											+= x;
			position.z											-= y;
			draw3DCharacter(targetPixels, app.TextOverlay.MetricsLetter, app.TextOverlay.MetricsMap, asciiCode, position, app.TextOverlay.LightVector0, matrixView, viewGeometries, app.TextOverlay.DrawCache, depthBuffer, app.Framework.FrameInfo.Seconds.Total, color);
			++colorIndex;
		}
	}

	{
		::std::lock_guard			lock					(app.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::std::lock_guard			lock					(app.LockRender);
		app.Offscreen			= target;
	}

	return 0;
}
