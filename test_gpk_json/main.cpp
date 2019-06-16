#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_json.h"

::gpk::error_t								printNode					(::gpk::SJSONNode* node, const ::gpk::view_const_string& testJson)			{
	char											bufferFormat [1024]			= {};
	uint32_t										lenString					= node->Object->Span.End - node->Object->Span.Begin;
	sprintf_s(bufferFormat, "Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
	info_printf(bufferFormat, node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &testJson[node->Object->Span.Begin]);
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], testJson);
	return 0;
}

int											main						()			{
	static const ::gpk::view_const_string			testJson					=
		"\n[ {}, {}, {   }, [  ]  , [], {\"a\":{}, \"b\":[]}"
		"\n, { \"NameId\" : \"ASD\", \"Bleh\":[234124,123,243234, 321   , 65198], \"Else\": [{\"Object\": false}, {}],\"Something\" : \"out\" }"
		"\n, { \"NameId\" : \"654\", \"Bleh\":21354, \"Else\\u1954\": \"in\"} "
		"\n, { \"NameId\" : true, \"B\\\"leh\": null, \"Else\": false} "
		"\n, { \"NameId\" : \"true\", \"Bleh\": \"null\", \"Else\": []} "
		//"\n, { \"NameId\" : \"true\", \"Bleh\": \"null\", \"Else\": \"false\", \"not\" : tres} "
		"\n, { \"NameId\" : .123, \"Bleh\": -456 , \"Else\": -.759 } "
		"\n]";
	info_printf("JSON string: %s.", testJson.begin());

	::gpk::SJSONReader								jsonReader;
	gpk_necall(::gpk::jsonParse(jsonReader, testJson), "Failed to parse json: '%s'.", testJson.begin());

	info_printf("%s", "----------------------------");
	::gpk::array_pod<char_t>						outputJson;
	::printNode(jsonReader.Tree->Children[0], testJson);
	info_printf("%s", "----------------------------");

	for(uint32_t iNode = 0; iNode < jsonReader.Object.size(); ++iNode) {
		const gpk::SJSONType							& node							= jsonReader.Object[iNode];
		::gpk::view_const_string						view							= jsonReader.View[iNode];
		char											bufferFormat [8192]				= {};
		uint32_t										lenString						= view.size();
		sprintf_s(bufferFormat, "Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
		info_printf(bufferFormat, node.Type, ::gpk::get_value_label(node.Type).begin(), node.Span.Begin, node.Span.End, node.ParentIndex, iNode, view.begin());
	}
	return 0;
}
