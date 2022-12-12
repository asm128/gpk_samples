#include "gpk_pool_draw.h"

::gpk::SColorFloat								lightCalcSpecular		(::gpk::SCoord3<float> gEyePosW, float specularPower, ::gpk::SColorFloat specularLight, ::gpk::SColorFloat specularMaterial, ::gpk::SCoord3<float> posW, ::gpk::SCoord3<float> normalW, ::gpk::SCoord3<float> lightVecW) {
	const ::gpk::SCoord3<float>							pointToEye				= (gEyePosW - posW).Normalize();
	const ::gpk::SCoord3<float>							reflected				= normalW.Reflect(-lightVecW);
	const float											factor					= powf((float)::gpk::max(reflected.Dot(pointToEye), 0.0), specularPower);
	::gpk::SColorFloat									result					= specularMaterial * specularLight * factor;
	result.a										= specularMaterial.a;
	return result;
}

::gpk::SColorFloat								lightCalcDiffuse		(::gpk::SColorFloat diffuserMaterial, ::gpk::SCoord3<float> normalW, ::gpk::SCoord3<float> lightVecW) {
	double												lightFactor				= normalW.Dot(lightVecW);
	const ::gpk::SColorFloat							result					= (diffuserMaterial * lightFactor).Clamp();
	return result;
}


