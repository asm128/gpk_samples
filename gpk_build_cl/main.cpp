#include "gpk_json.h"
#include "gpk_storage.h"

int										main				(int argc, char** argv)			{
	ree_if(argc < 2, "USAGE: \n\t%s [path/to/solution]", argv[0]);
	const ::gpk::view_const_string				solutionPath		= {};

	return 0;
}
