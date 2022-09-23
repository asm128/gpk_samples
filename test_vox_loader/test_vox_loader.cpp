#include "gpk_vox.h"


int main() {
	::gpk::array_pod<byte_t>		fileBytes;
	::gpk::array_static<char, 256>	folderNameData		= {"../gpk_data"};
	::gpk::array_static<char, 256>	folderNameChr		= {"vox/chr"};
	::gpk::array_static<char, 256>	fileName			= {"chr_old.vox"};

	char							pathToLoad[4096]	= {};
	sprintf_s(pathToLoad, "%s/%s/%s", folderNameData.Storage, folderNameChr.Storage, fileName.Storage);
	::gpk::fileToMemory(pathToLoad, fileBytes);

	::gpk::SVOXData					voxFile				= {};
	::gpk::vcc						viewBytes			= fileBytes;
	voxFile.Load(viewBytes);

	return 0;
}