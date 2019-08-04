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
		{ {"true"			, "'false' || 'true'"																					}	//   3
		, {"false"			, "'false' && 'false'"																					}	//   0
		, {"false"			, "'false' || 'false'"																					}	//   1
		, {"true"			, "'true'  || 'false'"																					}	//   2
		, {"false"			, "!married ? !alive : 'unknown'"																		}	//   4
		, {"unknown"		, "married ? !alive : 'unknown'"																		}	//   5
		, {"false"			, "married ? 'unknown' : !alive "																		}	//   6
		, {"unknown"		, "!married ? 'unknown' : !alive"																		}	//   7
		, {"true"			, "(alive )&&!(married)"																				}	//   8
		, {"false"			, "!(alive )&&!( married)"																				}	//   9
		, {"false"			, "!(alive )&&!(married)"																				}	//  10
		, {"false"			, "!(alive) &&!(married)"																				}	//  11
		, {"false"			, "!(alive) &&!( married)"																				}	//  12
		, {"unknown"		, "married && alive && married ? !alive : 'unknown'"													}	//  13
		, {"true"			, "(alive && alive) && alive &&!( married)"																}	//  14
		, {"true"			, "(alive) &&!(married)"																				}	//  15
		, {"true"			, "(alive) &&!( married)"																				}	//  16
		, {"true"			, "(!married) &&!( married)"																			}	//  17
		, {"false"			, "(alive )&&( married)"																				}	//  18
		, {"false"			, "(alive )&&(married)"																					}	//  19
		, {"false"			, "(alive) &&(married)"																					}	//  20
		, {"false"			, "(alive) &&( married)"																				}	//  21
		, {"false"			, "{'alive'}&&( married)"																				}	//  22
		, {"false"			, "{'alive'}&&(married)"																				}	//  23
		, {"false"			, "{'alive'}&&(married)"																				}	//  24
		, {"false"			, "{'alive'} &&( married)"																				}	//  25
		, {"false"			, "(alive )&&{ 'married'}"																				}	//  26
		, {"false"			, "(alive )&&{'married' }"																				}	//  27
		, {"false"			, "(alive) &&{'married' }"																				}	//  28
		, {"false"			, "(alive) &&{ 'married'}"																				}	//  29
		, {"false"			, "alive && married"																					}	//  30
		, {"false"			, "alive &&married"																						}	//  31
		, {"false"			, "alive&&married"																						}	//  32
		, {"false"			, "alive&& married"																						}	//  33
		, {"it's alive!"	, "(!(!(alive))) ? \"it's alive!\" : 'dead!'"															}	//  34
		, {"true"			, "alive"																								}	//  35
		, {"it's alive!"	, " alive  ? \"it's alive!\" : 'dead!'"																	}	//  36
		, {"true"			, "(alive)"																								}	//  37
		, {"it's alive!"	, "(alive) ? \"it's alive!\" : 'dead!'"																	}	//  38
		, {"false"			, "!(alive)"																							}	//  39
		, {"false"			, "(!(alive))"																							}	//  40
		, {"true"			, "!(!(alive))"																							}	//  41
		, {"true"			, "(!(!(alive)))"																						}	//  42
		, {"Carlos"			, "name"																								}	//  43
		, {"1.56"			, "height"																								}	//  44
		, {"brown"			, "color"																								}	//  45
		, {"b"				, "color['0']"																							}	//  45
		, {"red"			, "race"																								}	//  46
		, {"160"			, "weight"																								}	//  47
		, {"Carlos"			, "(name)"																								}	//  48
		, {"1.56"			, "(height)"																							}	//  49
		, {"brown"			, "(color)"																								}	//  50
		, {"red"			, "(race)"																								}	//  51
		, {"160"			, "(weight)"																							}	//  52
		, {"2"				, "selection.index"																						}	//  53
		, {"2"				, "(selection.('index'))"																				}	//  54
		, {"2"				, "((selection).index)"																					}	//  55
		, {"Not married"	, " married  ? 'true' : 'Not married'"																	}	//  56
		, {"Not married"	, "(married) ? 'true' : 'Not married'"																	}	//  57
		, {"true"			, " alive  ? ('true') : ('false')"																		}	//  58
		, {"true"			, "!!(!!alive) ? ('true') : ('false')"																	}	//  59
		, {"false"			, " married  ? ('true') : ('false')"																	}	//  60
		, {"false"			, "(married) ? ('true') : ('false')"																	}	//  61
		, {"true"			, "(alive  ? (married ? 'false' : 'true') )"															}	//  62
		, {"true"			, "(alive  ? married ? 'false' : 'true' : 'unknown')"													}	//  63
		, {"alive"			, "(!alive || married ? 'dead' : 'alive')"																}	//  64
		, {"true"			, " alive  ? married ? 'false' : 'true' : 'unknown'"													}	//  65
		, {"alive"			, "(alive && !married ? 'alive' : 'dead')"																}	//  66
		, {"it's alive!"	, "!alive? 'dead!' : \"it's alive!\""																	}	//  67
		, {"it's alive!"	, "!!(alive) ? \"it's alive!\" : 'dead!'"																}	//  68
		, {"it's alive!"	, "(!alive) ? 'dead!' : \"it's alive!\""																}	//  69
		, {"Rene"			, "children[('0')]"																						}	//  70
		, {"null"			, "children[('1')]"																						}	//  71
		, {"Jamie"			, "children[('2')]"																						}	//  72
		, {"Jamie"			, "children[selection.index]"																			}	//  73
		, {"Jamie"			, "children[(selection.('index'))]"																		}	//  74
		, {"Jamie"			, "children[({'selection'}.index)]"																		}	//  75
		, {"Jamie"			, "children[{'selection.index'}]"																		}	//  76
		, {"Alice"			, "parents['0'].name"																					}	//  77
		, {"Bob"			, "parents['1'].name"																					}	//  78
		, {"Alice"			, "'Alice'"																								}	//  79
		, {"Alice"			, "((parents['0']).name)"																				}	//  80
		, {"Bob"			, "((parents['1']).name)"																				}	//  81
		, {"Alice"			, "{\"(parents['0']).name\"}"																			}	//  82
		, {"Bob"			, "{\"(parents['1']).name\"}"																			}	//  83
		, {"2"				, "(selection . ('index'))"																				}	//  84
		, {"2"				, "(selection . index)"																					}	//  85
		, {"null"			, "(children[ ( ( '1') )] )"																			}	//  86
		, {"Jamie"			, "(children[((selection.index)) ])"																	}	//  87
		, {"is alive"		, "(alive ? married ? 'not alive' : 'is alive' : 'unknown')"											}	//  88
		, {"false"			, "!alive  ? ('is_alive') : (!'is dead')"																}	//  89
		, {"is alive"		, "(alive  ? (!alive ? 'is_dead' : 'is alive'))"														}	//  90
		, {"red"			, "married  ? !alive ? 'is_dead' : 'is_alive' : race"													}	//  91
		, {"unknown"		, "(!alive  ? (alive ? 'is_alive' : 'is_dead') : 'unknown')"											}	//  92
		, {"true"			, "!'0'"																								}	//  93
		, {"true"			, "!''"																									}	//  94
		, {"false"			, "!' '"																								}	//  95
		, {"true"			, "!'{}'"																								}	//  96
		, {"true"			, "!'[]'"																								}	//  97
		, {"true"			, "!'0'	&&	!'0'	 "																					}	//  98
		, {"true"			, "!''	&&	!''"																						}	//  99
		, {"false"			, "!' '	&&	!' '"																						}	// 100
		, {"true"			, "!'{}'&&	!'{}'"																						}	// 101
		, {"true"			, "!'[]'&&	!'[]'"																						}	// 102
		, {"true"			, "!'0'	||	!'0'	 "																					}	// 103
		, {"true"			, "!''	||	!''"																						}	// 104
		, {"false"			, "!' '	||	!' '"																						}	// 105
		, {"true"			, "!'{}'||	!'{}'"																						}	// 106
		, {"true"			, "!'[]'||	!'[]'"																						}	// 107
		, {"true"			, "'true'  || 'true'"																					}	// 108
		, {"32"				, "parents['1'].property.{selection.default_property_name_path}"										}	// 109
		, {"0"				, "!selection.active		? ('0') : selection.index"													}	// 110
		, {"0"				, "!(selection.active)		? ('0') : selection.index"													}	// 111
		, {"2"				, "(!(selection.active))	? (selection.index)	: '0'"													}	// 112
		, {"false"			, "selection.active"																					}	// 113
		, {"false"			, "(selection.active)"																					}	// 114
		, {"2"				, "(selection.active)	? ('0')	: (selection.index)"													}	// 115
		, {"race"			, "!selection.active ? properties[selection.index].name : 'No selection.'"								}	// 116
		, {"No selection."	, "selection.active ? 'is active' : 'No selection.'"													}	// 117
		, {"age"			, "properties[('0')].name"																				}	// 118
		, {"2"				, "(!selection.active) ? (selection.index) : ('1')"														}	// 119
		, {"No selection."	, "selection.active ? properties[selection.index].name : \"No selection.\""								}	// 120
		, {"race"			, "selection.active ? \"No selection.\" : properties[selection.index].name"								}	// 121
		, {"green"			, "parents['1'].property.(properties[selection.active ? selection.index : '1'].name)"					}	// 122
		, {"green"			, "parents['1'].property.(properties[selection.active ? (selection.index) : '1'].name)"					}	// 123
		, {"thin"			, "parents['1'].property.(properties[!selection.active ? (selection.index) : ('1')].name)"				}	// 124
		, {"green"			, "parents['1'].property.(properties[(selection.active) ? selection.index : '1'].name)"					}	// 125
		, {"16"				, "parents['0'].property.(properties[(selection.active) ? selection.index : '0'].name)"					}	// 126
		, {"2"				, "!selection.active	? selection.index : '1'"														}	// 127
		, {"1"				, "(!selection.active)	? '1' : selection.index"														}	// 128
		, {"green"			, "parents['1'].property.(properties[!selection.active		? '1'	: selection.index	].name)"		}	// 129
		, {"green"			, "parents['1'].property.(properties[!!!selection.active	? '1'	: (selection.index) ].name)"		}	// 130
		, {"green"			, "parents['1'].property.(properties[!selection.active		? ('1')	: (selection.index) ].name)"		}	// 131
		, {"32"				, "parents['1'].property.{selection.default_property_name_path}"										}	// 132
		, {"orange"			, "parents['0'].property.(properties[(!selection.active) ? '1'	: selection.index	].name)"			}	// 133
		, {"orange"			, "parents['0'].property.(properties[!(selection.active) ? '1'	: selection.index	].name)"			}	// 134
		};

	info_printf("Input JSON:\n%s", inputJson.begin());
	gpk_necall(::gpk::jsonParse(jsonReader, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
	for(uint32_t iTest = 0; iTest < ::gpk::size(resultExpression); ++iTest) {
		gerror_if(errored(testJSONExpression(jsonReader, resultExpression[iTest].Val, resultExpression[iTest].Key)), "Failed to resolve expression: %s", resultExpression[iTest].Val.begin())
		else
			++testsSucceeded;
	}

	const ::gpk::view_const_string					format						=
		"I want to replace this (but not \\{this}): \n{parents['1'].name}'s {properties[selection.active ? selection.index : '1'].name}: // this comment should appear here"
		"\n{parents['1'].property.(properties[selection.active ? selection.index : '1'].name // this comment should not appear here"
		"\n)}."
		;
	gerror_if(::testJSONFormatter(jsonReader, format, inputJson), "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJson.begin())
	else
		++testsSucceeded;
	info_printf("%u tests executed successfully", testsSucceeded);
	return 0;
}
