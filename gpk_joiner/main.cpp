#define GPK_CONSOLE_LOG_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"

int main(int argc, char ** argv) { 
	ree_if(2 > argc, "Usage:\n\t%s [filename]", argv[0]);
	const char						* fileNameDst		= argv[1];
	char							fileNameSrc	[1024]	= {};
	uint32_t						iFile				= 0;
	sprintf_s(fileNameSrc, "%s.split.%.2u", fileNameDst, iFile);
	::gpk::array_pod<byte_t>		fileInMemory;
	FILE							* fpDest;
	fopen_s(&fpDest, fileNameDst, "wb");
	ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
	while(0 == ::gpk::fileToMemory(fileNameSrc, fileInMemory)) {
		fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fpDest);
		sprintf_s(fileNameSrc, "%s.split.%.2u", fileNameDst, ++iFile);
	}
	fclose(fpDest);
	return 0; 
}
