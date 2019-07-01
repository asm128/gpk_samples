#include "klib_input.h"

#include "Menu.h"
#include "klib_timer.h"
#include "klib_ascii_color.h"

#include "gpk_view_grid.h"

#include <Windows.h>

#ifndef __DRAWMENU_H__98237409236409872364987236498__
#define __DRAWMENU_H__98237409236409872364987236498__

struct SDrawMenuGlobals {
							::klib::STimer					Timer;
							::klib::SAccumulator<double>	Accumulator;
};

extern					SDrawMenuGlobals				drawMenu_globals;

#define MENU_ROFFSET 8

namespace klib
{
	static inline constexpr		bool						mouseOver											(int32_t mouseX, int32_t mouseY, int32_t offsetX, int32_t offsetY, int32_t width)																							{ return (mouseY == offsetY) && (mouseX >= offsetX) && (mouseX < offsetX+width); }

#pragma pack(push, 1)
	struct SDrawMenuState {
								char						SlowTitle[96]										= {'_',};
								uint32_t					MenuItemAccum										= 0;
								uint32_t					CurrentPage											= 0;
								int32_t						CurrentOption										= 0;
		// - Ctor
	};
#pragma pack(pop)

								void						printMultipageHelp									(char* targetASCII, uint32_t targetWidth, uint32_t targetHeight, uint32_t currentPage, uint32_t pageCount, uint32_t posXOffset);

	template <size_t _FormatLen>
	static						int32_t						drawExitOption										(char* targetASCII, uint16_t* targetAttributes, uint32_t targetWidth, uint32_t targetHeight, uint32_t posXOffset, ::klib::ALIGN_SCREEN align, const char (&formatString)[_FormatLen], const std::string& exitText, bool bSelected )		{
		int32_t														offsetY												= (int32_t)targetHeight-MENU_ROFFSET-1;
		int32_t														actualOffsetX										= ::klib::printfToRect(targetASCII, targetWidth, targetHeight, offsetY, posXOffset, align, formatString, "0", exitText.c_str());	

		uint16_t													colorBkg											= bSelected ? COLOR_GREEN << 4 : COLOR_GREEN;  
		return ::klib::valueToRect(targetAttributes, targetWidth, targetHeight,  offsetY, actualOffsetX, ::klib::SCREEN_LEFT, &colorBkg, 1, (int32_t)exitText.size()+3);
	}

	template <size_t _ArraySize, typename _ReturnType>
								_ReturnType					processInput										(const ::klib::SInput& frameInput, uint32_t targetWidth, uint32_t targetHeight, SDrawMenuState& localPersistentState, bool& bResetMenuStuff, bool& bResetTitle, int32_t lineOffset, const klib::SMenuItem<_ReturnType>(&menuItems)[_ArraySize], uint32_t actualOptionCount, uint32_t pageCount, size_t itemOffset, int32_t exitOffset, int32_t numberCharsAvailable, const _ReturnType& noActionValue, const _ReturnType& exitValue, bool disableEscKeyClose, const ::gpk::label& exitText)	{
		bool														bMouseOverExit										= mouseOver(frameInput.Mouse.Deltas.x, frameInput.Mouse.Deltas.y, exitOffset-4, (int32_t)targetHeight-MENU_ROFFSET-1, (int32_t)exitText.size()+4);

		_ReturnType													resultVal											= noActionValue;
		if(localPersistentState.CurrentPage < (pageCount-1) && (frameInput.Keys[VK_NEXT] || frameInput.Keys[VK_RIGHT])) 
			(bResetMenuStuff = true) && ++localPersistentState.CurrentPage;
		else if(localPersistentState.CurrentPage > 0 && (frameInput.Keys[VK_PRIOR] || frameInput.Keys[VK_LEFT])) 
			(bResetMenuStuff = true) && --localPersistentState.CurrentPage;
		else if(frameInput.Keys[VK_DOWN])  {		// Test down and up keys.
			++localPersistentState.CurrentOption;
			bResetMenuStuff											= true;
			if( localPersistentState.CurrentOption == (int32_t)actualOptionCount ) {
				if(localPersistentState.CurrentPage < (pageCount-1) )
					(localPersistentState.CurrentOption = 0) && ++localPersistentState.CurrentPage;
				else
					localPersistentState.CurrentOption						= actualOptionCount-1;
			}
		}
		else if(frameInput.Keys[VK_UP]) {
			--localPersistentState.CurrentOption;
			bResetMenuStuff											= true;
			if( localPersistentState.CurrentOption < 0 ) {
				if(localPersistentState.CurrentPage > 0 )
					(localPersistentState.CurrentOption = 8) && --localPersistentState.CurrentPage;
				else
					localPersistentState.CurrentOption						= 0;
			}
		}
		else if(frameInput.Keys['0'] || frameInput.Keys[VK_NUMPAD0] || ((frameInput.Mouse.Buttons[4] || frameInput.Keys[VK_ESCAPE]) && !disableEscKeyClose) 	// Test exit request keys.
			|| (frameInput.Mouse.Buttons[0] && bMouseOverExit)
			)
		{
			bResetMenuStuff											= true;
			bResetTitle												= true;
			localPersistentState.CurrentPage						= 0;
			localPersistentState.CurrentOption						= 0;
			resultVal												= exitValue;
		}
		else if(frameInput.Keys[VK_RETURN] && localPersistentState.CurrentOption != -1) {	// Test execute keys.
			bResetMenuStuff											= true;
			resultVal												= menuItems[localPersistentState.CurrentOption+itemOffset].ReturnValue;
		}
		else { // look if any of the options was chose from the possible inputs
			for(uint32_t i=0, count = (uint32_t)actualOptionCount; i < count; i++) {
				int32_t														actualOffsetX										= (int32_t)(targetWidth-targetWidth/2-numberCharsAvailable/2);
				bool														bMouseOver											= ::klib::mouseOver(frameInput.Mouse.Deltas.x, frameInput.Mouse.Deltas.y, actualOffsetX-2, lineOffset+i, numberCharsAvailable + 2);
				if(frameInput.Keys['1'+i] || frameInput.Keys[VK_NUMPAD1+i] || (frameInput.Mouse.Buttons[0] && bMouseOver)) {
					bResetMenuStuff									= true;
					resultVal										= menuItems[i+itemOffset].ReturnValue;
					break;
				}
			}
		}	

		return resultVal;
	}

