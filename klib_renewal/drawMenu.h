#include "klib_input.h"

#include "Menu.h"
#include "klib_ascii_color.h"

#include "gpk_view_grid.h"

#include <Windows.h>

#ifndef __DRAWMENU_H__98237409236409872364987236498__
#define __DRAWMENU_H__98237409236409872364987236498__

#define MENU_ROFFSET 6

namespace klib
{
	static inline constexpr		bool						mouseOver						(::gpk::SCoord2<int32_t> mousePos, ::gpk::SCoord2<int32_t> targetPos, int32_t width)																							{ return (mousePos.y == targetPos.y) && (mousePos.x >= targetPos.x) && (mousePos.x < targetPos.x + width); }

#pragma pack(push, 1)
	struct SDrawMenuState {
								SMessageSlow				SlowTitle						= {};
								uint32_t					MenuItemAccum					= 0;
								uint32_t					CurrentPage						= 0;
								int32_t						CurrentOption					= 0;
	};
#pragma pack(pop)

								void						printMultipageHelp				(char* targetASCII, uint32_t targetWidth, uint32_t targetHeight, uint32_t currentPage, uint32_t pageCount, uint32_t posXOffset);

	template <size_t _FormatLen>
	static						int32_t						drawExitOption					(char* targetASCII, uint16_t* targetAttributes, uint32_t targetWidth, uint32_t targetHeight, uint32_t posXOffset, ::klib::ALIGN_SCREEN align, const char (&formatString)[_FormatLen], const ::gpk::view_const_char& exitText, bool bSelected )		{
		int32_t														offsetY							= (int32_t)targetHeight-MENU_ROFFSET-1;
		int32_t														actualOffsetX					= ::klib::printfToRect(targetASCII, targetWidth, targetHeight, offsetY, posXOffset, align, formatString, "0", exitText.begin());

		uint16_t													colorBkg						= bSelected ? ::klib::ASCII_COLOR_INDEX_GREEN << 4 : ::klib::ASCII_COLOR_INDEX_GREEN;
		return ::klib::valueToRect(targetAttributes, targetWidth, targetHeight,  offsetY, actualOffsetX, ::klib::SCREEN_LEFT, &colorBkg, 1, (int32_t)exitText.size()+3);
	}

	int32_t													drawMenu						(SDrawMenuState	& localPersistentState, ::gpk::v2c display, uint16_t* targetAttributes, const ::gpk::view_const_char& title, const ::gpk::view_array<const ::gpk::view_const_char>& menuItems, const ::klib::SInput& frameInput, const int32_t noActionValue=-1, uint32_t rowWidth=20, bool disableEscKeyClose=false, const ::gpk::view_const_char& exitText=::gpk::view_const_string{"Exit this menu"});
	template <typename _ReturnType>
	const _ReturnType	&									drawMenu						(SDrawMenuState	& localPersistentState, ::gpk::v2c display, uint16_t* targetAttributes, const ::gpk::view_const_char& title, const ::gpk::view_array<const _ReturnType> & menuItems, const ::gpk::view_array<const ::gpk::view_const_char>& menuItemsText, const ::klib::SInput& frameInput, const _ReturnType& exitValue, const _ReturnType& noActionValue=-1, uint32_t rowWidth=20, bool disableEscKeyClose=false, const ::gpk::view_const_char& exitText=::gpk::view_const_string{"Exit this menu"}) {
		const int32_t												exitValueInt					= ::klib::drawMenu(localPersistentState, display, targetAttributes, title, menuItemsText, frameInput, -1, rowWidth, disableEscKeyClose, exitText);
			 if(exitValueInt == (int32_t)menuItemsText.size())	return exitValue;
		else if(exitValueInt == -1)								return noActionValue;
		else 													return menuItems[exitValueInt];
	}
	template <typename _ReturnType>
	const _ReturnType	&									drawMenu						(SDrawMenuState	& localPersistentState, ::gpk::v2c display, uint16_t* targetAttributes, const ::gpk::view_const_char& title, const ::gpk::view_array<const ::klib::SMenuItem<_ReturnType>> & menuItems, const ::klib::SInput& frameInput, const _ReturnType& exitValue, const _ReturnType& noActionValue=-1, uint32_t rowWidth=20, bool disableEscKeyClose=false, const ::gpk::view_const_char& exitText=::gpk::view_const_string{"Exit this menu"}) {
		::gpk::array_obj<::gpk::view_const_char>					items;
		items.reserve(menuItems.size());
		for(uint32_t iItem = 0; iItem < menuItems.size(); ++iItem) {
			const ::gpk::view_const_char								menuText						= menuItems[iItem].Text;
			if(0 == menuText.size())
				break;
			items.push_back(menuText);
		}
		const int32_t												exitValueInt					= ::klib::drawMenu(localPersistentState, display, targetAttributes, title, items, frameInput, -1, rowWidth, disableEscKeyClose, exitText);
			 if(exitValueInt == (int32_t)items.size())	return exitValue;
		else if(exitValueInt == -1)						return noActionValue;
		else 											return menuItems[exitValueInt].ReturnValue;
	}

	template <typename _ReturnType>
	struct SMenuHeader {
		::gpk::view_const_string	Title;
		uint32_t					RowWidth;
		_ReturnType					ValueExit;
		bool						bDisableEscapeKey;
		::gpk::view_const_char		TextExit;
		SDrawMenuState				MenuState;

		constexpr					SMenuHeader				(_ReturnType exitValue, const ::gpk::view_const_string& title, uint32_t rowWidth=24, bool disableEscapeKey=false, const ::gpk::view_const_char& exitText = ::gpk::view_const_string{"Exit this menu"})
			: Title				(title)
			, RowWidth			(rowWidth)
			, ValueExit			(exitValue)
			, bDisableEscapeKey	(disableEscapeKey)
			, TextExit			(exitText)
			, MenuState			()
		{};
	};

	template <typename _ReturnType>
	const _ReturnType	&		drawMenu(::gpk::v2c& display, uint16_t* targetAttributes, SMenuHeader<_ReturnType>& menuInstance, const ::gpk::view_array<const ::klib::SMenuItem<_ReturnType>> & menuItems, const ::klib::SInput& frameInput, _ReturnType noActionValue = -1) {
		return ::klib::drawMenu(menuInstance.MenuState, display, targetAttributes, menuInstance.Title, menuItems, frameInput, menuInstance.ValueExit, noActionValue, menuInstance.RowWidth, menuInstance.bDisableEscapeKey, menuInstance.TextExit);
	}
} // namespace

#endif // __DRAWMENU_H__98237409236409872364987236498__
