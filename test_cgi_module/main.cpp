#include "gpk_error.h"
#include "gpk_cgi_module.h"
#include "gpk_cgi_runtime.h"

struct SCGIApp {
	::gpk::SCGIFramework		& Framework				;

								SCGIApp					(::gpk::SCGIFramework & framework)									: Framework(framework)	{}
};

GPK_DEFINE_CGI_MODULE_EXPORTS(SCGIApp, "Test CGI Module");

::gpk::error_t				setup					(::SCGIApp& applicationInstance)									{ applicationInstance;						return 0; }
::gpk::error_t				cleanup					(::SCGIApp& applicationInstance)									{ applicationInstance;						return 0; }
::gpk::error_t				update					(::SCGIApp& applicationInstance, bool systemRequestedExit)			{ applicationInstance, systemRequestedExit;	return 0; }
::gpk::error_t				draw					(::SCGIApp& applicationInstance, ::gpk::array_pod<char_t>& output)	{ applicationInstance, output;				
	output.append("<html><head></head><body><h3>module is working!!</h3></body></html>");
	return 0; 
}
