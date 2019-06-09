#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"

// Joins a file split into file.split.## parts.
int							main							(int argc, char ** argv)				{
	ree_if(2 > argc, "Usage:\n\t%s [filename]", argv[0]);
	const ::gpk::view_const_string	fileNameDst						= {argv[1], (uint32_t)-1};	// First parameter is the only parameter, which is the name of the original file that has been split.
	::gpk::error_t					countJoined						= ::gpk::fileJoin(fileNameDst);
	ree_if(errored(countJoined), "Failed to join file. Disk full? File name: %s.", fileNameDst.begin());
	info_printf("%u parts joined.", countJoined);
	return 0; 
}
