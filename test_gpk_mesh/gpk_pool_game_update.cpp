#include "gpk_pool_game.h"


static	::gpk::error_t		resolveCollision							
	( const ::gpk::SCoord3<double>	& initialVelocityA
	, const ::gpk::SCoord3<double>	& initialRotationA
	, const ::gpk::SCoord3<double>	& distanceDirection
	, double						& out_forceTransferRatioB	
	, ::gpk::SCoord3<float>			& out_finalRotationA
	, ::gpk::SCoord3<float>			& out_finalRotationB
	, ::gpk::SCoord3<float>			& out_finalVelocityA
	, ::gpk::SCoord3<float>			& out_finalVelocityB
	) {
	::gpk::SCoord3<double>			directionA			= initialVelocityA.Cast<double>();
	directionA.Normalize();
	out_forceTransferRatioB		= ::gpk::max(0.0f, (float)distanceDirection.Dot(directionA));
	if(0 >= out_forceTransferRatioB) {
		out_finalVelocityA			+= initialVelocityA.Cast<float>();
		out_finalRotationA			+= initialRotationA.Cast<float>();
		return 0;
	}
	if(1.0 <= out_forceTransferRatioB) {
		out_finalVelocityB			+= initialVelocityA.Cast<float>();
		out_finalRotationB			+= initialRotationA.Cast<float>();
		return 0;
	}

	bool							revert				= distanceDirection.Dot(directionA.RotateY(::gpk::math_pi_2)) >= 0; // ::revertCross(distanceDirection);
	double							speedA				= initialVelocityA.Length();
	const ::gpk::SCoord3<double>	vUp					= {0, revert ? -1 : 1.0f, 0};
	::gpk::SCoord3<double>			finalVelocityB		= distanceDirection * speedA * out_forceTransferRatioB;
	::gpk::SCoord3<double>			finalVelocityA		= ::gpk::SCoord3<double>{finalVelocityB}.Normalize().Cross(vUp).Normalize() * speedA * (1.0f - out_forceTransferRatioB);
	//const double					totalFinalSpeed		= finalVelocityA.Length() + finalVelocityB.Length();
	//const double					totalInitialSpeed	= initialVelocityA.Length();
	//if ((totalFinalSpeed - totalInitialSpeed) > 0.0001f) {
	//	warning_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
	//}
	//if ((totalFinalSpeed - totalInitialSpeed) < -0.0001f) {
	//	warning_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
	//}			

	out_finalVelocityA			+= finalVelocityA.Cast<float>();
	out_finalVelocityB			+= finalVelocityB.Cast<float>();

	//info_printf("Total initial rotation: %f", (float)(initialRotationA.Length()));

	const double					rotA				= initialRotationA.Length();
	const ::gpk::SCoord3<double>	finalRotationB		= initialRotationA.Cast<double>().Normalize() * rotA * out_forceTransferRatioB;
	const ::gpk::SCoord3<double>	finalRotationA		= initialRotationA.Cast<double>().Normalize() * rotA * (1.0 - out_forceTransferRatioB);
	//const double					finalRotA			= finalRotationB.Length();
	//const double					finalRotB			= finalRotationA.Length();
	//info_printf("Total final rotation: %f", (float)(finalRotA + finalRotB));
	out_finalRotationA			+= finalRotationA.Cast<float>();
	out_finalRotationB			+= finalRotationB.Cast<float>();
	return 0;
}

::gpk::error_t				gpk::collisionDetect			(const ::gpk::SEngine & engine, ::gpk::array_pod<::gpk::SContact> & contactsDetected) {
	for(uint32_t iEntityA = 0, countEntities = engine.ManagedEntities.Entities.size(); iEntityA < countEntities; ++iEntityA) {
		const ::gpk::SVirtualEntity		& entityA						= engine.ManagedEntities.Entities[iEntityA];
		if(entityA.RigidBody >= engine.Integrator.BodyFlags.size())
			continue;

		const ::gpk::SRigidBodyFlags	& flagsA						= engine.Integrator.BodyFlags[entityA.RigidBody];
		if(false == flagsA.Collides)
			continue;

		const ::gpk::SCoord3<float>		& positionA						= engine.Integrator.Centers[entityA.RigidBody].Position;
		const ::gpk::SCoord3<float>		& radiusA						= engine.Integrator.BoundingVolumes[entityA.RigidBody].HalfSizes;

		::gpk::SContact					contactBall						= {};
		contactBall.EntityA			= iEntityA;
		for(uint32_t iEntityB = iEntityA + 1; iEntityB < countEntities; ++iEntityB) {
			const ::gpk::SVirtualEntity		& entityB						= engine.ManagedEntities.Entities[iEntityB];
			if(entityB.RigidBody >= engine.Integrator.BodyFlags.size())
				continue;

			const ::gpk::SRigidBodyFlags	& flagsB						= engine.Integrator.BodyFlags[entityB.RigidBody];
			if(false == flagsB.Collides)
				continue;

			const ::gpk::SCoord3<float>		& positionB						= engine.Integrator.Centers[entityB.RigidBody].Position;
			const ::gpk::SCoord3<float>		& radiusB						= engine.Integrator.BoundingVolumes[entityB.RigidBody].HalfSizes;

			const float						maxDistance						= radiusA.x + radiusB.x;
			const float						collisionThreshold				= maxDistance * maxDistance;
			contactBall.Distance		= positionB - positionA;			
			const double					distanceSquared					= contactBall.Distance.LengthSquared();
			if(distanceSquared >= collisionThreshold) 
				continue;

			contactBall.EntityB			= iEntityB;
			contactBall.DistanceLength	= distanceSquared ? sqrt(distanceSquared) : 0.0f;
			contactsDetected.push_back(contactBall);
		}
	}
	return 0;
}

