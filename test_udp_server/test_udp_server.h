#include "gpk_udp_server_app.h"
#include "gpk_framework.h"

#ifndef TEST_UDP_SERVER_H_23627
#define TEST_UDP_SERVER_H_23627

struct SApplication {
	::gpk::SFramework			Framework	;
	::gpk::po<::gpk::SServer>	Server;

								SApplication	(::gpk::SRuntimeValues & runtimeValues)	noexcept : Framework(runtimeValues) {}
};

#endif // TEST_UDP_SERVER_H_23627
