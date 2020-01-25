#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_matrix.h"
#include "ced_model.h"
#include "ced_geometry.h"
#include"ced_game_gameplay.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734



namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{

	struct SScene	{
		::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>	Image					= {};
		::gpk::array_obj<::ced::SGeometryQuads>				Geometry				= {};
		::gpk::array_obj<::ced::SEntity>					Entities				= {};
		::gpk::array_pod<::gpk::SMatrix4<float>>			ModelMatricesLocal		= {};
		::gpk::array_pod<::gpk::SMatrix4<float>>			ModelMatricesGlobal		= {};
		::gpk::array_pod<::ced::SModel3D>					Models					= {};
		::ced::SCamera										Camera					= {};
		::gpk::SCoord3<float>								LightVector				= {15, 12, 0};
	};

	struct SApplication {
					::gpk::SFramework													Framework;
					::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen					= {};
					::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>					PNGImages					= {};

					int32_t																IdExit						= -1;

					::std::mutex														LockGUI;
					::std::mutex														LockRender;


					::gpk::array_pod<int32_t>											Health					= {};

					SScene																Scene					;
					::SStars															Stars					;
					::SShots															ShotsPlayer				;
					::SShots															ShotsEnemy				;
					::SDebris															Debris					;

					double																AnimationTime			= 0;

																						SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace


#endif // APPLICATION_H_2078934982734
