#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_json.h"

::gpk::error_t								printNode					(::gpk::SJSONNode* node)			{
	info_printf("Node type: %u. Node Span: {%u, %u}. Parent index: %u. Object index: %u.", node->Object->Type, node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex);
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren]);
	return 0;
}

int											main						()			{
	static const ::gpk::view_const_string			testJson					=
		"[ { \"NameId\" : \"ASD\", \"Bleh\":[23412, 123, 243234], \"Else\": [{\"Object\": false}, {}],\"Something\" : \"out\" }"
		", { \"NameId\" : \"654\", \"Bleh\":21354, \"Else\\u1954\": \"in\"} "
		", { \"NameId\" : true, \"B\\\"leh\": null, \"Else\": false} "
		", { \"NameId\" : \"true\", \"Bleh\": \"null\", \"Else\": \"false\"} "
		", { \"NameId\" : .123, \"Bleh\": -456, \"Else\": x759 } "
		"]";

	::gpk::SJSONReader								jsonReader;
	gpk_necall(::gpk::jsonParse(jsonReader, testJson), "Failed to parse json: '%s'.", testJson.begin());
	info_printf("%s", "----------------------------");
	::printNode(jsonReader.Tree->Children[0]);
	info_printf("%s", "----------------------------");
	for(uint32_t iNode = 0; iNode < jsonReader.Object.size(); ++iNode) {
		const gpk::SJSONType							& node							= jsonReader.Object[iNode];
		info_printf("Node type: %u. Node Span: {%u, %u}. Parent index: %u. Object index: %u.", node.Type, node.Span.Begin, node.Span.End, node.ParentIndex, iNode);
	}
	return 0;
}
