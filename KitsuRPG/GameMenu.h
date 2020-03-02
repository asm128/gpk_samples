#include "Menu.h"

#include <iostream>
#include <string>

#ifndef __GAMEMENU_H__9273409236492316409183763987234__
#define __GAMEMENU_H__9273409236492316409183763987234__

// returns -1 if the user entered an invalid numeric input
static inline int64_t										getNumericInput							()																												{
	// Get user input
	::std::string													userChoice;
	::std::getline(::std::cin, userChoice);

	// Convert the input string to a menuitem index.
	int64_t															selectedOption							= -1;
	try { selectedOption = ::std::stoll(userChoice); }
	catch(std::invalid_argument) {}	// this try/catch bullshit is required because std::stoi() throws exceptions if the input can't be converted to a number.

	return selectedOption;
}

template <typename _ReturnType>
_ReturnType													displayMenu								(size_t optionCount, const ::gpk::view_const_char& title, const ::gpk::view_array<const ::klib::SMenuItem<_ReturnType>> menuItems)		{
	optionCount													= (optionCount < menuItems.size()) ? optionCount : menuItems.size(); // Fix optionCount to the maximum size of the array if optionCount is higher than the allowed size.

	while (true) {	// Query for user input until a valid selection is made
		printf(	"\n-- %s.\n", title.begin() );	// Print menu title
		printf(	"Make your selection:\n" );

		// Print menu options
		for(uint32_t i=0; i < optionCount; ++i)
			printf("%u: %s.\n", (uint32_t)(i + 1), menuItems[i].Text.begin());

		// Get user input.
		const uint32_t													selectedIndex							= (uint32_t)(::getNumericInput()-1);	//	Convert the input string to a menuitem index.

		if(selectedIndex >= optionCount)	// We only accept from 0 to optionCount
			printf("Invalid answer. Answer again...\n");
		else {
			printf("\n");
			return menuItems[selectedIndex].ReturnValue;
		}
	}
}

template <typename _ReturnType>
_ReturnType													displayMenu								(const ::gpk::view_const_char& title, const ::gpk::view_array<const ::klib::SMenuItem<_ReturnType>> menuItems, int32_t maxItems = ~0U)		{
	return displayMenu((menuItems.size() > (uint32_t)maxItems) ? maxItems : menuItems.size(), title, menuItems);
}

#endif __GAMEMENU_H__9273409236492316409183763987234__
