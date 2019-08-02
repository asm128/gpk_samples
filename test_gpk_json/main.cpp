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
	info_printf("JSON string (%u characters): %s", testJson.size(), testJson.begin());

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
	info_printf("---------------------------- Access test. Test key: %s", test_key.begin());
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

	::gpk::error_t						testJSONFormatter			(::gpk::SJSONReader & jsonReader, const ::gpk::view_const_string format, const ::gpk::view_const_string inputJson)			{
	::gpk::ptr_obj<::gpk::SJSONNode>				root						= jsonReader.Tree[0];
	::printNode(root, inputJson);
	const ::gpk::error_t							indexOfFirstObject			= 0;
	info_printf("Test format:\n%s", format.begin());
	::gpk::array_pod<char_t>						formatted;
	gpk_necall(::gpk::jsonStringFormat(format, jsonReader, indexOfFirstObject, formatted), "%s", "Error formatting string from JSON object.");
	info_printf("Formatted string after jsonStringFormat():\n%s", formatted.begin());
	return 0;
}

	::gpk::error_t						testJSONExpression
	(	const ::gpk::SJSONReader						& jsonReader
	,	const ::gpk::view_const_string					expression
	,	const ::gpk::view_const_string					expected
	) {
	info_printf("Testing expression: %s", expression.begin());
	::gpk::view_const_string						result;
	::gpk::array_pod<char_t>						output;
	if(-1 == gpk::jsonExpressionResolve(expression, jsonReader, 0U, result) || result != expected) {
		error_printf("Failed to resolve expression. \nExpression: %s \nExpected: %s \nResult: ", expression.begin(), expected.begin());
		output										= result;
		output.append("\n");
		OutputDebugStringA(output.begin());
		return -1;
	}
	info_printf("Test succeeded. \nExpression: %s \nExpected: %s \nResult: ", expression.begin(), expected.begin());
	output										= result;
	output.append("\n");
	OutputDebugStringA(output.begin());
	return 0;
}

