#include "gpk_cgi_runtime.h"
#include "gpk_string_helper.h"

::gpk::error_t										generate_output(::gpk::array_pod<char_t> & output)					{
	::gpk::SCGIRuntimeValues								runtimeValues;
	::gpk::cgiRuntimeValuesLoad(runtimeValues);
	char													buffer[8192]		= {};
	const ::gpk::array_obj<::gpk::view_const_string>		& keyvalviews		= runtimeValues.QueryStringElements;
	if(runtimeValues.QueryString.size()) {
		output.append(buffer, sprintf_s(buffer, "\r\n[{ \"queryString\" : { \"length\" : %u, \"data\" : \"%s\", \"values\" : [", runtimeValues.QueryString.size(), runtimeValues.QueryString.begin()));
		for(uint32_t iKeyVal = 0; iKeyVal < keyvalviews.size(); ++iKeyVal) {
			const ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>	& keyval		= runtimeValues.QueryStringKeyVals[iKeyVal];
			output.append(buffer, ::gpk::formatForSize(keyval.Key, buffer, "\n { \"Key\": \"", "\""));
			if(keyvalviews.size() - 1 == iKeyVal)
				output.append(buffer, ::gpk::formatForSize(keyval.Val, buffer, "\n , \"Val\": \"", "\"\n }"));// without the trailing , comma character
			else
				output.append(buffer, ::gpk::formatForSize(keyval.Val, buffer, "\n , \"Val\": \"", "\"\n }, "));
		}
		output.append(buffer, sprintf_s(buffer, "\n] }, "));
	}
	else
		output.append(buffer, sprintf_s(buffer, "\r\n[{"));

	output.append(buffer, sprintf_s(buffer, "\n \"cgi_environment\" : "));
	output.append(buffer, sprintf_s(buffer, "\n { \"AUTH_PASSWORD\"			: \"%s\"", runtimeValues.Environment.AUTH_PASSWORD			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"AUTH_TYPE\"				: \"%s\"", runtimeValues.Environment.AUTH_TYPE				.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"AUTH_USER\"				: \"%s\"", runtimeValues.Environment.AUTH_USER				.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_COOKIE\"			: \"%s\"", runtimeValues.Environment.CERT_COOKIE			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_FLAGS\"			: \"%s\"", runtimeValues.Environment.CERT_FLAGS				.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_ISSUER\"			: \"%s\"", runtimeValues.Environment.CERT_ISSUER			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_KEYSIZE\"			: \"%s\"", runtimeValues.Environment.CERT_KEYSIZE			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SECRETKEYSIZE\"	: \"%s\"", runtimeValues.Environment.CERT_SECRETKEYSIZE		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SERIALNUMBER\"		: \"%s\"", runtimeValues.Environment.CERT_SERIALNUMBER		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SERVER_ISSUER\"	: \"%s\"", runtimeValues.Environment.CERT_SERVER_ISSUER		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SERVER_SUBJECT\"	: \"%s\"", runtimeValues.Environment.CERT_SERVER_SUBJECT	.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SUBJECT\"			: \"%s\"", runtimeValues.Environment.CERT_SUBJECT			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CF_TEMPLATE_PATH\"		: \"%s\"", runtimeValues.Environment.CF_TEMPLATE_PATH		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CONTENT_LENGTH\"		: \"%s\"", runtimeValues.Environment.CONTENT_LENGTH			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CONTENT_TYPE\"			: \"%s\"", runtimeValues.Environment.CONTENT_TYPE			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"CONTEXT_PATH\"			: \"%s\"", runtimeValues.Environment.CONTEXT_PATH			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"GATEWAY_INTERFACE\"		: \"%s\"", runtimeValues.Environment.GATEWAY_INTERFACE		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS\"					: \"%s\"", runtimeValues.Environment.HTTPS					.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS_KEYSIZE\"			: \"%s\"", runtimeValues.Environment.HTTPS_KEYSIZE			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS_SECRETKEYSIZE\"	: \"%s\"", runtimeValues.Environment.HTTPS_SECRETKEYSIZE	.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS_SERVER_ISSUER\"	: \"%s\"", runtimeValues.Environment.HTTPS_SERVER_ISSUER	.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS_SERVER_SUBJECT\"	: \"%s\"", runtimeValues.Environment.HTTPS_SERVER_SUBJECT	.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_ACCEPT\"			: \"%s\"", runtimeValues.Environment.HTTP_ACCEPT			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_ACCEPT_ENCODING\"	: \"%s\"", runtimeValues.Environment.HTTP_ACCEPT_ENCODING	.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_ACCEPT_LANGUAGE\"	: \"%s\"", runtimeValues.Environment.HTTP_ACCEPT_LANGUAGE	.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_CONNECTION\"		: \"%s\"", runtimeValues.Environment.HTTP_CONNECTION		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_COOKIE\"			: \"%s\"", runtimeValues.Environment.HTTP_COOKIE			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_HOST\"				: \"%s\"", runtimeValues.Environment.HTTP_HOST				.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_REFERER\"			: \"%s\"", runtimeValues.Environment.HTTP_REFERER			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_USER_AGENT\"		: \"%s\"", runtimeValues.Environment.HTTP_USER_AGENT		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"QUERY_STRING\"			: \"%s\"", runtimeValues.Environment.QUERY_STRING			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"REMOTE_ADDR\"			: \"%s\"", runtimeValues.Environment.REMOTE_ADDR			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"REMOTE_HOST\"			: \"%s\"", runtimeValues.Environment.REMOTE_HOST			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"REMOTE_USER\"			: \"%s\"", runtimeValues.Environment.REMOTE_USER			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"REQUEST_METHOD\"		: \"%s\"", runtimeValues.Environment.REQUEST_METHOD			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"SCRIPT_NAME\"			: \"%s\"", runtimeValues.Environment.SCRIPT_NAME			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_NAME\"			: \"%s\"", runtimeValues.Environment.SERVER_NAME			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_PORT\"			: \"%s\"", runtimeValues.Environment.SERVER_PORT			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_PORT_SECURE\"	: \"%s\"", runtimeValues.Environment.SERVER_PORT_SECURE		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_PROTOCOL\"		: \"%s\"", runtimeValues.Environment.SERVER_PROTOCOL		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_SOFTWARE\"		: \"%s\"", runtimeValues.Environment.SERVER_SOFTWARE		.begin()));
	output.append(buffer, sprintf_s(buffer, "\n , \"WEB_SERVER_API\"		: \"%s\"", runtimeValues.Environment.WEB_SERVER_API			.begin()));
	output.append(buffer, sprintf_s(buffer, "\n },"));
																			 
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
			output.append(buffer, sprintf_s(buffer, "\n \"content_body\" : {\"u0\" : %u, \"u1\" : %u, \"u2\" : %u, \"u3\" : %u, \"value\": \n%s }, "
				, iChar - iOffset, content_body.size(), runtimeValues.Content.Length, runtimeValues.Content.Body.size(), content_body.begin()));
		}
	}
	output.append(buffer, sprintf_s(buffer, "\n \"content_body_\" : {\"u0\" : %u, \"u1\" : %u, \"text\" : %s }", runtimeValues.Content.Length, runtimeValues.Content.Body.size(), runtimeValues.Content.Body.begin()));

	//output.append(buffer, sprintf_s(buffer, "%s", "<iframe width=\"100%%\" height=\"100%%\" src=\"http://localhost/home.html\"></iframe>\n"));
	output.append(buffer, sprintf_s(buffer, "\n%s\n", "}]"));
	return output.size();
}

static ::gpk::error_t								cgiMain				() {
	::gpk::array_pod<char_t>								output;
	::generate_output(output);
	printf("%s", output.begin());
	//::std::putc(0, stdout);
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
	int			argc = __argc;
	char**		argv = __argv;
	(void)argc;
	(void)argv;
	return ::cgiMain();
}
