#include "ced_demo_13_game.h"

#include "gpk_collision.h"

#include <Windows.h>
#include <mmsystem.h>

static	int											explosionAdd		(::gpk::array_obj<::ssg::SExplosion> & explosions, int32_t indexMesh, uint32_t triangleCount, const ::gpk::SCoord3<float> &collisionPoint, double debrisSpeed) {
	::ssg::SExplosion										newExplosion		= {};
	newExplosion.IndexMesh								= indexMesh;
	for(uint32_t iTriangle = 0; iTriangle < triangleCount; ) {
		const	uint32_t										sliceTriangleCount	= ::gpk::min((uint32_t)(rand() % 32), (uint32_t)(triangleCount - iTriangle));
		newExplosion.Slices.push_back({(uint16_t)iTriangle, (uint16_t)sliceTriangleCount});
		iTriangle											+= sliceTriangleCount;

		::gpk::SCoord3<float>									direction			= {0, 1, 0};
		direction.RotateX(rand() * (::gpk::math_2pi / RAND_MAX));
		direction.RotateY(rand() * (::gpk::math_2pi / RAND_MAX));
		direction.RotateZ(rand() * (::gpk::math_2pi / RAND_MAX));
		direction.Normalize();
		newExplosion.Particles.Spawn(collisionPoint, direction, (float)debrisSpeed);
	}
	for(uint32_t iExplosion = 0; iExplosion < explosions.size(); ++iExplosion) {
		::ssg::SExplosion										& explosion					= explosions[iExplosion];
		if(0 == explosion.Slices.size()) {
			explosion											= newExplosion;
			return iExplosion;
		}
	}
	return explosions.push_back(newExplosion);
}

static	int											applyDamage
	( const int32_t						weaponDamage
	, int32_t							& healthPart
	, int32_t							& healthParent
	) {
	const uint32_t											finalDamage					= ::gpk::min(weaponDamage, healthPart);
	healthPart											-= finalDamage;
	healthParent										-= finalDamage;
	return 0 >= healthPart;
}

static	int											collisionDetect		(::ssg::SShots & shots, const ::gpk::SCoord3<float> & modelPosition, ::gpk::array_pod<::gpk::SCoord3<float>> & collisionPoints)	{
	bool													detected			= false;
	collisionPoints.clear();
	for(uint32_t iShot = 0; iShot < shots.Particles.Position.size(); ++iShot) {
		const ::gpk::SLine3<float>								shotSegment			= {shots.PositionPrev[iShot], shots.Particles.Position[iShot]};
		float													t					= 0;
		::gpk::SCoord3<float>									collisionPoint		= {};
		if( ::gpk::intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), {1.2, modelPosition}, t, collisionPoint)
			&& t < 1
		) {
			detected											= true;
			collisionPoints.push_back(collisionPoint);
			shots.Remove(iShot);
			--iShot;
		}
	}
	return 0;
}

