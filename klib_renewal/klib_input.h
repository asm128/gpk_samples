#include "gpk_n3.h"
#include "gpk_error.h"

#ifndef KLIB_INPUT_H_23627
#define KLIB_INPUT_H_23627

namespace klib
{
	struct SMouseInput {
		::gpk::n3<int32_t>	Deltas					;
		uint8_t				Buttons		[16]		;
	};

	struct SInput		{
		stacxpr	uint16_t	KeyCount				= 256U;
		stacxpr	uint16_t	ButtonCount				= 16U;

		::klib::SMouseInput	Mouse					= {};
		::klib::SMouseInput	PreviousMouse			= {};

		uint8_t				Keys		[KeyCount]	= {};
		uint8_t				PreviousKeys[KeyCount]	= {};

		inline	bool		KeyUp					(uint8_t index)	const	noexcept	{ return 0 == Keys			[index] && 0 != PreviousKeys			[index]; }
		inline	bool		KeyDown					(uint8_t index)	const	noexcept	{ return 0 != Keys			[index] && 0 == PreviousKeys			[index]; }
		inline	bool		ButtonUp				(uint8_t index)	const	noexcept	{ return 0 == Mouse.Buttons	[index] && 0 != PreviousMouse.Buttons	[index]; }
		inline	bool		ButtonDown				(uint8_t index)	const	noexcept	{ return 0 != Mouse.Buttons	[index] && 0 == PreviousMouse.Buttons	[index]; }
	};

	::gpk::error_t		pollInput				(::klib::SInput& input);

} // namespace

#endif // KLIB_INPUT_H_23627
