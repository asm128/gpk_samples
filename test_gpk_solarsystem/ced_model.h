#include "gpk_array.h"
#include "gpk_matrix.h"

#ifndef CED_MODEL_H_2983472398
#define CED_MODEL_H_2983472398

namespace ced
{
	struct SModel3 {
		::gpk::n3<float>								Scale		;
		::gpk::n3<float>								Rotation	;
		::gpk::n3<float>								Position	;
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
		::gpk::n3<float>								Position			= {-0.000001f, 100, 0};
		::gpk::n3<float>								Target				= {};
		::gpk::n3<float>								Up					= {0, 1, 0};
	};


} // namespace

#endif // CED_MODEL_H_2983472398
