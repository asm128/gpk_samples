#include "gpk_lobby_client.h"
#include "gpk_bitmap_file.h"
#include "gpk_parse.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"
#include "gpk_tcpip.h"
#include "gpk_chrono.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

::gpk::error_t			cleanup		(::gme::SApplication & app)							{
	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	::gpk::clientDisconnect(app.LobbyClient.Client);
	::gpk::tcpipShutdown();
	::gpk::sleep(100);
	return 0;
}
::gpk::error_t			setup		(::gme::SApplication & app)						{
	::gpk::SFramework				& framework					= app.Framework;
	::gpk::SWindow					& mainWindow				= framework.RootWindow;
	es_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	::gpk::SGUI						& gui						= *framework.GUI;
	app.IdExit					= ::gpk::controlCreate(gui);
	::gpk::SControlPlacement					& controlExit				= gui.Controls.Placement[app.IdExit];
	controlExit.Area			= {{0, 0}, {64, 20}};
	controlExit.Border			= {1, 1, 1, 1};
	controlExit.Margin			= {1, 1, 1, 1};
	controlExit.Align			= ::gpk::ALIGN_BOTTOM_RIGHT;
	::gpk::SControlText				& controlText				= gui.Controls.Text[app.IdExit];
	controlText.Text			= "Exit";
	controlText.Align			= ::gpk::ALIGN_CENTER;
	::gpk::SControlConstraints		& controlConstraints		= gui.Controls.Constraints[app.IdExit];
	controlConstraints.AttachSizeToText.y	= app.IdExit;
	controlConstraints.AttachSizeToText.x	= app.IdExit;
	::gpk::controlSetParent(gui, app.IdExit, -1);
	gpk_necall(::gpk::tcpipInitialize(), "Failed to initialize network subsystem: '%s'.", "Unknown error");

	app.LobbyClient.Client.AddressConnect	= {};
	::gpk::tcpipAddress(9998, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, app.LobbyClient.Client.AddressConnect);	// If loading the remote IP from the json fails, we fall back to the local address.
	{ // attempt to load address from config file.
		const ::gpk::SJSONReader		& jsonReader				= framework.JSONConfig.Reader;
		{ //
			::gpk::vcs						jsonIP						= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve(::gpk::vcs{"application.test_udp_client.remote_ip"}, jsonReader, 0, jsonIP)), "Failed to load config from json! Last contents found: %s.", jsonIP.begin())
			else {
				info_printf("Remote IP: %s.", jsonIP.begin());
				e_if(errored(::gpk::tcpipAddress(jsonIP, {}, app.LobbyClient.Client.AddressConnect)), "Failed to read IP address from JSON config file: %s.", jsonIP.begin());	// turn the string into a SIPv4 struct.
			}
		}
		{ // load port from config file
			::gpk::vcs						jsonPort					= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve(::gpk::vcs{"application.test_udp_client.remote_port"}, jsonReader, 0, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin())
			else {
				uint64_t						port;
				::gpk::parseIntegerDecimal(jsonPort, port);
				app.LobbyClient.Client.AddressConnect.Port	= (uint16_t)port;
				info_printf("Remote port: %u.", (uint32_t)port);
			}
		}
	} 
	//::gpk::clientConnect(app.LobbyClient.Client);
	return 0;
}
::gpk::error_t			update		(::gme::SApplication & app, bool exitSignal)	{
	::gpk::STimer					timer;
	rvis_if(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard				lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	::gpk::SFramework				& framework					= app.Framework;
	app.LobbyClient.Update(framework.RootWindow.Input, framework.RootWindow.EventQueue);
	rvis_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui			= *framework.GUI;
	::gpk::acid					toProcess		= {};
	if(1 == ::gpk::guiProcessControls(gui, [&app](::gpk::cid_t iControl) { return one_if(iControl == app.IdExit); }))
		return 1;

	//static bool bSend = true;
	//reterr_gerror_if(app.LobbyClient.Client.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "Failed to connect to server.")
	//else 
	if(false)
	{
		//if(bSend) {
			::gpk::connectionPushData(app.LobbyClient.Client, app.LobbyClient.Client.Queue, "Message arrived!", true, true, 32);
		//	bSend = false;
		//}
		::gpk::connectionPushData(app.LobbyClient.Client, app.LobbyClient.Client.Queue, "Message arrived! 2", false, true, 0);
		::gpk::connectionPushData(app.LobbyClient.Client, app.LobbyClient.Client.Queue, "Message arrived! 3", true, false, 0);
		::gpk::connectionPushData(app.LobbyClient.Client, app.LobbyClient.Client.Queue, "Message arrived! 4", false, false, 0);
		::gpk::clientUpdate(app.LobbyClient.Client);
		::gpk::sleep(1);
		//
		::gpk::connectionPushData(app.LobbyClient.Client, app.LobbyClient.Client.Queue, "Message arrived! x1", true, true, 4);
		::gpk::clientUpdate(app.LobbyClient.Client);
		::gpk::sleep(1);
		::gpk::connectionPushData(app.LobbyClient.Client, app.LobbyClient.Client.Queue, "Message arrived! x2", false, true, 4);
		::gpk::clientUpdate(app.LobbyClient.Client);
		::gpk::sleep(1);
		::gpk::connectionPushData(app.LobbyClient.Client, app.LobbyClient.Client.Queue, "Message arrived! x3", true, false, 4);
		::gpk::clientUpdate(app.LobbyClient.Client);
		::gpk::sleep(1);
		::gpk::connectionPushData(app.LobbyClient.Client, app.LobbyClient.Client.Queue, "Message arrived! x4", false, false, 4);
		::gpk::clientUpdate(app.LobbyClient.Client); 
		::gpk::sleep(1);

	}
	//::gpk::sleep(1000);
	//::gpk::SUDPClient															& connectTest				= app.ClientTest1;
	//connectTest.AddressConnect												= app.Client.AddressConnect;
	//::gpk::clientConnect		(connectTest);
	//::gpk::connectionPushData	(connectTest, connectTest.Queue, "Connect test!");
	//::gpk::clientUpdate			(connectTest);
	//::gpk::clientDisconnect		(connectTest);

	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

::gpk::error_t			draw		(::gme::SApplication & app)						{
	::gpk::STimer					timer;
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, {0xFF, 0x20, 0x6F, 0xCF}, (uint32_t)-1);
	app.LobbyClient.Draw(target->Color);
	//::gpk::clearTarget(*target);
	{
		::std::lock_guard				lock		(app.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::std::lock_guard				lock		(app.LockRender);
		app.Offscreen				= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
