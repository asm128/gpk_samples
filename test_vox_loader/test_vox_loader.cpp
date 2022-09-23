#include "gpk_array.h"
#include "gpk_array_static.h"
#include "gpk_storage.h"

namespace gpk 
{
	struct SVOXChunk {
		uint8_t						Type				[4]	= {};
		::gpk::array_pod<uint8_t>	Data					= {};
	};

#pragma pack(push, 1)
	struct SVOXFileHeader {
		char						MagicNumber[4]			= {};
		uint32_t					Version					= {};
	};

	struct SVOXData {
		char						MagicNumber[4]			= {};
		uint32_t					Version					= {};
		::gpk::array_obj<SVOXChunk>	Chunks					= {};
	};
#pragma pack(pop)
} // namespace

int main() {
	::gpk::array_pod<byte_t>		fileBytes;
	::gpk::array_static<char, 256>	folderNameData		= {"./gpk_data"};
	::gpk::array_static<char, 256>	folderNameChr		= {"vox/chr"};
	::gpk::array_static<char, 256>	fileName			= {"chr_old.vox"};

	char pathToLoad[4096] = {};
	sprintf_s(pathToLoad, "%s/%s/%s", folderNameData.Storage, folderNameChr.Storage, fileName.Storage);
	::gpk::fileToMemory(pathToLoad, fileBytes);


	return 0;
}