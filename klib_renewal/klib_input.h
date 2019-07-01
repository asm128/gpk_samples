#include "gpk_coord.h"

#ifndef KLIB_INPUT_H_20937492837423
#define KLIB_INPUT_H_20937492837423

namespace klib
{
	struct SMouseInput {
								::gpk::SCoord3<int32_t>						Deltas									;
								uint8_t										Buttons					[16]			;
	};

	struct SInput		{
		static constexpr		const uint16_t								KeyCount								= 256U;
		static constexpr		const uint16_t								ButtonCount								= 16U;

								::klib::SMouseInput							Mouse									= {};
								::klib::SMouseInput							PreviousMouse							= {};

								uint8_t										Keys					[KeyCount]		= {};
								uint8_t										PreviousKeys			[KeyCount]		= {};

		inline					bool										KeyUp									(uint8_t index)							const	noexcept	{ return 0 == Keys			[index] && 0 != PreviousKeys			[index]; }
		inline					bool										KeyDown									(uint8_t index)							const	noexcept	{ return 0 != Keys			[index] && 0 == PreviousKeys			[index]; }
		inline					bool										ButtonUp								(uint8_t index)							const	noexcept	{ return 0 == Mouse.Buttons	[index] && 0 != PreviousMouse.Buttons	[index]; }
		inline					bool										ButtonDown								(uint8_t index)							const	noexcept	{ return 0 != Mouse.Buttons	[index] && 0 == PreviousMouse.Buttons	[index]; }
	};

							::gpk::error_t								pollInput								(::klib::SInput& input);

} // namespace

#endif // KLIB_INPUT_H_20937492837423
