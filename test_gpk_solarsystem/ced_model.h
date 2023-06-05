#include "gpk_array.h"
#include "gpk_matrix.h"

#ifndef CED_MODEL_H_2983472398
#define CED_MODEL_H_2983472398

namespace ced
{
	struct SModel3 {
		::gpk::n3f32								Scale		;
		::gpk::n3f32								Rotation	;
		::gpk::n3f32								Position	;
	};

	struct SModelMatrices {
		::gpk::m4<float>								Scale		;
		::gpk::m4<float>								Rotation	;
		::gpk::m4<float>								Position	;
	};

	struct SEntity {
		int32_t												Parent;
		::gpk::array_pod<int32_t>							Children			= {};
	};

	struct SCamera {
		::gpk::n3f32								Position			= {-0.000001f, 100, 0};
		::gpk::n3f32								Target				= {};
		::gpk::n3f32								Up					= {0, 1, 0};
	};


} // namespace

#endif // CED_MODEL_H_2983472398