static	int											handleCollisionPoint	(::ssg::SSolarSystem & solarSystem, int32_t weaponDamage, ::ssg::SShipPart& damagedPart, ::ssg::SShip & damagedShip, const ::gpk::SCoord3<float> & sphereCenter, const ::gpk::SCoord3<float> & collisionPoint, void* soundAlias)	{
	PlaySound((LPCTSTR)soundAlias, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
	const ::gpk::SCoord3<float>								bounceVector				= (collisionPoint - sphereCenter).Normalize();
	solarSystem.Debris.SpawnDirected(5, 0.3, bounceVector, collisionPoint, 50, 1);
	solarSystem.Score									+= 1;
	if(::applyDamage(weaponDamage, damagedPart.Health, damagedShip.Health)) {	// returns true if health reaches zero
		solarSystem.Score									+= 10;
		const ::ssg::SEntity									& entityGeometry			= solarSystem.Entities[damagedPart.Entity + 1];
		const int32_t											indexMesh					= entityGeometry.Geometry;
		const uint32_t											countTriangles				= solarSystem.Scene.Geometry[indexMesh].Triangles.size();
		::explosionAdd(solarSystem.Explosions, indexMesh, countTriangles, collisionPoint, 60);
		solarSystem.Debris.SpawnSpherical(30, collisionPoint, 60, 2);
		if(0 >= damagedShip.Health) {
			solarSystem.Score									+= 50;
			const ::gpk::SCoord3<float>								& parentPosition			= solarSystem.ShipPhysics.Transforms[solarSystem.Entities[damagedShip.Entity].Body].Position;
			::explosionAdd(solarSystem.Explosions, indexMesh, countTriangles, parentPosition, 13);
			solarSystem.Debris.SpawnSpherical(150, parentPosition, 13, 2.8f);
			solarSystem.Slowing									= true;
			solarSystem.TimeScale								= 1.0;
		}
		return 1;
	}
	return 0;
}

static	int											updateEntityTransforms		(uint32_t iEntity, ::gpk::array_obj<::ssg::SEntity> & entities, ::ssg::SShipScene & scene, ::gpk::SIntegrator3 & bodies)	{
	const ::ssg::SEntity									& entity					= entities[iEntity];
	if(-1 == entity.Body) {
		if(-1 == entity.Parent)
			scene.ModelMatricesGlobal[iEntity].Identity();
		else
			scene.ModelMatricesGlobal[iEntity]					= scene.ModelMatricesGlobal[entity.Parent];
	}
	else {
		::gpk::SMatrix4<float>									& matrixGlobal					= scene.ModelMatricesGlobal[iEntity];
		bodies.GetTransform(entity.Body, matrixGlobal);
		if(-1 != entity.Parent)
			matrixGlobal										*= scene.ModelMatricesGlobal[entity.Parent];
	}
	for(uint32_t iChild = 0; iChild < entity.Children.size(); ++iChild) {
		const uint32_t											iChildEntity				= entity.Children[iChild];
		::updateEntityTransforms(iChildEntity, entities, scene, bodies);
	}
	return 0;
}

static	int											updateShots				(::ssg::SSolarSystem & solarSystem, double secondsLastFrame)	{
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::ssg::SShip											& ship					= solarSystem.Ships[iShip];
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart)
			ship.Parts[iPart].Shots.Update((float)secondsLastFrame);
	}

	::gpk::array_pod<::gpk::SCoord3<float>>					collisionPoints;
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::ssg::SShip											& ship					= solarSystem.Ships[iShip];
		for(uint32_t iShip2 = 0; iShip2 < solarSystem.Ships.size(); ++iShip2) {
			::ssg::SShip											& ship2					= solarSystem.Ships[iShip2];
			if(ship2.Health <= 0 || ship.Team == ship2.Team)
				continue;
			for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
				::ssg::SShipPart										& shipPart				= ship.Parts[iPart];
				void													* soundAlias			= (iShip2 ? (void*)SND_ALIAS_SYSTEMHAND : (void*)SND_ALIAS_SYSTEMEXCLAMATION);
				for(uint32_t iPart2 = 0; iPart2 < ship2.Parts.size(); ++iPart2) {
					::ssg::SShipPart										& shipPart2				= ship2.Parts[iPart2];
					if(shipPart2.Health <= 0)
						continue;
					::ssg::SEntity											& entity					= solarSystem.Entities[shipPart2.Entity];
					if(-1 != entity.Geometry) {
						const ::gpk::SMatrix4<float>							matrixTransform			= solarSystem.Scene.ModelMatricesGlobal[shipPart2.Entity];
						const ::gpk::SCoord3<float>								entityPosition			= matrixTransform.GetTranslation();
						::collisionDetect(shipPart.Shots, entityPosition, collisionPoints);
						for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
							if(::handleCollisionPoint(solarSystem, shipPart.Shots.Damage, shipPart2, ship2, entityPosition, collisionPoints[iCollisionPoint], soundAlias))	// returns true if part health reaches zero.
								break;
					}
					for(uint32_t iEntity = 0; iEntity < entity.Children.size(); ++iEntity) {
						::ssg::SEntity											& entityChild				= solarSystem.Entities[entity.Children[iEntity]];
						if(-1 == entityChild.Parent)
							continue;
						if(-1 == entityChild.Geometry)
							continue;
						const ::gpk::SMatrix4<float>							matrixTransform			= solarSystem.Scene.ModelMatricesGlobal[entity.Children[iEntity]];
						const ::gpk::SCoord3<float>								entityPosition			= matrixTransform.GetTranslation();
						::collisionDetect(shipPart.Shots, entityPosition, collisionPoints);
						for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
							if(::handleCollisionPoint(solarSystem, shipPart.Shots.Damage, shipPart2, ship2, entityPosition, collisionPoints[iCollisionPoint], soundAlias))	// returns true if part health reaches zero.
								break;
					}
				}
			}
		}
	}
	return 0;
}

