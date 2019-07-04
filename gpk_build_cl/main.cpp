#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_json.h"
#include "gpk_storage.h"
#include "gpk_find.h"

struct SConfigBuild {
	::gpk::array_obj<::gpk::view_const_string>			LibFile;
};

struct SBuildConfig {
	::gpk::view_const_string							PathConfig						= {};
	::gpk::view_const_string							FilenameConfig					= {};
	::gpk::array_pod<byte_t>							JsonConfigOfBuild				= {};
	::gpk::SJSONReader									TreeConfigOfBuild				= {};
};

static	::gpk::error_t								buildSources					(const SBuildConfig & app, const ::gpk::view_array<::gpk::array_pod<char_t>> & listOfSourceFileNames) {
	(void)app;
	char													bufferFormat [4096]				= {};
	for(uint32_t iFile = 0; iFile < listOfSourceFileNames.size(); ++iFile) {
		const ::gpk::array_pod<char_t>				& sourceFileName				= listOfSourceFileNames[iFile];
		sprintf_s(bufferFormat, "Source to build: %%.%us.", sourceFileName.size());
		info_printf(bufferFormat, sourceFileName.begin());
	}
	return 0;
}

static	::gpk::error_t								buildProjects					(const SBuildConfig & app, int32_t indexOfBuildObject, const ::gpk::view_const_string & projectCollection, const ::gpk::view_const_string & extension) {
	::gpk::SPathContents									treeOfSolution;
	int32_t													indexLibNode					= ::gpk::jsonObjectValueGet(*app.TreeConfigOfBuild[indexOfBuildObject], app.TreeConfigOfBuild.View, projectCollection);
	if(-1 == indexLibNode) 
		info_printf("No projects found for this configuration.");
	else {
		info_printf("- * -- Building %s sources with %s extension -- * -", projectCollection.begin(), extension.begin());
		const ::gpk::SJSONNode									& collectionNode				= *app.TreeConfigOfBuild[indexLibNode];
		for(uint32_t iChild = 0, countChild = collectionNode.Children.size(); iChild < countChild; ++iChild) {
			::gpk::array_obj<::gpk::array_pod<char_t>>				listOfSourceFileNames;
			::gpk::array_pod<char_t>								pathToLibSources				= app.PathConfig;
			if(pathToLibSources[pathToLibSources.size() - 1] != '/')
				pathToLibSources.push_back('/');
			pathToLibSources.append(app.TreeConfigOfBuild.View[collectionNode.Children[iChild]->ObjectIndex]);
			pathToLibSources.push_back(0);
			info_printf("-- Sources path: '%s'.", pathToLibSources.begin());
			::gpk::array_obj<::gpk::array_pod<char_t>>				listOfAllFileNames;
			gpk_necall(::gpk::pathList({pathToLibSources.begin(), pathToLibSources.size() - 1}, listOfAllFileNames, false), "%s", "Unknown error.");
			for(uint32_t iFile = 0; iFile < listOfAllFileNames.size(); ++iFile) {
				::gpk::view_const_string								fileName						= {listOfAllFileNames[iFile].begin(), listOfAllFileNames[iFile].size()};
				int32_t													offsetExtensionExpected			= ((int32_t)fileName.size() - (int32_t)extension.size());
				int32_t													offsetExtensionFound			= ::gpk::find_sequence_pod(extension, fileName);
				if(offsetExtensionExpected == offsetExtensionFound)
					listOfSourceFileNames.push_back(fileName);
			}
			::buildSources(app, listOfSourceFileNames);
		}
	}
	return 0;
}

static	::gpk::error_t								buildConfig						(const SBuildConfig & app) {
	::gpk::view_const_string								extension						= ".cpp";
	{
		int32_t													indexOfBuildObject				= ::gpk::jsonArrayValueGet(*app.TreeConfigOfBuild[0], 0);
		::buildProjects(app, indexOfBuildObject, "libs", extension);
		::buildProjects(app, indexOfBuildObject, "dlls", extension);
		::buildProjects(app, indexOfBuildObject, "exes", extension);
	}
	return 0;
}

static	int											appMain						(::gpk::view_const_string filenameConfig)			{
	{ // Build single configuration
		char													bufferFormat [4096]				= {};
		SBuildConfig											configBuild						= {};
		configBuild.FilenameConfig							= filenameConfig;
		ree_if(::gpk::fileToMemory(configBuild.FilenameConfig, configBuild.JsonConfigOfBuild), "Failed to open build file: '%s'.", configBuild.FilenameConfig.begin());
		sprintf_s(bufferFormat, "Failed to read configuration file: %%.%us. Path: %%.%us. Contents:\n%%.%us", configBuild.FilenameConfig.size(), configBuild.PathConfig.size(), configBuild.JsonConfigOfBuild.size());
		ree_if(::gpk::jsonParse(configBuild.TreeConfigOfBuild, {configBuild.JsonConfigOfBuild.begin(), configBuild.JsonConfigOfBuild.size()}), bufferFormat, configBuild.FilenameConfig.begin(), configBuild.PathConfig.begin(), configBuild.JsonConfigOfBuild.begin());
		sprintf_s(bufferFormat, "Using configuration file: %%.%us. Path: %%.%us. Contents:\n%%.%us", configBuild.FilenameConfig.size(), configBuild.PathConfig.size(), configBuild.JsonConfigOfBuild.size());
		info_printf(bufferFormat, configBuild.FilenameConfig.begin(), configBuild.PathConfig.begin(), configBuild.JsonConfigOfBuild.begin());
		int32_t													indexOfLastSlash				= ::gpk::findLastSlash(configBuild.FilenameConfig);
		configBuild.PathConfig								= (-1 == indexOfLastSlash) ? "./" : ::gpk::view_const_string{configBuild.FilenameConfig.begin(), (uint32_t)indexOfLastSlash};
		gpk_necall(::buildConfig(configBuild), bufferFormat, configBuild.FilenameConfig.begin(), configBuild.PathConfig.begin(), configBuild.JsonConfigOfBuild.begin());
	}
	return 0;
}


int													main							(int argc, char** argv)			{
	ree_if(argc < 2, "USAGE: \n\t%s [path/to/solution]", argv[0]);
	return ::appMain({argv[1], (uint32_t)-1});
}
