#include "gpk_cgi_runtime.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"

static	::gpk::error_t								generate_output_qs				(::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::array_pod<char_t> & output)				{
	::gpk::array_pod<char>									buffer							= {};
	::gpk::view_const_string								querystring;
	::gpk::array_obj<::gpk::TKeyValConstString>				environBlockViews;
	::gpk::environmentBlockViews(runtimeValues.EntryPointArgs.EnvironmentBlock, environBlockViews);
	::gpk::find("QUERY_STRING", environBlockViews, querystring);
	buffer.resize(querystring.size() + 1024);
	output.push_back('{');
	output.append(buffer.begin(), sprintf_s(buffer.begin(), buffer.size(), "\n\"length\" : %u, \"data\" : \"%s\", \"values\" : ", querystring.size(), querystring.begin()));

	output.push_back('{');
	for(uint32_t iEnviron = 0; iEnviron < runtimeValues.QueryStringKeyVals.size(); ++iEnviron) {
		const ::gpk::TKeyValConstString							& keyval						= runtimeValues.QueryStringKeyVals[iEnviron];
		if(iEnviron > 0)
			output.push_back(',');
		::gpk::array_pod<char_t>								key				= keyval.Key;
		::gpk::array_pod<char_t>								val				= keyval.Val;
		key.push_back('\0');
		val.push_back('\0');
		output.append(buffer.begin(), sprintf_s(buffer.begin(), buffer.size(), "\n\"%s\" : \"%s\"", key.begin(), val.begin()));
	}
	output.push_back('}');

	output.push_back('}');
	return 0;
}

static	::gpk::error_t								generate_output_cgi_env			(::gpk::array_pod<char_t> & output, ::gpk::view_array<const ::gpk::TKeyValConstString> environViews)					{
	::gpk::array_pod<char>									buffer							= {};
	output.push_back('{');
	uint32_t												iComma							= 0;
	for(uint32_t iCGIEnviron	= 0; iCGIEnviron	< ::gpk::size(::gpk::cgi_environ)	; ++iCGIEnviron	)
	for(uint32_t iEnviron		= 0; iEnviron		< environViews.size()		; ++iEnviron	) {
		const ::gpk::TKeyValConstString							& keyval						= environViews[iEnviron];
		if(::gpk::cgi_environ[iCGIEnviron] == keyval.Key) {
			if(iComma > 0)
				output.push_back(',');
			::gpk::array_pod<char_t>								key								= keyval.Key;
			key.push_back('\0');
			buffer.resize(key.size() + keyval.Val.size() + 1024);
			output.append(buffer.begin(), sprintf_s(buffer.begin(), buffer.size(), "\n\"%s\" : \"%s\"", key.begin(), keyval.Val.begin()));
			++iComma;
			break;
		}
	}
	output.push_back('}');
	return 0;
}

static	::gpk::error_t								generate_output_process_env		(::gpk::array_pod<char_t> & output, ::gpk::view_array<const ::gpk::TKeyValConstString> environViews)					{
	::gpk::array_pod<char>									buffer							= {};
	output.push_back('{');
	for(uint32_t iEnviron = 0; iEnviron < environViews.size(); ++iEnviron) {
		const ::gpk::TKeyValConstString							& keyval						= environViews[iEnviron];
		if(iEnviron > 0)
			output.push_back(',');
		::gpk::array_pod<char_t>								key								= keyval.Key;
		key.push_back('\0');
		buffer.resize(key.size() + keyval.Val.size() + 1024);
		output.append(buffer.begin(), sprintf_s(buffer.begin(), buffer.size(), "\n\"%s\" : \"%s\"", key.begin(), keyval.Val.begin()));
	}
	output.push_back('}');
	return 0;
}

		::gpk::error_t								generate_output					(::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::array_pod<char_t> & output)					{
	output.append(::gpk::view_const_string{"\r\n"});
	output.push_back('[');
	output.push_back('{');
	{
		::gpk::array_obj<::gpk::TKeyValConstString>				environViews;
		::gpk::environmentBlockViews(runtimeValues.EntryPointArgs.EnvironmentBlock, environViews);
		{
			::gpk::view_const_string								querystring;
			::gpk::find("QUERY_STRING", environViews, querystring);
			if(querystring.size()) {
				output.append(::gpk::view_const_string{"\n \"queryString\" : "});
				::generate_output_qs(runtimeValues, output);
				output.push_back(',');
			}
		}
		{
			{
				output.append(::gpk::view_const_string{"\n \"cgi_environment\" : "});
				::generate_output_cgi_env(output, environViews);
				output.push_back(',');
			}
			{
				output.append(::gpk::view_const_string{"\n \"process_environment\" : "});
				::generate_output_process_env(output, environViews);
				output.push_back(',');
			}
		}
	}

	::gpk::array_pod<char>									buffer							= {};
	buffer.resize(runtimeValues.Content.Length + 4096);
	{
		::gpk::array_pod<char>									content_body				= {};
		content_body.resize(runtimeValues.Content.Length);
		uint32_t												iChar						= 0;
		char													iArg						= 0;
		while(iChar < runtimeValues.Content.Length) {
			int														count						= 0;
			//memset(content_body.begin(), 0, content_body.size());
			content_body.resize(0);
			uint32_t												iOffset						= iChar;
			bool													value						= false;
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
			output.append(buffer.begin(), sprintf_s(buffer.begin(), buffer.size(), "\n \"content_body\" : {\"u0\" : %u, \"u1\" : %u, \"u2\" : %u, \"u3\" : %u, \"value\": \n%s }, "
				, iChar - iOffset, content_body.size(), runtimeValues.Content.Length, runtimeValues.Content.Body.size(), content_body.begin()));
		}
	}
	output.append(buffer.begin(), sprintf_s(buffer.begin(), buffer.size(), "\n \"content_body_\" : {\"u0\" : %u, \"u1\" : %u, \"text\" : %s }", runtimeValues.Content.Length, runtimeValues.Content.Body.size(), runtimeValues.Content.Body.begin()));
	output.append(buffer.begin(), sprintf_s(buffer.begin(), buffer.size(), "\n%s\n", "}]"));
	return output.size();
}

static ::gpk::error_t								cgiMain							(int argc, char** argv, char**envv)		{
	(void)(envv);
	::gpk::SCGIRuntimeValues								runtimeValues;
	::gpk::cgiRuntimeValuesLoad(runtimeValues, {(const char**)argv, (uint32_t)argc});
	::gpk::array_pod<char_t>								output							= ::gpk::view_const_string{"Content-type: application/json\r\n"};
	::generate_output(runtimeValues, output);
	printf("%s", output.begin());
	return 0;
}

int													main							(int argc, char** argv, char**envv)		{ return ::cgiMain(argc, argv, envv); }

#ifdef GPK_WINDOWS
int WINAPI											WinMain
	(	_In_		HINSTANCE	hInstance
	,	_In_opt_	HINSTANCE	hPrevInstance
	,	_In_		LPSTR		lpCmdLine
	,	_In_		int			nShowCmd
	) {
	(void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nShowCmd;
	return ::cgiMain(__argc, __argv, environ);
}
#endif
