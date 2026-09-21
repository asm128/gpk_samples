#define GPK_STDOUT_LOG_ENABLED

#include "gpk_file.h"
#include "gpk_path.h"
#include "gpk_array_static.h"

using namespace gpk;

::gpk::error_t	replace(::gpk::vcsc_t tokenToReplace, ::gpk::vcsc_t tokenTarget, ::gpk::asc_t & modified) {
	int32_t					index;
	while(-1 != (index = ::gpk::find_sequence_pod(tokenToReplace, {modified})))
		if(index >= 0 && index < (int32_t)modified.size()) {
			modified.insert(index, tokenTarget);
			
			uint32_t oldTokenStart		= index + tokenTarget.size();
			uint32_t trailingPathStart	= oldTokenStart + tokenToReplace.size();
			uint32_t trailingPathSize	= modified.size() - trailingPathStart;
			if(trailingPathSize)
				memcpy(&modified[oldTokenStart], &modified[trailingPathStart], trailingPathSize);

			modified.resize(modified.size() - tokenToReplace.size());
		}

	return 0;
}

::gpk::error_t	replace(::gpk::vcsc_t tokenToReplace, ::gpk::vcsc_t tokenTarget, ::gpk::au0_t & modified) {
	return ::replace(tokenToReplace, tokenTarget, *(::gpk::asc_t*)&modified);
}


int main() {
	const ::gpk::vcsc_t		skip[]			= {".git", ".vs", ".obj", ".pdb", ".idb", "/obj", "obj/", "intermediate", "resfiles", "x64", "Win32", "Debug", "Release", "pch"};

	const ::gpk::vcsc_t		tokensToReplace	[]	= {"d1", "d1p", "pool_game", "pool_shader", "p\0o\0o\0l\0_\0s\0h\0a\0d\0e\0r", "P\0O\0O\0L\0_\0S\0H\0A\0D\0E\0R", "P\0o\0o\0l\0S\0h\0a\0d\0e\0r", "p\0o\0o\0l\0_\0g\0a\0m\0e", "P\0O\0O\0L\0_\0G\0A\0M\0E", "P\0o\0o\0l\0G\0a\0m\0e"};
	const ::gpk::vcsc_t		tokensTarget	[]	= {"test_engine", "test_engine", "test_game", "test_shader", "t\0e\0s\0t\0_\0s\0h\0a\0d\0e\0r", "T\0E\0S\0T\0_\0S\0H\0A\0D\0E\0R", "T\0e\0s\0t\0S\0h\0a\0d\0e\0r", "t\0e\0s\0t\0_\0g\0a\0m\0e", "T\0E\0S\0T\0_\0G\0A\0M\0E", "T\0e\0s\0t\0G\0a\0m\0e"};

	::gpk::SPathContents	pathContents;
	gpk_necs(::gpk::pathList("../gpk_games/test_engine", pathContents));

	::gpk::aachar			pathsOriginal;
	gpk_necs(::gpk::pathList(pathContents, pathsOriginal));
	::gpk::as2_t				indicesToSource;
	::gpk::aachar			pathsModified;
	for(uint32_t i = 0; i < 4; ++i) {
		for(uint32_t iPath = 0; iPath < pathsOriginal.size(); ++iPath) {
			asc_t					modified		= pathsOriginal[iPath];
			bool process = false;
			for(auto sskip : skip) {
				if(-1 == ::gpk::find_sequence_pod(sskip, {modified})) {
					process = true;
					break;
				}
			}
			if(process) {
				::replace(tokensToReplace[i], tokensTarget[i], modified);
				pathsModified.push_back(modified);
				indicesToSource.push_back(iPath);
			}
		}
	}
	for(uint32_t iPath = 0; iPath < indicesToSource.size(); ++iPath) {
		const ::gpk::vcsc_t		modified		= pathsModified[iPath];
		if(-1 == ::gpk::find_sequence_pod(tokensTarget[0], {modified}))
			continue;

		bool noprocess = false;
		for(auto sskip : skip) {
			if(-1 != ::gpk::find_sequence_pod(sskip, {modified})) {
				noprocess = true;
				break;
			}
		}
		if(noprocess)
			continue;

		const int32_t			iLastSlash		= ::gpk::findLastSlash(modified);
		if(-1 != iLastSlash) {
			::gpk::vcsc_t				folderName;
			if(-1 != modified.slice(folderName, 0, iLastSlash) && folderName.size())
				wf_if(::gpk::pathCreate(folderName), "'%s'.", folderName.begin());
		}

		const ::gpk::vcsc_t		original		= pathsOriginal[indicesToSource[iPath]];

		{
			::gpk::au0_t				fileContents;
			ce_if_failed(::gpk::fileToMemory(original.cc(), fileContents), "Failed to open file: '%s'.", original.begin());

			for(uint32_t iToken = 0; iToken < ::gpk::size(tokensTarget); ++iToken)
				::replace(tokensToReplace[iToken], tokensTarget[iToken], fileContents);

			ce_if_failed(::gpk::fileFromMemory(modified.cc(), fileContents), "Failed to write file: '%s'.", modified.begin());
			always_printf("\n'%s' -> '%s'.", original.begin(), modified.begin());
		}
	}
	return 0;
}