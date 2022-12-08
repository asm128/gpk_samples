#include "gpk_pool_game.h"

static	bool				revertCross				(const ::gpk::SCoord3<double> & distanceDirection)	{
	if(distanceDirection.x >= 0)
		return (distanceDirection.z >= 0);
	else 
		return (distanceDirection.z < 0);
}

static	::gpk::error_t		resolveCollision							
	( const ::gpk::SCoord3<double>	& initialVelocityA
	, const ::gpk::SCoord3<double>	& initialRotationA
	, const ::gpk::SCoord3<double>	& distanceDirection
	, float							& out_forceTransferRatioB	
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
	if(0.9999 <= out_forceTransferRatioB) {
		out_finalVelocityB			+= initialVelocityA.Cast<float>();
		out_finalRotationB			+= initialRotationA.Cast<float>();
		return 0;
	}

	bool							revert				= ::revertCross(distanceDirection);
	if(fabs(initialVelocityA.x) < fabs(initialVelocityA.z))
		revert = !revert;

	info_printf("Total initial speed: %f", (float)(initialVelocityA.Length()));

	double							speedA				= initialVelocityA.Length();
	const ::gpk::SCoord3<double>	vUp					= {0, revert ? -1 : 1.0f, 0};
	::gpk::SCoord3<double>			finalVelocityB		= distanceDirection * speedA * out_forceTransferRatioB;
	::gpk::SCoord3<double>			finalVelocityA		= distanceDirection.Cross(vUp).Normalize() * speedA * (1.0f - out_forceTransferRatioB);
	double finalSpeedA = finalVelocityB.Length();
	double finalSpeedB = finalVelocityA.Length();
	info_printf("Total final speed: %f", (float)(finalSpeedA + finalSpeedB));
	out_finalVelocityA			+= finalVelocityA.Cast<float>();
	out_finalVelocityB			+= finalVelocityB.Cast<float>();

	info_printf("Total initial rotation: %f", (float)(initialRotationA.Length()));

	double							rotA				= initialRotationA.Length();
	::gpk::SCoord3<double>			finalRotationB		= initialRotationA.Cast<double>().Normalize() * rotA * out_forceTransferRatioB;
	::gpk::SCoord3<double>			finalRotationA		= initialRotationA.Cast<double>().Normalize() * rotA * (1.0f - out_forceTransferRatioB);
	double finalRotA = finalRotationB.Length();
	double finalRotB = finalRotationA.Length();
	info_printf("Total final rotation: %f", (float)(finalRotA + finalRotB));
	out_finalRotationA			+= finalRotationA.Cast<float>();
	out_finalRotationB			+= finalRotationB.Cast<float>();
	return 0;
}

