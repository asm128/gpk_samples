#include "ced_game.h"

int													setupStars			(SStars & stars, ::gpk::SCoord2<uint32_t> targetSize)	{
	if(0 == targetSize.y) return 0;
	if(0 == targetSize.x) return 0;
	stars.Speed		.resize(128);
	stars.Brightness.resize(128);
	stars.Position	.resize(128);
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		stars.Speed			[iStar]						= float(1 + (rand() % 16));
		stars.Brightness	[iStar]						= float(1.0 / 65535 * rand());
		stars.Position		[iStar].y					= float(rand() % targetSize.y);
		stars.Position		[iStar].x					= float(rand() % targetSize.x);
	}
	return 0;
}

int													modelCreate			(::gme::SApplication & app)	{
	int32_t													indexModel			= app.Scene.ModelMatricesLocal.size();
	app.Health						.resize(indexModel + 7);
	app.Scene.ModelMatricesLocal	.resize(indexModel + 7);
	app.Scene.ModelMatricesGlobal	.resize(indexModel + 7);
	app.Scene.Models				.resize(indexModel + 7);
	app.Scene.Entities				.resize(indexModel + 7);
	app.Scene.Models		[indexModel]				= {};
	app.Scene.Models		[indexModel].Scale			= {1, 1, 1};
	if(0 == indexModel)
		app.Scene.Models		[indexModel].Rotation.z		= (float)(::gpk::math_pi_2);
	else
		app.Scene.Models		[indexModel].Rotation.z		= (float)(-::gpk::math_pi_2);
	app.Scene.Entities		[indexModel]				= {-1};
	for(uint32_t iModel = indexModel + 1; iModel < app.Scene.Models.size(); ++iModel) {
		::ced::SModel3D											& model			= app.Scene.Models[iModel];
		model.Scale											= {1, 1, 1};
		//model.Rotation										= {0, 1, 0};
		model.Position										= {2, 0.5};
		model.Position.RotateY(::gpk::math_2pi / (app.Scene.Models.size() - 1) * iModel);
		::ced::SEntity											& entity		= app.Scene.Entities[iModel];
		entity.Parent										= indexModel;
		app.Health[iModel]									= 1000;
 		app.Scene.Entities[indexModel].Children.push_back(iModel);
	}
	return indexModel;
}

int													setupGame			(::gme::SApplication & app)	{
	::gpk::SFramework										& framework			= app.Framework;
	::setupStars(app.Stars, framework.MainDisplayOffscreen->Color.metrics());

	//::gpk::geometryBuildCube(app.Geometry);
	//::gpk::geometryBuildGrid(app.Geometry, {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Scene.Geometry, 4U, 2U, 1, {0, 1});
	//::gpk::geometryBuildFigure0(app.Geometry, 10U, 10U, 1, {});

	app.Scene.Models[::modelCreate(app)].Position				= {-30};
	app.Scene.Models[::modelCreate(app)].Position				= {+30};

	app.Scene.Image.resize(256, 256);
	for(uint32_t y = 0; y < app.Scene.Image.metrics().y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < app.Scene.Image.metrics().x; ++x) {
		app.Scene.Image[y][x] = {(uint8_t)rand(), (uint8_t)rand(), (uint8_t)rand(), 0xFF};
	}

	return 0;
}



