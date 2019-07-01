/// Copyright 2014-2017 - asm128
#include "klib_ascii_screen.h"
#include "klib_ascii_display.h"

static struct SASCIIDisplayBuffered {
	::klib::SASCIITarget							BackBuffer							= {};
	::klib::SASCIITarget							FrontBuffer							= {};
	bool											bCreated							= false;

													~SASCIIDisplayBuffered						()																		{
		if( bCreated )	
			::klib::shutdownASCIIScreen();
	}
}												__g_ASCIIScreen;	// Main instance (Windows applications seem to support only one console per process, as well as many custom smaller systems)

::gpk::error_t									klib::getASCIIBackBuffer			(::klib::SASCIITarget& target)											{ target = __g_ASCIIScreen.BackBuffer	; return 0; }
::gpk::error_t									klib::getASCIIFrontBuffer			(::klib::SASCIITarget& target)											{ target = __g_ASCIIScreen.FrontBuffer	; return 0; }
::gpk::error_t									klib::setASCIIScreenTitle			(const char_t* title)													{ return ::klib::asciiDisplayTitleSet({title, (uint32_t)strlen(title)}); }
::gpk::error_t									klib::clearASCIIBackBuffer			(char_t value, uint16_t colorValue)										{ return ::klib::asciiTargetClear(__g_ASCIIScreen.BackBuffer, value, colorValue); }
::gpk::error_t									klib::presentASCIIFrontBuffer		()																		{ return ::klib::asciiDisplayPresent(__g_ASCIIScreen.FrontBuffer); }
::gpk::error_t									klib::presentASCIIBackBuffer		()																		{
	gerror_if(errored(::klib::swapASCIIBuffers()), "Why would this ever happen? Is the memory corrupt?");
	return ::klib::asciiDisplayPresent(__g_ASCIIScreen.FrontBuffer);
}

::gpk::error_t									klib::swapASCIIBuffers				()																		{
	::klib::SASCIITarget								bB									= __g_ASCIIScreen.BackBuffer;
	__g_ASCIIScreen.BackBuffer 						= __g_ASCIIScreen.FrontBuffer;
	__g_ASCIIScreen.FrontBuffer						= bB;
	return 0;
}

::gpk::error_t									klib::shutdownASCIIScreen			()																		{
	if(false == __g_ASCIIScreen.bCreated)
		return 1;
	gerror_if(errored(::klib::asciiDisplayDestroy()								 ), "Why would this evern happen?");
	gerror_if(errored(::klib::asciiTargetDestroy(__g_ASCIIScreen.BackBuffer		)), "Why would this evern happen?");
	gerror_if(errored(::klib::asciiTargetDestroy(__g_ASCIIScreen.FrontBuffer	)), "Why would this evern happen?");
	__g_ASCIIScreen.bCreated						= false;
	return 0;
}

::gpk::error_t									klib::initASCIIScreen				(uint32_t width, uint32_t height)										{
	if(false == __g_ASCIIScreen.bCreated) {
		gpk_necall(::klib::asciiTargetCreate(__g_ASCIIScreen.BackBuffer	, width, height), "Out of memory or something weird is going on.");
		gpk_necall(::klib::asciiTargetCreate(__g_ASCIIScreen.FrontBuffer	, width, height), "Out of memory or something weird is going on.");
		gpk_necall(::klib::asciiDisplayCreate(width, height), "Out of memory or something weird is going on.");
		__g_ASCIIScreen.bCreated						= true;
	}
	return 0;
}