::gpk::error_t				the1::poolGameUpdate			(::the1::SPoolGame & pool, double secondsElapsed) {
	::gpk::SEngine					& engine						= pool.Engine;
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		pool.PositionDeltas[iBall].push_back({});
		pool.GetBallPosition(iBall, pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1].A);
	}

	double							step							= .01f;
	pool.LastFrameContactsBall		.clear();
	pool.LastFrameContactsCushion	.clear();
	::gpk::array_pod<::the1::SContactBall>	lastFrameContactsBatchBall;
	::gpk::array_pod<::the1::SContactBall>	lastFrameContactsBatchCushion;
	while(secondsElapsed > 0) { 
		double							secondsThisStep					= ::gpk::min(step, secondsElapsed);
		secondsElapsed				-= secondsThisStep;

		engine.Update(secondsThisStep);

		lastFrameContactsBatchBall		.clear();
		lastFrameContactsBatchCushion	.clear();
		const float						collisionThreshold				= (pool.StartState.BallRadius * 2) * (pool.StartState.BallRadius * 2);
		for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
			const ::gpk::SCoord3<float>		& positionA			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
			::the1::SContactBall			contactBall			= {};
			::the1::SContactCushion			contactCushion		= {};
			contactBall.BallA			= iBall;
			contactCushion.BallA		= iBall;
			for(uint32_t iBall2 = iBall + 1; iBall2 < pool.StartState.BallCount; ++iBall2) {
				const ::gpk::SCoord3<float>		& positionB			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Position;
				contactBall.BallB				= iBall2;
				contactBall.Distance			= positionB - positionA;
				if(contactBall.Distance.LengthSquared() >= collisionThreshold) 
					continue;

				lastFrameContactsBatchBall.push_back(contactBall);
			}
		}

		for(uint32_t iContact = 0; iContact < lastFrameContactsBatchBall.size(); ++iContact) {
			::the1::SContactBall			& contact			= lastFrameContactsBatchBall[iContact];
			uint32_t						iBall				= contact.BallA;
			uint32_t						iBall2				= contact.BallB;

			contact.Result.DistanceDirection	= contact.Distance;
			double							distanceLength		= contact.Distance.Length();
			contact.Result.DistanceDirection.Normalize();

			// Separate balls
			::gpk::SCoord3<float>			& positionB			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Position;
			::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
			positionA					+= contact.Result.DistanceDirection * ::gpk::max(1.0f - distanceLength, 0.0) * -.5001f;
			positionB					+= contact.Result.DistanceDirection * ::gpk::max(1.0f - distanceLength, 0.0) * .5001f;


			// Calculate force transfer
			::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
			::gpk::SCoord3<float>			& velocityB			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Velocity;

			::gpk::SCoord3<float>			& rotationA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Rotation;
			::gpk::SCoord3<float>			& rotationB			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Rotation;

			contact.Result.InitialVelocityA	= velocityA;
			contact.Result.InitialVelocityB	= velocityB;
			contact.Result.InitialRotationA	= rotationA;
			contact.Result.InitialRotationB	= rotationB;
			velocityB					= {};
			velocityA					= {};
			rotationB					= {};
			rotationA					= {};
			if(contact.Result.InitialVelocityA.LengthSquared()) {
				engine.Integrator.BodyFlags[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Active = true;
				::gpk::SCoord3<float>			lvelocityB					= {};
				::gpk::SCoord3<float>			lvelocityA					= {};
				::resolveCollision(contact.Result.InitialVelocityA.Cast<double>(), contact.Result.InitialRotationA.Cast<double>(), contact.Result.DistanceDirection.Cast<double>(), contact.Result.ForceTransferRatioB, rotationA, rotationB, lvelocityA, lvelocityB);
				double totalFinalSpeed	= lvelocityA.Length() + lvelocityB.Length();
				double totalInitialSpeed = contact.Result.InitialVelocityA.Length();
				if ((totalFinalSpeed - totalInitialSpeed) > 0.0001f) {
					warning_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
				}
				if ((totalFinalSpeed - totalInitialSpeed) < -0.0001f) {
					warning_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
				}			
				velocityB					+= lvelocityB;
				velocityA					+= lvelocityA;
			}

			if(contact.Result.InitialVelocityB.LengthSquared()) {
				engine.Integrator.BodyFlags[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Active = true;
				::gpk::SCoord3<float>			lvelocityB					= {};
				::gpk::SCoord3<float>			lvelocityA					= {};
				::resolveCollision(contact.Result.InitialVelocityB.Cast<double>(), contact.Result.InitialRotationB.Cast<double>(), contact.Result.DistanceDirection.Cast<double>() * -1, contact.Result.ForceTransferRatioA, rotationA, rotationB, lvelocityB, lvelocityA);
				{
					double							totalFinalSpeed				= lvelocityA.Length() + lvelocityB.Length();
					double							totalInitialSpeed			= contact.Result.InitialVelocityB.Length();
					if ((totalFinalSpeed - totalInitialSpeed) > 0.0001f) {
						warning_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
					}
					if ((totalFinalSpeed - totalInitialSpeed) < -0.0001f) {
						warning_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
					}			
				}
				velocityB					+= lvelocityB;
				velocityA					+= lvelocityA;
				{
					double							totalFinalSpeed				= velocityA.Length() + velocityB.Length();
					double							totalInitialSpeed			= contact.Result.InitialVelocityA.Length() + contact.Result.InitialVelocityB.Length();
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
			pool.LastFrameContactsBall.append(lastFrameContactsBatchBall);
		}

		const gpk::SCoord3<float> halfDimensions = pool.StartState.TableDimensions * .5 - ::gpk::SCoord3<float>{pool.StartState.BallRadius, 0, pool.StartState.BallRadius};
		for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
			::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
			if(positionA.x	< -halfDimensions.x) {
				positionA.x = (-halfDimensions.x) - (positionA.x + halfDimensions.x);
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.x *= -1;
				velocityA *= pool.StartState.DampingCushion;
			}
			if(positionA.x	> halfDimensions.x) {
				positionA.x = halfDimensions.x - (positionA.x - halfDimensions.x);
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.x *= -1;
				velocityA *= pool.StartState.DampingCushion;
			}
			if(positionA.z	< -halfDimensions.z) {
				positionA.z = (-halfDimensions.z) - (positionA.z + halfDimensions.z);
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.z *= -1;
				velocityA *= pool.StartState.DampingCushion;
			}
			if(positionA.z	> halfDimensions.z) {
				positionA.z = halfDimensions.z - (positionA.z-halfDimensions.z);
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.z *= -1;
				velocityA *= pool.StartState.DampingCushion;
			}
			if(positionA.y	< 0) {
				positionA.y *= -1;
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.y *= -1;
				velocityA *= pool.StartState.DampingGround;
			}
		}
	}

	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		::gpk::SLine3<float> & delta = pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1];
		pool.GetBallPosition(iBall, delta.B);
		if((delta.B - delta.A).LengthSquared() > 0.00001) {
			::gpk::SCoord3<float>			& rotationA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Rotation;
			::gpk::SCoord3<float>			rotationResult		= (delta.B - delta.A) * ::gpk::math_2pi;
			rotationA	+= {rotationResult.z, 0, -rotationResult.x};
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