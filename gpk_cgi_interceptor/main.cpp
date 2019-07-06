#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"
#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_storage.h"

#include <string>

#include <Windows.h>

::gpk::error_t										metrics_split			(const ::gpk::view_const_string& input_string, ::gpk::SCoord2<int32_t>& output_metrics)												{
	uint32_t												iChar					= 0;
	for(iChar = 0; iChar < input_string.size(); ++iChar) {
		if('x' == input_string[iChar]) {
			::gpk::array_pod<char>									sx						= {};
			::gpk::array_pod<char>									sy						= {};
			gpk_necall(sy.append((char*)input_string.begin(), iChar), "%s", "");
			gpk_necall(sx.append((char*)&input_string[iChar + 1], input_string.size() - (iChar + 1)), "%s", "");
			try {
				output_metrics.x									= ::std::stoi(::std::string{sy.begin(), sy.size()});
				output_metrics.y									= ::std::stoi(::std::string{sy.begin(), sy.size()});
			}
			catch (...) {
				output_metrics										= {};
				return -1;
			}
			break;
		}
	}
	return 0;
}

static	int											cgiBootstrap			(const ::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::array_pod<char> & output)										{
	::gpk::array_pod<char_t>								environmentBlock		= {};
	{	// Prepare CGI environment and request content packet to send to the service.
		ree_if(errored(::gpk::getEnvironmentBlock(environmentBlock)), "%s", "Failed");
		environmentBlock.append(runtimeValues.Content.Body.begin(), runtimeValues.Content.Body.size());
		environmentBlock.push_back(0);
	}
	output.append("\r\n<html><head><title>Placeholder response</title></head><body>Placeholder body.</body></html>");
	return 0;
}

static int											cgiMain				()	{
	::gpk::SCGIRuntimeValues								runtimeValues;
	gpk_necall(::gpk::cgiRuntimeValuesLoad(runtimeValues), "%s", "Failed to load cgi runtime values.");
	::gpk::array_pod<char>									responseBody;
	if errored(::cgiBootstrap(runtimeValues, responseBody)) {
		printf("%s\r\n", "Content-Type: text/html"
			"\r\nCache-Control: no-store"
			"\r\n\r\n"
			"<html><head><title>Internal server error</title></head><body>Failed to process request.</body></html>"
			"\r\n"
			"\r\n"
		);
	}
	else {
		printf("%s\r\n", "Content-Type: application/json"
			"\r\nCache-Control: no-store"
		);
		responseBody.push_back('\0');
		OutputDebugStringA(responseBody.begin());
		printf("%s", responseBody.begin());
	}
	return 0;
}

int													main				(int argc, char** argv, char**envv)	{
	(void)argc, (void)argv, (void)envv;
	return ::cgiMain();
}

int WINAPI											WinMain				
	(	_In_		HINSTANCE	hInstance
	,	_In_opt_	HINSTANCE	hPrevInstance
	,	_In_		LPSTR		lpCmdLine
	,	_In_		int			nShowCmd
	) {
	(void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nShowCmd;
	return ::cgiMain();
}

// 17070035
// 0810-122-2666
