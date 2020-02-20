#include "ced_game.h"
#include "gpk_png.h"

int													setupStars			(SStars & stars, ::gpk::SCoord2<uint32_t> targetSize)	{
	if(0 == targetSize.y) return 0;
	if(0 == targetSize.x) return 0;
	stars.Speed		.resize(128, 0);
	stars.Brightness.resize(128, 0);
	stars.Position	.resize(128, {});
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		stars.Speed			[iStar]						= float(16 + (rand() % 64));
		stars.Brightness	[iStar]						= float(1.0 / 65535 * rand());
		stars.Position		[iStar].y					= float(rand() % targetSize.y);
		stars.Position		[iStar].x					= float(rand() % targetSize.x);
	}
	return 0;
}

int													modelCreate			(::gme::SApplication & app)	{
	int32_t													indexModel			= app.Scene.ModelMatricesLocal.size();
	uint32_t												countParts			= 6;
	app.Health						.resize(indexModel + 1 + countParts);
	app.Scene.ModelMatricesLocal	.resize(indexModel + 1 + countParts);
	app.Scene.ModelMatricesGlobal	.resize(indexModel + 1 + countParts);
	app.Scene.Models				.resize(indexModel + 1 + countParts);
	app.Scene.Entities				.resize(indexModel + 1 + countParts);

	app.Scene.Models		[indexModel]				= {};
	app.Scene.Models		[indexModel].Scale			= {1, 1, 1};
	if(0 == indexModel)
		app.Scene.Models		[indexModel].Rotation.z		= (float)(-::gpk::math_pi_2);
	else
		app.Scene.Models		[indexModel].Rotation.z		= (float)(::gpk::math_pi_2);
	app.Scene.Entities		[indexModel]				= {-1};
	const uint32_t												partHealth			= 1000;
	app.Health[indexModel]									= partHealth * countParts;
	for(uint32_t iModel = indexModel + 1; iModel < app.Scene.Models.size(); ++iModel) {
		::ced::SModel3D											& model			= app.Scene.Models[iModel];
		model.Scale											= {1, 1, 1};
		model.Rotation										= {0, 0, 0};
		model.Position										= {2, 0.5};
		model.Position.RotateY(::gpk::math_2pi / countParts * iModel);
		::ced::SEntity											& entity		= app.Scene.Entities[iModel];
		entity.Parent										= indexModel;
		app.Health[iModel]									= partHealth;
 		app.Scene.Entities[indexModel].Children.push_back(iModel);
	}
	return indexModel;
}

int													setupGame			(::gme::SApplication & app)	{
	::gpk::SFramework										& framework			= app.Framework;
	::setupStars(app.Stars, framework.MainDisplay.Size);


	//srand((uint32_t)time(0));

	app.Scene.Geometry.resize(5);
	//::ced::geometryBuildGrid(app.Scene.Geometry[0], {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Scene.Geometry[0],  12U, 8U, 1, {0, 1});
	//::ced::geometryBuildCube(app.Scene.Geometry[0]);

	//::ced::geometryBuildSphere(app.Scene.Geometry[1],  4U, 3U, 1, {0, 1});
	::ced::geometryBuildCube(app.Scene.Geometry[1]);
	//::ced::geometryBuildGrid(app.Scene.Geometry[1], {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Scene.Geometry[2],  3U, 2U, 1, {0, 1});
	::ced::geometryBuildSphere(app.Scene.Geometry[3],  4U, 2U, 1, {0, 1});
	::ced::geometryBuildSphere(app.Scene.Geometry[4],  16U, 2U, 1, {0, 1});
	//::ced::geometryBuildFigure0(app.Geometry, 10U, 10U, 1, {});

	app.Scene.Models[::modelCreate(app)].Position		= {-30};
	app.Scene.Models[::modelCreate(app)].Position		= {+20};
	app.Scene.Models[::modelCreate(app)].Position		= {+25};
	app.Scene.Models[::modelCreate(app)].Position		= {+30};
	app.Scene.Models[::modelCreate(app)].Position		= {+35};

	::gpk::SColorFloat										baseColor	[4]	=
		{ ::gpk::LIGHTGREEN
		, ::gpk::LIGHTBLUE
		, ::gpk::LIGHTRED
		, ::gpk::LIGHTCYAN
		};

	app.Scene.Image.resize(5);
	for(uint32_t iImage = 0; iImage < app.Scene.Image.size(); ++iImage) {
		if(app.Scene.Image[iImage].Texels.size())
			continue;
		app.Scene.Image[iImage].resize(24, 8);
		for(uint32_t y = 0; y < app.Scene.Image[iImage].metrics().y; ++y) {// Generate noise color for planet texture
			//bool yAffect = rand() % 3;
			bool xAffect = (y % 2);
			::gpk::SColorFloat										lineColor					= baseColor[rand() % ::std::size(baseColor)];
			//bool zAffect = 0 == y % 2;
			for(uint32_t x = 0; x < app.Scene.Image[iImage].metrics().x; ++x) {
				app.Scene.Image[iImage][y][x] = lineColor * (xAffect ? ::gpk::max(.5, sin(x)) : 1);
			}
		}
	}
	app.Scene.Camera.Target				= {};
	app.Scene.Camera.Position			= {-0.000001f, 100, 0};
	app.Scene.Camera.Up					= {0, 1, 0};


	return 0;
}

