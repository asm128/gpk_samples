#include "gpk_view_grid.h"
#include "gpk_array.h"
#include "gpk_color.h"

#ifndef GPK_DEMO_08_GAME_H_293874239874
#define GPK_DEMO_08_GAME_H_293874239874

struct SStars	{
	::gpk::array_pod<::gpk::SCoord2<float>>		Position			= {};
	::gpk::array_pod<float>						Speed				= {};
	::gpk::array_pod<float>						Brightness			= {};

	int											Update				(uint32_t yMax, float lastFrameSeconds)	{
		for(uint32_t iStar = 0; iStar < Brightness.size(); ++iStar) {
			::gpk::SCoord2<float>									 & starPos			= Position[iStar];
			float													starSpeed			= Speed[iStar];
			starPos.y											+= starSpeed * lastFrameSeconds;
			Brightness[iStar]									= float(1.0 / 65535.0 * rand());
			if(starPos.y >= yMax) {
				Speed		[iStar]								= float(16 + (rand() % 64));
				starPos.y											= 0;
			}
		}
		return 0;
	}
};

struct SParticles3 {
	::gpk::array_pod<::gpk::SCoord3<float>>		Position			= {};
	::gpk::array_pod<::gpk::SCoord3<float>>		Direction			= {};
	::gpk::array_pod<float>						Speed				= {};

	int											IntegrateSpeed		(double lastFrameSeconds)	{
		for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
			::gpk::SCoord3<float>							& direction						= Direction	[iShot];
			::gpk::SCoord3<float>							& position						= Position	[iShot];
			float											& speed							= Speed		[iShot];
			position									+= direction * speed * lastFrameSeconds;
		}
		return 0;
	}
	int											Remove				(int32_t iParticle)	{
		Position	.remove_unordered(iParticle);
		Direction	.remove_unordered(iParticle);
		return Speed.remove_unordered(iParticle);
	}
};

struct SDebris	{
	::gpk::SColorBGRA							Colors[4]			=
		{ {0x80, 0xAF, 0xFF, }
		, {0x40, 0x80, 0xFF, }
		, {0x20, 0x80, 0xFF, }
		, {0x00, 0x00, 0xFF, }
		};
	::SParticles3								Particles			= {};
	::gpk::array_pod<float>						Brightness			= {};

	int											Spawn				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed, float brightness)	{
		Particles.Position	.push_back(position);
		Particles.Direction	.push_back(direction);
		Particles.Speed		.push_back(speed);
		Brightness			.push_back(brightness);
		return 0;
	}
	int											SpawnSpherical			(uint32_t countDebris, const ::gpk::SCoord3<float> & position, float speedDebris, float brightness)	{
		for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
			::gpk::SCoord3<float>									direction				= {0, 1, 0};
			direction.RotateX(rand() * (::gpk::math_2pi / RAND_MAX));
			direction.RotateY(rand() * (::gpk::math_2pi / RAND_MAX));
			direction.RotateZ(rand() * (::gpk::math_2pi / RAND_MAX));
			direction.Normalize();
			Spawn(position, direction, speedDebris, brightness);
		}
		return 0;
	}
	int											Update				(float lastFrameSeconds)	{
		Particles.IntegrateSpeed(lastFrameSeconds);
		for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
			float											& speed							= Particles.Speed		[iShot];
			float											& brightness 					= Brightness[iShot];
			brightness									-= lastFrameSeconds;
			speed										-= lastFrameSeconds *  (rand() % 16);
			if(brightness < 0) {
				Particles.Remove(iShot);
				Brightness.remove_unordered(iShot);
			}
		}
		return 0;
	}
};

struct SShots	{
	double										Delay				= 0;
	::gpk::array_pod<::gpk::SCoord3<float>>		PositionPrev		= {};
	::gpk::array_pod<float>						Brightness			= {};
	::SParticles3								Particles;

	int											Spawn				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed)	{
		if(Delay < 1)
			return 0;
		PositionPrev		.push_back(position);
		Particles.Position	.push_back(position);
		Particles.Direction	.push_back(direction);
		Particles.Speed		.push_back(speed);
		Brightness			.push_back(1);
		Delay										= 0;
		return 0;
	}

	int											Update				(float lastFrameSeconds)	{
		memcpy(PositionPrev.begin(), Particles.Position.begin(), Particles.Position.size() * sizeof(::gpk::SCoord3<float>));
		Particles.IntegrateSpeed(lastFrameSeconds);
		for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
			if (Particles.Position[iShot].Length() > 100)
				Remove(iShot);
		}
		return 0;
	}
	int											Remove			(uint32_t iShot) {
		Particles.Remove(iShot);
		PositionPrev	.remove_unordered(iShot);
		Brightness		.remove_unordered(iShot);
		return Brightness.size();
	}
};

#endif // GPK_DEMO_08_GAME_H_293874239874
