#define GPK_CONSOLE_LOG_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"

int main(int argc, char ** argv) { 
	ree_if(2 > argc, "Usage:\n\t%s [filename]", argv[0]);
	const ::gpk::view_const_string		fileNameSrc						= {argv[1], (uint32_t)-1};
	::gpk::array_pod<byte_t>			fileInMemory;
	gpk_necall(::gpk::fileToMemory(fileNameSrc, fileInMemory), "Failed to load file: \"%s\".", fileNameSrc);
	static constexpr const uint32_t		sizePartMax						= 15*1024*1024;
	uint32_t							countParts						= fileInMemory.size() / sizePartMax + one_if(fileInMemory.size() % sizePartMax);
	char								fileNameDst	[1024]				= {};
	for(uint32_t iPart = 0; iPart < countParts; ++iPart) {
		const uint32_t						offsetPart						= sizePartMax * iPart;
		sprintf_s(fileNameDst, "%s.split.%.2u", fileNameSrc.begin(), iPart);
		info_printf("Creating part %u: '%s'.", iPart, fileNameDst);
		FILE								* fpDest						= 0;
		fopen_s(&fpDest, fileNameDst, "wb");
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		fwrite(&fileInMemory[offsetPart], 1, (iPart == countParts - 1) ? fileInMemory.size() - offsetPart : sizePartMax, fpDest);
		fclose(fpDest);
	}
	return 0; 
}
