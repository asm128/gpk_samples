#include "gpk_array.h"
#include "gpk_matrix.h"

#ifndef CED_MODEL_H_2983472398
#define CED_MODEL_H_2983472398

namespace ced
{
	struct SModel3D {
		::gpk::SCoord3<float>								Scale;
		::gpk::SCoord3<float>								Rotation;
		::gpk::SCoord3<float>								Position;
	};

	struct SModelTransform {
		::gpk::SMatrix4<float>								Scale		;
		::gpk::SMatrix4<float>								Rotation	;
		::gpk::SMatrix4<float>								Position	;
	};

	struct SEntity {
		int32_t												Parent;
		::gpk::array_pod<int32_t>							Children			= {};
	};

	struct SCamera {
		::gpk::SCoord3<float>								Target				= {};
		::gpk::SCoord3<float>								Position			= {-0.000001f, 100, 0};
		::gpk::SCoord3<float>								Up					= {0, 1, 0};
	};
} // namespace

#endif // CED_MODEL_H_2983472398
