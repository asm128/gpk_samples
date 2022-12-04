#include "gpk_color.h"
#include "gpk_coord.h"


#ifndef GPK_ENGINE_RENDERIMAGE_H
#define GPK_ENGINE_RENDERIMAGE_H

namespace gpk 
{
#pragma pack(push, 1)
	struct SSurfaceDescription {
		::gpk::COLOR_TYPE							ColorType			;
		int8_t										BitDepth			;
		int8_t										MethodCompression	;
		int8_t										MethodFilter		;
		int8_t										MethodInterlace		;
		::gpk::SCoord2<uint16_t>					Dimensions			;
	};
#pragma pack(pop)

	struct SSurface {
		::gpk::SSurfaceDescription					Desc;
		::gpk::array_pod<uint8_t>					Data;
	};

	struct SSurfaceManager {
		::gpk::array_pobj<::gpk::SSurface>			Surfaces;
		::gpk::array_obj<::gpk::vcc>				SurfaceNames;

		::gpk::error_t								Create				()					{ SurfaceNames.push_back({});				return Surfaces .push_back({}); }
		::gpk::error_t								Delete				(uint32_t index)	{ SurfaceNames.remove_unordered(index);		return Surfaces .remove_unordered(index); }

	};
} // namespace

#endif
