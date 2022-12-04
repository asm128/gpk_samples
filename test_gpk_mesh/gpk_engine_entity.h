#include "gpk_ptr.h"

#ifndef GPK_ENGINE_ENTITY_H
#define GPK_ENGINE_ENTITY_H

namespace gpk 
{
#pragma pack(push, 1)
	struct SVirtualEntity {
		uint32_t										RenderNode	;
		uint32_t										RigidBody	;
		uint32_t										Parent		;
	};
#pragma pack(pop)

	struct SVirtualEntityManager {
		::gpk::array_pod<::gpk::SVirtualEntity>			Entities		= {};
		::gpk::array_pobj<::gpk::array_pod<uint32_t>>	EntityChildren	= {};

		::gpk::error_t									Create			() {
			EntityChildren.push_back({});
			return Entities.push_back({});
		}

		::gpk::error_t									Delete			(uint32_t index) {
			EntityChildren.remove_unordered(index);
			return Entities.remove_unordered(index);
		}
	};
} // namespace

#endif
