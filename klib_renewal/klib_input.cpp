#include "klib_input.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#endif

::gpk::error_t												klib::pollInput									(::klib::SInput& input)		{
	static constexpr	const uint32_t								keyCount										= ::klib::SInput::KeyCount;
	static constexpr	const uint32_t								buttonCount										= ::klib::SInput::ButtonCount;
	memcpy(input.PreviousKeys, input.Keys, keyCount);
#if defined(GPK_WINDOWS)
	for(uint32_t i=0; i<keyCount; i++)
		input.Keys[i]												= ::GetAsyncKeyState(i) ? 1 : 0;
#else
#	error "Not implemented."
#endif
	memcpy(input.PreviousMouse.Buttons, input.Mouse.Buttons, buttonCount);

#if defined(GPK_WINDOWS)
						INPUT_RECORD								recordIn										= {};
						DWORD										NumRead											= 0;
						HANDLE										handleIn										= ::GetStdHandle(STD_INPUT_HANDLE);
						DWORD										eventCount										= 0;

	::GetNumberOfConsoleInputEvents(handleIn, &eventCount);
	if( 0 == eventCount )
		return 0;

	ReadConsoleInput(handleIn, &recordIn, 1, &NumRead);

	switch (recordIn.EventType) {
	case KEY_EVENT:
		// Do stuff
		//input.Keys[recordIn.Event.KeyEvent.wVirtualKeyCode] = recordIn.Event.KeyEvent.bKeyDown ? 1 : 0;
		break;
	case MOUSE_EVENT:
		input.Mouse.Deltas.x								= recordIn.Event.MouseEvent.dwMousePosition.X;
		input.Mouse.Deltas.y								= recordIn.Event.MouseEvent.dwMousePosition.Y;
		input.Mouse.Buttons[0]								= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED	;
		input.Mouse.Buttons[1]								= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED	;
		input.Mouse.Buttons[2]								= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_3RD_BUTTON_PRESSED	;
		input.Mouse.Buttons[3]								= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_4TH_BUTTON_PRESSED	;
		input.Mouse.Buttons[4]								= recordIn.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED		;
		break;
	}
#else
#	error "Not implemented."
#endif
	return 0;
}
