#include "gpk_noise.h"

#ifndef KLIB_RANDOM_GENERATOR_H_29843293874
#define KLIB_RANDOM_GENERATOR_H_29843293874

namespace klib 
{
#pragma pack(push, 1)
	struct SRandomGenerator {
				uint64_t					Seed				= 16381;
//#if defined(NWOL_MTSUPPORT)
//				refcount_t					Position			= 0;
//#else
				uint32_t					Position			= 0;
//#endif
				double						Value				= 0.0;

				void						Reset				()																										noexcept	{ Reset(Seed); }
				void						Reset				(uint64_t seed)																							noexcept	{
			Seed								= seed;
			Position							= 0;
			Value								= 0.0;
		}

		inline	double						Next				()																										noexcept	{
//#if defined(NWOL_MTSUPPORT)
//			return Value						= ::gpk::noiseNormal1D(nwol_sync_increment(Position), Seed);
//#else
			return Value						= ::gpk::noiseNormal1D(++Position, Seed);
//#endif
		}
	};

	static inline	double					randNoise			(uint64_t seed)																					noexcept	{
			static	::klib::SRandomGenerator		generator;
			if( generator.Seed != seed )
				generator.Reset(seed);
			return generator.Next();
	}


#pragma pack(pop)
} // namespace

#endif // KLIB_RANDOM_GENERATOR_H_29843293874
