#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_json.h"

::gpk::error_t								printNode					(::gpk::SJSONNode* node, const ::gpk::view_const_char& testJson)			{
	char											bufferFormat [1024]			= {};
	uint32_t										lenString					= node->Object->Span.End - node->Object->Span.Begin;
	sprintf_s(bufferFormat, "Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
	info_printf(bufferFormat, node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &testJson[node->Object->Span.Begin]);
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], testJson);

	return 0;
}

namespace gpk
{
	GDEFINE_ENUM_TYPE(JSON_FORMATTER_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(JSON_FORMATTER_TYPE, LITERAL	, 0);
	GDEFINE_ENUM_VALUE(JSON_FORMATTER_TYPE, TOKEN	, 1);
	GDEFINE_ENUM_VALUE(JSON_FORMATTER_TYPE, COUNT	, 2);
	GDEFINE_ENUM_VALUE(JSON_FORMATTER_TYPE, UNKNOWN	, -1);

	struct SJSONFormatterType {
		int32_t												ParentIndex;
		JSON_FORMATTER_TYPE									Type;
		::gpk::SSlice<uint32_t>								Span;
	};

	struct SJSONFormatterState {
		uint32_t											IndexCurrentChar		= 0;
		int32_t												IndexCurrentElement		= -1;
		::gpk::SJSONFormatterType							* CurrentElement		= 0;
		int32_t												NestLevel				= 0;
		char												CharCurrent				= 0;
		bool												Escaping				= false;
		bool												InsideToken				= false;
	};
}

::gpk::error_t								jsonStringFormatParseToken		(::gpk::SJSONFormatterState & work_state, ::gpk::array_pod<::gpk::SJSONFormatterType> & out_types, const ::gpk::view_const_string& in_format)		{ 
	in_format;
	switch(work_state.CharCurrent) {
	default		: break;
	case '}'	: 
		out_types[work_state.IndexCurrentElement].Span.End	= work_state.IndexCurrentChar;
		out_types.push_back({-1, ::gpk::JSON_FORMATTER_TYPE_LITERAL, {work_state.IndexCurrentChar, work_state.IndexCurrentChar}});
	}
	return 0; 
}
::gpk::error_t								jsonStringFormatParseLiteral	(::gpk::SJSONFormatterState & work_state, ::gpk::array_pod<::gpk::SJSONFormatterType> & out_types, const ::gpk::view_const_string& in_format)		{ 
	in_format;
	switch(work_state.CharCurrent) {
	default		: break;
	case '{'	: 
		if(work_state.Escaping)
			break;
		out_types[out_types.size() - 1].Span.End	= work_state.IndexCurrentChar;
		work_state.IndexCurrentElement				= out_types.push_back({-1, ::gpk::JSON_FORMATTER_TYPE_TOKEN, {work_state.IndexCurrentChar, work_state.IndexCurrentChar}});
		// do work
		break;
	case '\\'	: 
		if(false == work_state.Escaping) {
			work_state.Escaping							= true;
			return 0;
		}
		break;
	}
	work_state.Escaping							= false;
	return 0; 
}

::gpk::error_t								jsonStringFormatParseStep		(::gpk::SJSONFormatterState & work_state, ::gpk::array_pod<::gpk::SJSONFormatterType> & out_types, const ::gpk::view_const_string& in_format)		{

	if(work_state.InsideToken)
		jsonStringFormatParseToken		(work_state, out_types, in_format);
	else {
		if(0 == out_types.size()) 
			work_state.IndexCurrentElement = out_types.push_back({-1, ::gpk::JSON_FORMATTER_TYPE_LITERAL, {work_state.IndexCurrentChar, work_state.IndexCurrentChar}});

		jsonStringFormatParseLiteral	(work_state, out_types, in_format);
	}
	return 0;
}


::gpk::error_t								jsonStringFormatParse			(::gpk::SJSONFormatterState & work_state, ::gpk::array_pod<::gpk::SJSONFormatterType> & out_types, const ::gpk::view_const_string& in_format)		{

	for(work_state.IndexCurrentChar = 0; work_state.IndexCurrentChar < in_format.size(); ++work_state.IndexCurrentChar) {
		work_state.CharCurrent						= in_format[work_state.IndexCurrentChar];
		gpk_necall(jsonStringFormatParseStep(work_state, out_types, in_format), "Parse step failed.");
	}
	return 0;
}

::gpk::error_t								jsonStringFormat			(const ::gpk::view_const_string& format, const ::gpk::SJSONNode& input, ::gpk::array_pod<char_t>& )			{
	format, input;
	return 0;
}

int											main						()			{
	static const ::gpk::view_const_string			testJson					= //"[{\"a\":[123 3,32,1156]}]";
		//"\n[ { \"NameId\" : \"654\", \"Bleh\":21354, \"Else\\u1954\": \"in\"} "
		"\n[ { \"Bleh\": \"test\" }, {}, {   }, [  ]  , [], {\"a\":{}, \"b\":[123,]}"
		"\n, { \"NameId\" : \"ASD\", \"Bleh\" :[234124,123,243234, 321   , 65198], \"Else\": [{\"Object\": false}, {}],\"Something\" : \"out\",}"
		"\n, { \"NameId\" : \"654\", \"Bleh\":21354, \"Else\\u1954\": \"in\"} "
		"\n, { \"NameId\" : true, \"B\\\"leh\": null, \"Else\": false} "
		"\n, { \"NameId\" : \"654\", \"Bleh\":21354, \"Else\\u1954\": \"in\"} "
		"\n, { \"NameId\" : true, \"B\\\"leh\": null, \"Else\": false} "
		"\n, { \"NameId\" : \"true\", \"Bleh\": \"null\", \"Else\": []} "
		//"\n, { \"NameId\" : 12344, \"Bleh\": \"\" \"\", \"Else\": \"false\", \"not\" : tres} "
		"\n, { \"NameId\" : .123, \"Bleh\": -456 , \"Else\": -.759 } "
		"\n]";
	info_printf("JSON string (%u characters): %s.", testJson.size(), testJson.begin());

	char in = 'c';
	switch(in){
	case 'a': break;
	case 'b': break;
	case 'c': break;
	case 'd': break;
	}
	const char			bleh[] = "";
	info_printf("%s", bleh);

	::gpk::SJSONReader								jsonReader;
	gpk_necall(::gpk::jsonParse(jsonReader, testJson), "Failed to parse json: '%s'.", testJson.begin());

	info_printf("---------------------------- Linear iteration: %u objects.", jsonReader.Object.size());
	::gpk::array_pod<char_t>						outputJson;
	::gpk::ptr_obj<::gpk::SJSONNode>				root							= jsonReader.Tree[0];
	::printNode(root, testJson);

	info_printf("---------------------------- Linear iteration: %u objects.", jsonReader.Object.size());
	for(uint32_t iNode = 0; iNode < jsonReader.Object.size(); ++iNode) {
		const gpk::SJSONType							& node							= jsonReader.Object[iNode];
		if( ::gpk::JSON_TYPE_VALUE	== node.Type
		 || ::gpk::JSON_TYPE_KEY	== node.Type
		 )
			continue;
		::gpk::view_const_string						view							= jsonReader.View[iNode];
		char											bufferFormat [8192]				= {};
		uint32_t										lenString						= view.size();
		sprintf_s(bufferFormat, "Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
		info_printf(bufferFormat, node.Type, ::gpk::get_value_label(node.Type).begin(), node.Span.Begin, node.Span.End, node.ParentIndex, iNode, view.begin());
	}

	const ::gpk::view_const_string					test_key						= "Bleh";
	info_printf("---------------------------- Access test. Test key: %s.", test_key.begin());
	::gpk::error_t									indexOfFirstObject				= ::gpk::jsonArrayValueGet(*root, 0);
	::gpk::ptr_obj<::gpk::SJSONNode>				object							= jsonReader.Tree[indexOfFirstObject];
	::gpk::error_t									indexOfElement					= ::gpk::jsonObjectValueGet(*object, jsonReader.View, test_key);
	gpk_necall((uint32_t)indexOfElement >= jsonReader.View.size(), "Test key '%s' not found: %i.", test_key.begin(), indexOfElement);
	const gpk::SJSONType							& node							= jsonReader.Object	[indexOfElement];	
	::gpk::view_const_string						view							= jsonReader.View	[indexOfElement - 1];	// get the parent in order to retrieve the view with the surrounding in the case of strings "".
	char											bufferFormat [8192]				= {};
	uint32_t										lenString						= view.size();
	sprintf_s(bufferFormat, "Found test value {'%%s':%%%u.%us}. Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u.", lenString, lenString);
	info_printf(bufferFormat, test_key.begin(), view.begin(), node.Type, ::gpk::get_value_label(node.Type).begin(), node.Span.Begin, node.Span.End, node.ParentIndex, indexOfElement, view.begin());
	return 0;
}
