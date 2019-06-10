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
	if(2 < argc)
		gpk_necall(sprintf_s(namePathDst, "%s", argv[2]), "%s", "Buffer overflow.");
	else if(2 == argc) {
		gpk_necall(sprintf_s(namePathDst, "%s", argv[1]), "%s", "Buffer overflow.");
		const ::gpk::view_const_string				extension						= ".";
		::gpk::error_t								indexSequence					= ::gpk::rfind_sequence_pod(extension, ::gpk::view_const_string{nameFileSrc});
		if(-1 != indexSequence) {
			namePathDst[indexSequence]				= '\\';
			namePathDst[indexSequence + 1]			= 0;
		}
	}
	gpk_necall(::gpk::folderUnpackToDisk(namePathDst, nameFileSrc), "Failed to unpack file '%s' to folder '%s'.", nameFileSrc, namePathDst);
	return 0; 
}
