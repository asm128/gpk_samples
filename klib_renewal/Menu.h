#include "gpk_string.h"

#ifndef KLIB_MENU_H_92634982716439274098216986259842
#define KLIB_MENU_H_92634982716439274098216986259842

namespace klib
{
	template <typename _ReturnType>
	class SMenuItem {
	public:
		_ReturnType			ReturnValue;
		::gpk::string		Text;
	};
};

#endif // KLIB_MENU_H_92634982716439274098216986259842
