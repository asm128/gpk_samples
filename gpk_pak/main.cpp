#define GPK_CONSOLE_LOG_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"
#include "gpk_coord.h"
#include "gpk_deflate.h"

int main(int argc, char ** argv) { 
	if(2 > argc) {
		printf("Usage:\n\t%s [input folder name] [output file name (optional)]", argv[0]);
		return -1;
	}
	char							nameFileDst[512]		= {};	// compose filename
	if(2 < argc)
		sprintf_s(nameFileDst, "%s", argv[2]);
	else
		sprintf_s(nameFileDst, "%s.gpk", argv[1]);

	::gpk::SPackHeader 				fileHeader;
	// -- The following two arrays store the file table and the file contents that are going to be compressed and stored on disk
	::gpk::array_pod<byte_t>		tableFiles				= {};
	::gpk::array_pod<byte_t>		contentsPacked			= {};
	{
		::gpk::SPathContents			pathTree				= {};
		const ::gpk::view_const_string	nameFolderSrc			= {argv[1], (uint32_t)strlen(argv[1])};;
		gpk_necall(::gpk::pathList(nameFolderSrc, pathTree), "Failed to list folder: %s.", nameFolderSrc.begin());
		::gpk::array_obj<::gpk::array_pod<char_t>>	listFiles				= {};
		gpk_necall(::gpk::pathList(pathTree, listFiles), "Failed to list folder: %s.", nameFolderSrc.begin());
		//gpk_necall(::gpk::pathList(nameFolderSrc, listFiles, false), "Failed to list folder: %s.", nameFolderSrc);
		::gpk::array_pod<byte_t>		contentsTemp			= {};
		::gpk::SRange<uint32_t>			fileLocation			= {0, 0};
		for(uint32_t iFile = 0; iFile < listFiles.size(); ++iFile) {
			fileLocation.Offset			= contentsPacked.size();
			const ::gpk::view_const_string	pathToLoad			= {listFiles[iFile].begin(), listFiles[iFile].size()};
			if(0 == pathToLoad.size())
				continue;
			printf("pathToLoad (%u): '%s'.\n", iFile, pathToLoad.begin());
			gpk_necall(::gpk::fileToMemory(pathToLoad, contentsTemp), "Failed to load file: %s. Out of memory?", pathToLoad.begin());

			fileLocation.Count			= contentsTemp.size();
			gpk_necall(tableFiles.append((const byte_t*)&fileLocation, sizeof(::gpk::SRange<uint32_t>)), "Failed to append data! %s.", "Out of memory?");
			uint32_t						pathLen					= pathToLoad.size();
			gpk_necall(tableFiles.append((const byte_t*)&pathLen, sizeof(uint32_t)), "Failed to append data bytes. Buffer sizes:"
				"tableFiles     : %u."
				"contentsPacked : %u."
				, tableFiles		.size()
				, contentsPacked	.size()
				);
			gpk_necall(tableFiles.append(pathToLoad.begin(), pathLen), "Failed to append data bytes. Buffer sizes:"
				"tableFiles     : %u."
				"contentsPacked : %u."
				, tableFiles		.size()
				, contentsPacked	.size()
				);
			gpk_necall(contentsPacked.append(contentsTemp.begin(), contentsTemp.size()), "Failed to append data bytes. Buffer sizes:"
				"tableFiles     : %u."
				"contentsPacked : %u."
				, tableFiles		.size()
				, contentsPacked	.size()
				);
			contentsTemp.clear();
			++fileHeader.TotalFileCount;
		}
	}
	fileHeader.SizeUncompressedTableFiles		= tableFiles		.size();
	fileHeader.SizeUncompressedContentsPacked	= contentsPacked	.size();
	::gpk::array_pod<byte_t>		compressedTableFiles		= {};
	::gpk::array_pod<byte_t>		compressedContentsPacked	= {};
	compressedTableFiles		.resize(tableFiles		.size()	);
	compressedContentsPacked	.resize(contentsPacked	.size()	);
	{	// compress
		gpk_necall(::gpk::arrayDeflate(tableFiles		, compressedTableFiles		), "%s", "Unknown error.");
		gpk_necall(::gpk::arrayDeflate(contentsPacked	, compressedContentsPacked	), "%s", "Unknown error.");
		fileHeader.SizeCompressedTableFiles		= compressedTableFiles		.size();
		fileHeader.SizeCompressedContentsPacked	= compressedContentsPacked	.size();
	}
	{
		FILE							* fp					= 0;
		fopen_s(&fp, nameFileDst, "wb");
		ree_if(0 == fp, "Failed to create file: %s.", nameFileDst);
		fwrite(&fileHeader		, 1, sizeof(::gpk::SPackHeader)				, fp);
		fwrite(compressedTableFiles		.begin	(), 1, compressedTableFiles		.size()	, fp);
		fwrite(compressedContentsPacked	.begin	(), 1, compressedContentsPacked	.size()	, fp);
		fclose(fp);
	}
	printf("Successfully written '%s'?", nameFileDst);
	return 0; 
}
