#include "ced_rigidbody.h"

void												ced::transformInertiaTensor			(::gpk::SMatrix3<float> & iitWorld, const ::gpk::SMatrix3<float> &iitBody, const ::gpk::SMatrix4<float> &rotmat)		{
	float													t4									= rotmat._11*iitBody._11 + rotmat._21*iitBody._12 + rotmat._31*iitBody._13;
	float													t9									= rotmat._11*iitBody._21 + rotmat._21*iitBody._22 + rotmat._31*iitBody._23;
	float													t14									= rotmat._11*iitBody._31 + rotmat._21*iitBody._32 + rotmat._31*iitBody._33;
	float													t28									= rotmat._12*iitBody._11 + rotmat._22*iitBody._12 + rotmat._32*iitBody._13;
	float													t33									= rotmat._12*iitBody._21 + rotmat._22*iitBody._22 + rotmat._32*iitBody._23;
	float													t38									= rotmat._12*iitBody._31 + rotmat._22*iitBody._32 + rotmat._32*iitBody._33;
	float													t52									= rotmat._13*iitBody._11 + rotmat._23*iitBody._12 + rotmat._33*iitBody._13;
	float													t57									= rotmat._13*iitBody._21 + rotmat._23*iitBody._22 + rotmat._33*iitBody._23;
	float													t62									= rotmat._13*iitBody._31 + rotmat._23*iitBody._32 + rotmat._33*iitBody._33;

	iitWorld._11										= t4 *rotmat._11 + t9 *rotmat._21 + t14*rotmat._31;
	iitWorld._21										= t4 *rotmat._12 + t9 *rotmat._22 + t14*rotmat._32;
	iitWorld._31										= t4 *rotmat._13 + t9 *rotmat._23 + t14*rotmat._33;
	iitWorld._12										= t28*rotmat._11 + t33*rotmat._21 + t38*rotmat._31;
	iitWorld._22										= t28*rotmat._12 + t33*rotmat._22 + t38*rotmat._32;
	iitWorld._32										= t28*rotmat._13 + t33*rotmat._23 + t38*rotmat._33;
	iitWorld._13										= t52*rotmat._11 + t57*rotmat._21 + t62*rotmat._31;
	iitWorld._23										= t52*rotmat._12 + t57*rotmat._22 + t62*rotmat._32;
	iitWorld._33										= t52*rotmat._13 + t57*rotmat._23 + t62*rotmat._33;
}

void												ced::updateTransform					(::ced::STransform3 & bodyTransform, ::gpk::SMatrix4<float> & transformLocal)	{
	//transformLocal.Scale( bodyTransform.Scale, true );
	bodyTransform.Orientation.Normalize();
	transformLocal.SetOrientation( bodyTransform.Orientation );
	transformLocal.SetTranslation( bodyTransform.Position, false );
}

int32_t												ced::integrateForces					(double duration, ::ced::SRigidBodyFrame& bodyFrame, ::ced::SForce3 & bodyForce, const ::ced::SMass3 & bodyMass) {
	// -- Calculate linear acceleration from force inputs.
	bodyFrame.LastFrameAcceleration						= bodyForce.Acceleration;
	bodyFrame.LastFrameAcceleration.AddScaled(bodyFrame.AccumulatedForce, bodyMass.InverseMass);
	// -- Adjust velocities
	bodyForce.Velocity.AddScaled(bodyFrame.LastFrameAcceleration, (float)duration);	// Update linear velocity from both acceleration and impulse.

	// -- Calculate angular acceleration from torque inputs.
	::gpk::SCoord3<float>									angularAcceleration						= bodyFrame.InverseInertiaTensorWorld.Transform(bodyFrame.AccumulatedTorque);
	bodyForce.Rotation.AddScaled(angularAcceleration, duration);	// Update angular velocity from both acceleration and impulse.

	// Impose drag.
	bodyForce.Velocity									*= pow((double)bodyMass.LinearDamping , duration);
	bodyForce.Rotation									*= pow((double)bodyMass.AngularDamping, duration);
	bodyFrame.ClearAccumulators();
	return 0;
}

// ------------- Adjust positions
int32_t												ced::integratePosition				(double duration, double durationHalfSquared, ::ced::SRigidBodyFlags& bodyFlags, ::ced::STransform3 & bodyTransform, const ::ced::SForce3 & bodyForces) {
	bodyTransform.Position		.AddScaled(bodyForces.Velocity, duration);
	bodyTransform.Position		.AddScaled(bodyForces.Velocity, durationHalfSquared);
	bodyTransform.Orientation	.AddScaled(bodyForces.Rotation, duration);	// Update angular position.
	bodyFlags.OutdatedTransform = bodyFlags.OutdatedTensorWorld	= true;	// Normalize the orientation, and update the matrices with the new position and orientation.
	return 0;
}
