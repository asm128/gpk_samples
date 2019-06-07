#include "gpk_log.h"
#include "gpk_storage.h"
#include "gpk_coord.h"

int main(int argc, char ** argv) { 
	if(2 > argc) {
		printf("Usage:\n\t%s [input file name] [output folder (optional)]", argv[0]);
		return -1;
	}
	::gpk::array_obj<::gpk::array_pod<char_t>>	listFiles				= {};
	const char									* nameFileSrc			= argv[1];	
	if(2 < argc)
		sprintf_s(nameFileDst, "%s", argv[2]);
	else
		sprintf_s(nameFileDst, "%s.gpk", argv[1]);
	fopen_s();
	::gpk::array_pod<byte_t>		tableFiles				= {};
	::gpk::array_pod<byte_t>		contentsPacked			= {};
	{
		const char						* nameFolderSrc			= argv[1];
		::gpk::pathList({nameFolderSrc, (uint32_t)strlen(nameFolderSrc)}, listFiles, false);
		::gpk::array_pod<byte_t>		contentsTemp			= {};
		::gpk::SRange<uint32_t>			fileLocation			= {0, 0};
		for(uint32_t iFile = 0; iFile < listFiles.size(); ++iFile) {
			fileLocation.Offset			= fileLocation.Offset + contentsTemp.size();
			const ::gpk::label				& pathToLoad			= listFiles[iFile];
			contentsTemp.clear();
			ree_if(::gpk::fileToMemory(pathToLoad, contentsTemp), "Failed to load file: %s. Out of memory?", pathToLoad.begin());
			fileLocation.Count			= contentsTemp.size();
			tableFiles.append((const byte_t*)&fileLocation, sizeof(::gpk::SRange<uint32_t>));
			uint32_t						pathLen					= pathToLoad.size();
			tableFiles.append((const byte_t*)&pathLen, sizeof(uint32_t));
			tableFiles.append(pathToLoad.begin(), pathLen);
			contentsPacked.append(contentsTemp.begin(), contentsTemp.size());
		}
	}
	{
		FILE							* fp					= 0;
		fopen_s(&fp, nameFileDst, "wb");
		ree_if(0 == fp, "Failed to create file: %s.", nameFileDst);
		fwrite(&listFiles		.size	(), 1, sizeof(uint32_t)			, fp);
		fwrite(tableFiles		.begin	(), 1, tableFiles		.size()	, fp);
		fwrite(contentsPacked	.begin	(), 1, contentsPacked	.size()	, fp);
		fclose(fp);
	}
	return 0; 
}
