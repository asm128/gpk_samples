#define GPK_STDOUT_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_json.h"
#include "gpk_json_expression.h"

::gpk::error_t								printNode						(::gpk::SJSONNode* node, const ::gpk::vcc& testJson)			{
	const ::gpk::array_pod<char>						viewPrintable					= ::gpk::toString({&testJson[node->Token->Span.Begin], node->Token->Span.End - node->Token->Span.Begin});
	const ::gpk::vcc						viewTokenType					= ::gpk::get_value_label(node->Token->Type);
	//info_printf("Entering JSON node type: %u (%s). Node Span: {%u, %u}. Parent index: %u. Object index: %u. Text: %s", node->Token->Type, viewTokenType.begin(), node->Token->Span.Begin, node->Token->Span.End, node->Token->ParentIndex, node->ObjectIndex, viewPrintable.begin());
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], testJson);
	//info_printf("Exiting JSON node type: %u (%s). Node Span: {%u, %u}. Parent index: %u. Object index: %u. Text: %s", node->Token->Type, viewTokenType.begin(), node->Token->Span.Begin, node->Token->Span.End, node->Token->ParentIndex, node->ObjectIndex, viewPrintable.begin());
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

	info_printf("---------------------------- Linear iteration: %u objects.", jsonReader.Token.size());
	::gpk::array_pod<char>						outputJson;
	::gpk::pobj<::gpk::SJSONNode>				root							= jsonReader.Tree[0];
	::printNode(root, testJson);

	info_printf("---------------------------- Linear iteration: %u objects.", jsonReader.Token.size());
	for(uint32_t iNode = 0; iNode < jsonReader.Token.size(); ++iNode) {
		const gpk::SJSONToken							& node							= jsonReader.Token[iNode];
		if( ::gpk::JSON_TYPE_VALUE	== node.Type
		 || ::gpk::JSON_TYPE_KEY	== node.Type
		 )
			continue;
		::gpk::vcc							view							= jsonReader.View[iNode];
		char											bufferFormat [8192]				= {};
		uint32_t										lenString						= view.size();
		sprintf_s(bufferFormat, "Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
		info_printf(bufferFormat, node.Type, ::gpk::get_value_label(node.Type).begin(), node.Span.Begin, node.Span.End, node.ParentIndex, iNode, view.begin());
	}

	const ::gpk::view_const_string					test_key						= "Bleh";
	info_printf("---------------------------- Access test. Test key: %s", test_key.begin());
	::gpk::error_t									indexOfFirstObject				= ::gpk::jsonArrayValueGet(*root, 0);
	::gpk::pobj<::gpk::SJSONNode>				object							= jsonReader.Tree[indexOfFirstObject];
	::gpk::error_t									indexOfElement					= ::gpk::jsonObjectValueGet(*object, jsonReader.View, test_key);
	gpk_necall((uint32_t)indexOfElement >= jsonReader.View.size(), "Test key '%s' not found: %i.", test_key.begin(), indexOfElement);
	const gpk::SJSONToken							& node							= jsonReader.Token	[indexOfElement];
	::gpk::vcc							view							= jsonReader.View	[indexOfElement - 1];	// get the parent in order to retrieve the view with the surrounding in the case of strings "".
	char											bufferFormat [8192]				= {};
	uint32_t										lenString						= view.size();
	sprintf_s(bufferFormat, "Found test value {'%%s':%%%u.%us}. Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u.", lenString, lenString);
	info_printf(bufferFormat, test_key.begin(), view.begin(), node.Type, ::gpk::get_value_label(node.Type).begin(), node.Span.Begin, node.Span.End, node.ParentIndex, indexOfElement, view.begin());
	return 0;
}

	::gpk::error_t						testJSONFormatter			(::gpk::SJSONReader & jsonReader, const ::gpk::vcc format, const ::gpk::vcc inputJson)			{
	::gpk::pobj<::gpk::SJSONNode>			root						= jsonReader.Tree[0];
	::printNode(root, inputJson);
	const ::gpk::error_t						indexOfFirstObject			= 0;
	info_printf("Test format:\n%s", format.begin());
	::gpk::array_pod<char>						formatted;
	gpk_necall(::gpk::jsonStringFormat(format, jsonReader, indexOfFirstObject, formatted), "%s", "Error formatting string from JSON object.");
	info_printf("Formatted string after jsonStringFormat():\n%s", formatted.begin());
	return 0;
}

	::gpk::error_t						testJSONExpression
	(	const ::gpk::SJSONReader			& jsonReader
	,	const ::gpk::vcc		expression
	,	const ::gpk::vcc		expected
	) {
	info_printf("Testing expression: %s", ::gpk::toString(expression).begin());
	::gpk::view_const_string						result;
	::gpk::array_pod<char>						output;
	if(-1 == ::gpk::jsonExpressionResolve(expression, jsonReader, 0U, result) || result != expected) {
		error_printf("Failed to resolve expression. \nExpression: %s \nExpected: %s \nResult: ", expression.begin(), expected.begin());
		output										= result;
		output.append("\n");
		info_printf("%s", output.begin());
		return -1;
	}
	info_printf("Test succeeded. \nExpression: %s \nExpected: %s \nResult: ", expression.begin(), expected.begin());
	output										= result;
	output.append("\n");
	info_printf("%s", output.begin());
	return 0;
}