	template <size_t _ArraySize, typename _ReturnType>
	_ReturnType												drawMenu											(char* targetASCII, uint16_t* targetAttributes, uint32_t targetWidth, uint32_t targetHeight, uint32_t optionCount, const std::string& title, const klib::SMenuItem<_ReturnType>(&menuItems)[_ArraySize], const ::klib::SInput& frameInput, const _ReturnType& exitValue, const _ReturnType& noActionValue=-1, uint32_t rowWidth=20, bool disableEscKeyClose=false, const ::gpk::label& exitText="Exit this menu") {
		drawMenu_globals.Timer.Frame();

		optionCount												= (optionCount < _ArraySize) ? optionCount : _ArraySize; // Fix optionCount to the maximum size of the array if optionCount is higher than the allowed size.

		int32_t														lineOffset											= (int32_t)(targetHeight-MENU_ROFFSET-4-std::min((int32_t)optionCount, 9));
		const int32_t												clearOffset											= (int32_t)(targetHeight-MENU_ROFFSET-4-9);
		const int32_t												optionsOffset										= lineOffset;

		std::string													clearString											(::std::max(rowWidth, 128U), ' ');
		for(int32_t i=-2, count = (int32_t)targetHeight-clearOffset; i<count; ++i)
			::klib::printfToRectColored(targetASCII, targetWidth, targetHeight, targetAttributes, (COLOR_BLACK<<4) | COLOR_YELLOW, clearOffset+i, 0, ::klib::SCREEN_CENTER, "%s", clearString.c_str()); // clear all lines where we're going to draw

		const bool													multipage											= optionCount > 9;
		const uint32_t												pageCount											= (uint32_t)((multipage == false) ? 1 : optionCount/9 + ((optionCount%9)?1:0));

		static SDrawMenuState										localPersistentState;
		if( localPersistentState.CurrentPage >= pageCount )
			localPersistentState.CurrentPage = 0;

		// Build formatted menu title
		char														titleFormat		[]									= "-- %s --";
		char														titleFormatted	[128]								= {}; 
		int32_t														titleLength											= (int32_t)sprintf_s(titleFormatted, titleFormat, title.c_str());

		rowWidth												= ::std::max((uint32_t)titleLength, std::max((uint32_t)exitText.size()+3, rowWidth));
		const bool													bDonePrinting										= ::klib::getMessageSlow(localPersistentState.SlowTitle, titleFormatted, titleLength, drawMenu_globals.Timer.LastTimeSeconds*4);
		int32_t														actualOffsetX										= ::klib::lineToRect(targetASCII, targetWidth, targetHeight, lineOffset, 0, ::klib::SCREEN_CENTER, localPersistentState.SlowTitle);		//"-- %s --", title.c_str() );	// Print menu title
		for(uint32_t i=0; i<rowWidth+1; i++)
			targetAttributes[lineOffset*targetWidth+actualOffsetX+i] = COLOR_GREEN;

		if( !bDonePrinting )
			return noActionValue;

		lineOffset												+= 2;

		// Print menu options
		uint32_t													numberCharsAvailable								= rowWidth-4;	// 4 is for "%2.2s: "
		char														formatString	[24]								= {};
		char														numberKey		[4]									= {};
		sprintf_s(formatString, "%%2.2s: %%-%u.%us", numberCharsAvailable, numberCharsAvailable);

		// Draw options
		uint32_t													actualOptionCount									= std::min(9U, (uint32_t)(optionCount-(localPersistentState.CurrentPage*9)));
		const uint32_t											itemOffset											= localPersistentState.CurrentPage*9;
		if(drawMenu_globals.Accumulator.Value < 0.575 && localPersistentState.MenuItemAccum < actualOptionCount)
			drawMenu_globals.Accumulator.Value						= 0.575;

		for(uint32_t i = 0, count = (uint32_t)actualOptionCount; i < count; ++i) {
			actualOffsetX											= (int32_t)(targetWidth-targetWidth/2-numberCharsAvailable/2);
			if(::klib::mouseOver(frameInput.Mouse.Deltas.x, frameInput.Mouse.Deltas.y, actualOffsetX-2, lineOffset+i, numberCharsAvailable + 2)) {
				localPersistentState.CurrentOption						= i;
				break;
			}
		}

		if( localPersistentState.CurrentOption >= (int32_t)actualOptionCount )
			localPersistentState.CurrentOption						= actualOptionCount-1;
		
		const uint32_t												posXOffset											= 0;
		for(uint32_t i = 0, count = (localPersistentState.MenuItemAccum < actualOptionCount) ? localPersistentState.MenuItemAccum : actualOptionCount; i < count; ++i) {
			::sprintf_s(numberKey, "%u", (uint32_t)(i+1));
			actualOffsetX											= ::klib::printfToRect(targetASCII, targetWidth, targetHeight, lineOffset, posXOffset, ::klib::SCREEN_CENTER, formatString, numberKey, menuItems[itemOffset+i].Text.begin());
			if(localPersistentState.CurrentOption == (int32_t)i)
				for(uint32_t j = 0; j < rowWidth + 1; ++j)
					targetAttributes[lineOffset * targetWidth + actualOffsetX + j]	= COLOR_YELLOW << 4;
			else															  
				for(uint32_t j = 0; j < rowWidth + 1; ++j)						  
					targetAttributes[lineOffset * targetWidth + actualOffsetX + j]	= COLOR_YELLOW;

			++lineOffset;
		}

		int32_t														exitOffset											= 0;
		// Print Exit option at the end.
		if(localPersistentState.MenuItemAccum > actualOptionCount)
			exitOffset												= ::klib::drawExitOption(targetASCII, targetAttributes, targetWidth, targetHeight, posXOffset, ::klib::SCREEN_CENTER, formatString, exitText.begin(), localPersistentState.CurrentOption == 10);

		// Print page control help if multipage.
		if(multipage) 
			printMultipageHelp(targetASCII, targetWidth, targetHeight, localPersistentState.CurrentPage, pageCount, posXOffset);
		
		_ReturnType													resultVal											= noActionValue;
		bool														bResetMenuStuff										= false;
		bool														bResetTitle											= false;
		if( drawMenu_globals.Accumulator.Accumulate(drawMenu_globals.Timer.LastTimeSeconds) ) {
			if(localPersistentState.MenuItemAccum <= actualOptionCount) // Don't process keys until the menu has finished displaying
				(drawMenu_globals.Accumulator.Value = 0.575) && ++localPersistentState.MenuItemAccum;
			else	// Process page change keys first.
				resultVal												= processInput(frameInput, targetWidth, targetHeight, localPersistentState, bResetMenuStuff, bResetTitle, lineOffset - (int32_t)actualOptionCount, menuItems, actualOptionCount, pageCount, itemOffset, exitOffset, numberCharsAvailable, noActionValue, exitValue, disableEscKeyClose, exitText);
		}

		if(bResetMenuStuff)
			drawMenu_globals.Accumulator.Value						= 0;

		if(bResetTitle) {
			::klib::resetCursorString(localPersistentState.SlowTitle);
			localPersistentState.MenuItemAccum						= actualOptionCount >> 1;
		}

		return resultVal;
	}

