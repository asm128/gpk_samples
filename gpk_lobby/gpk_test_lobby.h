
#include "gpk_framework.h"
#include "gpk_gui.h"

#include "gpk_lobby.h"

#include <mutex>

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework				Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen							= {};
		::gpk::SLobbyServer				LobbyServer;
		::gpk::cid_t					IdExit								= -1;

		::std::mutex					LockGUI;
		::std::mutex					LockRender;

										SApplication						(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_23627

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


