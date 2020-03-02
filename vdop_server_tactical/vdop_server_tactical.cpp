#include "vdop_server_tactical.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"

#include <time.h>

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "VDoP Server");

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
	srand((uint32_t)time(0));

	const ::gpk::SCoord2<uint32_t>											metricsMap			= app.TextOverlay.MetricsMap;
	const ::gpk::SCoord2<uint32_t>											metricsLetter		= app.TextOverlay.MetricsLetter;
	::gpk::SImage<::gpk::SColorBGRA>										fontImage;
	::gpk::pngFileLoad(::gpk::view_const_string{"../gpk_data/images/Codepage_437_24_12x12.png"}, fontImage);
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


#define DEFAULT_ASCII_DISPLAY_HEIGHT	70
#define DEFAULT_ASCII_DISPLAY_WIDTH		((uint32_t)(DEFAULT_ASCII_DISPLAY_HEIGHT * 2.666666f))
	::klib::initASCIIScreen(DEFAULT_ASCII_DISPLAY_WIDTH, DEFAULT_ASCII_DISPLAY_HEIGHT);

	::klib::SGame												& instanceGame		= app.Game;
	instanceGame.GlobalDisplay	.Resize({DEFAULT_ASCII_DISPLAY_WIDTH, DEFAULT_ASCII_DISPLAY_HEIGHT});
	instanceGame.TacticalDisplay.Resize({::klib::GAME_MAP_WIDTH, ::klib::GAME_MAP_DEPTH});

	::klib::initGame(instanceGame);

	::gpk::tcpipInitialize();
	::gpk::serverStart(app.TacticalServer, 51515, 0);

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

	{
		::gpk::mutex_guard														lock						(app.TacticalServer.Mutex);
		app.MessagesToProcess.resize(app.TacticalServer.Clients.size());
		for(uint32_t iClient = 0, countClients = app.TacticalServer.Clients.size(); iClient < countClients; ++iClient) {
			::gpk::ptr_nco<::gpk::SUDPConnection>									client						= app.TacticalServer.Clients[iClient];
			if(client->State != ::gpk::UDP_CONNECTION_STATE_IDLE || 0 == client->KeyPing)
				continue;
			{
				::gpk::mutex_guard														lockRecv					(client->Queue.MutexReceive);
				for(int32_t iMessage = 0; iMessage < (int32_t)client->Queue.Received.size(); ++iMessage) {
					if(client->Queue.Received[iMessage]->Command.Type == ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE)
						continue;
					::gpk::ptr_obj<::gpk::SUDPConnectionMessage>							messageReceived				= client->Queue.Received[iMessage];
					gpk_necall(app.MessagesToProcess[iClient].push_back(messageReceived), "%s", "Out of memory?");
					client->Queue.Received.remove_unordered(iMessage--);
				}
			}

		}
		::gpk::sleep(10);
	}

	static	uint32_t														currentMessage;
	char																	messageToSend	[256]		= {};
	for(uint32_t iClient = 0; iClient < app.MessagesToProcess.size(); ++iClient) {
		const ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	& clientQueue				= app.MessagesToProcess[iClient];
		for(uint32_t iMessage = 0; iMessage < clientQueue.size(); ++iMessage) {
			::gpk::ptr_obj<::gpk::SUDPConnectionMessage>							messageReceived				= clientQueue[iMessage];
			::gpk::view_const_byte													viewPayload					= messageReceived->Payload;
			info_printf("Client %i received: %s.", iClient, viewPayload.begin());
			{
				::gpk::mutex_guard														lock						(app.TacticalServer.Mutex);
				::gpk::ptr_nco<::gpk::SUDPConnection>									client						= app.TacticalServer.Clients[iClient];
				if(client->State != ::gpk::UDP_CONNECTION_STATE_IDLE)
					continue;
				sprintf_s(messageToSend, "Message arrived(true, true    ): %u", currentMessage++); ::gpk::connectionPushData(*client, client->Queue, ::gpk::view_const_string{messageToSend}, true, true	, 10);
				sprintf_s(messageToSend, "Message arrived(false, true   ): %u", currentMessage++); ::gpk::connectionPushData(*client, client->Queue, ::gpk::view_const_string{messageToSend}, false, true	, 10);
				sprintf_s(messageToSend, "Message arrived(true, false   ): %u", currentMessage++); ::gpk::connectionPushData(*client, client->Queue, ::gpk::view_const_string{messageToSend}, true, false	, 10);
				sprintf_s(messageToSend, "Message arrived(false, false  ): %u", currentMessage++); ::gpk::connectionPushData(*client, client->Queue, ::gpk::view_const_string{messageToSend}, false, false	, 10);

				sprintf_s(messageToSend, "Message arrived(true, true	) x: %u", currentMessage++); ::gpk::connectionPushData(*client, client->Queue, ::gpk::view_const_string{messageToSend}, true, true	, 10);
				sprintf_s(messageToSend, "Message arrived(false, true	) x: %u", currentMessage++); ::gpk::connectionPushData(*client, client->Queue, ::gpk::view_const_string{messageToSend}, false, true	, 10);
				sprintf_s(messageToSend, "Message arrived(true, false	) x: %u", currentMessage++); ::gpk::connectionPushData(*client, client->Queue, ::gpk::view_const_string{messageToSend}, true, false	, 10);
				sprintf_s(messageToSend, "Message arrived(false, false	) x: %u", currentMessage++); ::gpk::connectionPushData(*client, client->Queue, ::gpk::view_const_string{messageToSend}, false, false, 10);
			}
		}
	}
	app.MessagesToProcess.clear();


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
		::gpk::setPixel(targetPixels, pixelCoord, (targetPixels[pixelCoord.Cast<uint32_t>()] * .25) + (texelColor * (lightFactorDirectional * 2) + texelColor * factorWave + texelColor * factorWave2));
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
	::gpk::SCoord3<float>									translation				= {};
	translation.x										= float(position.x * metricsCharacter.x);
	translation.z										= float(position.z * metricsCharacter.y);
	if(asciiCode == 0x05) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0x0B) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0x0C) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0xE8) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0xAB) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	if(asciiCode == 0xAC) { matrixRotation.RotationX(-::gpk::math_pi_2); translation.y += metricsCharacter.y / 2; }
	matrixPosition	.SetTranslation	(translation, true);
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
	//::gpk::view_grid<::gpk::SColorBGRA>						targetPixels			= target->Color;
	//::gpk::view_grid<uint32_t>								depthBuffer				= target->DepthStencil;
	//app.TextOverlay.DrawCache							= {};

	//app.TextOverlay.LightVector0.Normalize();
	//::gpk::view_grid<::gpk::SGeometryQuads>					viewGeometries		= {app.TextOverlay.GeometryLetters, {16, 16}};
	//uint32_t												colorIndex			= 0;

	//::gpk::SMatrix4<float>									matrixView					= {};
	//::gpk::SMatrix4<float>									matrixProjection			= {};
	//::gpk::SMatrix4<float>									matrixViewport				= {};
	//matrixView		.Identity();
	//matrixProjection.Identity();
	//matrixViewport	.Identity();

	//matrixProjection.FieldOfView(::gpk::math_pi * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 5000);
	//matrixViewport.ViewportLH(targetPixels.metrics());
	//matrixView											*= matrixProjection;
	//matrixView											*= matrixViewport;

	//{
	//	::gpk::mutex_guard										lock						(app.LockRender);
	//	::gpk::view_grid<char>									mapToDraw					= app.Game.GlobalDisplay.Screen;
	//	::gpk::view_grid<uint16_t>								mapColors					= app.Game.GlobalDisplay.TextAttributes;
	//	matrixView.LookAt(app.TextOverlay.CameraPosition, app.TextOverlay.CameraTarget, app.TextOverlay.CameraUp);
	//	matrixView											*= matrixProjection;
	//	matrixView											*= matrixViewport;
	//	::gpk::SCoord3<float>									offset			= {};//app.TextOverlay.ControlTranslation;
	//	offset												-= ::gpk::SCoord3<float>{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
	//	for(uint32_t y = 0; y < mapToDraw.metrics().y; ++y)
	//	for(uint32_t x = 0; x < mapToDraw.metrics().x; ++x) {
	//		const uint8_t											asciiCode			= mapToDraw[y][x];
	//		if(0 == asciiCode)
	//			continue;
	//		const uint16_t											asciiColor			= mapColors[y][x];
	//		::gpk::SColorFloat										color					= app.Framework.GUI->Palette[(asciiColor & 0xF)];//::gpk::COLOR_TABLE[((int)timeAnimation) % ::gpk::size(::gpk::COLOR_TABLE)];
	//		::gpk::SCoord3<float>									position			= offset;
	//		position.x											+= x;
	//		position.z											-= y;
	//		draw3DCharacter(targetPixels, app.TextOverlay.MetricsLetter, app.TextOverlay.MetricsMap, asciiCode, position, app.TextOverlay.LightVector0, matrixView, viewGeometries, app.TextOverlay.DrawCache, depthBuffer, app.Framework.FrameInfo.Seconds.Total, color);
	//		++colorIndex;
	//	}
	//}

	//{
	//	::gpk::mutex_guard										lock						(app.LockRender);
	//	::gpk::view_grid<char>									mapToDraw					= app.Game.TacticalDisplay.Screen;
	//	::gpk::view_grid<uint16_t>								mapColors					= app.Game.TacticalDisplay.TextAttributes;

	//	if((app.Game.State.State != ::klib::GAME_STATE_START_MISSION && app.Game.State.State != ::klib::GAME_STATE_TACTICAL_CONTROL) || 0 > app.Game.TacticalInfo.CurrentPlayer)
	//		matrixView.LookAt(app.TextOverlay.CameraPosition, app.TextOverlay.CameraTarget, app.TextOverlay.CameraUp);
	//	else {
	//		::klib::SPlayer											& player			= app.Game.Players[app.Game.TacticalInfo.Setup.Players[app.Game.TacticalInfo.CurrentPlayer]];
	//		::gpk::SCoord3<float>									agentPosition		= player.Army[player.Squad.Agents[player.Selection.PlayerUnit]]->Position.Cast<float>();
	//		agentPosition.Scale({1, 1, -1});
	//		agentPosition										-= ::gpk::SCoord3<float>{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
	//		::gpk::SCoord3<float>									cameraPosition		= agentPosition;
	//		agentPosition.Scale(12);
	//		cameraPosition										+= {-24, 6, -24};
	//		cameraPosition.Scale(12);
	//		matrixView.LookAt(cameraPosition, agentPosition, {0, 1, 0});//app.TextOverlay.CameraUp);
	//	}
	//	matrixView											*= matrixProjection;
	//	matrixView											*= matrixViewport;

	//	::gpk::SCoord3<float>									offset			= {};//app.TextOverlay.ControlTranslation;
	//	offset												-= ::gpk::SCoord3<float>{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
	//	for(uint32_t y = 0; y < mapToDraw.metrics().y; ++y)
	//	for(uint32_t x = 0; x < mapToDraw.metrics().x; ++x) {
	//		const uint8_t											asciiCode			= mapToDraw[y][x];
	//		if(0 == asciiCode)
	//			continue;
	//		const uint16_t											asciiColor			= mapColors[y][x];
	//		::gpk::SColorFloat										color					= app.Framework.GUI->Palette[(asciiColor & 0xF)];//::gpk::COLOR_TABLE[((int)timeAnimation) % ::gpk::size(::gpk::COLOR_TABLE)];
	//		::gpk::SCoord3<float>									position			= offset;
	//		position.x											+= x;
	//		position.z											-= y;
	//		draw3DCharacter(targetPixels, app.TextOverlay.MetricsLetter, app.TextOverlay.MetricsMap, asciiCode, position, app.TextOverlay.LightVector0, matrixView, viewGeometries, app.TextOverlay.DrawCache, depthBuffer, app.Framework.FrameInfo.Seconds.Total, color);
	//		++colorIndex;
	//	}
	//}

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
