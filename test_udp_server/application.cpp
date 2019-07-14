#include "application.h"
#include "gpk_bitmap_file.h"
#include "gpk_tcpip.h"
#include "gpk_parse.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t											cleanup						(::gme::SApplication & app)						{
	::gpk::serverStop(app.Server);
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::gpk::tcpipShutdown();
	return 0;
}

			::gpk::error_t											setup						(::gme::SApplication & app)						{
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SDisplay															& mainWindow				= framework.MainDisplay;
	framework.Input.create();
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
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
	::gpk::tcpipInitialize();
	uint64_t																port						= 9998;
	{ // load port from config file
		::gpk::view_const_string												jsonPort					= {};
		const ::gpk::SJSONReader												& jsonReader						= framework.JSONConfig.Reader;
		gwarn_if(errored(::gpk::jsonExpressionResolve("application.test_udp_server.listen_port"	, jsonReader, 0, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
		else {
			::gpk::parseIntegerDecimal(jsonPort, &port);
			info_printf("Remote port: %u.", (uint32_t)port);
		}
	}
	uint64_t																adapter						= 0;
	{ // load port from config file
		::gpk::view_const_string												jsonPort					= {};
		const ::gpk::SJSONReader												& jsonReader						= framework.JSONConfig.Reader;
		gwarn_if(errored(::gpk::jsonExpressionResolve("application.test_udp_server.adapter"	, jsonReader, 0, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
		else {
			::gpk::parseIntegerDecimal(jsonPort, &adapter);
			info_printf("Adapter: %u.", (uint32_t)adapter);
		}
	}
	::gpk::serverStart(app.Server, (uint16_t)port, (uint16_t)adapter);
	return 0;
}

			::gpk::error_t											update						(::gme::SApplication & app, bool exitSignal)	{
	::gpk::STimer															timer;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
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
	{
		::gpk::mutex_guard														lock						(app.Server.Mutex);
		for(uint32_t iClient = 0; iClient < app.Server.Clients.size(); ++iClient) {
			::gpk::ptr_nco<::gpk::SUDPConnection>									client						= app.Server.Clients[iClient];
			{
				::gpk::mutex_guard														lockRecv					(client->Queue.MutexReceive);
				for(uint32_t iMessage = 0; iMessage < client->Queue.Received.size(); ++iMessage) {
					::gpk::view_const_byte													viewPayload					= client->Queue.Received[iMessage]->Payload;
					info_printf("Client %i received: %s.", iClient, viewPayload.begin());
				}
				client->Queue.Received.clear();
			}
			if(client->State != ::gpk::UDP_CONNECTION_STATE_IDLE || 0 == client->KeyPing)
				continue;
			::gpk::connectionPushData(*client, client->Queue, "Message arrived!"	, true, true);
			::gpk::connectionPushData(*client, client->Queue, "Message arrived! 2", false, true);
			::gpk::connectionPushData(*client, client->Queue, "Message arrived! 3", true, false);
			::gpk::connectionPushData(*client, client->Queue, "Message arrived! 4", false, false);
			::gpk::sleep(10);

			::gpk::connectionPushData(*client, client->Queue, "Message arrived! x1", true, true);
			::gpk::connectionPushData(*client, client->Queue, "Message arrived! x2", false, true);
			::gpk::connectionPushData(*client, client->Queue, "Message arrived! x3", true, false);
			::gpk::connectionPushData(*client, client->Queue, "Message arrived! x4", false, false);
		}
	}

	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}


			::gpk::error_t											draw					(::gme::SApplication & app)						{
	::gpk::STimer															timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, {0xFF, 0x40, 0x7F, 0xFF}, (uint32_t)-1);
	{
		::gpk::mutex_guard														lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
