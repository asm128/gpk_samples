#include "gpk_udp_client_app.h"
#include "gpk_framework.h"

#ifndef TEST_UDP_CLIENT_H_23627
#define TEST_UDP_CLIENT_H_23627

struct SApplication {
	::gpk::SFramework		Framework	;
	::gpk::SClient			Client			= {};

							SApplication	(::gpk::SRuntimeValues & runtimeValues)	noexcept : Framework(runtimeValues) {}
};

#endif // TEST_UDP_CLIENT_H_23627
