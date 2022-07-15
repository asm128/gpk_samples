#define GPK_STDOUT_LOG_ENABLED

#include "gpk_xml_reader.h"
#include "gpk_storage.h"

#include <cstdio>

int xmlTestDoc(::gpk::vcc xmlDoc) {
	printf("\nTest XML: %s\n", xmlDoc.begin());
	printf("-------------------\n");

	::gpk::SXMLReader reader;
	gerror_if(xmlParse(reader, xmlDoc), "%s", "Failed to parse %s.", ::gpk::toString(xmlDoc).begin());
	printf("\nNest level: %i", reader.StateRead.NestLevel);
	printf("\nToken count: %i", reader.Token.size());
	printf("\nTokens:");
	for(uint32_t iToken = 0; iToken < reader.Token.size(); ++iToken) {
		printf("\n-------------------");
		const ::gpk::SXMLToken	& token = reader.Token[iToken];
		printf("\nType: %i (%s)"		, token.Type, ::gpk::get_value_label(token.Type).begin());
		printf("\nValue: '%s'"			, ::gpk::toString({&xmlDoc[token.Range.Offset], token.Range.Count}).begin());
		printf("\nOffset: %i"			, token.Range.Offset);
		printf("\nLength: %i"			, token.Range.Count);
		printf("\nParent: %i"			, token.IndexParent);
		printf("\nParent type: %i (%s)"
			, (-1 != token.IndexParent) ? reader.Token[token.IndexParent].Type : -1
			, (-1 != token.IndexParent) ? ::gpk::get_value_label(reader.Token[token.IndexParent].Type).begin() : ""
			);
	}
	return 0;
}

int xmlTestFile(::gpk::vcs xmlFilename) {
	::gpk::array_pod<char_t> testXmlFile = {};
	printf("\ntesting file '%s'", ::gpk::toString(xmlFilename).begin());
	::gpk::fileToMemory(xmlFilename, testXmlFile);
	return xmlTestDoc(testXmlFile);
}

int main(int argc, char** argv) {
	if(argc > 1) {
		const ::gpk::vcs testXmlFilename = ::gpk::vcs{argv[1], (uint32_t)strlen(argv[1])};
	}
	else {
		const ::gpk::vcs testXmlFilenames[] =
			{ "emptynode.xml"
			, "simplenode.xml"
			, "simpleattr.xml"
			, "nestednode.xml"
			, "nestedattr.xml"
			, "children.xml"
			, "bookstore.xml"
			};
		for(auto fn : testXmlFilenames) {
			printf("\n----------------\n");
			xmlTestFile(fn);
		}
	}

	return 0;
}