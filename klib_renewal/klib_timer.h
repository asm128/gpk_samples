/// Copyright 2014-2017 - asm128
#include "gpk_typeint.h"
#include "gpk_timer.h"

#ifndef KLIB_TIMER_H_23627
#define KLIB_TIMER_H_23627

namespace klib
{
#pragma pack(push, 1)
	template<typename _tValue>
	struct SAccumulator {
		_tValue					Value;
		_tValue					MaxValue;

		// The Accumulate and Deplete functions of SAccumulator won't prevent overflow when using negative numbers as arguments.
		// This is so we don't have unnecessary condition checks in places where we are going to operate always with positive values
		_tValue					Accumulate				( _tValue amount )	noexcept	{
			if( (Value += amount) > MaxValue ) {
				_tValue						difference				= Value - MaxValue;
				Value					= MaxValue;
				return difference;
			}
			return 0;
		}

		// The Accumulate and Deplete functions of SAccumulator won't prevent overflow when using negative numbers as arguments.
		// This is so we don't have unnecessary condition checks in places where we know that we are going to operate always with positive values
		_tValue					Deplete					( _tValue amount )	noexcept	{
			if( (Value -= amount) < 0 ) {
				_tValue						difference				= Value;
				Value					= 0;
				return difference;
			}
			return 0;
		}
	};	// struct

	//template<typename _tValue>
	//struct SAccumulatorStats {
	//			_tValue			Accumulated;
	//			_tValue			Depleted;
	//			uint64_t		TimesAccumulated;
	//			uint64_t		TimesDepleted;
	//
	//	inline	_tValue			Accumulated	( _tValue amount ){ ++TimesAccumulated	;	return Accumulated	+= amount; }
	//	inline	_tValue			Depleted	( _tValue amount ){ ++TimesDepleted		;	return Depleted		+= amount; }
	//};

	// --- This thing is to provide high-precision
	struct STimer : public ::gpk::STimer {
		double					FramesLastSecond		= 0;
		double					FrameCounterSeconds		= 0.0;				// The following are to get the FPS.
		int64_t					FramesThisSecond		= 0;

		inline					STimer					()								{ Reset(); }

		void					Reset					();
		void					Frame					();	// Calculate time elapsed since the last Frame() or Reset() call.
	};	// struct

#pragma pack(pop)
};


#endif // KLIB_TIMER_H_23627
