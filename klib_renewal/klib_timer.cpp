/// Copyright 2014-2017 - asm128
#include "klib_timer.h"
#include <chrono>

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#else
#	include <chrono>
#endif

void							klib::STimer::Reset								()				{
	::gpk::STimer::Reset();
	FramesLastSecond				= 0;
}

void							klib::STimer::Frame								()				{
	::gpk::STimer::Frame();

	++FramesThisSecond;
	FrameCounterSeconds				+= LastTimeSeconds;

	int32_t								totalFrames										= int32_t(FramesThisSecond	/ FrameCounterSeconds);
	int32_t								framesPerSecond									= int32_t(totalFrames		/ FrameCounterSeconds);
	while(FrameCounterSeconds >= 1.0) {
		FramesLastSecond				= framesPerSecond;
		FrameCounterSeconds				-= 1.0f;
		FramesThisSecond				= 0;
	}
}
