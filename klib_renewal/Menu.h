#include "gpk_label.h"

#ifndef __MENU_H__926349827164392740982169862598423__
#define __MENU_H__926349827164392740982169862598423__

namespace klib
{
	template <typename _ReturnType>
	class SMenuItem {
	public:
		_ReturnType								ReturnValue;
		::gpk::array_pod<char>				Text;
	};
};

#endif // __MENU_H__926349827164392740982169862598423__
