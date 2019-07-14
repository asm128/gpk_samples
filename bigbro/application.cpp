#include "application.h"
#include "gpk_bitmap_file.h"
#include "gpk_tcpip.h"
#include "gpk_find.h"
#include "gpk_process.h"

#include "gpk_parse.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::brt::SApplication, "Module Explorer");

			::gpk::error_t											cleanup						(::brt::SApplication & app)						{
	::gpk::serverStop(app.Server);
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::gpk::tcpipShutdown();
	return 0;
}

			::gpk::error_t											setup						(::brt::SApplication & app)						{
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SDisplay															& mainWindow				= framework.MainDisplay;
	framework.Input.create();
	mainWindow.Size														= {320, 200};
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
	uint64_t																adapter						= 0;
	{ // load port from config file
		::gpk::view_const_string												jsonPort					= {};
		const ::gpk::SJSONReader												& jsonReader						= framework.ReaderJSONConfig;
		const int32_t															indexObjectApp						= ::gpk::jsonExpressionResolve("application.bigbro", jsonReader, 0, jsonPort);
		gwarn_if(errored(indexObjectApp), "Failed to find application node (%s) in json configuration file: '%s'", "application.bigbro", framework.FileNameJSONConfig.begin())
		else {
			jsonPort															= "";
			gwarn_if(errored(::gpk::jsonExpressionResolve("listen_port"						, jsonReader, indexObjectApp, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
			else {
				::gpk::parseIntegerDecimal(jsonPort, &port);
				info_printf("Port to listen on: %u.", (uint32_t)port);
			}
			jsonPort															= "";
			gwarn_if(errored(::gpk::jsonExpressionResolve("adapter"	, jsonReader, indexObjectApp, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
			else {
				::gpk::parseIntegerDecimal(jsonPort, &adapter);
				info_printf("Adapter: %u.", (uint32_t)adapter);
			}
		}
	}
	gpk_necall(::gpk::serverStart(app.Server, (uint16_t)port, (int16_t)adapter), "Failed to start server on port %u. Port busy?", (uint32_t)port);
	return 0;
}

		::gpk::error_t											update						(::brt::SApplication & app, bool exitSignal)	{
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
	::gpk::array_obj<::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>>	& receivedPerClient		= app.ReceivedPerClient;
	{	// pick up messages for later processing
		::gpk::mutex_guard																	lock						(app.Server.Mutex);
		receivedPerClient.resize(app.Server.Clients.size());
		for(uint32_t iClient = 0; iClient < app.Server.Clients.size(); ++iClient) {
			::gpk::ptr_obj<::gpk::SUDPConnection>												conn						= app.Server.Clients[iClient];
			::gpk::mutex_guard																	lockRecv					(conn->Queue.MutexReceive);
			receivedPerClient[iClient]														= app.Server.Clients[iClient]->Queue.Received;
			app.Server.Clients[iClient]->Queue.Received.clear();
		}
	}

	{	// Exectue processes
		for(uint32_t iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			for(uint32_t iMessage = 0; iMessage < receivedPerClient[iClient].size(); ++iMessage) {
				info_printf("Client %i received: %s.", iClient, receivedPerClient[iClient][iMessage]->Payload.begin());	
				::gpk::view_byte										environmentBlock		= receivedPerClient[iClient][iMessage]->Payload;
				// llamar proceso
				::gpk::view_const_byte									payload					= receivedPerClient[iClient][iMessage]->Payload;
				::gpk::error_t											contentOffset			= ::gpk::find_sequence_pod(::gpk::view_const_byte{"\0"}, payload);
				ce_if(errored(contentOffset), "Failed to find environment block stop code.");
				//if(payload.size() && (payload.size() > (uint32_t)contentOffset + 2))
				//	e_if(errored(::writeToPipe(app.ClientIOHandles[iClient], {&payload[contentOffset + 2], payload.size() - contentOffset - 2})), "Failed to write request content to process' stdin.");
			}
		}
	}
	Sleep(10);
	::gpk::array_obj<::gpk::array_obj<::gpk::array_pod<char_t>>>						& clientResponses		= app.ClientResponses;
	clientResponses.resize(receivedPerClient.size());
	{	// Read processes output if they're done processing.
		for(uint32_t iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			clientResponses[iClient].resize(receivedPerClient[iClient].size());
			for(uint32_t iMessage = 0; iMessage < receivedPerClient[iClient].size(); ++iMessage) {
				info_printf("Client %i received: %s.", iClient, receivedPerClient[iClient][iMessage]->Payload.begin());	
			//	// generar respuesta proceso
				clientResponses[iClient][iMessage]		= "\r\n{ \"Respuesta\" : \"bleh\"}";
			//	clientResponses[iClient][iMessage].clear();
			//	::readFromPipe(process, iohandles, clientResponses[iClient][iMessage]);
			}
		}
	}
	for(uint32_t iClient = 0; iClient < clientResponses.size(); ++iClient) {
		for(uint32_t iMessage = 0; iMessage < clientResponses[iClient].size(); ++iMessage) { // contestar 
			if(clientResponses[iClient][iMessage].size()) {
				::gpk::mutex_guard														lock						(app.Server.Mutex);
				::gpk::ptr_obj<::gpk::SUDPConnection>									conn						= app.Server.Clients[iClient];
				::gpk::connectionPushData(*conn, conn->Queue, clientResponses[iClient][iMessage], true, true);
				receivedPerClient[iClient][iMessage]		= {};
			}
		}
	}
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

			::gpk::error_t											draw					(::brt::SApplication & app)						{
	::gpk::STimer															timer;
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
	::gpk::sleep(15);
	return 0;
}