static	int											updateShipPart			(::ssg::SSolarSystem & solarSystem, int32_t team, ::ssg::SShipPart & shipPart, double secondsLastFrame)	{
	for(uint32_t iParticle = 0; iParticle < shipPart.Shots.Particles.Position.size(); ++iParticle)
		shipPart.Shots.Particles.Position[iParticle].x		-= (float)(solarSystem.RelativeSpeedCurrent * secondsLastFrame * .2);

	shipPart.Shots.Delay								+= secondsLastFrame;

	::gpk::SCoord3<float>									targetPositionOriginal		= {};
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::ssg::SShip											& ship					= solarSystem.Ships[iShip];
		if(ship.Team == team || ship.Health <= 0)
			continue;
		targetPositionOriginal								= solarSystem.ShipPhysics.Transforms[solarSystem.Entities[ship.Entity].Body].Position;
	}
	::gpk::SCoord3<float>									targetPosition				= targetPositionOriginal;

	const ::gpk::SMatrix4<float>							& shipModuleMatrix			= solarSystem.Scene.ModelMatricesGlobal[solarSystem.Entities[shipPart.Entity + 1].Transform];
	::gpk::SCoord3<float>									positionGlobal				= shipModuleMatrix.GetTranslation();
	::gpk::SCoord3<float>									targetDistance				= targetPosition - positionGlobal;
	if(shipPart.Shots.Weapon == ::ssg::WEAPON_TYPE_CANNON) {
		if(1 < targetDistance.LengthSquared()) {
			::gpk::SCoord3<float>									direction					= targetDistance;
			direction.Normalize();
			shipPart.Shots.SpawnDirected(.02, positionGlobal, direction, 25, 1);
			if(solarSystem.AnimationTime > 1) {
				solarSystem.ShipPhysics.Forces[solarSystem.Entities[shipPart.Entity + 1].Body].Rotation	= {};
				::gpk::SMatrix4<float>									inverseTransform			= shipModuleMatrix.GetInverse();
				::gpk::STransform3										& shipModuleTransform		= solarSystem.ShipPhysics.Transforms[solarSystem.Entities[shipPart.Entity + 1].Body];
				::gpk::SCoord3<float>									up							= {1, 0, 0};
				::gpk::SCoord3<float>									front						= {0, 1, 0};
				//const ::gpk::SMatrix4<float>							& shipMatrix				= solarSystem.Scene.ModelMatricesGlobal[solarSystem.Entities[solarSystem.Entities[shipPart.Entity].Parent].Transform];
				targetPosition										= inverseTransform.Transform(targetPosition);
				::gpk::SQuaternion<float>								q;
				q.LookAt(shipModuleTransform.Position, targetPosition, up, front);
				shipModuleTransform.Orientation						= q;
			}
		}
	}
	else if(shipPart.Shots.Weapon == ::ssg::WEAPON_TYPE_GUN) {
		::gpk::SCoord3<float>									direction				= {team ? -1.0f : 1.0f, 0, 0};
		if(shipPart.Shots.Type == ::ssg::MUNITION_TYPE_RAY)
			shipPart.Shots.Spawn(positionGlobal, direction, 750, .75f);
		else
			shipPart.Shots.SpawnDirected(.02, positionGlobal, direction, 200, 5.0f);
	}
	else if(shipPart.Shots.Weapon == ::ssg::WEAPON_TYPE_SHOTGUN) {
		::gpk::SCoord3<float>									direction				= {team ? -1.0f : 1.0f, 0, 0};
		if(shipPart.Shots.Type == ::ssg::MUNITION_TYPE_RAY)
			shipPart.Shots.SpawnDirected(5, .05, positionGlobal, direction, 750, .75f);
		else
			shipPart.Shots.SpawnDirected(5, .05, positionGlobal, direction, 200, 5.0f);
	}
	return 0;
}

