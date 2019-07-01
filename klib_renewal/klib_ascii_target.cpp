#include "klib_ascii_target.h"
#include "gpk_memory.h"
#include "klib_ascii_color.h"
#include "klib_fill.h"

				::gpk::error_t									klib::asciiTargetCreate							(::klib::SASCIITarget& console, uint32_t width, uint32_t height)		{ 
	console.Characters												= {(uint8_t *)::gpk::gpk_malloc(sizeof(uint8_t ) * width * height), width, height}; 
	console.Colors													= {(uint16_t*)::gpk::gpk_malloc(sizeof(uint16_t) * width * height), width, height}; 
	return 0; 
}

				::gpk::error_t									klib::asciiTargetDestroy						(::klib::SASCIITarget& console)											{ 
	if(console.Characters	.begin()) ::gpk::gpk_free(console.Characters	.begin());
	if(console.Colors		.begin()) ::gpk::gpk_free(console.Colors		.begin());
	return 0; 
}

				::gpk::error_t									klib::asciiTargetClear							(::klib::SASCIITarget& console, uint8_t character, uint16_t color)		{ 
	::klib::fillArray(console.Characters	.begin(), character	, console.Characters	.size(), console.Characters	.size());
	::klib::fillArray(console.Colors		.begin(), color		, console.Colors		.size(), console.Colors		.size());
	return 0; 
}
