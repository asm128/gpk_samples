#include "gpk_error.h"
#include "gpk_cgi_module.h"
#include "gpk_cgi_runtime.h"
#include "gpk_string_helper.h"

struct SCGIApp {
	::gpk::SCGIFramework		& Framework				;

								SCGIApp					(::gpk::SCGIFramework & framework)									: Framework(framework)	{}
};

GPK_DEFINE_CGI_MODULE_EXPORTS(SCGIApp, "Test CGI Module");

#define FILENAME_RT_EXE "gpk_cgi_rt.exe"

static constexpr	const char			html_script	[]			= 
	"\n<script>"
	"\nfunction risaiz() {"
    "\n	self.location = \"./" FILENAME_RT_EXE "?bt=1&m=test_cgi_module\" + \"&w=\" + document.documentElement.clientWidth + \"&h=\" + document.documentElement.clientHeight;"
	"\n}"
	"\n</script>"
	;

::gpk::error_t				setup					(::SCGIApp& applicationInstance)									{ applicationInstance;						return 0; }
::gpk::error_t				cleanup					(::SCGIApp& applicationInstance)									{ applicationInstance;						return 0; }
::gpk::error_t				update					(::SCGIApp& applicationInstance, bool systemRequestedExit)			{ applicationInstance, systemRequestedExit;	return 0; }
::gpk::error_t				draw					(::SCGIApp& applicationInstance, ::gpk::array_pod<char_t>& output)	{ applicationInstance, output;				
	//{
	//	char buffer[256];
	//	::gpk::view_const_string	value;
	//	value = "<html>";										output.append(value.begin(), value.size());
	//	value = "<head>";										output.append(value.begin(), value.size());
	//	value = html_script;									output.append(value.begin(), value.size());
	//	value = "</head>";										output.append(value.begin(), value.size());
	//	value = "<body onresize=\"risaiz()\" ><h3>";			output.append(value.begin(), value.size());
	//	sprintf_s(buffer, "w: %u, h: %u", applicationInstance.Framework.TargetSize.x, applicationInstance.Framework.TargetSize.y);
	//	value = buffer;											output.append(value.begin(), value.size());
	//	value = "</h3></body>";									output.append(value.begin(), value.size());
	//	value = "</html>";										output.append(value.begin(), value.size());
	//}
	{
		::gpk::SCGIFramework									framework			= applicationInstance.Framework;
		::gpk::SCGIRuntimeValues								runtimeValues		= framework.RuntimeValues;
		char													buffer[4096]		= {};
		output.append(buffer, sprintf_s(buffer, "%s", 
			"<html>"
			"\n<head>"
		));
		//output.append(buffer, sprintf_s(buffer, html_script, framework.ModuleName.size() ? framework.ModuleName.begin() : "test_cgi_module"));
		output.append(buffer, sprintf_s(buffer, "%s", 
			"\n<link rel=\"stylesheet\" href=\"./page.css\">"
			"\n</head>"
		));
		output.append(buffer, sprintf_s(buffer, "\n<body style=\"width:95%%; height:95%%; background-color:#FFCCAA; \" %s>", framework.Bootstrapped ? "" : "onload=\"bootstrap()\"" ));
		const ::gpk::array_obj<::gpk::view_const_string>		& keyvalviews		= runtimeValues.QueryStringElements;
		output.append(buffer, sprintf_s(buffer, "\n<h4>Booting %s...</h4>", framework.ModuleName.begin()));
		if(runtimeValues.QueryString.size())
			output.append(buffer, sprintf_s(buffer, "\n<h4>QueryString (%u): %s</h4>", runtimeValues.QueryString.size(), runtimeValues.QueryString.begin()));
		for(uint32_t iChar = 0; iChar < keyvalviews.size(); ++iChar) {
			output.append(buffer, ::gpk::formatForSize(keyvalviews[iChar], buffer, "\n<h3>KeyVal: ", "</h3>"));

			const ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>	& keyval		= runtimeValues.QueryStringKeyVals[iChar];
			output.append(buffer, ::gpk::formatForSize(keyval.Key, buffer, "\n<h3>Key: ", "</h3>"));
			output.append(buffer, ::gpk::formatForSize(keyval.Val, buffer, "\n<h3>Val: ", "</h3>"));
		}
		output.append(buffer, ::gpk::formatForSize({runtimeValues.ContentLength	.begin(), runtimeValues.ContentLength	.size()}, buffer, "\n<h2>CONTENT_LENGTH: "	, "</h2>"));
		output.append(buffer, ::gpk::formatForSize({runtimeValues.ContentType	.begin(), runtimeValues.ContentType		.size()}, buffer, "\n<h2>CONTENT_TYPE: "	, "</h2>"));
		output.append(buffer, sprintf_s(buffer, "\n<h2>Client area size: %u x %u</h2>"	, (uint32_t)framework.TargetSize.x, (uint32_t)framework.TargetSize.y));
		output.append(buffer, sprintf_s(buffer, "\n<h4>Bootstrapped: %s</h4>"			, framework.Bootstrapped ? "true" : "false"));
		output.append(buffer, sprintf_s(buffer, "\n<h4>IP: %u.%u.%u.%u:%u</h4>"			, GPK_IPV4_EXPAND(framework.RuntimeValues.RemoteIP)));
		output.append(buffer, ::gpk::formatForSize({runtimeValues.StrRemoteIP	.begin(), runtimeValues.StrRemoteIP		.size()}, buffer, "\n<h4>String IP: "	, "</h4>"));
		output.append(buffer, ::gpk::formatForSize({runtimeValues.StrRemotePort	.begin(), runtimeValues.StrRemotePort	.size()}, buffer, "\n<h4>String Port: "	, "</h4>"));
		output.append(buffer, sprintf_s(buffer, "\n<h4>Bootstrapped: %s</h4>"			, framework.Bootstrapped ? "true" : "false"));

		//int														argc					= __argc; 
		//char													** argv					= __argv;
		//for(int32_t iArg = 0; iArg < argc; ++iArg) 
		//	output.append(buffer, sprintf_s(buffer, "\n<h1>argv[%u]: %s</h1>", iArg, argv[iArg]));
		{
			::gpk::array_pod<char>									content_body			= {};
			content_body.resize(runtimeValues.Content.Length);
			uint32_t												iChar					= 0;
			char													iArg					= 0;
			while(iChar < runtimeValues.Content.Length) {
				int														count					= 0;
				//memset(content_body.begin(), 0, content_body.size());
				content_body.resize(0);
				uint32_t												iOffset					= iChar;
				bool													value					= false;
				while(iChar < runtimeValues.Content.Length && (iArg = runtimeValues.Content.Body[iChar++])) {
					if(iArg == '\n') {
						++count;
						if(count >= 2) {
							value												= true;
							count												= 0;
							//break;
						}
					}
					else if(iArg != '\r')
						count												= 0;
					content_body.push_back(iArg);
				}
				if(0 == content_body.size())
					break;
				content_body.push_back(0);
				output.append(buffer, sprintf_s(buffer, "\n<h1>content_body (%u:%u/%u:%u): %s</h1>", iChar - iOffset, content_body.size(), runtimeValues.Content.Length, runtimeValues.Content.Body.size(), content_body.begin()));
			}
		}
		output.append(buffer, sprintf_s(buffer, "\n<h1>content_body (Raw:%u:%u): %s</h1>", runtimeValues.Content.Length, runtimeValues.Content.Body.size(), runtimeValues.Content.Body.begin()));

		//output.append(buffer, sprintf_s(buffer, "%s", "<iframe width=\"100%%\" height=\"100%%\" src=\"http://localhost/home.html\"></iframe>\n"));
		output.append(buffer, sprintf_s(buffer, "\n%s\n", "</body>\n</html>"));
	}

	return 0; 
}
