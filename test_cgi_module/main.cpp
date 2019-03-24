#include "gpk_error.h"

extern "C" {
	::gpk::error_t gpk_moduleVersion	() { return 0; }
	::gpk::error_t gpk_moduleCreate		() { return 0; }
	::gpk::error_t gpk_moduleDelete		() { return 0; }
	::gpk::error_t gpk_moduleSetup		() { return 0; }
	::gpk::error_t gpk_moduleCleanup	() { return 0; }
	::gpk::error_t gpk_moduleRender		() { return 0; }
	::gpk::error_t gpk_moduleUpdate		() { return 0; }
	::gpk::error_t gpk_moduleTitle		() { return 0; }
}