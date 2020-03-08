#include "klib_text.h"

bool								klib::getMessageSlow	(::klib::SMessageSlow & message, const char* textToPrint, uint32_t sizeToPrint, double lastFrameSeconds, double limit)	{
	::gpk::view_array<char_t>				_mes					= message.Message;
	int32_t									outputLen				= (int32_t)strlen(message.Message);
	if(0 == outputLen) {
		_mes[outputLen++]		= '_';
		_mes[outputLen]		= '\0';
	}
	float									& nextTick				= message.NextTick;
	uint32_t								& tickCount				= message.TickCount;
	::gpk::view_const_char					text					= {textToPrint, sizeToPrint};
	if(memcmp(message.Message, textToPrint, ::gpk::max(0, ::gpk::min((int32_t)sizeToPrint, outputLen - 1)))) {
		resetCursorString(message.Message);
		outputLen							= (uint32_t)strlen(message.Message);
	}
	nextTick							+= (float)lastFrameSeconds;
	if(nextTick > limit) {
		++tickCount;
		if(outputLen <= (int32_t)sizeToPrint) {
			_mes[outputLen - 1]		= text[outputLen - 1];
			_mes[outputLen]			= '_';
			_mes[outputLen + 1]		= '\0';
			nextTick							= 0.0f;
		}
		else if(0 == (tickCount % 20))
			_mes[outputLen - 1]		= (_mes[outputLen - 1] == ' ') ? '_' : ' ';
	}

	return ( outputLen - 1 == (int32_t)sizeToPrint );
}
