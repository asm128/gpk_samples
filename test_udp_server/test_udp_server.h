#include "gpk_udp_server.h"

#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_aobj_pobj.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework					Framework;
		::gpk::pobj<::gpk::rtbgra8d32>		Offscreen			= {};
		::gpk::SUDPServer					Server				= {};

		::gpk::cid_t						IdExit				= -1;

		::std::mutex						LockGUI;
		::std::mutex						LockRender;


		::gpk::aapobj<::gpk::SUDPMessage>	MessagesToProcess;

											SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
