#define GPK_CONSOLE_LOG_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"
#include "gpk_coord.h"
#include "gpk_deflate.h"

struct SGPKPak {
	::gpk::array_pod<byte_t>							DataContents	;
	::gpk::array_pod<byte_t>							DataInfo		;
	::gpk::array_pod<::gpk::view_array<const char_t>>	Contents		;
	::gpk::array_pod<::gpk::view_const_string>			Names			;
};

int											main				(int argc, char ** argv)						{
	if(2 > argc) {
		printf("Usage:\n\t%s [input file name] [output folder (optional)]", argv[0]);
		return -1;
	}
	char										nameFileSrc	[4096]		= {};
	const int32_t								sizeNameSrc				= (int32_t)sprintf_s(nameFileSrc, "%s", argv[1]);	

	char										namePathDst	[4096]		= {};
	sprintf_s(namePathDst, "%s", argv[2]);
	if(2 == argc) {
		sprintf_s(namePathDst, "%s", argv[2]);
		::gpk::error_t								indexSequence			= ::gpk::rfind_sequence_pod(::gpk::view_array<const char_t>{".gpk"}, ::gpk::view_array<const char_t>{nameFileSrc});
		if(-1 != indexSequence)
			namePathDst[indexSequence]				= 0;
	}
	SGPKPak										virtualFolder			= {};
	{
		::gpk::array_pod<byte_t>					rawFileInMemory			= {};
		gpk_necall(::gpk::fileToMemory(nameFileSrc, rawFileInMemory), "Failed to load pak file: %s.", nameFileSrc);
		const ::gpk::SPackHeader					& header				= *(::gpk::SPackHeader*)&rawFileInMemory[0];
		virtualFolder.DataInfo		.resize(header.SizeUncompressedTableFiles		);
		virtualFolder.DataContents	.resize(header.SizeUncompressedContentsPacked	);
		virtualFolder.Names			.resize(header.TotalFileCount);
		virtualFolder.Contents		.resize(header.TotalFileCount);
		gpk_necall(::gpk::arrayInflate({&rawFileInMemory[0] + sizeof(::gpk::SPackHeader)									, header.SizeCompressedTableFiles		}, virtualFolder.DataInfo		), "Failed to uncompress file table.");
		gpk_necall(::gpk::arrayInflate({&rawFileInMemory[0] + sizeof(::gpk::SPackHeader) + header.SizeCompressedTableFiles	, header.SizeCompressedContentsPacked	}, virtualFolder.DataContents	), "Failed to uncompress file contents.");
	}

	{
		uint32_t			offsetInfo		= 0;
		for(uint32_t iFile = 0; iFile < virtualFolder.Names.size(); ++iFile) {
			const ::gpk::SRange<uint32_t>		& fileLocation			= *(const ::gpk::SRange<uint32_t>*)&virtualFolder.DataInfo[offsetInfo];
			offsetInfo						+= sizeof(const ::gpk::SRange<uint32_t>);
			const uint32_t						lenName					= *(uint32_t*)&virtualFolder.DataInfo		[offsetInfo];
			offsetInfo						+= sizeof(uint32_t);
			virtualFolder.Names			[iFile] = {&virtualFolder.DataInfo		[offsetInfo], lenName};
			offsetInfo						+= lenName;

			virtualFolder.Contents		[iFile] = {&virtualFolder.DataContents	[fileLocation.Offset], fileLocation.Count};
		}
	}
	for(uint32_t iFile = 0, countFiles = virtualFolder.Names.size(); iFile < countFiles; ++iFile) {
		info_printf("File found (%u): %s. Size: %u.", iFile, virtualFolder.Names[iFile].begin(), virtualFolder.Contents[iFile].size());
		char					pathName [4096];
		sprintf_s(pathName, virtualFolder.Names[iFile].begin());
		::gpk::error_t			indexSlash = ::gpk::rfind_sequence_pod(::gpk::view_array<const char>{"\\", 1}, ::gpk::view_array<const char>{pathName});
		if(-1 != indexSlash) {
			pathName[indexSlash + 1] = 0;
			::gpk::dirCreate(pathName);
		}

		FILE	* fp = 0;
		fopen_s(&fp, virtualFolder.Names[iFile].begin(), "wb");
		ce_if(0 == fp, "Failed to create file: %s.", virtualFolder.Names[iFile].begin());
		fwrite(virtualFolder.Contents[iFile].begin(), 1, virtualFolder.Contents[iFile].size(), fp);
		fclose(fp);
	}
	return 0; 
}