	template <typename _tCell, size_t _ItemCount, typename _ReturnType>
	inline _ReturnType										drawMenu			(::gpk::view_grid<_tCell> display, uint16_t* targetAttributes, uint32_t optionCount, const std::string& title, const klib::SMenuItem<_ReturnType>(&menuItems)[_ItemCount], const ::klib::SInput& frameInput, _ReturnType exitValue, _ReturnType noActionValue = -1, uint32_t rowWidth=20, bool disableEscapeKey=false, const std::string& exitText="Exit this menu") {
		return ::klib::drawMenu(display.begin(), targetAttributes, display.metrics().x, display.metrics().y, optionCount, title, menuItems, frameInput, exitValue, noActionValue, rowWidth, disableEscapeKey, {exitText.data(), (uint32_t)exitText.size()});
	}
	
	template <typename _tCell, size_t _ItemCount, typename _ReturnType>
	inline _ReturnType										drawMenu			(::gpk::view_grid<_tCell> display, uint16_t* targetAttributes, const std::string& title, const ::klib::SMenuItem<_ReturnType>(&menuItems)[_ItemCount], const ::klib::SInput& frameInput, _ReturnType exitValue, _ReturnType noActionValue = -1, uint32_t rowWidth=20, bool disableEscapeKey=false, const std::string& exitText="Exit this menu") {
		return ::klib::drawMenu(display.begin(), targetAttributes, display.metrics().x, display.metrics().y, _ItemCount, title, menuItems, frameInput, exitValue, noActionValue, rowWidth, disableEscapeKey, {exitText.data(), (uint32_t)exitText.size()});
	}

