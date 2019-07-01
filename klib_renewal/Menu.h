#include "gpk_label.h"
#include <iostream>

#ifndef __MENU_H__926349827164392740982169862598423__
#define __MENU_H__926349827164392740982169862598423__

namespace klib
{
#pragma pack(push, 1)
	template <typename _ReturnType>
	class SMenuItem {
	public:
		_ReturnType								ReturnValue;
		::gpk::label							Text;
	};
#pragma pack(pop)
};

#endif // __MENU_H__926349827164392740982169862598423__