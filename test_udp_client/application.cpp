#include "application.h"
#include "gpk_bitmap_file.h"
#include "gpk_parse.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"
#include "gpk_tcpip.h"
#include "gpk_chrono.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t											cleanup					(::gme::SApplication & app)							{
	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	::gpk::clientDisconnect(app.Client);
	::gpk::tcpipShutdown();
	return 0;
}
			::gpk::error_t											setup						(::gme::SApplication & app)						{
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SWindow															& mainWindow				= framework.RootWindow;
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");
	::gpk::SGUI																& gui						= *framework.GUI;
	app.IdExit															= ::gpk::controlCreate(gui);
	::gpk::SControl															& controlExit				= gui.Controls.Controls[app.IdExit];
	controlExit.Area													= {{0, 0}, {64, 20}};
	controlExit.Border													= {1, 1, 1, 1};
	controlExit.Margin													= {1, 1, 1, 1};
	controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
	::gpk::SControlText														& controlText				= gui.Controls.Text[app.IdExit];
	controlText.Text													= "Exit";
	controlText.Align													= ::gpk::ALIGN_CENTER;
	::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdExit];
	controlConstraints.AttachSizeToText.y								= app.IdExit;
	controlConstraints.AttachSizeToText.x								= app.IdExit;
	::gpk::controlSetParent(gui, app.IdExit, -1);
	gpk_necall(::gpk::tcpipInitialize(), "Failed to initialize network subsystem: '%s'.", "Unknown error");

	app.Client.AddressConnect											= {};
	::gpk::tcpipAddress(9998, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, app.Client.AddressConnect);	// If loading the remote IP from the json fails, we fall back to the local address.
	{ // attempt to load address from config file.
		const ::gpk::SJSONReader												& jsonReader						= framework.JSONConfig.Reader;
		{ //
			::gpk::view_const_string												jsonIP								= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve(::gpk::vcs{"application.test_udp_client.remote_ip"}, jsonReader, 0, jsonIP)), "Failed to load config from json! Last contents found: %s.", jsonIP.begin())
			else {
				info_printf("Remote IP: %s.", jsonIP.begin());
				gerror_if(errored(::gpk::tcpipAddress(jsonIP, {}, app.Client.AddressConnect)), "Failed to read IP address from JSON config file: %s.", jsonIP.begin());	// turn the string into a SIPv4 struct.
			}
		}
		{ // load port from config file
			::gpk::view_const_string												jsonPort							= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve(::gpk::vcs{"application.test_udp_client.remote_port"}, jsonReader, 0, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin())
			else {
				uint64_t																port;
				::gpk::parseIntegerDecimal(jsonPort, &port);
				app.Client.AddressConnect.Port										= (uint16_t)port;
				info_printf("Remote port: %u.", (uint32_t)port);
			}
		}
	} 
	::gpk::clientConnect(app.Client);
	return 0;
}
			::gpk::error_t											update						(::gme::SApplication & app, bool exitSignal)	{
	::gpk::STimer															timer;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer									= app.Offscreen;
	}
	::gpk::SFramework														& framework					= app.Framework;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");

	::gpk::SGUI																& gui						= *framework.GUI;
	::gpk::array_pod<uint32_t>												controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		uint32_t																idControl					= controlsToProcess[iControl];
		const ::gpk::SControlState												& controlState				= gui.Controls.States[idControl];
		if(controlState.Execute) {
			info_printf("Executed %u.", idControl);
			if(idControl == (uint32_t)app.IdExit)
				return 1;
		}
	}

	//static bool bSend = true;
	reterr_gerror_if(app.Client.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "Failed to connect to server.")
	else {
		{
			::gpk::mutex_guard														lockRecv					(app.Client.Queue.MutexReceive);
			for(uint32_t iMessage = 0; iMessage < app.Client.Queue.Received.size(); ++iMessage) {
				//gpk_necall(app.MessagesToProcess.push_back(client->Queue.Received[iMessage]), "%s", "Out of memory?");
				::gpk::view_const_byte													viewPayload					= app.Client.Queue.Received[iMessage]->Payload;
				info_printf("Client received: %s.", viewPayload.begin());
			}
			app.Client.Queue.Received.clear();
		}

		//if(bSend) {
			::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived!", true, true	  , 32);
		//	bSend = false;
		//}
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! 2", false, true , 0);
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! 3", true, false , 0);
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! 4", false, false, 0);
		::gpk::clientUpdate(app.Client);
		::gpk::sleep(10);
		//
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! x1", true, true, 4);
		::gpk::clientUpdate(app.Client);
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! x2", false, true, 4);
		::gpk::clientUpdate(app.Client);
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! x3", true, false, 4);
		::gpk::clientUpdate(app.Client);
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! x4", false, false, 4);
		::gpk::clientUpdate(app.Client);

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

			::gpk::error_t												draw					(::gme::SApplication & app)						{
	::gpk::STimer																timer;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, {0xFF, 0x40, 0x7F, 0xFF}, (uint32_t)-1);
	//::gpk::clearTarget(*target);
	{
		::gpk::mutex_guard															lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard															lock					(app.LockRender);
		app.Offscreen															= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
