#include "Menu.h"

#ifndef __GAMEMENU_H__9273409236492316409183763987234__
#define __GAMEMENU_H__9273409236492316409183763987234__

// returns -1 if the user entered an invalid numeric input
static inline int64_t										getNumericInput							()																												{
	// Get user input
	::std::string													userChoice;
	::std::getline(::std::cin, userChoice);
	
	// Convert the input string to a menuitem index.
	int64_t															selectedOption							= -1;
	try { selectedOption = std::stoll(userChoice); }
	catch(std::invalid_argument) {}	// this try/catch bullshit is required because std::stoi() throws exceptions if the input can't be converted to a number.

	return selectedOption;
}

template <size_t _ArraySize, typename _ReturnType>
_ReturnType													displayMenu								(size_t optionCount, const std::string& title, const klib::SMenuItem<_ReturnType>(&menuItems)[_ArraySize])		{
	optionCount													= (optionCount < _ArraySize) ? optionCount : _ArraySize; // Fix optionCount to the maximum size of the array if optionCount is higher than the allowed size.

	while (true) {	// Query for user input until a valid selection is made
		printf(	"\n-- %s.\n", title.c_str() );	// Print menu title
		printf(	"Make your selection:\n" );
		
		// Print menu options
		for(size_t i=0; i<optionCount; i++)
			printf("%u: %s.\n", (uint32_t)(i+1), menuItems[i].Text.begin());	
		
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

template <size_t _Size, typename _ReturnType>
_ReturnType													displayMenu								(const std::string& title, const klib::SMenuItem<_ReturnType>(&menuItems)[_Size], int32_t maxItems = ~0U)		{
	return displayMenu((_Size > maxItems) ? maxItems : _Size, title, menuItems);
}

#endif __GAMEMENU_H__9273409236492316409183763987234__