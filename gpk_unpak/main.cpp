#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"
#include "gpk_coord.h"
#include "gpk_deflate.h"

int										main							(int argc, char ** argv)						{
	ree_if(2 > argc, "Usage:\n\t%s [input file name] [output folder (optional)]", argv[0]);
	char										nameFileSrc	[4096]				= {};
	const int32_t								sizeNameSrc						= (int32_t)sprintf_s(nameFileSrc, "%s", argv[1]);	
	char										namePathDst	[4096]				= {};
	sprintf_s(namePathDst, "%s", argv[2]);
	if(2 == argc) {
		sprintf_s(namePathDst, "%s", argv[1]);
		::gpk::error_t								indexSequence					= ::gpk::rfind_sequence_pod(::gpk::view_const_string{".gpk"}, ::gpk::view_const_string{nameFileSrc});
		if(-1 != indexSequence) {
			namePathDst[indexSequence]					= '\\';
			namePathDst[indexSequence + 1]				= 0;
		}
	}
	::gpk::SFolderInMemory						virtualFolder					= {};
	gpk_necall(::gpk::folderUnpack(virtualFolder, ::gpk::view_const_string{nameFileSrc}), "Failed to unpack file: %s.", nameFileSrc);
	gpk_necall(::gpk::folderToDisk(virtualFolder, namePathDst), "Failed to write folder to disk. Disk full or insufficient permissions. File name: %s. Destionation Path: %s.", nameFileSrc, namePathDst);
	return 0; 
}
