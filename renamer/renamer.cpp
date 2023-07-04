#define GPK_STDOUT_LOG_ENABLED

#include "gpk_file.h"
#include "gpk_path.h"
#include "gpk_array_static.h"

using namespace gpk;

::gpk::error_t	replace(::gpk::vcc tokenToReplace, ::gpk::vcc tokenTarget, ::gpk::achar & modified) {
	int32_t					index;
	while(-1 != (index = ::gpk::find_sequence_pod(tokenToReplace, {modified})))
		if(index >= 0 && index < (int32_t)modified.size()) {
			always_printf("\nFound '%s'.", modified.begin());
			modified.insert(index, tokenTarget);
			
			uint32_t oldTokenStart		= index + tokenTarget.size();
			uint32_t trailingPathStart	= oldTokenStart + tokenToReplace.size();
			uint32_t trailingPathSize	= modified.size() - trailingPathStart;
			if(trailingPathSize)
				memcpy(&modified[oldTokenStart], &modified[trailingPathStart], trailingPathSize);

			modified.resize(modified.size() - tokenToReplace.size());
			always_printf("\nModified: '%s'.", modified.begin());
		}

	return 0;
}

::gpk::error_t	replace(::gpk::vcc tokenToReplace, ::gpk::vcc tokenTarget, ::gpk::au8 & modified) {
	return ::replace(tokenToReplace, tokenTarget, *(::gpk::achar*)&modified);
}


int main() {
	const ::gpk::vcc		skip[]			= {::gpk::vcs{".git"}, ::gpk::vcs{".vs"}};

	const vcc				tokensToReplace	[3]	= {::gpk::vcs{"d1"			}, "d\01"								, "D\01"							};
	const vcc				tokensTarget	[3]	= {::gpk::vcs{"test_engine" }, "t\0e\0s\0t\0_\0e\0n\0g\0i\0n\0e"	, "T\0E\0S\0T\0_\0E\0N\0G\0I\0N\0E"	};

	::gpk::SPathContents	pathContents;
	gpk_necs(::gpk::pathList("../", pathContents));

	::gpk::aachar			pathsOriginal;
	gpk_necs(::gpk::pathList(pathContents, pathsOriginal));

	::gpk::aachar			pathsModified;

	for(uint32_t iPath = 0; iPath < pathsOriginal.size(); ++iPath) {
		achar					modified		= pathsOriginal[iPath];
		bool process = false;
		for(auto sskip : skip) {
			if(-1 == ::gpk::find_sequence_pod(sskip, {modified})) {
				process = true;
				break;
			}
		}
		if(process)
			::replace(tokensToReplace[0], tokensTarget[0], modified);
		pathsModified.push_back(modified);
	}

	for(uint32_t iPath = 0; iPath < pathsOriginal.size(); ++iPath) {
		const ::gpk::vcc		modified		= pathsModified[iPath];
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
			::gpk::vcc				folderName;
			if(-1 != modified.slice(folderName, 0, iLastSlash) && folderName.size())
				w_if(::gpk::pathCreate(folderName), "Failed to create path: '%s'.", folderName.begin());
		}

		const ::gpk::vcc		original		= pathsOriginal[iPath];

		{
			::gpk::au8				fileContents;
			ce_if_failed(::gpk::fileToMemory(original.cc(), fileContents), "Failed to open file: '%s'.", original.begin());

			for(uint32_t iToken = 0; iToken < ::gpk::size(tokensTarget); ++iToken)
				::replace(tokensToReplace[iToken], tokensTarget[iToken], fileContents);

			ce_if_failed(::gpk::fileFromMemory(modified.cc(), fileContents), "Failed to write file: '%s'.", modified.begin());
		}
	}
	return 0;
}