#include "gpk_view_grid.h"
#include "gpk_array.h"

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

struct SDebris	{
	::gpk::array_pod<::gpk::SCoord3<float>>		Position			= {};
	::gpk::array_pod<::gpk::SCoord3<float>>		Direction			= {};
	::gpk::array_pod<float>						Speed				= {};
	::gpk::array_pod<float>						Brightness			= {};

	int											Spawn				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed)	{
		Position		.push_back(position);
		Direction		.push_back(direction);
		Speed			.push_back(speed);
		Brightness		.push_back(1);
		return 0;
	}
	int											Update				(float lastFrameSeconds)	{
		for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
			Position	[iShot]							+= Direction[iShot] * (Speed[iShot] * lastFrameSeconds);
			Brightness 	[iShot]							-= lastFrameSeconds;
			if(Position[iShot].Length() > 50) {
				Direction	.remove_unordered(iShot);
				Position	.remove_unordered(iShot);
				Speed		.remove_unordered(iShot);
				Brightness	.remove_unordered(iShot);
			}
		}
		return 0;
	}
};

struct SShots	{
	double										Delay				= 0;
	::gpk::array_pod<::gpk::SCoord3<float>>		PositionPrev		= {};
	::gpk::array_pod<::gpk::SCoord3<float>>		Position			= {};
	::gpk::array_pod<::gpk::SCoord3<float>>		Direction			= {};
	::gpk::array_pod<float>						Speed				= {};
	::gpk::array_pod<float>						Brightness			= {};

	int											Spawn				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed)	{
		if(Delay < 1)
			return 0;
		PositionPrev	.push_back(position);
		Position		.push_back(position);
		Direction		.push_back(direction);
		Speed			.push_back(speed);
		Brightness		.push_back(1);
		Delay										= 0;
		return 0;
	}

	int											Update				(float lastFrameSeconds)	{
		for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
			PositionPrev[iShot]							= Position	[iShot];
			Position	[iShot]							+= Direction[iShot] * (Speed[iShot] * lastFrameSeconds);
			if (Position[iShot].Length() > 100)
				Remove(iShot);
		}
		return 0;
	}
	int											Remove			(uint32_t iShot) {
		Direction		[iShot]						= Direction		[Position.size() - 1];
		PositionPrev	[iShot]						= PositionPrev	[Position.size() - 1];
		Position		[iShot]						= Position		[Position.size() - 1];
		Speed			[iShot]						= Speed			[Position.size() - 1];
		Brightness		[iShot]						= Brightness	[Position.size() - 1];
		PositionPrev	.resize(PositionPrev	.size() - 1);
		Position		.resize(Position		.size() - 1);
		Speed			.resize(Speed			.size() - 1);
		Brightness		.resize(Brightness		.size() - 1);
		Direction		.resize(Direction		.size() - 1);
		return Position.size();
	}
};

#endif // GPK_DEMO_08_GAME_H_293874239874
