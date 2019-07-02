#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_json.h"
#include "gpk_json_expression.h"

::gpk::error_t								printNode						(::gpk::SJSONNode* node, const ::gpk::view_const_char& testJson)			{
	char											bufferFormat [1024]				= {};
	uint32_t										lenString						= node->Object->Span.End - node->Object->Span.Begin;
	sprintf_s(bufferFormat, "Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
//	info_printf(bufferFormat, node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &testJson[node->Object->Span.Begin]);
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], testJson);

	return 0;
}

::gpk::error_t								testJSONReader				()			{
	static const ::gpk::view_const_string			testJson					= //"[{\"a\":[123 3,32,1156]}]";
		"\n            "
		"\n[\t {\"Bleh\": \"test\" }, {}, {   }, [  ]  , [], {\"a\":{}, \"b\":[123,]}"
		"\n,\t {\"NameId\" : \"ASD\", \"Bleh\" :[234124,123,243234, 321   , 65198], \"Else\": [{\"Object\": false}, {}],\"Something\" : \"out\",}"
		"\n,\t {\"NameId\" : \"654\", \"Bleh\":21354, \"Else\\u1954\": \"in\"} "
		"\n,\t {\"NameId\" : true, \"B\\\"leh\": null, \"Else\": false}		"
		"\n,\t {\"NameId\" : \"654\", \"Bleh\":21354, \"Else\\u1954\": \"in\"} "
		"\n,\t {\"NameId\" : true, \"B\\\"leh\": null, \"Else\": false} "
		"\n,\t {\"NameId\" : \"true\", \"Bleh\": \"null\", \"Else\": []} "
		//"\n, { \"NameId\" : 12344, \"Bleh\": \"\" \"\", \"Else\": \"false\", \"not\" : tres} "
		"\n,\t { \"NameId\" : .123, \"Bleh\": -456 , \"Else\": -.759 } "
		"\n]"
		"\n           "
		;
	info_printf("JSON string (%u characters): %s.", testJson.size(), testJson.begin());

	const char										bleh[]						= "";
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

::gpk::error_t								testJSONFormatter			(const ::gpk::view_const_string format, const ::gpk::view_const_string inputJson)			{
	::gpk::array_pod<char_t>						formatted;
	::gpk::SJSONReader								jsonReader;
	gpk_necall(::gpk::jsonParse(jsonReader, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
	::gpk::ptr_obj<::gpk::SJSONNode>				root						= jsonReader.Tree[0];
	::printNode(root, inputJson);
	const ::gpk::error_t							indexOfFirstObject			= ::gpk::jsonArrayValueGet(*root, 0);
	info_printf("Test format: '%s'.", format.begin());
	gpk_necall(::gpk::jsonStringFormat(format, jsonReader, indexOfFirstObject, formatted), "%s", "Error formatting string from JSON object.");
	info_printf("Formatted string after jsonStringFormat(): '%s'.", formatted.begin());
	return 0;
}

int											main						()	{
	gpk_necall(::testJSONReader(), "%s", "Failed to read JSON!");

	const ::gpk::view_const_string					inputJsonEasy				= 
		"["
		"\n\t{\"child_selected\" : {\"index\" : 2, \"active\" : true}, \"color\" : \"red\", \"race\" : \"brown\", \"weight\" : 160"
		"\n\t\t, \"parent\" : {\"name\" : \"lucas\"}"
		"\n\t\t, \"children\": [\"marta\", \"venus\", \"crystal\"]"
		"\n\t\t, \"height\" : \"1.56\", \"name\" : \"carlos\""
		"\n\t},"
		"\n\t{\"child_selected\" : {\"index\" : 2, \"active\" : true}, \"color\" : \"red\", \"race\" : \"brown\", \"weight\" : 160"
		"\n\t\t, \"parent\" : {\"name\" : \"lucas\"}"
		"\n\t\t, \"children\": [\"marta\", \"venus\", \"crystal\"]"
		"\n\t\t, \"height\" : \"1.56\", \"name\" : \"carlos\""
		"\n\t},"
		"]";
	{ // first-level expression resolution tests.
		{
			const ::gpk::view_const_string					format						= "I want to replace this (but not \\{this}): Guy name: {name}.";
			gpk_necall(::testJSONFormatter(format, inputJsonEasy), "%s", "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJsonEasy.begin());
			info_printf("Test succeeded:\nInput expression:\n%s\nInput JSON:\n%s", format.begin(), inputJsonEasy.begin());
		}
		{
			const ::gpk::view_const_string					format						= "I want to replace this (but not \\{this}): Parent name: {parent.name}.";
			gpk_necall(::testJSONFormatter(format, inputJsonEasy), "%s", "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJsonEasy.begin());
			info_printf("Test succeeded:\nInput expression:\n%s\nInput JSON:\n%s", format.begin(), inputJsonEasy.begin());
		}
		char											bufferFormat	[1024]		= {};
		for(uint32_t iChild = 0; iChild < 3; ++iChild) {
			sprintf_s(bufferFormat, "I want to replace this (but not \\{this}): A child of {name}'s name: {children[%u]}.", iChild);
			gpk_necall(::testJSONFormatter(bufferFormat, inputJsonEasy), "%s", "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", bufferFormat, inputJsonEasy.begin());
			info_printf("Test succeeded:\nInput expression:\n%s\nInput JSON:\n%s", bufferFormat, inputJsonEasy.begin());
		}
	}

	{ // multilevel expression resolution tests.
		{
			const ::gpk::view_const_string					format						= "I want to replace this (but not \\{this}): A child of carlos's name: {children[child_selected.index]}.";
			gpk_necall(::testJSONFormatter(format, inputJsonEasy), "%s", "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJsonEasy.begin());
			info_printf("Test succeeded:\nInput expression:\n%s\nInput JSON:\n%s", format.begin(), inputJsonEasy.begin());
		}
		const ::gpk::view_const_string					inputJsonHard					= 
			"[	"
			"\n	{ \"properties\" : [{ \"name\" : \"color\", \"type\" : \"string\"}, { \"name\" : \"age\", \"type\" : \"int\"} ]"
			"\n	, \"selection\" : {\"index\" : 1 , \"active\" : true }"
			"\n	, \"people\" : "
			"\n		[{ \"name\" : \"David\""
			"\n		 , \"property\" : "
			"\n			{ \"color\" : \"red\", \"age\" : 25 }"
			"\n		},"
			"\n		{ \"name\" : \"Charles\""
			"\n		 , \"property\" : "
			"\n			{ \"color\" : \"green\", \"age\" : 32 }"
			"\n		},"
			"\n		{ \"name\" : \"Charles\""
			"\n		 , \"property\" : "
			"\n			{ \"color\" : \"blue\", \"age\" : 95 }"
			"\n		},"
			"\n		],"
			"\n	},"
			"\n]";
		const ::gpk::view_const_string					format						= "I want to replace this (but not \\{this}): {people[2].name}'s {properties[selection.index].name}: {people[2].property.{properties[selection.index].name}}.";
		gpk_necall(::testJSONFormatter(format, inputJsonHard), "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJsonHard.begin());
		info_printf("Test succeeded:\nInput expression:\n%s\nInput JSON:\n%s", format.begin(), inputJsonHard.begin());
	}
	return 0;
}