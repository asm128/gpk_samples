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
		const gpk::SJSONToken							& node							= jsonReader.Object[iNode];
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
	const gpk::SJSONToken							& node							= jsonReader.Object	[indexOfElement];
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
	//gpk_necall(::testJSONReader(), "%s", "Failed to read JSON!");
	uint32_t										testsSucceeded			= 0;
	{
		const ::gpk::view_const_string					inputJson				=
			"\n{\t \"name\"           : \"Carlos\""
			"\n\t, \"height\"         : \"1.56\""
			"\n\t, \"color\"          : \"brown\""
			"\n\t, \"race\"           : \"red\""
			"\n\t, \"weight\"         : 160"
			"\n\t, \"alive\"          : true"
			"\n\t, \"married\"        : false"
			"\n\t, \"children\"       : [\"Rene\", null, \"Jamie\"]"
			"\n\t, \"selection\"      : { \"index\" : 2, \"name\" : \"children\", \"active\" : false , \"default_property_name_path\" : \"properties['0'].name\" }"
			"\n\t, \"parents\"        : "
			"\n\t    [{ \"name\" : \"Alice\""
			"\n\t        , \"property\" : "
			"\n\t        { \"color\" : \"orange\", \"age\" : 16, \"race\" : \"fat\" }"
			"\n\t    },"
			"\n\t    { \"name\" : \"Bob\" // this is another comment"
			"\n\t        , \"property\" : "
			"\n\t        { \"color\" : \"green\", \"age\" : 32, \"race\" : \"thin\" }"
			"\n\t    }"
			"\n\t    ]"
			"\n\t, \"properties\" : [{ \"name\" : \"age\", \"type\" : \"int\"}, { \"name\" : \"color\", \"type\" : \"string\"}, { \"name\" : \"race\", \"type\" : \"string\"} ]"
			"\n}"
			;
		::gpk::SJSONReader								jsonReader;
		::gpk::TKeyValConstString						resultExpression[]		=
			{ {"false"			, "'false' || 'false'"														}	// 94
			, {"true"			, "'true'  || 'false'"													}	// 92
			, {"true"			, "'false' || 'true'"														}	// 93
			, {"false"			, "!married ? !alive : 'unknown'"										}	// 0
			, {"unknown"		, "married ? !alive : 'unknown'"										}	// 1
			, {"false"			, "married ? 'unknown' : !alive "										}	// 2
			, {"unknown"		, "!married ? 'unknown' : !alive"										}	// 3
			, {"true"			, "(alive )&&!(married)"												}	// 4
			, {"false"			, "!(alive )&&!( married)"												}	// 5
			, {"false"			, "!(alive )&&!(married)"												}	// 6
			, {"false"			, "!(alive) &&!(married)"												}	// 7
			, {"false"			, "!(alive) &&!( married)"												}	// 8
			, {"unknown"		, "married && alive && married ? !alive : 'unknown'"					}	// 9
			, {"true"			, "(alive && alive) && alive &&!( married)"								}	// 10
			, {"true"			, "(alive) &&!(married)"												}	// 11
			, {"true"			, "(alive) &&!( married)"												}	// 12
			, {"true"			, "(!married) &&!( married)"											}	// 13
			, {"false"			, "(alive )&&( married)"												}	// 14
			, {"false"			, "(alive )&&(married)"													}	// 15
			, {"false"			, "(alive) &&(married)"													}	// 16
			, {"false"			, "(alive) &&( married)"												}	// 17
			, {"false"			, "{'alive'}&&( married)"												}	// 18
			, {"false"			, "{'alive'}&&(married)"												}	// 19
			, {"false"			, "{'alive'}&&(married)"												}	// 20
			, {"false"			, "{'alive'} &&( married)"												}	// 21
			, {"false"			, "(alive )&&{ 'married'}"												}	// 22
			, {"false"			, "(alive )&&{'married' }"												}	// 23
			, {"false"			, "(alive) &&{'married' }"												}	// 24
			, {"false"			, "(alive) &&{ 'married'}"												}	// 25
			, {"false"			, "alive && married"													}	// 26
			, {"false"			, "alive &&married"														}	// 27
			, {"false"			, "alive&&married"														}	// 28
			, {"false"			, "alive&& married"														}	// 29
			, {"it's alive!"	, "(!(!(alive))) ? \"it's alive!\" : 'dead!'"							}	// 30
			, {"true"			, "alive"																}	// 31
			, {"it's alive!"	, " alive  ? \"it's alive!\" : 'dead!'"									}	// 32
			, {"true"			, "(alive)"																}	// 33
			, {"it's alive!"	, "(alive) ? \"it's alive!\" : 'dead!'"									}	// 34
			, {"false"			, "!(alive)"															}	// 35
			, {"false"			, "(!(alive))"															}	// 36
			, {"true"			, "!(!(alive))"															}	// 37
			, {"true"			, "(!(!(alive)))"														}	// 38
			, {"Carlos"			, "name"																}	// 39
			, {"1.56"			, "height"																}	// 40
			, {"brown"			, "color"																}	// 41
			, {"red"			, "race"																}	// 42
			, {"160"			, "weight"																}	// 43
			, {"Carlos"			, "(name)"																}	// 44
			, {"1.56"			, "(height)"															}	// 45
			, {"brown"			, "(color)"																}	// 46
			, {"red"			, "(race)"																}	// 47
			, {"160"			, "(weight)"															}	// 48
			, {"2"				, "selection.index"														}	// 49
			, {"2"				, "(selection.('index'))"												}	// 50
			, {"2"				, "((selection).index)"													}	// 51
			, {"Not married"	, " married  ? 'true' : 'Not married'"									}	// 52
			, {"Not married"	, "(married) ? 'true' : 'Not married'"									}	// 53
			, {"true"			, " alive  ? ('true') : ('false')"										}	// 54
			, {"true"			, "!!(!!alive) ? ('true') : ('false')"									}	// 55
			, {"false"			, " married  ? ('true') : ('false')"									}	// 56
			, {"false"			, "(married) ? ('true') : ('false')"									}	// 57
			, {"true"			, "(alive  ? (married ? 'false' : 'true') )"							}	// 58
			, {"true"			, "(alive  ? married ? 'false' : 'true' : 'unknown')"					}	// 59
			, {"alive"			, "(!alive || married ? 'dead' : 'alive')"							}	// 59
			, {"true"			, " alive  ? married ? 'false' : 'true' : 'unknown'"					}	// 60
			, {"alive"			, "(alive && !married ? 'alive' : 'dead')"							}	// 59
			, {"it's alive!"	, "!alive? 'dead!' : \"it's alive!\""									}	// 61
			, {"it's alive!"	, "!!(alive) ? \"it's alive!\" : 'dead!'"								}	// 62
			, {"it's alive!"	, "(!alive) ? 'dead!' : \"it's alive!\""								}	// 63
			, {"Rene"			, "children[('0')]"														}	// 64
			, {"null"			, "children[('1')]"														}	// 65
			, {"Jamie"			, "children[('2')]"														}	// 66
			, {"Jamie"			, "children[selection.index]"											}	// 67
			, {"Jamie"			, "children[(selection.('index'))]"										}	// 68
			, {"Jamie"			, "children[({'selection'}.index)]"										}	// 69
			, {"Jamie"			, "children[{'selection.index'}]"										}	// 70
			, {"Alice"			, "parents['0'].name"													}	// 71
			, {"Bob"			, "parents['1'].name"													}	// 72
			, {"Alice"			, "'Alice'"																}	// 73
			, {"Alice"			, "((parents['0']).name)"												}	// 74
			, {"Bob"			, "((parents['1']).name)"												}	// 75
			, {"Alice"			, "{\"(parents['0']).name\"}"											}	// 76
			, {"Bob"			, "{\"(parents['1']).name\"}"											}	// 77
			, {"2"				, "(selection . ('index'))"												}	// 78
			, {"2"				, "(selection . index)"													}	// 79
			, {"null"			, "(children[ ( ( '1') )] )"											}	// 80
			, {"Jamie"			, "(children[((selection.index)) ])"									}	// 81
			, {"is alive"		, "(alive ? married ? 'not alive' : 'is alive' : 'unknown')"			}	// 82
			, {"false"			, "!alive  ? ('is_alive') : (!'is dead')"								}	// 83
			, {"is alive"		, "(alive  ? (!alive ? 'is_dead' : 'is alive'))"						}	// 84
			, {"red"			, "married  ? !alive ? 'is_dead' : 'is_alive' : race"					}	// 85
			, {"unknown"		, "(!alive  ? (alive ? 'is_alive' : 'is_dead') : 'unknown')"			}	// 86
			, {"true"			, "!'0'"																}	// 87
			, {"true"			, "!''"																	}	// 88
			, {"false"			, "!' '"																}	// 89
			, {"true"			, "!'{}'"																}	// 90
			, {"true"			, "!'[]'"																}	// 91
			, {"true"			, "!'0'	&&	!'0'	 "													}	// 92
			, {"true"			, "!''	&&	!''"														}	// 93
			, {"false"			, "!' '	&&	!' '"														}	// 94
			, {"true"			, "!'{}'&&	!'{}'"														}	// 95
			, {"true"			, "!'[]'&&	!'[]'"														}	// 96
			, {"true"			, "!'0'	||	!'0'	 "													}	// 92
			, {"true"			, "!''	||	!''"														}	// 93
			, {"false"			, "!' '	||	!' '"														}	// 94
			, {"true"			, "!'{}'||	!'{}'"														}	// 95
			, {"true"			, "!'[]'||	!'[]'"														}	// 96
			, {"true"			, "'true'  || 'true'"														}	// 95

			, {"32"				, "parents['1'].property.{selection.default_property_name_path}"		}	// 97
			};

			info_printf("Input JSON:\n%s", inputJson.begin());
			gpk_necall(::gpk::jsonParse(jsonReader, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
			for(uint32_t iTest = 0; iTest < ::gpk::size(resultExpression); ++iTest) {
				gerror_if(errored(testJSONExpression(jsonReader, resultExpression[iTest].Val, resultExpression[iTest].Key)), "Failed to resolve expression: %s", resultExpression[iTest].Val.begin())
				else
					++testsSucceeded;
			}

	}
	::gpk::SJSONReader								jsonReader;
	const ::gpk::view_const_string					format						=
		"I want to replace this (but not \\{this}): \n{people['1'].name}'s {properties[selection.active ? selection.index : '1'].name}: // this comment should appear here"
		"\n{people['1'].property.(properties[selection.active ? selection.index : '1'].name // this comment should not appear here"
		"\n)}."
		;
	{
		const ::gpk::view_const_string					inputJson				=
			"\n{ \"properties\" : [{ \"name\" : \"age\", \"type\" : \"int\"}, { \"name\" : \"color\", \"type\" : \"string\"}, { \"name\" : \"race\", \"type\" : \"string\"} ]"
			"\n, \"selection\" : {\"index\" : 2, \"active\" : true }"
			"\n, \"people\" : "
			"\n    [{ \"name\" : \"David\""
			"\n        , \"property\" : "
			"\n        { \"color\" : \"orange\", \"age\" : 16, \"race\" : \"fat\" }"
			"\n    },"
			"\n    { \"name\" : \"Charles\" // this is another comment"
			"\n        , \"property\" : "
			"\n        { \"color\" : \"green\", \"age\" : 32, \"race\" : \"thin\" }"
			"\n    },"
			"\n    ]"
			"\n}"
			;
		::gpk::TKeyValConstString						pairsResultExpression[]			=
			{ {"race"		, "selection.active			? properties[(selection.index)].name : 'No selection.'"								} //  0
			, {"2"			, "!selection.active		? ('0') : selection.index"															} //  1
			, {"2"			, "!(selection.active)		? ('0') : selection.index"															} //  2
			, {"0"			, "(!(selection.active))	? (selection.index)	: '0'"															} //  3
			, {"true"		, "selection.active"																							} //  4
			, {"true"		, "(selection.active)"																							} //  5
			, {"0"			, "(selection.active)	? ('0')	: (selection.index)"															} //  6
			, {"race"		, "selection.active ? properties[selection.index].name : 'No selection.'"										} //  7
			, {"is active"	, "selection.active ? 'is active' : 'No selection.'"															} //  8
			, {"age"		, "properties[('0')].name"																						} //  9
			, {"age"		, "properties[selection.active ? '0' : selection.index].name"													} // 10
			, {"race"		, "selection.active ? selection.index ? properties[(selection.index)].name : 'No selection.'"					} // 11
			, {"thin"		, "people['1'].property.(properties[selection.active		? (selection.index) : '1'].name)"					} // 12
			, {"fat"		, "people['0'].property.(properties[selection.active		? selection.index : ('0')].name)"					} // 13
			, {"16"			, "people['0'].property.(properties[(selection.active)		? '0'	: selection.index]	.name)"					} // 14
			, {"16"			, "people['0'].property.(properties[((selection.active))	? '0'	: (selection.index)].name)"					} // 15
			, {"fat"		, "people['0'].property.(properties[!selection.active		? ('0')				: selection.index	].name)"	} // 16
			, {"fat"		, "people['0'].property.(properties[!selection.active		? ('0')				: selection.index	].name)"	} // 17
			, {"16"			, "people['0'].property.(properties[!(selection.active)		? selection.index	: '0'				].name)"	} // 18
			, {"16"			, "people['0'].property.(properties[(!(selection.active))	? (selection.index)	: '0'				].name)"	} // 19
			, {"16"			, "people['0'].property.(properties[!(selection.active)		? (selection.index)	: ('0')				].name)"	} // 20
			};
		info_printf("Input JSON:\n%s", inputJson.begin());
		jsonReader = {};
		gpk_necall(::gpk::jsonParse(jsonReader, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
		for(uint32_t iTest = 0; iTest < ::gpk::size(pairsResultExpression); ++iTest)
			gerror_if(errored(testJSONExpression(jsonReader, pairsResultExpression[iTest].Val, pairsResultExpression[iTest].Key)), "Failed to resolve expression: %s", pairsResultExpression[iTest].Val.begin())
			else
				++testsSucceeded;
		gerror_if(::testJSONFormatter(jsonReader, format, inputJson), "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJson.begin())
		else
			++testsSucceeded;
	}
	{
		const ::gpk::view_const_string					inputJson				=
			"\n{ \"properties\" : [{ \"name\" : \"age\", \"type\" : \"int\"}, { \"name\" : \"color\", \"type\" : \"string\"}, { \"name\" : \"race\", \"type\" : \"string\"} ]"
			"\n, \"selection\" : { \"index\" : 2, \"active\" : false , \"default_property_name_path\" : \"properties[!selection.active ? ('1')	: (selection.index) ].name\" }"
			"\n, \"people\" : "
			"\n    [    { \"name\" : \"David\""
			"\n         , \"property\" : "
			"\n             { \"color\" : \"blue\", \"age\" : 25, \"race\" : \"fat\" }"
			"\n         },"
			"\n        { \"name\" : \"Charles\" // this is another comment"
			"\n        , \"property\" : "
			"\n            { \"color\" : \"green\", \"age\" : 32, \"race\" : \"thin\" }"
			"\n        },"
			"\n    ]"
			"\n}"
			;
		::gpk::TKeyValConstString						pairsResultExpression[]			=
			{ {"1"				, "(selection.active) ? (selection.index) : ('1')"											}	//  0
			, {"2"				, "(!selection.active) ? (selection.index) : ('1')"											}	//  0
			, {"No selection."	, "selection.active ? properties[selection.index].name : \"No selection.\""					}	//  1
			, {"race"			, "selection.active ? \"No selection.\" : properties[selection.index].name"					}	//  2
			, {"green"			, "people['1'].property.(properties[selection.active ? selection.index : '1'].name)"		}	//  3
			, {"green"			, "people['1'].property.(properties[selection.active ? (selection.index) : '1'].name)"		}	//  4
			, {"green"			, "people['1'].property.(properties[selection.active ? (selection.index) : ('1')].name)"	}	//  5
			, {"green"			, "people['1'].property.(properties[(selection.active) ? selection.index : '1'].name)"		}	//  6
			, {"25"				, "people['0'].property.(properties[(selection.active) ? selection.index : '0'].name)"		}	//  7
			, {"2"				, "!selection.active	? selection.index : '1'"											}	//  8
			, {"1"				, "(!selection.active)	? '1' : selection.index"											}	//  9
			, {"green"			, "people['1'].property.(properties[!selection.active	? '1'	: selection.index	].name)"}	// 10
			, {"green"			, "people['1'].property.(properties[!!!selection.active	? '1'	: (selection.index) ].name)"}	// 11
			, {"green"			, "people['1'].property.(properties[!selection.active	? ('1')	: (selection.index) ].name)"}	// 12
			, {"green"			, "people['1'].property.{selection.default_property_name_path}"}	// 13
			, {"blue"			, "people['0'].property.(properties[(!selection.active) ? '1'	: selection.index	].name)"}	// 14
			, {"blue"			, "people['0'].property.(properties[!(selection.active) ? '1'	: selection.index	].name)"}	// 14
			};
		info_printf("Input JSON:\n%s", inputJson.begin());
		jsonReader = {};
		gpk_necall(::gpk::jsonParse(jsonReader, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
		for(uint32_t iTest = 0; iTest < ::gpk::size(pairsResultExpression); ++iTest)
			gerror_if(errored(testJSONExpression(jsonReader, pairsResultExpression[iTest].Val, pairsResultExpression[iTest].Key)), "Failed to resolve expression: %s", pairsResultExpression[iTest].Val.begin())
			else
				++testsSucceeded;
		gerror_if(::testJSONFormatter(jsonReader, format, inputJson), "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJson.begin())
		else
			++testsSucceeded;
	}
	info_printf("%u tests executed successfully", testsSucceeded);
	return 0;
}
