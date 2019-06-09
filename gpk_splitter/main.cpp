#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"

// Splits a file into file.split.## parts.
int								main							(int argc, char ** argv)		{
	ree_if(2 > argc, "Usage:\n\t%s [filename]", argv[0]);

	// -- Load source file.
	const ::gpk::view_const_string		fileNameSrc						= {argv[1], (uint32_t)-1};	// First parameter is the only parameter, which is the name of the source file to be split.
	ree_if(::gpk::fileSplit(fileNameSrc), "Failed to split file: %s.", fileNameSrc);
	return 0; 
}