int											main						()	{
	gpk_necall(::testJSONReader(), "%s", "Failed to read JSON!");
	{
		const ::gpk::view_const_string					inputJson				=
			"\n{\t \"name\" : \"carlos\""
			"\n\t, \"parent\" : {\"name\" : \"lucas\"}"
			"\n\t, \"height\" : \"1.56\""
			"\n\t, \"color\" : \"brown\""
			"\n\t, \"race\" : \"red\""
			"\n\t, \"weight\" : 160"
			"\n\t, \"alive\" : true"
			"\n\t, \"dead\" : false"
			"\n\t, \"children\": [\"marta\", \"venus\", \"crystal\"]"
			"\n\t, \"child_selected\" : {\"index\" : 2}"
			"\n}"
			;
		::gpk::TKeyValConstString						pairsResultExpression[]			=
			{ {"carlos"	, "name"}										// 0
			, {"lucas"	, "parent.name"}								// 1
			, {"lucas"	, "'lucas'"}									// 1
			, {"1.56"	, "height"}										// 2
			, {"brown"	, "color"}										// 3
			, {"red"	, "race"}										// 4
			, {"160"	, "weight"}										// 5
			, {"2"		, "child_selected.index"}						// 6
			, {"venus"	, "children[('1')]"}							// 7
			, {"crystal", "children[(child_selected.index)]"}			// 8
			, {"carlos"	, "(name)"}										// 9
			, {"lucas"	, "({'parent'}.name)"}							// 10
			, {"1.56"	, "(height)"}									// 11
			, {"brown"	, "(color)"}									// 12
			, {"red"	, "(race)"}										// 13
			, {"160"	, "(weight)"}									// 14
			, {"2"		, "(child_selected.('index'))"}					// 15
			, {"2"		, "(child_selected.index)"}						// 15
			, {"venus"	, "(children[('1')])"}							// 16
			, {"crystal", "(children[(child_selected.index)])"}			// 17
			, {"true"	, " alive  ? 'true' : 'false'"		}			// 17
			, {"true"	, "(alive) ? 'true' : 'false'"		}			// 17
			, {"false"	, " dead   ? 'true' : 'false'"		}			// 17
			, {"false"	, "(dead)  ? 'true' : 'false'"		}			// 17
			, {"true"	, " alive  ? ('true') : ('false')"		}			// 17
			, {"true"	, "(alive) ? ('true') : ('false')"		}			// 17
			, {"false"	, " dead   ? ('true') : ('false')"		}			// 17
			, {"false"	, "(dead)  ? ('true') : ('false')"		}			// 17
			};
		info_printf("Input JSON:\n%s", inputJson.begin());
		::gpk::SJSONReader								jsonReaderEasy;
		gpk_necall(::gpk::jsonParse(jsonReaderEasy, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
		for(uint32_t iTest = 0; iTest < ::gpk::size(pairsResultExpression); ++iTest)
			ree_if(errored(testJSONExpression(jsonReaderEasy, pairsResultExpression[iTest].Val, pairsResultExpression[iTest].Key)), "Failed to resolve expression: %s", pairsResultExpression[iTest].Val.begin());
	}
	const ::gpk::view_const_string					format						=
		"I want to replace this (but not \\{this}): \n{people['1'].name}'s {properties[selection.active ? selection.index : '1'].name}: // this comment should appear here"
		"\n{people['1'].property.(properties[selection.active ? selection.index : '1'].name // this comment should not appear here"
		"\n)}."
		;
	{
		const ::gpk::view_const_string					inputJson				=
			"\n	{ \"properties\" : [{ \"name\" : \"age\", \"type\" : \"int\"}, { \"name\" : \"color\", \"type\" : \"string\"}, { \"name\" : \"race\", \"type\" : \"string\"} ]"
			"\n	, \"selection\" : {\"index\" : 2, \"active\" : true }"
			"\n	, \"people\" : "
			"\n		[{ \"name\" : \"David\""
			"\n		 , \"property\" : "
			"\n			{ \"color\" : \"orange\", \"age\" : 16, \"race\" : \"fat\" }"
			"\n		},"
			"\n		{ \"name\" : \"Charles\" // this is another comment"
			"\n		 , \"property\" : "
			"\n			{ \"color\" : \"green\", \"age\" : 32, \"race\" : \"thin\" }"
			"\n		},"
			"\n		]"
			"\n	}"
			;
		::gpk::TKeyValConstString						pairsResultExpression[]			=
			{ { "age"	, "properties[('0')].name"}
			, { "age"	, "properties[selection.active ? '0' : selection.index].name"}
			, {"race"	, "selection.active ? properties[(selection.index)].name : 'No selection.'"}
			, {"thin"	, "people['1'].property.(properties[selection.active ? (selection.index) : '1'].name)"}
			, {"fat"	, "people['0'].property.(properties[selection.active ? selection.index : ('0')].name)"}
			, {"true"	, "selection.active"}
			, {"16"		, "people['0'].property.(properties[(selection.active) ? '0' : selection.index].name)"}
			, {"16"		, "people['0'].property.(properties[(selection.active) ? '0' : (selection.index)].name)"}
			, {"16"		, "people['0'].property.(properties[(selection.active) ? ('0') : (selection.index)].name)"}
			};
		info_printf("Input JSON:\n%s", inputJson.begin());
		::gpk::SJSONReader								jsonReaderHard;
		gpk_necall(::gpk::jsonParse(jsonReaderHard, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
		for(uint32_t iTest = 0; iTest < ::gpk::size(pairsResultExpression); ++iTest) {
			ree_if(errored(testJSONExpression(jsonReaderHard, pairsResultExpression[iTest].Val, pairsResultExpression[iTest].Key)), "Failed to resolve expression: %s", pairsResultExpression[iTest].Val.begin());
		}
		gpk_necall(::testJSONFormatter(jsonReaderHard, format, inputJson), "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJson.begin());
	}
	{
		const ::gpk::view_const_string					inputJson				=
			"\n	{ \"properties\" : [{ \"name\" : \"age\", \"type\" : \"int\"}, { \"name\" : \"color\", \"type\" : \"string\"}, { \"name\" : \"race\", \"type\" : \"string\"} ]"
			"\n	, \"selection\" : {\"index\" : 0, \"active\" : false }"
			"\n	, \"people\" : "
			"\n		[{ \"name\" : \"David\""
			"\n		 , \"property\" : "
			"\n			{ \"color\" : \"blue\", \"age\" : 25, \"race\" : \"fat\" }"
			"\n		},"
			"\n		{ \"name\" : \"Charles\" // this is another comment"
			"\n		 , \"property\" : "
			"\n			{ \"color\" : \"green\", \"age\" : 32, \"race\" : \"thin\" }"
			"\n		},"
			"\n		]"
			"\n	}"
			;
		::gpk::TKeyValConstString						pairsResultExpression[]			=
			{ {"1"			, "(selection.active) ? (selection.index) : ('1')"}
			//{ {"false"			, "selection.active"}
			, {"No selection."	, "selection.active ? properties[selection.index].name : \"No selection.\""}
			, {"age"			, "selection.active ? \"No selection.\" : properties[selection.index].name"}
			, {"green"			, "people['1'].property.(properties[selection.active ? selection.index : '1'].name)"}
			, {"green"			, "people['1'].property.(properties[selection.active ? (selection.index) : '1'].name)"}
			, {"green"			, "people['1'].property.(properties[selection.active ? (selection.index) : ('1')].name)"}
			, {"green"			, "people['1'].property.(properties[(selection.active) ? selection.index : '1'].name)"}
			, {"green"			, "people['1'].property.(properties[selection.active ? (selection.index) : '1'].name)"}
			, {"green"			, "people['1'].property.(properties[selection.active ? (selection.index) : ('1')].name)"}
			, {"25"				, "people['0'].property.(properties[(selection.active) ? selection.index : '0'].name)"}
			};
		info_printf("Input JSON:\n%s", inputJson.begin());
		::gpk::SJSONReader								jsonReaderHard;
		gpk_necall(::gpk::jsonParse(jsonReaderHard, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
		for(uint32_t iTest = 0; iTest < ::gpk::size(pairsResultExpression); ++iTest)
			ree_if(errored(testJSONExpression(jsonReaderHard, pairsResultExpression[iTest].Val, pairsResultExpression[iTest].Key)), "Failed to resolve expression: %s", pairsResultExpression[iTest].Val.begin());
		gpk_necall(::testJSONFormatter(jsonReaderHard, format, inputJson), "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJson.begin());
	}

	return 0;
}
