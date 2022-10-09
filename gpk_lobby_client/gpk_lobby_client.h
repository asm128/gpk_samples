#include "gpk_lobby.h"
#include "gpk_framework.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{


	struct SApplication {
		::gpk::SFramework								Framework;
		::gpk::ptr_obj<::gpk::SFramework::TOffscreen>	Offscreen			= {};

		int32_t											IdExit				= -1;

		::std::mutex									LockGUI;
		::std::mutex									LockRender;

		::gpk::SLobbyClient								LobbyClient;

														SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