int													updateStars			(::SStars & stars, uint32_t yMax, float lastFrameSeconds)	{
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::gpk::SCoord2<float>									 & starPos			= stars.Position[iStar];
		float													starSpeed			= stars.Speed[iStar];
		starPos.y											+= starSpeed * lastFrameSeconds;
		stars.Brightness[iStar]								= float(1.0 / 65535 * rand());
		if(starPos.y >= yMax) {
			stars.Speed		[iStar]								= float(1 + (rand() % 16));
			starPos.y											= 0;
		}
	}
	return 0;
}

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
// returns t value of intersection and intersection point q
int													intersectRaySphere		(const ::gpk::SCoord3<float> & p, const ::gpk::SCoord3<float> & d, const ::gpk::SCoord3<float> & sphereCenter, double sphereRadius, float &t, ::gpk::SCoord3<float> &q) {
	const ::gpk::SCoord3<float>								m						= p - sphereCenter;
	double													b						= m.Dot(d);
	double													c						= m.Dot(m) - sphereRadius * sphereRadius;

	if (c > 0.0f && b > 0.0f)	// Exit if r�s origin outside s (c > 0) and r pointing away from s (b > 0)
		return 0;
	double													discr					= b * b - c;

	if (discr < 0.0f)	// A negative discriminant corresponds to ray missing sphere
		return 0;

	t					= (float)(-b - sqrt(discr));	// Ray now found to intersect sphere, compute smallest t value of intersection
	if (t < 0.0f)	// If t is negative, ray started inside sphere so clamp t to zero
		t					= 0.0f;

	q					= p + d * t;
	return 1;
}
int													updateGame				(::gme::SApplication & app)	{
	::gpk::SFramework										& framework			= app.Framework;
	//------------------------------------------- Handle input
	double													speed				= 10;
	double													lastFrameSeconds	= framework.Timer.LastTimeSeconds;
	app.AnimationTime									+= lastFrameSeconds;
	app.ShotsPlayer.Delay								+= lastFrameSeconds * 20;
	app.ShotsEnemy.Delay								+= lastFrameSeconds * 7;
	::ced::SModel3D											& modelPlayer		= app.Scene.Models[0];
	::ced::SModel3D											& modelEnemy		= app.Scene.Models[7];

	modelEnemy.Position.z								= (float)(sin(app.AnimationTime) * 20);

	if(GetAsyncKeyState(VK_SPACE)) {
		::gpk::SCoord3<float>									direction			= {1, 0, 0};
		direction.RotateY(rand() * (1.0 / 65535) * ::gpk::math_pi * .0185 * ((rand() % 2) ? -1 : 1));
		direction.RotateZ(rand() * (1.0 / 65535) * ::gpk::math_pi * .0185 * ((rand() % 2) ? -1 : 1));
		app.ShotsPlayer.Spawn(modelPlayer.Position, direction, 200);
	}

	if(1 < (modelPlayer.Position - modelEnemy.Position).Length()) {
		::gpk::SCoord3<float>									direction			= modelPlayer.Position - modelEnemy.Position;
		direction.RotateY(rand() * (1.0 / 65535) * ::gpk::math_pi * .0185 * ((rand() % 2) ? -1 : 1));
		app.ShotsEnemy.Spawn(modelEnemy.Position, direction.Normalize(), 20);
	}

	if(GetAsyncKeyState('Q')) app.Scene.Camera.Position.y			-= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) app.Scene.Camera.Position.y			+= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	if(GetAsyncKeyState('W')) app.Scene.Models[0].Position.x		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('S')) app.Scene.Models[0].Position.x		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('A')) app.Scene.Models[0].Position.z		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('D')) app.Scene.Models[0].Position.z		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));

	if(GetAsyncKeyState(VK_NUMPAD8)) app.Scene.Models[0].Rotation.x	-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD2)) app.Scene.Models[0].Rotation.x	+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD6)) app.Scene.Models[0].Rotation.z	-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD4)) app.Scene.Models[0].Rotation.z	+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));

	app.Scene.Models[0].Rotation.y							+= (float)lastFrameSeconds;
	for(uint32_t iModel = 1; iModel < app.Scene.Models.size(); ++iModel)
		app.Scene.Models[iModel].Rotation.y						+= (float)lastFrameSeconds * 5;

	app.Scene.LightVector										= app.Scene.LightVector.RotateY(lastFrameSeconds * 2);
	if(framework.MainDisplay.Resized)
		::setupStars(app.Stars, framework.MainDisplayOffscreen->Color.metrics());
	::updateStars(app.Stars, framework.MainDisplayOffscreen->Color.metrics().y, (float)lastFrameSeconds);
	app.ShotsPlayer	.Update((float)lastFrameSeconds);
	app.ShotsEnemy	.Update((float)lastFrameSeconds);
	app.Debris		.Update((float)lastFrameSeconds);

	::ced::SModelTransform									matricesParent;
	for(uint32_t iShot = 0; iShot < app.ShotsPlayer.Position.size(); ++iShot) {
		const ::gpk::SLine3D<float>								shotSegment			= {app.ShotsPlayer.PositionPrev[iShot], app.ShotsPlayer.Position[iShot]};
		for(uint32_t iModel = 7; iModel < app.Scene.Models.size(); ++iModel) {
			if(-1 == app.Scene.Entities[iModel].Parent)
				continue;
			if(app.Health[iModel] <= 0)
				continue;
			::gpk::SCoord3<float>									sphereCenter		= app.Scene.Models[iModel].Position;
			matricesParent.Scale	.Scale			(app.Scene.Models[app.Scene.Entities[iModel].Parent].Scale, true);
			matricesParent.Rotation	.Rotation		(app.Scene.Models[app.Scene.Entities[iModel].Parent].Rotation);
			matricesParent.Position	.SetTranslation	(app.Scene.Models[app.Scene.Entities[iModel].Parent].Position, true);

			::gpk::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
			sphereCenter										= matrixTransformParent.Transform(sphereCenter);

			float													t				= 0;
			::gpk::SCoord3<float>									q				= {};

			if( intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), sphereCenter, 1, t, q)
			 && t < 1
			) {
				app.Health[iModel]									-= 500;
				for(uint32_t i = 0; i < 10; ++i) {
					::gpk::SCoord3<float>			direction			= {0, 1, 0};
					direction.RotateX(rand() * (::gpk::math_2pi / 65535));
					direction.RotateY(rand() * (::gpk::math_2pi / 65535));
					direction.RotateZ(rand() * (::gpk::math_2pi / 65535));
					direction.Normalize();
					app.Debris.Spawn(q, direction, 50);
				}
				app.ShotsPlayer.Remove(iShot);
				--iShot;
				break;
			}
		}
	}

	for(uint32_t iShot = 0; iShot < app.ShotsEnemy.Position.size(); ++iShot) {
		const ::gpk::SLine3D<float>								shotSegment			= {app.ShotsEnemy.PositionPrev[iShot], app.ShotsEnemy.Position[iShot]};
		for(uint32_t iModel = 0; iModel < 7; ++iModel) {
			if(-1 == app.Scene.Entities[iModel].Parent)
				continue;
			if(app.Health[iModel] <= 0)
				continue;
			::gpk::SCoord3<float>									sphereCenter		= app.Scene.Models[iModel].Position;
			matricesParent.Scale	.Scale			(app.Scene.Models[app.Scene.Entities[iModel].Parent].Scale, true);
			matricesParent.Rotation	.Rotation		(app.Scene.Models[app.Scene.Entities[iModel].Parent].Rotation);
			matricesParent.Position	.SetTranslation	(app.Scene.Models[app.Scene.Entities[iModel].Parent].Position, true);

			::gpk::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
			sphereCenter										= matrixTransformParent.Transform(sphereCenter);

			float													t				= 0;
			::gpk::SCoord3<float>									q				= {};

			if( intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), sphereCenter, 1, t, q)
			 && t < 1
			) {
				app.Health[iModel]									-= 500;
				for(uint32_t i = 0; i < 10; ++i) {
					::gpk::SCoord3<float>									direction			= {0, 1, 0};
					direction.RotateX(rand() * (::gpk::math_2pi / 65535));
					direction.RotateY(rand() * (::gpk::math_2pi / 65535));
					direction.RotateZ(rand() * (::gpk::math_2pi / 65535));
					direction.Normalize();
					app.Debris.Spawn(q, direction, 50);
				}
				app.ShotsEnemy.Remove(iShot);
				--iShot;
				break;
			}
		}
	}
	return 0;
}