	template <typename _ReturnType>
	struct SMenu {
		::gpk::label				Title;
		uint32_t					RowWidth;
		_ReturnType					ValueExit;
		bool						bDisableEscapeKey;
		::gpk::label				TextExit;
		SDrawMenuState				MenuState;

		constexpr					SMenu				(_ReturnType exitValue, const std::string& title, uint32_t rowWidth=24, bool disableEscapeKey=false, const ::gpk::label& exitText="Exit this menu")
			: Title				({title.data(), (uint32_t)title.size()})
			, RowWidth			(rowWidth)
			, ValueExit			(exitValue)
			, bDisableEscapeKey	(disableEscapeKey)
			, TextExit			(exitText)
			, MenuState			()
		{};
	};

	template <typename _tCell, size_t _ItemCount, typename _ReturnType>
	_ReturnType drawMenu(::gpk::view_grid<_tCell>& display, uint16_t* targetAttributes, const SMenu<_ReturnType>& menuInstance, const klib::SMenuItem<_ReturnType>(&menuItems)[_ItemCount], const ::klib::SInput& frameInput, _ReturnType noActionValue = -1) {
		return ::klib::drawMenu(display.begin(), targetAttributes, display.metrics().x, display.metrics().y, (uint32_t)_ItemCount, menuInstance.Title.begin(), menuItems, frameInput, menuInstance.ValueExit, noActionValue, menuInstance.RowWidth, menuInstance.bDisableEscapeKey, menuInstance.TextExit);
	}

	template <typename _tCell, size_t _ItemCount, typename _ReturnType>
	_ReturnType drawMenu(::gpk::view_grid<_tCell>& display, uint16_t* targetAttributes, const SMenu<_ReturnType>& menuInstance, const klib::SMenuItem<_ReturnType>(&menuItems)[_ItemCount], uint32_t optionCount, const ::klib::SInput& frameInput, _ReturnType noActionValue = -1) {
		return ::klib::drawMenu(display.begin(), targetAttributes, display.metrics().x, display.metrics().y, optionCount, menuInstance.Title.begin(), menuItems, frameInput, menuInstance.ValueExit, noActionValue, menuInstance.RowWidth, menuInstance.bDisableEscapeKey, menuInstance.TextExit);
	}
} // namespace

#endif // __DRAWMENU_H__98237409236409872364987236498__
