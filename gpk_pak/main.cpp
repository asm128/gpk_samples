#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"
#include "gpk_coord.h"
#include "gpk_deflate.h"

int										main						(int argc, char ** argv)	{
	ree_if(2 > argc, "Usage:\n\t%s [input folder name] [output file name (optional)]", argv[0]);

	char										nameFileDst[512]			= {};	// compose filename
	if(2 < argc)
		sprintf_s(nameFileDst, "%s", argv[2]);
	else
		sprintf_s(nameFileDst, "%s.gpk", argv[1]);

	const ::gpk::view_const_string				nameFolderSrc				= {argv[1], (uint32_t)strlen(argv[1])};;
	gpk_necall(::gpk::folderPackToDisk(nameFileDst, nameFolderSrc), "Failed to pack folder '%s' to file '%s'.", nameFolderSrc.begin(), nameFileDst);
	printf("Successfully written '%s'?", nameFileDst);
	return 0; 
}
