// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "test_udp_client.h"
#include "gpk_sun.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

#include <DirectXColors.h>

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "UDP Client Test");

static	::gpk::error_t	loadNetworkConfig	(const ::gpk::SJSONReader & jsonConfig, ::gpk::vcsc_t & remote_ip, ::gpk::vcsc_t & remote_port) {
	::gpk::error_t				appNodeIndex;
	if_fail_fe(appNodeIndex = ::gpk::jsonExpressionResolve(::gpk::vcs{"application.test_udp_client"}, jsonConfig, 0));
	return ::gpk::loadClientConfig(jsonConfig, appNodeIndex, remote_ip, remote_port);
}

// --- Cleanup application resources.
::gpk::error_t			cleanup				(::SApplication & app)											{
	::gpk::SFramework			& framework			= app.Framework;
	::gpk::SWindow				& mainWindow		= framework.RootWindow;

	if_fail_w(::gpk::clientDisconnect(app.Client.UDP));
	if_fail_w(::gpk::mainWindowDestroy(mainWindow));
	if_fail_w(::gpk::tcpipShutdown());
	return 0;
}

static	::gpk::error_t	updateSizeDependentResources(::SApplication & app)											{
	::gpk::SWindow				& mainWindow		= app.Framework.RootWindow;
	const ::gpk::n2u1_t			newSize				= mainWindow.Size;
	if_fail_fe(mainWindow.BackBuffer->resize(newSize, ::gpk::bgra{0, 0, 0, 0}, 0xFFFFFFFF));
	mainWindow.Resized		= false;
	return 0;
}

static	::gpk::error_t	processScreenEvent	(::SApplication & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Create:
	case ::gpk::EVENT_SCREEN_Resize: 
		if_fail_fe(::updateSizeDependentResources(app));
		break;
	}
	return 0;
}

static	::gpk::error_t	processSystemEvent	(::SApplication & app, const ::gpk::SEventSystem & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN		>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_SCREEN		> & screenEvent) { return processScreenEvent(app, screenEvent); });
	case ::gpk::SYSTEM_EVENT_GUI	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_GUI_CONTROL	>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL	> & screenEvent) { return processGUIEvent	(app.Client, *app.Framework.GUI, screenEvent); });
	}
	return 0;
}

::gpk::error_t			setup				(::SApplication& app)											{
	::gpk::SFramework			& framework			= app.Framework;
	::gpk::SWindow				& mainWindow		= framework.RootWindow;
	mainWindow.Size			= {1280, 720};
	if_fail_fe(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input));	// create platform widnow

	::gpk::SGUI					& gui				= *framework.GUI;
	if_fail_fe(::gpk::setupGUI(app.Client.UI, gui));

	if_fail_fe(::gpk::tcpipInitialize());
	if_fail_w(::loadNetworkConfig(framework.JSONConfig.Reader, app.Client.RemoteIp, app.Client.RemotePort));
	if_fail_e(::gpk::clientConnect(app.Client, *framework.GUI));	// Preemptively try to connect to the gate server. 

	return 0;
}

::gpk::error_t			update				(::SApplication & app, bool systemRequestedExit)					{
	::gpk::SFramework			& framework			= app.Framework;
	::gpk::SWindow				& mainWindow		= app.Framework.RootWindow;
	::gpk::SGUI					& gui				= *framework.GUI;

	bool						systemExit			= false;

	gpk::TQueueSystemEvent		eventsToProcess		= mainWindow.EventQueue;
	if_fail_fe(eventsToProcess.append(gui.Controls.EventQueue));
	if_fail_fe(eventsToProcess.for_each([&app, &systemExit](const ::gpk::pobj<::gpk::SEventSystem> & sysEvent) { 
		::gpk::error_t				result; 
		if_fail_fe(result = ::processSystemEvent(app, *sysEvent)); 
		if(result == 1) 
			systemExit				= true; 
		return result;
	}));

	if_true_vif(::gpk::APPLICATION_STATE_EXIT, systemExit || systemRequestedExit, "%s || %s", ::gpk::bool2char(systemExit) || ::gpk::bool2char(systemRequestedExit));

	::gpk::pau8					payloadCache;
	if_fail_e(eventsToProcess.for_each([&app, &payloadCache](::gpk::pobj<::gpk::SEventSystem> & ev){
		payloadCache.create();
		if_fail_fe(ev->Save(*payloadCache));
		app.Client.QueueToSend.push_back(payloadCache);
 		return 0;
	}));

	int32_t						clientResult;
	if_fail_fe(clientResult = ::gpk::clientUpdate(app.Client, gui));
	rvi_if(::gpk::APPLICATION_STATE_EXIT, clientResult > 0, "User requested close (%i). Terminating execution.", clientResult);

	if_fail_e(app.Client.QueueReceived.for_each([&app](::gpk::pobj<::gpk::SUDPMessage> & udp){ 
		if(udp && udp->Payload.size()) {
			::gpk::pobj<::gpk::SEventSystem>	eventReceived;
			::gpk::vcu0_t							inputBytes			= udp->Payload;
			if_fail_fe(eventReceived->Load(inputBytes)); 
		}
		return 0;
	}));

	//-----------------------------
	::gpk::STimer				& timer					= app.Framework.Timer;
	char						buffer	[256]			= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND						windowHandle			= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	
	return ::gpk::updateFramework(framework);
}

::gpk::error_t			draw					(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::gpk::n3f2_t			sunlightPos				= ::gpk::calcSunPosition();
	const double				sunlightFactor			= ::gpk::calcSunlightFactor();
	const ::gpk::rgbaf			clearColor				= ::gpk::interpolate_linear(::gpk::DARKBLUE * .25, ::gpk::LIGHTBLUE * 1.1, sunlightFactor);

	::gpk::SFramework			& framework				= app.Framework;
	::gpk::prtbgra8d32			backBuffer				= framework.RootWindow.BackBuffer;
	if_fail_fe(backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), clearColor, (uint32_t)-1));
	if_fail_fe(::gpk::guiDraw(*framework.GUI, backBuffer->Color));
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer	= backBuffer;
	return 0;
}
