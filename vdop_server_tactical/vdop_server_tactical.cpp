#include "vdop_server_tactical.h"
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
	mainWindow.Size														= {1280, 720};
	es_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	{ // Build the exit button
		::gpk::SGUI						& gui								= *framework.GUI;
		gui.ColorModeDefault		= ::gpk::GUI_COLOR_MODE_3D;
		gui.ThemeDefault			= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;

		app.IdExit					= ::gpk::controlCreate(gui);

		::gpk::SControlPlacement					& controlExit						= gui.Controls.Placement[app.IdExit];
		controlExit.Area			= {{}, {64, 20}};
		controlExit.Border			= {10, 10, 10, 10};
		controlExit.Margin			= {1, 1, 1, 1};
		controlExit.Align			= ::gpk::ALIGN_BOTTOM_RIGHT;

		::gpk::SControlText				& controlText						= gui.Controls.Text[app.IdExit];
		controlText.Text			= "Exit";
		controlText.Align			= ::gpk::ALIGN_CENTER;

		::gpk::SControlConstraints		& controlConstraints				= gui.Controls.Constraints[app.IdExit];
		controlConstraints.AttachSizeToControl	= {app.IdExit, -1};
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}
	srand((uint32_t)time(0));

	const ::gpk::n2u32							metricsMap						= app.TextOverlay.MetricsMap;
	const ::gpk::n2u32							metricsLetter					= app.TextOverlay.MetricsLetter;
	::gpk::img8bgra								fontImage;
	::gpk::pngFileLoad(::gpk::vcs{"../gpk_data/images/Codepage_437_24_12x12.png"}, fontImage);
	::gpk::grid<::gpk::SGeometryQuads>		viewGeometries					= {app.TextOverlay.GeometryLetters, {16, 16}};
	const uint32_t								imagePitch						= metricsLetter.x * metricsMap.x;

	::gpk::apod<::gpk::STile>					tiles;
	for(uint32_t y = 0; y < metricsMap.y; ++y)
	for(uint32_t x = 0; x < metricsMap.x; ++x) {
		tiles.clear();
		const uint32_t								asciiCode			= y * app.TextOverlay.MetricsMap.x + x;
		const ::gpk::n2u32							asciiCoords			= {asciiCode %		metricsMap.x, asciiCode / app.TextOverlay.MetricsMap.x};
		const uint32_t								offsetPixelCoord	= (asciiCoords.y *	metricsLetter.y) * imagePitch + (asciiCoords.x * app.TextOverlay.MetricsLetter.x);
		::gpk::geometryBuildTileListFromImage({&fontImage.Texels[offsetPixelCoord], app.TextOverlay.MetricsLetter}, tiles, app.TextOverlay.MetricsLetter.x * app.TextOverlay.MetricsMap.x);
		::gpk::geometryBuildGridFromTileList(app.TextOverlay.GeometryLetters[asciiCode], {tiles.begin(), app.TextOverlay.MetricsLetter}, {}, {1, 6.0f, 1});
	}


	stacxpr uint32_t		DEFAULT_ASCII_DISPLAY_HEIGHT	= 70;
	stacxpr uint32_t		DEFAULT_ASCII_DISPLAY_WIDTH		= (uint32_t)(DEFAULT_ASCII_DISPLAY_HEIGHT * 2.666666f);
	::klib::initASCIIScreen(DEFAULT_ASCII_DISPLAY_WIDTH, DEFAULT_ASCII_DISPLAY_HEIGHT);

	app.Game.push_back({});
	::gpk::pobj<::klib::SGame>							& instanceGame		= app.Game[0];
	instanceGame->GlobalDisplay		.Resize({DEFAULT_ASCII_DISPLAY_WIDTH, DEFAULT_ASCII_DISPLAY_HEIGHT});
	instanceGame->TacticalDisplay	.Resize({::klib::GAME_MAP_WIDTH, ::klib::GAME_MAP_DEPTH});

	::klib::initGame(*instanceGame);
	::gpk::tcpipInitialize();
	::gpk::serverStart(app.TacticalServer, 51515, 0);
	return 0;
}

::gpk::error_t			update		(SApplication & app, bool exitSignal)	{
	::gpk::SFramework				& framework	= app.Framework;
	//::gpk::STimer															timer;
	(void)exitSignal;
	//rvis_if(::gpk::APPLICATION_STATE_EXIT, exitSignal);
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
		::gpk::m4<float>					& matrixProjection	= app.TextOverlay.MatrixProjection;
		matrixProjection.FieldOfView(::gpk::math_pi * .25, framework.RootWindow.Size.x / (double)framework.RootWindow.Size.y, 0.01, 500.0);
		::gpk::m4<float>					matrixViewport		= {};
		matrixViewport.ViewportLH(framework.RootWindow.Size.Cast<uint16_t>());
		matrixProjection				*= matrixViewport;
	}
	::klib::SASCIITarget				target;
	::klib::getASCIIBackBuffer(target);
	::klib::clearASCIIBackBuffer(' ', ::klib::ASCII_COLOR_INDEX_WHITE);
	if(app.Game.size()) {
		::klib::pollInput(app.Game[0]->FrameInput);
 		::klib::drawAndPresentGame(*app.Game[0], target);
	}
	::klib::presentASCIIBackBuffer();

	{
		::std::lock_guard					lock						(app.TacticalServer.Mutex);
		app.MessagesToProcess.resize(app.TacticalServer.Clients.size());
		for(uint32_t iClient = 0, countClients = app.TacticalServer.Clients.size(); iClient < countClients; ++iClient) {
			::gpk::pnco<::gpk::SUDPConnection>	client						= app.TacticalServer.Clients[iClient];
			if(client->State != ::gpk::UDP_CONNECTION_STATE_IDLE || 0 == client->KeyPing)
				continue;
			{
				::std::lock_guard														lockRecv					(client->Queue.MutexReceive);
				for(int32_t iMessage = 0; iMessage < (int32_t)client->Queue.Received.size(); ++iMessage) {
					if(client->Queue.Received[iMessage]->Command.Type == ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE)
						continue;
					::gpk::pobj<::gpk::SUDPMessage>							messageReceived				= client->Queue.Received[iMessage];
					gpk_necall(app.MessagesToProcess[iClient].push_back(messageReceived), "%s", "Out of memory?");
					client->Queue.Received.remove_unordered(iMessage--);
				}
			}

		}
		::gpk::sleep(10);
	}

	static	uint32_t							currentMessage;
	char										messageToSend	[256]		= {};
	for(uint32_t iClient = 0; iClient < app.MessagesToProcess.size(); ++iClient) {
		const ::gpk::apobj<::gpk::SUDPMessage>		& clientQueue				= app.MessagesToProcess[iClient];
		for(uint32_t iMessage = 0; iMessage < clientQueue.size(); ++iMessage) {
			::gpk::pobj<::gpk::SUDPMessage>				messageReceived				= clientQueue[iMessage];
			::gpk::vcu8									viewPayload					= messageReceived->Payload;
			info_printf("Server connection %i received: %s.", iClient, viewPayload.begin());
			{
				::std::lock_guard							lock						(app.TacticalServer.Mutex);
				::gpk::pnco<::gpk::SUDPConnection>			client						= app.TacticalServer.Clients[iClient];
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

int													draw					(SApplication & app) {
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::DARKGREEN, 0xFFFFFFFFU);

	{
		::std::lock_guard														lock					(app.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::std::lock_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}

	return 0;
}
