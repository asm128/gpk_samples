#include "gpk_color.h"
#include "gpk_coord.h"

#ifndef GPK_ENGINE_SHADER_H
#define GPK_ENGINE_SHADER_H

namespace gpk
{
	::gpk::SColorFloat			lightCalcSpecular		(::gpk::SCoord3<float> gEyePosW, float specularPower, ::gpk::SColorFloat specularLight, ::gpk::SColorFloat specularMaterial, ::gpk::SCoord3<float> posW, ::gpk::SCoord3<float> normalW, ::gpk::SCoord3<float> lightVecW);
	::gpk::SColorFloat			lightCalcDiffuse		(::gpk::SColorFloat diffuserMaterial, ::gpk::SCoord3<float> normalW, ::gpk::SCoord3<float> lightVecW);
}  // namespace


#endif // GPK_ENGINE_SHADER_H