int													ssg::solarSystemUpdate				(::ssg::SSolarSystem & solarSystem, double actualSecondsLastFrame, ::gpk::SCoord2<uint32_t> screenSize)	{
	//------------------------------------------- Handle input
	double													speed							= 10;
	double													secondsLastFrame				= ::gpk::min(actualSecondsLastFrame, 0.15);
	if(solarSystem.Slowing) {
		solarSystem.TimeScale								-= secondsLastFrame * .35;
		if(solarSystem.TimeScale < .1)
			solarSystem.Slowing									= false;
	}
	else {
		if(solarSystem.TimeScale < .99)
			solarSystem.TimeScale							= ::gpk::min(1.0, solarSystem.TimeScale += secondsLastFrame * .45);
	}
	secondsLastFrame									*= solarSystem.TimeScale;
	solarSystem.AnimationTime							+= secondsLastFrame;

	solarSystem.ShipPhysics.Integrate(secondsLastFrame);

	::gpk::SCamera											& camera				= solarSystem.Scene.Camera[solarSystem.Scene.CameraMode];

	if(GetAsyncKeyState('Q')) camera.Position.y	-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) camera.Position.y	+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	::gpk::STransform3										& playerBody			= solarSystem.ShipPhysics.Transforms[solarSystem.Entities[0].Body];
	{
			 if(GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP		)) { playerBody.Position.x			+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8)); solarSystem.AccelerationControl	= +1; }
		else if(GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN	)) { playerBody.Position.x			-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8)); solarSystem.AccelerationControl	= -1; }
		else
			solarSystem.AccelerationControl	= 0;

		if(GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT	)) { playerBody.Position.z			+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8)); }
		if(GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT	)) { playerBody.Position.z			-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8)); }
	}

	solarSystem.CameraSwitchDelay									+= actualSecondsLastFrame;
	if(GetAsyncKeyState('C') && solarSystem.CameraSwitchDelay > .2) {
		solarSystem.CameraSwitchDelay									= 0;
		solarSystem.Scene.Camera[CAMERA_MODE_SKY].Target				= {};
		solarSystem.Scene.Camera[CAMERA_MODE_SKY].Position				= {-0.000001f, 250, 0};
		solarSystem.Scene.Camera[CAMERA_MODE_SKY].Up					= {0, 1, 0};

		solarSystem.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Target		= {};
		solarSystem.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Position		= {-0.000001f, 135, 0};
		solarSystem.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Up			= {0, 1, 0};
		solarSystem.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Position.RotateZ(::gpk::math_pi * .25);

		solarSystem.Scene.CameraMode = (CAMERA_MODE)((solarSystem.Scene.CameraMode + 1) % CAMERA_MODE_COUNT);
	}
	if(solarSystem.Scene.CameraMode == CAMERA_MODE_FOLLOW) {
		solarSystem.Scene.Camera[CAMERA_MODE_FOLLOW].Position			= playerBody.Position + ::gpk::SCoord3<float>{-80.f, 25, 0};
		solarSystem.Scene.Camera[CAMERA_MODE_FOLLOW].Target				= playerBody.Position + ::gpk::SCoord3<float>{1000.f, 0, 0};
		solarSystem.Scene.Camera[CAMERA_MODE_FOLLOW].Up					= {0, 1, 0};
	}


	if(camera.Position.y > 0.001f)
	if(camera.Position.y > 0.001f) if(GetAsyncKeyState(VK_HOME)) camera.Position.RotateZ(::gpk::math_pi * secondsLastFrame);
	if(camera.Position.x < 0.001f)
	if(camera.Position.x < 0.001f) if(GetAsyncKeyState(VK_END	)) camera.Position.RotateZ(::gpk::math_pi * -secondsLastFrame);

	if(camera.Position.y < 0) camera.Position.y = 0.0001f;
	if(camera.Position.y < 0) camera.Position.y = 0.0001f;
	if(camera.Position.x > 0) camera.Position.x = -0.0001f;
	if(camera.Position.x > 0) camera.Position.x = -0.0001f;

	if(GetAsyncKeyState(VK_NUMPAD5))
		playerBody.Orientation.MakeFromEulerTaitBryan({0, 0, (float)-::gpk::math_pi_2});
	else {
		if(GetAsyncKeyState(VK_NUMPAD8)) playerBody.Orientation.z		-= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD2)) playerBody.Orientation.z		+= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD6)) playerBody.Orientation.x		-= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD4)) playerBody.Orientation.x		+= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	}
	playerBody.Orientation.Normalize();

	bool													playing					= false;
	static constexpr const double							frameStep				= 0.01;
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::ssg::SShip											& enemyShip				= solarSystem.Ships[iShip];
		if(0 >= enemyShip.Health)
			continue;

		::gpk::STransform3										& shipTransform			= solarSystem.ShipPhysics.Transforms[solarSystem.Entities[enemyShip.Entity].Body];
		if(iShip) {
			shipTransform.Position.z							= (float)(sin(iShip + solarSystem.AnimationTime) * (iShip * 5.0) * ((iShip % 2) ? -1 : 1));
			shipTransform.Position.x							= (float)((iShip * 5.0) - solarSystem.Stage + 10 - (solarSystem.RelativeSpeedCurrent * solarSystem.RelativeSpeedCurrent * .0005 * ((solarSystem.RelativeSpeedCurrent >= 0) ? 1 : -1)  ));
			double													timeWaveVertical					= .1;
			if(0 == (solarSystem.Stage % 7)) {
				if(iShip % 2)
					shipTransform.Position.z							= (float)(cos(iShip + solarSystem.AnimationTime) * ((solarSystem.Ships.size() - 1 - iShip) * 4.0) * ((iShip % 2) ? -1 : 1));
				else
					shipTransform.Position.z							= (float)(sin(iShip + solarSystem.AnimationTime) * (iShip * 4.0) * ((iShip % 2) ? -1 : 1));
					 if(0 == (iShip % 2)) timeWaveVertical	= .65;
				else if(0 == (iShip % 3)) timeWaveVertical	= .80;
				else if(0 == (iShip % 7)) timeWaveVertical	= .80;
			}
			else if(0 == (solarSystem.Stage % 5)) {
					shipTransform.Position.z							= (float)(cos(iShip + solarSystem.AnimationTime) * ((solarSystem.Ships.size() - 1 - iShip) * 3.0) * ((iShip % 2) ? -1 : 1));
					 if(0 == (iShip % 2)) timeWaveVertical	= .50;
				else if(0 == (iShip % 3)) timeWaveVertical	= .75;
				else if(0 == (iShip % 7)) timeWaveVertical	= .80;
			}
			else if(0 == (solarSystem.Stage % 3)) {
					shipTransform.Position.z							= (float)(cos(iShip + solarSystem.AnimationTime) * ((solarSystem.Ships.size() - 1 - iShip) * 2.0) * ((iShip % 2) ? -1 : 1));
					 if(0 == (iShip % 2)) timeWaveVertical	= .25;
				else if(0 == (iShip % 3)) timeWaveVertical	= .50;
				else if(0 == (iShip % 7)) timeWaveVertical	= .75;
			}
			else {
					 if(0 == (iShip % 2)) timeWaveVertical	= .50;
				else if(0 == (iShip % 3)) timeWaveVertical	= .25;
				else if(0 == (iShip % 7)) timeWaveVertical	= .15;
			}
			shipTransform.Position.x								+= (float)(sin(solarSystem.AnimationTime * timeWaveVertical * ::gpk::math_2pi) * ((iShip % 2) ? -1 : 1));
		}
		if(iShip)
			playing												= true;
	}

	{
		::std::lock_guard<::std::mutex>							lockUpdate			(solarSystem.LockUpdate);
		for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
			::ssg::SShip											& ship					= solarSystem.Ships[iShip];
			for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
				::ssg::SShipPart										& shipPart				= ship.Parts[iPart];
				memcpy(shipPart.Shots.PositionDraw.begin(), shipPart.Shots.Particles.Position.begin(), shipPart.Shots.Particles.Position.size() * sizeof(::gpk::SCoord3<float>));
			}
		}
	}
	{
		::std::lock_guard<::std::mutex>							lockUpdate			(solarSystem.LockUpdate);

		double													secondsToProcess		= secondsLastFrame;
		while(secondsToProcess > frameStep) {
			for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
				::ssg::SShip											& ship					= solarSystem.Ships[iShip];
				if(0 >= ship.Health)
					continue;
				for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
					::ssg::SShipPart										& shipPart				= ship.Parts[iPart];
					if(0 >= shipPart.Health)
						continue;
					::updateShipPart(solarSystem, ship.Team, shipPart, frameStep);
				}
			}
			::updateShots(solarSystem, frameStep);
			secondsToProcess									-= frameStep;
		}
		{
			for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
				::ssg::SShip											& ship					= solarSystem.Ships[iShip];
				for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
					::ssg::SShipPart										& shipPart				= ship.Parts[iPart];
					if(0 >= shipPart.Health)
						continue;
					::updateShipPart(solarSystem, ship.Team, shipPart, secondsToProcess);
				}
			}
			::updateShots(solarSystem, secondsToProcess);
		}

		const int32_t											randDebris			= rand();
		solarSystem.Debris.Spawn({200.0f, ((randDebris % 2) ? -1.0f : 1.0f) * (randDebris % 400), ((randDebris % 3) ? -1.0f : 1.0f) * (randDebris % 400)}, {-1, 0, 0}, 400, 2);
		solarSystem.Debris.Update((float)secondsLastFrame);
		solarSystem.Stars.Update(screenSize.y, (float)secondsLastFrame);
		for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion) {
			solarSystem.Explosions[iExplosion].Update((float)secondsLastFrame);
		}
	}
	if(false == playing) {
		playing												= false;
		for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion) {
			if(solarSystem.Explosions[iExplosion].Slices.size()) {
				playing											= true;
				break;
			}
		}
		if(false == playing)  {	// Set up enemy ships
			::std::lock_guard<::std::mutex>							lockUpdate			(solarSystem.LockUpdate);
			::ssg::stageSetup(solarSystem);
			playing												= true;
		}
	}

	for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion) {
		for(uint32_t iParticle = 0; iParticle < solarSystem.Explosions[iExplosion].Particles.Position.size(); ++iParticle)
			solarSystem.Explosions[iExplosion].Particles.Position[iParticle].x	-= (float)(solarSystem.RelativeSpeedCurrent * secondsLastFrame);
	}
	for(uint32_t iParticle = 0; iParticle < solarSystem.Debris.Particles.Position.size(); ++iParticle)
		solarSystem.Debris.Particles.Position[iParticle].x	-= (float)(solarSystem.RelativeSpeedCurrent * secondsLastFrame);

	static constexpr	const double relativeAcceleration = 20;
	if(solarSystem.AccelerationControl == 0) {
		if(solarSystem.RelativeSpeedCurrent > solarSystem.RelativeSpeedTarget)
			solarSystem.RelativeSpeedCurrent	-= secondsLastFrame * relativeAcceleration;
		else if(solarSystem.RelativeSpeedCurrent < solarSystem.RelativeSpeedTarget)
			solarSystem.RelativeSpeedCurrent	+= secondsLastFrame * relativeAcceleration;
	}
	else if(solarSystem.AccelerationControl > 0) {
		solarSystem.RelativeSpeedCurrent	+= secondsLastFrame * relativeAcceleration;
	}
	else if(solarSystem.AccelerationControl < 0) {
		solarSystem.RelativeSpeedCurrent	-= secondsLastFrame * relativeAcceleration;
	}

	for(uint32_t iEntity = 0; iEntity < solarSystem.Entities.size(); ++iEntity) {
		const ::ssg::SEntity									& entity					= solarSystem.Entities[iEntity];
		if(-1 == entity.Parent)	// process root entities
			updateEntityTransforms(iEntity, solarSystem.Entities, solarSystem.Scene, solarSystem.ShipPhysics);
	}
	//solarSystem.Scene.LightVector									= solarSystem.Scene.LightVector.RotateY(secondsLastFrame * 2);
	return 0;
}
