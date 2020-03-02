#include "klib_text.h"

bool								klib::getMessageSlow	(::klib::SMessageSlow & message, const char* textToPrint, uint32_t sizeToPrint, double lastFrameSeconds, double limit)	{
	uint32_t								inputLen				= (int32_t)strlen(textToPrint);
	uint32_t								outputLen				= (int32_t)strlen(message.Message);
	float									& nextTick				= message.NextTick;
	uint32_t								& tickCount				= message.TickCount;
	if(0 == inputLen)
		return true;

	if(memcmp(message.Message, textToPrint, ::gpk::max(0U, ::gpk::min(sizeToPrint, outputLen - 1)))) {
		resetCursorString(message.Message);
		outputLen							= (uint32_t)strlen(message.Message);
	}
	nextTick							+= (float)lastFrameSeconds;
	if(nextTick > limit) {
		++tickCount;
		if(outputLen <= sizeToPrint) {
			message.Message[outputLen - 1]		= textToPrint[outputLen - 1];
			message.Message[outputLen]			= '_';
			message.Message[outputLen + 1]		= '\0';
			nextTick							= 0.0f;
		}
		else if(0 == (tickCount % 20))
			message.Message[outputLen - 1]		= (message.Message[outputLen - 1] == ' ') ? '_' : ' ';
	}

	return ( outputLen - 1 == sizeToPrint );
}
