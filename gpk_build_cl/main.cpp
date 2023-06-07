//	We need to get this program to call something like: C:\WINDOWS\System32\cmd.exe /d /c msbuild /property:GenerateFullPaths=true /t:build /p:Configuration=Release /p:Platform=x64 -m
//	For linux builds, however, we need it to call gcc or clang for every file
#define GPK_STDOUT_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_json.h"
#include "gpk_path.h"
#include "gpk_file.h"

struct SConfigBuild {
	::gpk::avcc							LibFile;
};

struct SBuildConfig {
	::gpk::vcs			PathConfig						= {};
	::gpk::vcs			FilenameConfig					= {};
	::gpk::apod<char>	JsonConfigOfBuild				= {};
	::gpk::SJSONReader	TreeConfigOfBuild				= {};
};

static	::gpk::error_t					buildSources					(const ::SBuildConfig & app, const ::gpk::view<::gpk::array_pod<char>> & listOfSourceFileNames) {
	(void)app;
	char										bufferFormat [4096]				= {};
	for(uint32_t iFile = 0; iFile < listOfSourceFileNames.size(); ++iFile) {
		const ::gpk::apod<char>						& sourceFileName				= listOfSourceFileNames[iFile];
		sprintf_s(bufferFormat, "Source to build: %%.%us.", sourceFileName.size());
		info_printf(bufferFormat, sourceFileName.begin());
	}
	return 0;
}

static	::gpk::error_t					buildProjects					(const ::SBuildConfig & app, int32_t indexOfBuildObject, const ::gpk::vcs & projectCollection, const ::gpk::view_const_string & extension) {
	::gpk::SPathContents						treeOfSolution;
	int32_t										indexLibNode					= ::gpk::jsonObjectValueGet(*app.TreeConfigOfBuild[indexOfBuildObject], app.TreeConfigOfBuild.View, projectCollection);
	if(-1 == indexLibNode)
		info_printf("No projects found for this configuration.");
	else {
		info_printf("- * -- Building %s sources with %s extension -- * -", projectCollection.begin(), extension.begin());
		const ::gpk::SJSONNode						& collectionNode				= *app.TreeConfigOfBuild[indexLibNode];
		::gpk::aobj<::gpk::apod<char>>				listOfSourceFileNames			= {};
		::gpk::apod<char>							pathToLibSources				= {};
		for(uint32_t iChild = 0, countChild = collectionNode.Children.size(); iChild < countChild; ++iChild) {
			listOfSourceFileNames.clear();
			pathToLibSources						= app.PathConfig;
			if(pathToLibSources[pathToLibSources.size() - 1] != '/')
				pathToLibSources.push_back('/');
			pathToLibSources.append(app.TreeConfigOfBuild.View[collectionNode.Children[iChild]->ObjectIndex]);
			pathToLibSources.push_back(0);
			info_printf("-- Sources path: '%s'.", pathToLibSources.begin());
			::gpk::aobj<::gpk::apod<char>>			listOfAllFileNames;
			gpk_necall(::gpk::pathList({pathToLibSources.begin(), pathToLibSources.size() - 1}, listOfAllFileNames, false), "%s", "Unknown error.");
			for(uint32_t iFile = 0; iFile < listOfAllFileNames.size(); ++iFile) {
				::gpk::vcs									fileName						= {listOfAllFileNames[iFile].begin(), listOfAllFileNames[iFile].size()};
				int32_t										offsetExtensionExpected			= ((int32_t)fileName.size() - (int32_t)extension.size());
				int32_t										offsetExtensionFound			= ::gpk::find_sequence_pod(extension, fileName);
				if(offsetExtensionExpected == offsetExtensionFound)
					listOfSourceFileNames.push_back(fileName);
			}
			e_if(::buildSources(app, listOfSourceFileNames), "%s", "");
		}
	}
	return 0;
}

static	::gpk::error_t	buildConfig						(const SBuildConfig & app) {
	const ::gpk::vcs			extension						= ".cpp";
	::gpk::pnco<::gpk::SJSONNode>	rootJSONArray				= app.TreeConfigOfBuild[0];
	for(uint32_t iBuild = 0, countBuilds = ::gpk::jsonArraySize(*rootJSONArray); iBuild < countBuilds; ++iBuild) {
		const int32_t				indexOfBuildObject				= ::gpk::jsonArrayValueGet(*rootJSONArray, iBuild);
		e_if(::gpk::failed(::buildProjects(app, indexOfBuildObject, "libs", extension)), "%s", "");
		e_if(::gpk::failed(::buildProjects(app, indexOfBuildObject, "dlls", extension)), "%s", "");
		e_if(::gpk::failed(::buildProjects(app, indexOfBuildObject, "exes", extension)), "%s", "");
	}
	return 0;
}

static	int				appMain			(::gpk::vcs filenameConfig)			{
	{ // Build single configuration
		SBuildConfig				configBuild					= {};
		configBuild.FilenameConfig	= filenameConfig;
		{	// load file.
			ree_if(::gpk::fileToMemory(configBuild.FilenameConfig, configBuild.JsonConfigOfBuild), "Failed to open build file: '%s'.", configBuild.FilenameConfig.begin());
		}
		{	// process json
			ree_if(::gpk::jsonParse(configBuild.TreeConfigOfBuild, {configBuild.JsonConfigOfBuild.begin(), configBuild.JsonConfigOfBuild.size()})
				, "Failed to process configuration file: %s. Path: %s. Contents:\n%s"
				, ::gpk::toString(configBuild.FilenameConfig	).begin()
				, ::gpk::toString(configBuild.PathConfig		).begin()
				, ::gpk::toString(configBuild.JsonConfigOfBuild	).begin()
			);
		}
		{	// build selected config
			info_printf	("Using configuration file: %s. Path: %s. Contents:\n%s"
				, ::gpk::toString(configBuild.FilenameConfig	).begin()
				, ::gpk::toString(configBuild.PathConfig		).begin()
				, ::gpk::toString(configBuild.JsonConfigOfBuild	).begin()
			);
			int32_t						indexOfLastSlash			= ::gpk::findLastSlash(configBuild.FilenameConfig);
			configBuild.PathConfig	= (-1 == indexOfLastSlash) ? "./" : ::gpk::view_const_string{configBuild.FilenameConfig.begin(), (uint32_t)indexOfLastSlash};
			gpk_necall	(::buildConfig(configBuild)
				, "Build failed. Configuration file: %s. Path: %s. Contents:\n%s"
				, ::gpk::toString(configBuild.FilenameConfig	).begin()
				, ::gpk::toString(configBuild.PathConfig		).begin()
				, ::gpk::toString(configBuild.JsonConfigOfBuild	).begin()
			);
		}
	}
	return 0;
}

int						main			(int argc, char** argv)			{
	ree_if(argc < 2, "USAGE: \n\t%s [path/to/solution]", argv[0]);
	return ::appMain({argv[1], (uint32_t)-1});
}