int													handleShotCollision	(const ::gpk::SCoord3<float> & collisionPoint, int32_t & healthParth, int32_t & healthParent, ::SDebris & debris)	{
	//PlaySoundA((LPCSTR)soundAlias, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
	healthParth									-= 100;
	healthParent								-= 100;
	float													debrisSpeed					= 50;
	float													debrisBright				= 1;
	uint32_t												debrisCount					= 10;
	if(0 >= healthParth)
		if(0 >= healthParent) {
			debrisSpeed					= 10	;
			debrisCount					= 100	;
			debrisBright				= 3	;
		}
		else {
			debrisSpeed					= 60	;
			debrisCount					= 150	;
			debrisBright				= 2		;
		}
	debris.SpawnSpherical(debrisCount, collisionPoint, debrisSpeed, debrisBright);
	return 0;
}

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
// returns t value of intersection and intersection point q
int													intersectRaySphere		(const ::gpk::SCoord3<float> & p, const ::gpk::SCoord3<float> & d, const ::gpk::SCoord3<float> & sphereCenter, double sphereRadius, float &t, ::gpk::SCoord3<float> &q) {
	const ::gpk::SCoord3<float>								m						= p - sphereCenter;
	double													b						= m.Dot(d);
	double													c						= m.Dot(m) - sphereRadius * sphereRadius;

	if (c > 0.0f && b > 0.0f)	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
		return 0;
	double													discr					= b * b - c;

	if (discr < 0.0f)	// A negative discriminant corresponds to ray missing sphere
		return 0;

	t													= (float)(-b - sqrt(discr));	// Ray now found to intersect sphere, compute smallest t value of intersection
	if (t < 0.0f)	// If t is negative, ray started inside sphere so clamp t to zero
		t													= 0.0f;

	q													= p + d * t;
	return 1;
}
int													updateGame				(::gme::SApplication & app)	{
	::gpk::SFramework										& framework			= app.Framework;
	//------------------------------------------- Handle input
	double													speed				= 10;
	double													lastFrameSeconds	= ::gpk::min(framework.Timer.LastTimeSeconds, 0.200);
	app.AnimationTime									+= lastFrameSeconds;
	::ced::SModelTransform									matricesParent;
	::ced::SModel3D											& modelPlayer		= app.Scene.Models[0];
	for(uint32_t iEnemy = 7; iEnemy < app.Scene.Models.size(); ++iEnemy) {
		const int32_t											indexParent				= app.Scene.Entities[iEnemy].Parent;
		if(0 >= app.Health[iEnemy])
			continue;
		::ced::SModel3D											& modelEnemy		= app.Scene.Models[iEnemy];
		if(-1 == indexParent) {
			modelEnemy.Position.z								= (float)(sin(app.AnimationTime) * iEnemy * 3) * ((iEnemy % 2) ? -1 : 1);
		}
		else {
			app.Scene.Models[iEnemy].Rotation.y					+= (float)lastFrameSeconds * 1;
			matricesParent										= {};
			app.ShotsEnemy.Delay								+= lastFrameSeconds / 2;
			const ::ced::SModel3D									& modelParent			= app.Scene.Models[indexParent];
			matricesParent.Scale	.Scale			(modelParent.Scale, true);
			matricesParent.Rotation	.Rotation		(modelParent.Rotation);
			matricesParent.Position	.SetTranslation	(modelParent.Position, true);
			::gpk::SCoord3<float>									positionGlobal			= (matricesParent.Scale * matricesParent.Rotation * matricesParent.Position).Transform(modelEnemy.Position);
			if(1 < (modelPlayer.Position - positionGlobal).Length()) {
				::gpk::SCoord3<float>									direction			= modelPlayer.Position - positionGlobal;
				direction.RotateY(rand() * (1.0 / 65535) * ::gpk::math_pi * .0185 * ((rand() % 2) ? -1 : 1));
				app.ShotsEnemy.Spawn(positionGlobal, direction.Normalize(), 20);
			}
		}
	}

	if(GetAsyncKeyState(VK_SPACE)) {
		for(uint32_t iEnemy = 1; iEnemy < 7; ++iEnemy) {
			if(0 >= app.Health[iEnemy])
				continue;
			matricesParent										= {};
			const int32_t											indexParent				= app.Scene.Entities[iEnemy].Parent;
			::ced::SModel3D											& modelEnemy			= app.Scene.Models[iEnemy];
			const ::ced::SModel3D									& modelParent			= app.Scene.Models[indexParent];
			matricesParent.Scale	.Scale			(modelParent.Scale, true);
			matricesParent.Rotation	.Rotation		(modelParent.Rotation);
			matricesParent.Position	.SetTranslation	(modelParent.Position, true);
			::gpk::SCoord3<float>									positionGlobal			= (matricesParent.Scale * matricesParent.Rotation * matricesParent.Position).Transform(modelEnemy.Position);
			//positionGlobal.x									+= 1.5;
			app.ShotsPlayer.Delay								+= lastFrameSeconds * 10;
			::gpk::SCoord3<float>									direction			= {1, 0, 0};
			//direction.RotateY(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			//direction.RotateZ(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			app.ShotsPlayer.Spawn(positionGlobal, direction, 200);
		}
	}

	if(GetAsyncKeyState('Q')) app.Scene.Camera.Position.y			-= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) app.Scene.Camera.Position.y			+= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	if(GetAsyncKeyState('W')) app.Scene.Models[0].Position.x		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('S')) app.Scene.Models[0].Position.x		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('A')) app.Scene.Models[0].Position.z		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('D')) app.Scene.Models[0].Position.z		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));

	if(GetAsyncKeyState(VK_NUMPAD8)) app.Scene.Models[0].Rotation.x	-= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD2)) app.Scene.Models[0].Rotation.x	+= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD6)) app.Scene.Models[0].Rotation.z	-= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD4)) app.Scene.Models[0].Rotation.z	+= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD5)) app.Scene.Models[0].Rotation	= {0, 0, (float)::gpk::math_pi_2};

	modelPlayer.Rotation.y							+= (float)lastFrameSeconds * .5f;
	for(uint32_t iEnemy = 1; iEnemy < app.Scene.Models.size(); ++iEnemy)
		app.Scene.Models[iEnemy].Rotation.y					+= (float)lastFrameSeconds * (.1f * iEnemy);

	app.Scene.LightVector										= app.Scene.LightVector.RotateY(lastFrameSeconds * 2);
	if(framework.MainDisplay.Resized)
		::setupStars(app.Stars, framework.MainDisplay.Size);
	app.ShotsPlayer	.Update((float)lastFrameSeconds);
	app.ShotsEnemy	.Update((float)lastFrameSeconds);
	app.Debris		.Update((float)lastFrameSeconds);
	app.Stars		.Update(framework.MainDisplay.Size.y, (float)lastFrameSeconds);

	for(uint32_t iShot = 0; iShot < app.ShotsPlayer.Particles.Position.size(); ++iShot) {
		const ::gpk::SLine3<float>								shotSegment			= {app.ShotsPlayer.PositionPrev[iShot], app.ShotsPlayer.Particles.Position[iShot]};
		for(uint32_t iModel = 7; iModel < app.Scene.Models.size(); ++iModel) {
			const int32_t											indexParent				= app.Scene.Entities[iModel].Parent;
			if(-1 == indexParent)
				continue;
			if(app.Health[iModel] <= 0)
				continue;
			::gpk::SCoord3<float>									sphereCenter		= app.Scene.Models[iModel].Position;
			const ::ced::SModel3D									& modelParent			= app.Scene.Models[indexParent];
			matricesParent.Scale	.Scale			(modelParent.Scale, true);
			matricesParent.Rotation	.Rotation		(modelParent.Rotation);
			matricesParent.Position	.SetTranslation	(modelParent.Position, true);

			::gpk::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
			sphereCenter										= matrixTransformParent.Transform(sphereCenter);

			float													t				= 0;
			::gpk::SCoord3<float>									collisionPoint				= {};

			if( intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), sphereCenter, 1, t, collisionPoint)
			 && t < 1
			) {
				//PlaySoundA((LPCSTR)SND_ALIAS_SYSTEMHAND, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
				::handleShotCollision(collisionPoint, app.Health[iModel], app.Health[indexParent], app.Debris);
				app.ShotsPlayer.Remove(iShot);
				--iShot;
				break;
			}
		}
	}

	for(uint32_t iShot = 0; iShot < app.ShotsEnemy.Particles.Position.size(); ++iShot) {
		const ::gpk::SLine3<float>								shotSegment				= {app.ShotsEnemy.PositionPrev[iShot], app.ShotsEnemy.Particles.Position[iShot]};
		for(uint32_t iModel = 0; iModel < 7; ++iModel) {
			const int32_t											indexParent				= app.Scene.Entities[iModel].Parent;
			if(-1 == indexParent)
				continue;
			if(app.Health[iModel] <= 0)
				continue;
			::gpk::SCoord3<float>									sphereCenter			= app.Scene.Models[iModel].Position;
			const ::ced::SModel3D									& modelParent			= app.Scene.Models[indexParent];
			matricesParent.Scale	.Scale			(modelParent.Scale, true);
			matricesParent.Rotation	.Rotation		(modelParent.Rotation);
			matricesParent.Position	.SetTranslation	(modelParent.Position, true);

			::gpk::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
			sphereCenter										= matrixTransformParent.Transform(sphereCenter);

			float													t						= 0;
			::gpk::SCoord3<float>									collisionPoint						= {};

			if( intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), sphereCenter, 1, t, collisionPoint)
			 && t < 1
			) {
				//PlaySoundA((LPCSTR)SND_ALIAS_SYSTEMEXCLAMATION, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
				::handleShotCollision(collisionPoint, app.Health[iModel], app.Health[indexParent], app.Debris);
				app.ShotsEnemy.Remove(iShot);
				--iShot;
				break;
			}
		}
	}
	return 0;
}
