#include "gpk_the_one.h"

static	bool				revertCross				(const ::gpk::SCoord3<double> & distanceDirection)	{
	if(distanceDirection.x > 0)
		return (distanceDirection.z > 0);
	else 
		return (distanceDirection.z < 0);
}

static	::gpk::error_t		resolveCollision							
	( const ::gpk::SCoord3<double>	& initialVelocityA
	, const ::gpk::SCoord3<double>	& distanceDirection
	, float							& out_forceTransferRatioB	
	, ::gpk::SCoord3<float>			& out_finalVelocityA
	, ::gpk::SCoord3<float>			& out_finalVelocityB
	) {
	::gpk::SCoord3<double>			directionA			= initialVelocityA.Cast<double>();
	directionA.Normalize();
	out_forceTransferRatioB		= ::gpk::max(0.0f, (float)distanceDirection.Dot(directionA));
	if(0 >= out_forceTransferRatioB) {
		out_finalVelocityA			+= initialVelocityA.Cast<float>();
		return 0;
	}
	if(0.9999 <= out_forceTransferRatioB) {
		out_finalVelocityB			+= initialVelocityA.Cast<float>();
		return 0;
	}

	always_printf("Total initial speed: %f", (float)(initialVelocityA.Length()));

	bool							revert				= ::revertCross(distanceDirection);

	double							speedA				= initialVelocityA.Length();
	const ::gpk::SCoord3<double>	vUp					= {0, revert ? -1 : 1.0f, 0};
	::gpk::SCoord3<double>			finalVelocityB		= distanceDirection * speedA * out_forceTransferRatioB;
	::gpk::SCoord3<double>			finalVelocityA		= distanceDirection.Cross(vUp).Normalize() * speedA * (1.0f - out_forceTransferRatioB);
	double finalSpeedA = finalVelocityB.Length();
	double finalSpeedB = finalVelocityA.Length();
	always_printf("Total final speed: %f", (float)(finalSpeedA + finalSpeedB));
	out_finalVelocityA			+= finalVelocityA.Cast<float>();
	out_finalVelocityB			+= finalVelocityB.Cast<float>();
	return 0;
}

::gpk::error_t				the1::poolGameUpdate			(::the1::SPoolGame & pool, double secondsElapsed) {
	::gpk::SEngine					& engine						= pool.Engine;
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		pool.PositionDeltas[iBall].push_back({});
		pool.GetBallPosition(iBall, pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1].A);
	}

	double							step							= .01f;
	while(secondsElapsed > 0) { 
		double							secondsThisStep					= ::gpk::min(step, secondsElapsed);
		secondsElapsed				-= secondsThisStep;

		engine.Update(secondsThisStep);

		pool.Contacts.clear();
		for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
			const ::gpk::SCoord3<float>		& positionA			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
			::the1::SBallContact			contact				= {};
			contact.BallA				= iBall;
			for(uint32_t iBall2 = iBall + 1; iBall2 < pool.StartState.BallCount; ++iBall2) {
				const ::gpk::SCoord3<float>		& positionB			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Position;
				contact.BallB				= iBall2;
				contact.Distance			= positionB - positionA;
				if(contact.Distance.LengthSquared() >= 1) 
					continue;

				pool.Contacts.push_back(contact);
			}
		}

		for(uint32_t iContact = 0; iContact < pool.Contacts.size(); ++iContact) {
			::the1::SBallContact			& contact			= pool.Contacts[iContact];
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
			contact.Result.InitialVelocityA	= velocityA;
			contact.Result.InitialVelocityB	= velocityB;
			velocityB					= {};
			velocityA					= {};
			if(contact.Result.InitialVelocityA.LengthSquared()) {
				::gpk::SCoord3<float>			lvelocityB					= {};
				::gpk::SCoord3<float>			lvelocityA					= {};
				::resolveCollision(contact.Result.InitialVelocityA.Cast<double>(), contact.Result.DistanceDirection.Cast<double>(), contact.Result.ForceTransferRatioB, lvelocityA, lvelocityB);
				double totalFinalSpeed	= lvelocityA.Length() + lvelocityB.Length();
				double totalInitialSpeed = contact.Result.InitialVelocityA.Length();
				if ((totalFinalSpeed - totalInitialSpeed) > 0.0001f) {
					always_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
				}
				if ((totalFinalSpeed - totalInitialSpeed) < -0.0001f) {
					always_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
				}			
				velocityB					+= lvelocityB;
				velocityA					+= lvelocityA;
			}

			if(contact.Result.InitialVelocityB.LengthSquared()) {
				::gpk::SCoord3<float>			lvelocityB					= {};
				::gpk::SCoord3<float>			lvelocityA					= {};
				::resolveCollision(contact.Result.InitialVelocityB.Cast<double>(), contact.Result.DistanceDirection.Cast<double>() * -1, contact.Result.ForceTransferRatioA, lvelocityB, lvelocityA);
				{
					double							totalFinalSpeed				= lvelocityA.Length() + lvelocityB.Length();
					double							totalInitialSpeed			= contact.Result.InitialVelocityB.Length();
					if ((totalFinalSpeed - totalInitialSpeed) > 0.0001f) {
						always_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
					}
					if ((totalFinalSpeed - totalInitialSpeed) < -0.0001f) {
						always_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
					}			
				}
				velocityB					+= lvelocityB;
				velocityA					+= lvelocityA;
				{
					double							totalFinalSpeed				= velocityA.Length() + velocityB.Length();
					double							totalInitialSpeed			= contact.Result.InitialVelocityA.Length() + contact.Result.InitialVelocityB.Length();
					if ((totalFinalSpeed - totalInitialSpeed) / totalInitialSpeed > 0.01f) {
						always_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
					}
					if ((totalFinalSpeed - totalInitialSpeed) / totalInitialSpeed < -0.01f) {
						always_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
					}
				}
			}

			contact.Result.FinalVelocityA	= velocityA;
			contact.Result.FinalVelocityB	= velocityB;
	}

		for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
			::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
			if(positionA.x	< -20) {
				positionA.x = -20;
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.x *= -1;
				//velocityA *= .9f;
			}
			if(positionA.x	> 20) {
				positionA.x = 20;
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.x *= -1;
				//velocityA *= .9f;
			}
			if(positionA.z	< -10) {
				positionA.z = -10;
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.z *= -1;
				//velocityA *= .9f;
			}
			if(positionA.z	> 10) {
				positionA.z = 10;
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.z *= -1;
				//velocityA *= .9f;
			}
			if(positionA.y	< 0) {
				positionA.y = 0;
				::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
				velocityA.y *= -1;
				//velocityA *= .9f;
			}
		}
	}

	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		pool.GetBallPosition(iBall, pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1].B);
	}

	return 0;
}