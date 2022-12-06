#include "gpk_the_one.h"

static	::gpk::error_t						resolveCollision							
	( const ::gpk::SCoord3<float>	& initialVelocity
	, const ::gpk::SCoord3<float>	& distanceDirection
	, ::gpk::SCoord3<float>			& out_finalVelocityA
	, ::gpk::SCoord3<float>			& out_finalVelocityB
	) {
	::gpk::SCoord3<float>			directionA			= initialVelocity;	
	directionA.Normalize();
	float							factorB				= ::gpk::max(0.0f, (float)distanceDirection.Dot(directionA));

	bool							revert				= (distanceDirection.z > 0) || (distanceDirection.x < 0);
	::gpk::SCoord3<float>			finalVelocityA		= distanceDirection.Cross({0, revert ? -1 : 1.0f, 0}).Normalize() * initialVelocity.Length() * (1.0f - factorB);
	::gpk::SCoord3<float>			finalVelocityB		= distanceDirection * initialVelocity.Length() * factorB;

	out_finalVelocityA			+= finalVelocityA;
	out_finalVelocityB			+= finalVelocityB;
	return 0;
}

::gpk::error_t								poolGameUpdate			(::SPoolGame & pool, double secondsElapsed) {
	::gpk::SEngine										& engine					= pool.Engine;
	engine.Update(secondsElapsed);

	for(uint32_t iBall = 0; iBall < 16; ++iBall) {
		::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
		::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
		for(uint32_t iBall2 = iBall + 1; iBall2 < 16; ++iBall2) {
			::gpk::SCoord3<float>			& positionB			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Position;
			::gpk::SCoord3<float>			distance			= positionB - positionA;
			::gpk::SCoord3<float>			distanceDirection	= distance;
			double							distanceLength		= distance.Length();
			distanceDirection.Normalize();

			if(distance.LengthSquared() < 1) {
				::gpk::SCoord3<float>			& velocityB			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Velocity;
				const ::gpk::SCoord3<float>		prevA				= velocityA;
				const ::gpk::SCoord3<float>		prevB				= velocityB;
				velocityB					= {};
				velocityA					= {};
				if(prevA.LengthSquared()) 
					::resolveCollision(prevA, distanceDirection, velocityA, velocityB);

				positionB += distanceDirection * ::gpk::max(1.0f - distanceLength, 0.0) * .51;
				if(prevB.LengthSquared()) {
					distanceDirection			*= -1;
					::resolveCollision(prevB, distanceDirection, velocityA, velocityB);
				}
				positionA += distanceDirection * ::gpk::max(1.0f - distanceLength, 0.0) * .51;
				//velocityA					*= .9f;
				//velocityB					*= .9f;
				}
		}
		if(positionA.x	< -20) {
			positionA.x = -20;
			velocityA.x *= -1;
			//velocityA *= .9f;
		}
		if(positionA.x	> 20) {
			positionA.x = 20;
			velocityA.x *= -1;
			//velocityA *= .9f;
		}
		if(positionA.z	< -10) {
			positionA.z = -10;
			velocityA.z *= -1;
			//velocityA *= .9f;
		}
		if(positionA.z	> 10) {
			positionA.z = 10;
			velocityA.z *= -1;
			//velocityA *= .9f;
		}
		if(positionA.y	< 0) {
			positionA.y = 0;
			velocityA.y *= -1;
			//velocityA *= .9f;
		}
	}
	return 0;
}