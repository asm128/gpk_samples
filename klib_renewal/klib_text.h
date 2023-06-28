/// Copyright 2016-2017 - asm128
#include "klib_ascii_target.h"
#include "klib_draw_misc.h"
#include "gpk_label.h"
#include "gpk_array.h"

#ifndef KLIB_TEXT_H_23627
#define KLIB_TEXT_H_23627

namespace klib
{
	static inline				int32_t	lineToRect				( char* rectangleTopLeft, uint32_t width, uint32_t height, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* text, int32_t charCount = -1 )	{
		int32_t									textLen					= (int32_t)strlen(text);
		if( textLen > charCount && charCount >= 0 )
			textLen								= charCount;

		return valueToRect(rectangleTopLeft, width, height, offsetLine, offsetColumn, align, text, textLen);
	}

	template<typename... _tArgs>
								int32_t	printfToRect			( char* rectangleTopLeft, uint32_t width, uint32_t height, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* format, _tArgs&&... args )		{
		char									precookStr[1024]		= {};
		int32_t									precookLen				= sprintf_s(precookStr, format, args...);
		return valueToRect(rectangleTopLeft, width, height, offsetLine, offsetColumn, align, precookStr, precookLen);
	}

	static						int32_t	lineToRectColored		( char* display, uint32_t width, uint32_t height, uint16_t* textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF ) {
		int32_t									actualX					= lineToRect(display, width, height, offsetLine, offsetColumn, align, text, charCount);
		uint32_t								stringLen				= (uint32_t)strlen(text);
		uint32_t								cellCount				= ((stringLen < charCount) ? stringLen : charCount);
		uint32_t								maxCount				= (uint32_t)width-actualX;
		cellCount							= (cellCount < maxCount) ? cellCount : maxCount;
		for(uint32_t i=0; i<cellCount; ++i)
			textAttributes[offsetLine*width+actualX+i] = messageColor;
		return actualX;
	}

	template<typename... _Args>
								int32_t	printfToRectColored		(char* display, uint32_t width, uint32_t height, uint16_t* textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* format, _Args&&... args) {
		char									precookStr[1024]		= {};
		const int32_t							precookLen				= sprintf_s(precookStr, format, args...);
		precookLen;
		int32_t									actualX					= lineToRectColored(display, width, height, textAttributes, messageColor, offsetLine, offsetColumn, align, precookStr);
		return actualX;
	}

	static inline				int32_t	lineToRectColored		(::klib::SASCIITarget & target, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF)		{
		return lineToRectColored	((char*)target.Characters.begin(), target.Width(), target.Height(), target.Colors.begin(), messageColor, offsetLine, offsetColumn, align, text, charCount);
	}
	static inline				int32_t	lineToRectColored		(::gpk::gchar chars, ::gpk::gu16 colors, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF)		{
		return lineToRectColored	((char*)chars.begin(), chars.metrics().x, chars.metrics().y, colors.begin(), messageColor, offsetLine, offsetColumn, align, text, charCount);
	}
	template<typename... _Args>	int32_t	printfToRectColored		(::klib::SASCIITarget & target, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* format, _Args&&... args)						{
		return printfToRectColored	((char*)target.Characters.begin(), target.Width(), target.Height(), target.Colors.begin(), messageColor, offsetLine, offsetColumn, align, format, args...);
	}

	template<typename... _Args>	int32_t	printfToRectColored		(::gpk::gchar chars, ::gpk::gu16 colors, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const char* format, _Args&&... args)						{
		return printfToRectColored	(chars.begin(), chars.metrics().x, chars.metrics().y, colors.begin(), messageColor, offsetLine, offsetColumn, align, format, args...);
	}


	struct SMessageSlow {
		char								Message[256]			= {'_',};
		float								NextTick				= 0.0f;
		uint32_t							TickCount				= 0;
	};
	static inline	void				resetCursorString		(::gpk::view<char> message)			{ message[message[1] = 0] = '_'; }
	static inline	void				resetCursorString		(::klib::SMessageSlow & message)	{ return resetCursorString(message.Message); }

	bool								getMessageSlow			(::klib::SMessageSlow & message, const char* textToPrint, uint32_t sizeToPrint, double lastFrameSeconds, double limit = 0.025f);
	static inline	bool				getMessageSlow			(::klib::SMessageSlow & message, const ::gpk::vcc & textToPrint, double lastFrameSeconds, double limit = 0.025f)	{ return getMessageSlow(message, textToPrint.begin(), textToPrint.size(), lastFrameSeconds, limit); }
} // namespace

#endif // KLIB_TEXT_H_23627