::gpk::error_t				the1::poolGameUpdate			(::the1::SPoolGame & pool, double secondsElapsed) {
	::gpk::SEngine					& engine						= pool.Engine;
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		pool.PositionDeltas[iBall].push_back({});
		pool.GetBallPosition(iBall, pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1].A);
	}

	double							step							= .005f;
	pool.LastFrameContactsBall		.clear();
	pool.LastFrameContactsCushion	.clear();
	::gpk::array_pod<::the1::SContactBall>	lastFrameContactsBatchBall;
	::gpk::array_pod<::the1::SContactBall>	lastFrameContactsBatchCushion;


	while(secondsElapsed > 0) { 
		double							secondsThisStep					= ::gpk::min(step, secondsElapsed);
		secondsElapsed				-= secondsThisStep;

		for(uint32_t iBallA = 0; iBallA < pool.StartState.BallCount; ++iBallA) {
			::gpk::SBodyForces				& forces						= engine.Integrator.Forces		[engine.ManagedEntities.Entities[pool.StartState.Balls[iBallA].Entity].RigidBody];
			::gpk::SRigidBodyFrame			& bodyAccum						= engine.Integrator.BodyFrames	[engine.ManagedEntities.Entities[pool.StartState.Balls[iBallA].Entity].RigidBody];
			if(::gpk::SCoord3<float>{forces.Velocity}.Normalize().Dot(::gpk::SCoord3<float>{forces.Rotation.z, 0, -forces.Rotation.x}.Normalize()) < 0) {
				bodyAccum.AccumulatedForce += ::gpk::SCoord3<float>{forces.Rotation.z, 0, -forces.Rotation.x} * secondsElapsed;
			}
			if(forces.Rotation.y)
				forces.Velocity.RotateY(forces.Rotation.y * secondsElapsed * .05);
		}
		engine.Update(secondsThisStep);


		lastFrameContactsBatchBall		.clear();
		::gpk::collisionDetect(pool.Engine, lastFrameContactsBatchBall);

		lastFrameContactsBatchCushion	.clear();

		for(uint32_t iContact = 0; iContact < lastFrameContactsBatchBall.size(); ++iContact) {
			::the1::SContactBall			& contact			= lastFrameContactsBatchBall[iContact];
			const ::gpk::SVirtualEntity		& entityA			= engine.ManagedEntities.Entities[contact.EntityA]; 
			const ::gpk::SVirtualEntity		& entityB			= engine.ManagedEntities.Entities[contact.EntityB]; 

			contact.Result.DistanceDirection	= contact.Distance;
			double							distanceLength		= contact.DistanceLength;
			contact.Result.DistanceDirection.Normalize();

			// Separate balls
			::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[entityA.RigidBody].Position;
			::gpk::SCoord3<float>			& positionB			= engine.Integrator.Centers[entityB.RigidBody].Position;
			positionA					+= contact.Result.DistanceDirection * ::gpk::max(1.0f - distanceLength, 0.0) * -.51f;
			positionB					+= contact.Result.DistanceDirection * ::gpk::max(1.0f - distanceLength, 0.0) * .51f;


			// Calculate force transfer
			::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[entityA.RigidBody].Velocity;
			::gpk::SCoord3<float>			& velocityB			= engine.Integrator.Forces[entityB.RigidBody].Velocity;
			::gpk::SCoord3<float>			& rotationA			= engine.Integrator.Forces[entityA.RigidBody].Rotation;
			::gpk::SCoord3<float>			& rotationB			= engine.Integrator.Forces[entityB.RigidBody].Rotation;

			contact.Result.InitialVelocityA	= velocityA;
			contact.Result.InitialVelocityB	= velocityB;
			contact.Result.InitialRotationA	= rotationA;
			contact.Result.InitialRotationB	= rotationB;
			velocityB					= {};
			velocityA					= {};
			rotationB					= {};
			rotationA					= {};
			if(contact.Result.InitialVelocityA.LengthSquared() || contact.Result.InitialRotationA.LengthSquared()) {
				engine.Integrator.BodyFlags[entityB.RigidBody].Active = true;
				::gpk::SCoord3<float>			lvelocityB					= {};
				::gpk::SCoord3<float>			lvelocityA					= {};
				::resolveCollision(contact.Result.InitialVelocityA.Cast<double>(), contact.Result.InitialRotationA.Cast<double>(), contact.Result.DistanceDirection.Cast<double>(), contact.Result.ForceTransferRatioB, rotationA, rotationB, lvelocityA, lvelocityB);
				velocityB					+= lvelocityB;
				velocityA					+= lvelocityA;
			}

			if(contact.Result.InitialVelocityB.LengthSquared() || contact.Result.InitialRotationB.LengthSquared()) {
				engine.Integrator.BodyFlags[entityA.RigidBody].Active = true;
				::gpk::SCoord3<float>			lvelocityB					= {};
				::gpk::SCoord3<float>			lvelocityA					= {};
				::resolveCollision(contact.Result.InitialVelocityB.Cast<double>(), contact.Result.InitialRotationB.Cast<double>(), contact.Result.DistanceDirection.Cast<double>() * -1, contact.Result.ForceTransferRatioA, rotationB, rotationA, lvelocityB, lvelocityA);
				velocityB					+= lvelocityB;
				velocityA					+= lvelocityA;
				{
					const double					initialSpeedA				= contact.Result.InitialVelocityA.Length();
					const double					initialSpeedB				= contact.Result.InitialVelocityB.Length();
					const double					finalSpeedA					= velocityA.Length();
					const double					finalSpeedB					= velocityB.Length();
					const double					totalInitialSpeed			= initialSpeedA + initialSpeedB;
					const double					totalFinalSpeed				= finalSpeedA + finalSpeedB;
					if ((totalFinalSpeed - totalInitialSpeed) / totalInitialSpeed > 0.01f) {
						warning_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
					}
					if ((totalFinalSpeed - totalInitialSpeed) / totalInitialSpeed < -0.01f) {
						warning_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
					}
				}
			}

			contact.Result.FinalVelocityA	= (velocityA *= pool.StartState.DampingCollision);
			contact.Result.FinalVelocityB	= (velocityB *= pool.StartState.DampingCollision);
			contact.Result.FinalRotationA	= (rotationA *= pool.StartState.DampingCollision);
			contact.Result.FinalRotationB	= (rotationB *= pool.StartState.DampingCollision);
		}

		pool.LastFrameContactsBall.append(lastFrameContactsBatchBall);

		const gpk::SCoord2<float>		tableHalfDimensions	= pool.StartState.Table.Size * .5f;
		for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
			const gpk::SCoord2<float>		ballLimits			= tableHalfDimensions - ::gpk::SCoord2<float>{pool.StartState.Balls[iBall].BallRadius, pool.StartState.Balls[iBall].BallRadius};
			const ::gpk::SVirtualEntity		& entityA			= engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity]; 
			::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[entityA.RigidBody].Position;
			if((positionA.x	< -ballLimits.x) || (positionA.x > ballLimits.x)) {
				positionA.x					= (positionA.x	< -ballLimits.x) 
					? (-ballLimits.x) - (positionA.x + ballLimits.x)
					:   ballLimits.x  - (positionA.x - ballLimits.x)
					;
				::gpk::SBodyForces				& forces			= engine.Integrator.Forces[entityA.RigidBody];
				forces.Velocity.x			*= -1;
				forces.Velocity				*= pool.StartState.DampingCushion;
				forces.Rotation.z			*= -1;
			}
			if((positionA.z	< -ballLimits.y) || (positionA.z > ballLimits.y)) {
				positionA.z					= (positionA.z	< -ballLimits.y) 
					? (-ballLimits.y) - (positionA.z + ballLimits.y)
					:   ballLimits.y  - (positionA.z - ballLimits.y)
					;
				::gpk::SBodyForces				& forces			= engine.Integrator.Forces[entityA.RigidBody];
				forces.Velocity.z			*= -1;
				forces.Velocity				*= pool.StartState.DampingCushion;
				forces.Rotation.x			*= -1;
			}
			if(positionA.y	< 0) {
				positionA.y					*= -1;

				::gpk::SBodyForces				& forces			= engine.Integrator.Forces[entityA.RigidBody];
				forces.Velocity.y			*= -1;
				forces.Velocity				*= pool.StartState.DampingGround;
			}
		}
	}

	const float radius = 0.5f;
	const float diameter = radius * 2;
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		::gpk::SLine3<float> & delta = pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1];
		pool.GetBallPosition(iBall, delta.B);
		if((delta.B - delta.A).LengthSquared() > 0.000005) {
			::gpk::SBodyForces				& forces			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody];

			::gpk::SCoord3<float>			rotationResult		= (delta.B - delta.A) * ::gpk::math_2pi * diameter;
			forces.Rotation				+= {rotationResult.z, 0, -rotationResult.x};
		}
		if(pool.PositionDeltas[iBall].size() > 10)
			for(uint32_t iDelta = 0; iDelta < pool.PositionDeltas[iBall].size(); ++iDelta) {
				if((delta.B - delta.A).LengthSquared() < 0.00001f) {
					pool.PositionDeltas[iBall].remove_unordered(iDelta--);
				}
			}
	}

	return 0;
}