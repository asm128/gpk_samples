#include "gpk_the_one.h"

bool										the1::revert				(const ::gpk::SCoord3<float> & distanceDirection, const ::gpk::SCoord3<float> & initialVelocity)	{
	(void)distanceDirection; (void)initialVelocity;
	return initialVelocity.Dot(distanceDirection) < 0;
	////return false;
	//return (distanceDirection.z > 0);// || initialVelocity.x < 0;
	//return (distanceDirection.z > 0) || (distanceDirection.x < 0);
}

static	::gpk::error_t		resolveCollision							
	( const ::gpk::SCoord3<float>	& initialVelocityA
	, const ::gpk::SCoord3<float>	& distanceDirection
	, float							& out_forceTransferRatioB	
	, ::gpk::SCoord3<float>			& out_finalVelocityA
	, ::gpk::SCoord3<float>			& out_finalVelocityB
	) {
	::gpk::SCoord3<float>			directionA			= initialVelocityA;	
	directionA.Normalize();
	out_forceTransferRatioB		= ::gpk::max(0.0f, (float)distanceDirection.Dot(directionA));

	bool							revert				= ::the1::revert(distanceDirection, initialVelocityA);

	::gpk::SCoord3<float>			finalVelocityB		= distanceDirection * initialVelocityA.Length() * out_forceTransferRatioB;
	::gpk::SCoord3<float>			finalVelocityA		= distanceDirection.Cross({0, revert ? -1 : 1.0f, 0}).Normalize() * initialVelocityA.Length() * (1.0f - out_forceTransferRatioB);

	out_finalVelocityA			+= finalVelocityA;
	out_finalVelocityB			+= finalVelocityB;
	return 0;
}

::gpk::error_t				the1::poolGameUpdate			(::the1::SPoolGame & pool, double secondsElapsed) {
	::gpk::SEngine					& engine						= pool.Engine;
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		pool.PositionDeltas[iBall].push_back({engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position, });
	}

	engine.Update(secondsElapsed);

	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1].B = engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
	}

	pool.Contacts.clear();
	for(uint32_t iBall = 0; iBall < pool.StartState.BallCount; ++iBall) {
		::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
		::the1::SBallContact			contact				= {};
		contact.BallA				= iBall;
		for(uint32_t iBall2 = iBall + 1; iBall2 < pool.StartState.BallCount; ++iBall2) {
			::gpk::SCoord3<float>			& positionB			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Position;
			contact.BallB				= iBall2;
			contact.Distance			= positionB - positionA;
			if(contact.Distance.LengthSquared() < 1) 
				pool.Contacts.push_back(contact);
		}
	}

	for(uint32_t iContact = 0; iContact < pool.Contacts.size(); ++iContact) {
		::the1::SBallContact			& contact			= pool.Contacts[iContact];
		uint32_t						iBall				= contact.BallA;
		uint32_t						iBall2				= contact.BallB;
		::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Velocity;
		::gpk::SCoord3<float>			& velocityB			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Velocity;
		contact.Result.DistanceDirection	= contact.Distance;
		double							distanceLength		= contact.Distance.Length();
		contact.Result.DistanceDirection.Normalize();
		::gpk::SCoord3<float>			& positionB			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall2].Entity].RigidBody].Position;
		::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[engine.ManagedEntities.Entities[pool.StartState.Balls[iBall].Entity].RigidBody].Position;
		positionA					+= contact.Result.DistanceDirection * ::gpk::max(1.0f - distanceLength, 0.0) * .505f * -1;
		positionB					+= contact.Result.DistanceDirection * ::gpk::max(1.0f - distanceLength, 0.0) * .505f;
		contact.Result.InitialVelocityA	= velocityA;
		contact.Result.InitialVelocityB	= velocityB;
		velocityB					= {};
		velocityA					= {};
		if(contact.Result.InitialVelocityA.LengthSquared()) 
			::resolveCollision(contact.Result.InitialVelocityA, contact.Result.DistanceDirection, contact.Result.ForceTransferRatioB, velocityA, velocityB);

		if(contact.Result.InitialVelocityB.LengthSquared()) {
			contact.Result.DistanceDirection	*= -1;
			::resolveCollision(contact.Result.InitialVelocityB, contact.Result.DistanceDirection, contact.Result.ForceTransferRatioA, velocityB, velocityA);
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
	return 0;
}