int											main						()	{
	//gpk_necall(::testJSONReader(), "%s", "Failed to read JSON!");
	uint32_t										testsSucceeded			= 0;
	const ::gpk::view_const_string					inputJson				=
		"\n{\t \"name\"           : \"Carlos\""
		"\n\t, \"height\"         : 1.60"
		"\n\t, \"color\"          : \"brown\""
		"\n\t, \"race\"           : \"red\""
		"\n\t, \"weight\"         : 160"
		"\n\t, \"bpm\"            : 160."
		"\n\t, \"hindsight\"      : 1.600000000"
		"\n\t, \"alive\"          : true"
		"\n\t, \"married\"        : false"
		"\n\t, \"children\"       : [\"Rene\", null, \"Jamie\"]"
		"\n\t, \"selection\"      : { \"index\" : 2, \"name\" : \"children\", \"active\" : false , \"default_property_name_path\" : \"properties['0'].name\" }"
		"\n\t, \"parents\"        : "
		"\n\t    [{ \"name\"    : \"Alice\""
		"\n\t    , \"property\" : "
		"\n\t    { \"color\"    : \"orange\", \"age\" : 16, \"race\" : \"fat\" }"
		"\n\t    },"
		"\n\t    { \"name\"     : \"Bob\" // this is another comment"
		"\n\t    , \"property\" : "
		"\n\t    { \"color\"    : \"green\", \"age\" : 32, \"race\" : \"thin\" }"
		"\n\t    }]"
		"\n\t, \"properties\" : [{ \"name\" : \"age\", \"type\" : \"int\"}, { \"name\" : \"color\", \"type\" : \"string\"}, { \"name\" : \"race\", \"type\" : \"string\"} ]"
		"\n}"
		;
	::gpk::SJSONReader								jsonReader;
	::gpk::TKeyValConstString						resultExpression[]		=
		{ {::gpk::vcs{"true"			}, ::gpk::vcs{"weight			== bpm																							" }}	//   0
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"height			== hindsight																					" }}	//   0
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"hindsight		== height																						" }}	//   0
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"bpm				== weight																						" }}	//   0
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"height			== weight																						" }}	//   0
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"hindsight		== bpm																							" }}	//   0
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"weight			== height																						" }}	//   0
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"bpm				== hindsight																					" }}	//   0
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"'Not'			== children[('0')]																				" }}	//   0
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"'Found'			== children[('1')]																				" }}	//   1
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"'Here'			== children[('2')]																				" }}	//   2
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"'Perro'			== 'Perre'																						" }}	//   3
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'null'			== 'false'																						" }}	//   3
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'null'			== ''																							" }}	//   3
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'false'			== 'null'																						" }}	//   3
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'false'			== ''																							" }}	//   3
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"	'false'			== 'null'																						" }}	//   3
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"	''				== 'null'																						" }}	//   3
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"	'null'			== 'false'																						" }}	//   3
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"	''				== 'false'																						" }}	//   3
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'Rene'			== children['0']																				" }}	//   4
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'null'			== children['1']																				" }}	//   5
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'false'			== children['1']																				" }}	//   5
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"children[('1')]	== children['1']																				" }}	//   5
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"children[('2')]	== children[('1')]																				" }}	//   5
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'Jamie'			== children[('2')]																				" }}	//   6
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"alive			== married																						" }}	//   7
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!alive			== !married																						" }}	//   8
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(!alive)			==(married)																						" }}	//   9
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!alive			==married																						" }}	//  10
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(!alive)			==(!married)																					" }}	//  11
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(alive )			==(!married)																					" }}	//  12
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!alive			==!married																						" }}	//  13
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!alive			==married																						" }}	//  14
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(alive )			=={ 'married'}																					" }}	//  15
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(alive )			==!{ 'married'}																					" }}	//  15
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"'false'			== 'true'																						" }}	//  16
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'true'			== 'true'																						" }}	//  17
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'false'			== 'false'																						" }}	//  18
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'Perro'			== 'Perro'																						" }}	//  19
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(alive )			&&{ 'married'}																					" }}	//  20
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(alive )			&&( married)																					" }}	//  21
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'false'			|| 'true'																						" }}	//  22
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(alive )			&&(married)																						" }}	//  23
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(alive)			&&(married)																						" }}	//  24
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(alive)			&&( married)																					" }}	//  25
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"'false'			&& 'false'																						" }}	//  26
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"'false'			|| 'false'																						" }}	//  27
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'true'			|| 'false'																						" }}	//  28
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!married			 ? !alive : 'unknown'																			" }}	//  29
		, {::gpk::vcs{"unknown"			}, ::gpk::vcs{"married			? !alive : 'unknown'																			" }}	//  30
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"married			? 'unknown' : !alive 																			" }}	//  31
		, {::gpk::vcs{"unknown"			}, ::gpk::vcs{"!married			 ? 'unknown' : !alive																			" }}	//  32
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(alive )			&&!(married)																					" }}	//  33
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!(alive )		&&!( married)																					" }}	//  34
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!(alive )		&&!(married)																					" }}	//  35
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!(alive)			&&!(married)																					" }}	//  36
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!(alive)			&&!( married)																					" }}	//  37
		, {::gpk::vcs{"unknown"			}, ::gpk::vcs{"married			&& alive && married ? !alive : 'unknown'														" }}	//  38
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(alive && alive)	&& alive &&!( married)																			" }}	//  39
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(alive)			&&!(married)																					" }}	//  40
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(alive)			&&!( married)																					" }}	//  41
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(!married) &&!( married)																							" }}	//  42
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"{'alive'}&&( married)																							" }}	//  43
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"{'alive'}&&(married)																								" }}	//  44
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"{'alive'}&&(married)																								" }}	//  45
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"{'alive'} &&( married)																							" }}	//  46
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(alive )&&{'married' }																							" }}	//  47
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(alive) &&{'married' }																							" }}	//  48
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(alive) &&{ 'married'}																							" }}	//  49
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"alive && married																									" }}	//  50
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"alive &&married																									" }}	//  51
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"alive&&married																									" }}	//  52
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"alive&& married																									" }}	//  53
		, {::gpk::vcs{"it's alive!"		}, ::gpk::vcs{"(!(!(alive))) ? \"it's alive!\" : 'dead!'																		" }}	//  54
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"alive																											" }}	//  55
		, {::gpk::vcs{"it's alive!"		}, ::gpk::vcs{" alive  ? \"it's alive!\" : 'dead!'																				" }}	//  56
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(alive)																											" }}	//  57
		, {::gpk::vcs{"it's alive!"		}, ::gpk::vcs{"(alive) ? \"it's alive!\" : 'dead!'																				" }}	//  58
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!(alive)																											" }}	//  59
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(!(alive))																										" }}	//  60
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!(!(alive))																										" }}	//  61
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(!(!(alive)))																									" }}	//  62
		, {::gpk::vcs{"Carlos"			}, ::gpk::vcs{"name																												" }}	//  63
		, {::gpk::vcs{"1.60"			}, ::gpk::vcs{"height																											" }}	//  64
		, {::gpk::vcs{"brown"			}, ::gpk::vcs{"color																											" }}	//  65
		, {::gpk::vcs{"b"				}, ::gpk::vcs{"color['0']																										" }}	//  66
		, {::gpk::vcs{"red"				}, ::gpk::vcs{"race																												" }}	//  67
		, {::gpk::vcs{"160"				}, ::gpk::vcs{"weight																											" }}	//  68
		, {::gpk::vcs{"Carlos"			}, ::gpk::vcs{"(name)																											" }}	//  69
		, {::gpk::vcs{"1.60"			}, ::gpk::vcs{"(height)																											" }}	//  70
		, {::gpk::vcs{"brown"			}, ::gpk::vcs{"(color)																											" }}	//  71
		, {::gpk::vcs{"red"				}, ::gpk::vcs{"(race)																											" }}	//  72
		, {::gpk::vcs{"160"				}, ::gpk::vcs{"(weight)																											" }}	//  73
		, {::gpk::vcs{"2"				}, ::gpk::vcs{"selection.index																									" }}	//  74
		, {::gpk::vcs{"2"				}, ::gpk::vcs{"(selection.('index'))																							" }}	//  75
		, {::gpk::vcs{"2"				}, ::gpk::vcs{"((selection).index)																								" }}	//  76
		, {::gpk::vcs{"Not married"		}, ::gpk::vcs{" married  ? 'true' : 'Not married'																				" }}	//  77
		, {::gpk::vcs{"Not married"		}, ::gpk::vcs{"(married) ? 'true' : 'Not married'																				" }}	//  78
		, {::gpk::vcs{"true"			}, ::gpk::vcs{" alive  ? ('true') : ('false')																					" }}	//  79
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!!(!!alive) ? ('true') : ('false')																				" }}	//  80
		, {::gpk::vcs{"false"			}, ::gpk::vcs{" married  ? ('true') : ('false')																					" }}	//  81
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(married) ? ('true') : ('false')																					" }}	//  82
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(alive  ? (married ? 'false' : 'true') )																			" }}	//  83
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"(alive  ? married ? 'false' : 'true' : 'unknown')																" }}	//  84
		, {::gpk::vcs{"alive"			}, ::gpk::vcs{"(!alive || married ? 'dead' : 'alive')																			" }}	//  85
		, {::gpk::vcs{"true"			}, ::gpk::vcs{" alive  ? married ? 'false' : 'true' : 'unknown'																	" }}	//  86
		, {::gpk::vcs{"alive"			}, ::gpk::vcs{"(alive && !married ? 'alive' : 'dead')																			" }}	//  87
		, {::gpk::vcs{"it's alive!"		}, ::gpk::vcs{"!alive? 'dead!' : \"it's alive!\"																				" }}	//  88
		, {::gpk::vcs{"it's alive!"		}, ::gpk::vcs{"!!(alive) ? \"it's alive!\" : 'dead!'																			" }}	//  89
		, {::gpk::vcs{"it's alive!"		}, ::gpk::vcs{"(!alive) ? 'dead!' : \"it's alive!\"																				" }}	//  90
		, {::gpk::vcs{"Rene"			}, ::gpk::vcs{"children[('0')]																									" }}	//  91
		, {::gpk::vcs{"null"			}, ::gpk::vcs{"children[('1')]																									" }}	//  92
		, {::gpk::vcs{"Jamie"			}, ::gpk::vcs{"children[('2')]																									" }}	//  93
		, {::gpk::vcs{"Jamie"			}, ::gpk::vcs{"children[selection.index]																						" }}	//  94
		, {::gpk::vcs{"Jamie"			}, ::gpk::vcs{"children[(selection.('index'))]																					" }}	//  95
		, {::gpk::vcs{"Jamie"			}, ::gpk::vcs{"children[({'selection'}.index)]																					" }}	//  96
		, {::gpk::vcs{"Jamie"			}, ::gpk::vcs{"children[{'selection.index'}]																					" }}	//  97
		, {::gpk::vcs{"Alice"			}, ::gpk::vcs{"parents['0'].name																								" }}	//  98
		, {::gpk::vcs{"Bob"				}, ::gpk::vcs{"parents['1'].name																								" }}	//  99
		, {::gpk::vcs{"Alice"			}, ::gpk::vcs{"'Alice'																											" }}	// 100
		, {::gpk::vcs{"Alice"			}, ::gpk::vcs{"((parents['0']).name)																							" }}	// 101
		, {::gpk::vcs{"Bob"				}, ::gpk::vcs{"((parents['1']).name)																							" }}	// 102
		, {::gpk::vcs{"Alice"			}, ::gpk::vcs{"{\"(parents['0']).name\"}																						" }}	// 103
		, {::gpk::vcs{"Bob"				}, ::gpk::vcs{"{\"(parents['1']).name\"}																						" }}	// 104
		, {::gpk::vcs{"2"				}, ::gpk::vcs{"(selection . ('index'))																							" }}	// 105
		, {::gpk::vcs{"2"				}, ::gpk::vcs{"(selection . index)																								" }}	// 106
		, {::gpk::vcs{"null"			}, ::gpk::vcs{"(children[ ( ( '1') )] )																							" }}	// 107
		, {::gpk::vcs{"Jamie"			}, ::gpk::vcs{"(children[((selection.index)) ])																					" }}	// 108
		, {::gpk::vcs{"is alive"		}, ::gpk::vcs{"(alive ? married ? 'not alive' : 'is alive' : 'unknown')															" }}	// 109
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!alive  ? ('is_alive') : (!'is dead')																			" }}	// 110
		, {::gpk::vcs{"is alive"		}, ::gpk::vcs{"(alive  ? (!alive ? 'is_dead' : 'is alive'))																		" }}	// 111
		, {::gpk::vcs{"red"				}, ::gpk::vcs{"married  ? !alive ? 'is_dead' : 'is_alive' : race																" }}	// 112
		, {::gpk::vcs{"unknown"			}, ::gpk::vcs{"(!alive  ? (alive ? 'is_alive' : 'is_dead') : 'unknown')															" }}	// 113
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!'0'																												" }}	// 114
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!''																												" }}	// 115
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!' '																												" }}	// 116
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!'{}'																											" }}	// 117
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!'[]'																											" }}	// 118
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!'0'	&&	!'0'	 																								" }}	// 119
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!''	&&	!''																										" }}	// 120
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!' '	&&	!' '																									" }}	// 121
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!'{}'&&	!'{}'																									" }}	// 122
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!'[]'&&	!'[]'																									" }}	// 123
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!'0'	||	!'0'	 																								" }}	// 124
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!''	||	!''																										" }}	// 125
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"!' '	||	!' '																									" }}	// 126
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!'{}'||	!'{}'																									" }}	// 127
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"!'[]'||	!'[]'																									" }}	// 128
		, {::gpk::vcs{"true"			}, ::gpk::vcs{"'true'  || 'true'																								" }}	// 129
		, {::gpk::vcs{"32"				}, ::gpk::vcs{"parents['1'].property.{selection.default_property_name_path}														" }}	// 130
		, {::gpk::vcs{"0"				}, ::gpk::vcs{"!selection.active		? ('0') : selection.index																" }}	// 131
		, {::gpk::vcs{"0"				}, ::gpk::vcs{"!(selection.active)		? ('0') : selection.index																" }}	// 132
		, {::gpk::vcs{"2"				}, ::gpk::vcs{"(!(selection.active))	? (selection.index)	: '0'																" }}	// 133
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"selection.active																									" }}	// 134
		, {::gpk::vcs{"false"			}, ::gpk::vcs{"(selection.active)																								" }}	// 135
		, {::gpk::vcs{"2"				}, ::gpk::vcs{"(selection.active)	? ('0')	: (selection.index)																	" }}	// 136
		, {::gpk::vcs{"race"			}, ::gpk::vcs{"!selection.active ? properties[selection.index].name : 'No selection.'											" }}	// 137
		, {::gpk::vcs{"No selection."	}, ::gpk::vcs{"selection.active ? 'is active' : 'No selection.'																	" }}	// 138
		, {::gpk::vcs{"age"				}, ::gpk::vcs{"properties[('0')].name																							" }}	// 139
		, {::gpk::vcs{"2"				}, ::gpk::vcs{"(!selection.active) ? (selection.index) : ('1')																	" }}	// 140
		, {::gpk::vcs{"No selection."	}, ::gpk::vcs{"selection.active ? properties[selection.index].name : 'No selection.'											" }}	// 141
		, {::gpk::vcs{"race"			}, ::gpk::vcs{"selection.active ? 'No selection.' : properties[selection.index].name											" }}	// 142
		, {::gpk::vcs{"green"			}, ::gpk::vcs{"parents['1'].property.(properties[selection.active ? selection.index : '1'].name)								" }}	// 143
		, {::gpk::vcs{"green"			}, ::gpk::vcs{"parents['1'].property.(properties[selection.active ? (selection.index) : '1'].name)								" }}	// 144
		, {::gpk::vcs{"thin"			}, ::gpk::vcs{"parents['1'].property.(properties[!selection.active ? (selection.index) : ('1')].name)							" }}	// 145
		, {::gpk::vcs{"green"			}, ::gpk::vcs{"parents['1'].property.(properties[(selection.active) ? selection.index : '1'].name)								" }}	// 146
		, {::gpk::vcs{"16"				}, ::gpk::vcs{"parents['0'].property.(properties[(selection.active) ? selection.index : '0'].name)								" }}	// 147
		, {::gpk::vcs{"2"				}, ::gpk::vcs{"!selection.active	? selection.index : '1'																		" }}	// 148
		, {::gpk::vcs{"1"				}, ::gpk::vcs{"(!selection.active)	? '1' : selection.index																		" }}	// 149
		, {::gpk::vcs{"green"			}, ::gpk::vcs{"parents['1'].property.(properties[!selection.active		? '1'	: selection.index	].name)						" }}	// 150
		, {::gpk::vcs{"green"			}, ::gpk::vcs{"parents['1'].property.(properties[!!!selection.active || married	? '1' : (selection.index)].name)				" }}	// 151
		, {::gpk::vcs{"green"			}, ::gpk::vcs{"parents['1'].property.(properties[!!!selection.active || married	? '1' : (selection.index)].name)				" }}	// 152
		, {::gpk::vcs{"green"			}, ::gpk::vcs{"parents['1'].property.(properties[!selection.active		? ('1')	: (selection.index) ].name)						" }}	// 153
		, {::gpk::vcs{"32"				}, ::gpk::vcs{"parents['1'].property.{selection.default_property_name_path}														" }}	// 154
		, {::gpk::vcs{"orange"			}, ::gpk::vcs{"parents['0'].property.(properties[(!selection.active) ? '1'	: selection.index	].name)							" }}	// 155
		, {::gpk::vcs{"orange"			}, ::gpk::vcs{"parents['0'].property.(properties[!(selection.active) ? '1'	: selection.index	].name)							" }}	// 156
		, {::gpk::vcs{"green"			}, ::gpk::vcs{"parents['1'].property.(properties[!(!!selection.active) && (married || alive) ? '1' : (selection.index)].name)	" }}	// 157
		, {::gpk::vcs{"green"			}, ::gpk::vcs{"parents['1'].property.(properties[!(!!selection.active) && (alive || married) ? '1' : (selection.index)].name)	" }}	// 158
		};

	info_printf("Input JSON:\n%s", inputJson.begin());
	gpk_necall(::gpk::jsonParse(jsonReader, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
	for(uint32_t iTest = 0; iTest < ::gpk::size(resultExpression); ++iTest) {
		gerror_if(::gpk::failed(testJSONExpression(jsonReader, resultExpression[iTest].Val, resultExpression[iTest].Key)), "Failed to resolve expression: %s", ::gpk::toString(resultExpression[iTest].Val).begin())
		else
			++testsSucceeded;
	}

	//::gpk::SJSONFile	fileCases;
	//::gpk::jsonFileRead(fileCases, "case.json");
	//for(uint32_t iCase = 0; iCase < (uint32_t)::gpk::jsonArraySize(*fileCases.Reader[0]); ++iCase) {
	//	const uint32_t			indexMain		= ::gpk::jsonArrayValueGet(*fileCases.Reader[0], iCase);
	//	const uint32_t			indexResult		= ::gpk::jsonArrayValueGet(*fileCases.Reader[indexMain], 0);
	//	const uint32_t			indexScript		= ::gpk::jsonArrayValueGet(*fileCases.Reader[indexMain], 1);
	//	const ::gpk::SJSONNode	& nodeResult	= *fileCases.Reader[indexResult];
	//	const ::gpk::SJSONNode	& nodeScript	= *fileCases.Reader[indexScript];
	//	gerror_if(::gpk::failed(testJSONExpression(jsonReader, fileCases.Reader.View[nodeScript.ObjectIndex], fileCases.Reader.View[nodeResult.ObjectIndex])), "Failed to resolve expression: %s", ::gpk::toString(fileCases.Reader.View[nodeScript.ObjectIndex]).begin())
	//	else
	//		++testsSucceeded;
	//}

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
