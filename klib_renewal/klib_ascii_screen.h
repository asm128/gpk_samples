/// Copyright 2014-2017 - asm128
#include "klib_ascii_target.h"

#ifndef KLIB_ASCIISCREEN_H_20983472903740928374234
#define KLIB_ASCIISCREEN_H_20983472903740928374234

#define DEFAULT_ASCII_SCREEN_HEIGHT		83
#define DEFAULT_ASCII_SCREEN_WIDTH		((uint32_t)(DEFAULT_ASCII_SCREEN_HEIGHT*2.666666f))

namespace klib
{
	::gpk::error_t			initASCIIScreen					(uint32_t width = DEFAULT_ASCII_SCREEN_WIDTH, uint32_t height = DEFAULT_ASCII_SCREEN_HEIGHT);
	::gpk::error_t			shutdownASCIIScreen				();
	::gpk::error_t			setASCIIScreenTitle				(const char_t* title);
	::gpk::error_t			getASCIIBackBuffer				(::klib::SASCIITarget& target);
	::gpk::error_t			getASCIIFrontBuffer				(::klib::SASCIITarget& target);
	::gpk::error_t			swapASCIIBuffers				();
	::gpk::error_t			clearASCIIBackBuffer			(char_t value, uint16_t colorValue=15);
	::gpk::error_t			presentASCIIFrontBuffer			();
	::gpk::error_t			presentASCIIBackBuffer			();
};

#endif // KLIB_ASCIISCREEN_H_20983472903740928374234
