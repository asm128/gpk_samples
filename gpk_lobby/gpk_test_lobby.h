
#include "gpk_framework.h"
#include "gpk_gui.h"

#include "gpk_lobby.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework													Framework;
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen							= {};
		::gpk::SLobbyServer													LobbyServer;
		int32_t																IdExit								= -1;

		::std::mutex														LockGUI;
		::std::mutex														LockRender;



																			SApplication						(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734

//// Protocol
// 
// 1. Client - Connect 
// 2. Server - Connected
// 3. Client - Credentials
// 4. Server - Authorize
// 5. Client - Lobby
// 6. Server - Game list - Player list
// 7. Client - Create/Join
// 8. Server - Room Id


