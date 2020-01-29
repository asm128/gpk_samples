#include "gpk_matrix.h"
#include "gpk_array.h"

#ifndef CED_RIGIDBODY_H_23487239847234
#define CED_RIGIDBODY_H_23487239847234

namespace ced
{
#pragma pack(push, 1)
	// The following structure stores forces information for a 3D rigid body. These change almost every frame during runtime
	struct SForce3 {
		::gpk::SCoord3<float>							Velocity						= {};
		::gpk::SCoord3<float>							Acceleration					= {};
		::gpk::SCoord3<float>							Rotation						= {};
	};

	// The following structure stores mass information for a 3D rigid body. These almost never change during runtime
	struct SMass3 {
		float											LinearDamping					= 1.0f;
		float											AngularDamping					= 1.0f;
		float											InverseMass						= 0;
		::gpk::SMatrix3<float>							InverseAngularMassTensor		= {1,0,0,0,1,0,0,0,1};
	};

	struct STransform3 {
		::gpk::SQuaternion	<float>						Orientation						= {0, 0, 0, 1};
		::gpk::SCoord3		<float>						Position						= {};
	};

	struct SRigidBodyFlags {
		bool											OutdatedTransform				: 1;
		bool											OutdatedTensorWorld				: 1;	// Tell the object that our matrices are up to date
		bool											Active							: 1;

		constexpr										SRigidBodyFlags					()
			: OutdatedTransform		(true)
			, OutdatedTensorWorld	(true)
			, Active				(true)
		{}
	};

	struct SRigidBodyFrame {
		::gpk::SMatrix3<float>							InverseInertiaTensorWorld		= {1, 0, 0, 0, 1, 0, 0, 0, 1};
		::gpk::SCoord3<float>							LastFrameAcceleration			= {0, 0, 0};
		::gpk::SCoord3<float>							AccumulatedForce				= {0, 0, 0};
		::gpk::SCoord3<float>							AccumulatedTorque				= {0, 0, 0};

		int32_t											ClearAccumulators				()	{ AccumulatedForce = AccumulatedTorque = {}; return 0; }
	};
#pragma pack(pop)
	void											updateTransform					(::ced::STransform3 & bodyTransform, ::gpk::SMatrix4<float> & transformLocal);
	int32_t											integrateForces					(double duration, ::ced::SRigidBodyFrame& bodyFrame, ::ced::SForce3 & bodyForce, const ::ced::SMass3 & bodyMass);
	int32_t											integratePosition				(double duration, double durationHalfSquared, ::ced::SRigidBodyFlags& bodyFlags, ::ced::STransform3 & bodyTransform, const ::ced::SForce3 & bodyForces);
	void											transformInertiaTensor			(::gpk::SMatrix3<float> & iitWorld, const ::gpk::SMatrix3<float> &iitBody, const ::gpk::SMatrix4<float> &rotmat);

	struct SIntegrator3 {
		::gpk::array_pod<::ced::SRigidBodyFrame	>		BodyFrames						= {};
		::gpk::array_pod<::ced::SRigidBodyFlags	>		BodyFlags						= {};
		::gpk::array_pod<::ced::SForce3			>		Forces							= {};
		::gpk::array_pod<::ced::SMass3			>		Masses							= {};
		::gpk::array_pod<::ced::STransform3		>		Transforms						= {};
		::gpk::array_pod<::gpk::SMatrix4<float>	>		TransformsLocal					= {};

		static constexpr const ::gpk::SMatrix4<float>	MatrixIdentity4					= {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		static constexpr const ::gpk::SMatrix3<float>	MatrixIdentity3					= {1,0,0,0,1,0,0,0,1};

		int32_t 										Clear							()	{
			BodyFrames				.clear();
			BodyFlags				.clear();
			Forces					.clear();
			Masses					.clear();
			Transforms				.clear();
			return TransformsLocal	.clear();
		}
		int32_t 										Spawn							()	{
			BodyFrames				.push_back({});
			BodyFlags				.push_back({});
			Forces					.push_back({});
			Masses					.push_back({});
			Transforms				.push_back({});
			return TransformsLocal	.push_back(MatrixIdentity4);
		}

		int32_t 										Spawn							(uint32_t bodyCount)	{
			//uint32_t											offset							= BodyFrames.size();
			BodyFrames		.resize(BodyFrames		.size() + bodyCount, {});
			BodyFlags		.resize(BodyFlags		.size() + bodyCount, {});
			Forces			.resize(Forces			.size() + bodyCount, {});
			Masses			.resize(Masses			.size() + bodyCount, {});
			Transforms		.resize(Transforms		.size() + bodyCount, {});
			TransformsLocal	.resize(TransformsLocal	.size() + bodyCount, {});
			return TransformsLocal.size();
		}

		int32_t 										GetTransform					(uint32_t iBody, ::gpk::SMatrix4<float>	& transform)	{
			::ced::SRigidBodyFlags								& bodyFlags						= BodyFlags			[iBody];
			::gpk::SMatrix4<float>								& bodyTransformLocal			= TransformsLocal	[iBody];
			const bool											requiresUpdate					= bodyFlags.OutdatedTransform || bodyFlags.OutdatedTensorWorld;
			if(requiresUpdate) {
				if(bodyFlags.OutdatedTransform) {
					::ced::STransform3									& bodyTransform					= Transforms[iBody];
					::ced::updateTransform(bodyTransform, bodyTransformLocal);
					bodyFlags.OutdatedTransform						= false;
				}
				if(bodyFlags.OutdatedTensorWorld) {
					::ced::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
					::ced::SMass3										& bodyMass						= Masses	[iBody];
					::ced::transformInertiaTensor(bodyFrame.InverseInertiaTensorWorld, bodyMass.InverseAngularMassTensor, bodyTransformLocal);
					bodyFlags.OutdatedTensorWorld					= false;
				}
			}
			transform										= bodyTransformLocal;
			return requiresUpdate ? 1 : 0;
		}
		void											AddForceAtPoint					(uint32_t iBody, const ::gpk::SCoord3<float>& force, ::gpk::SCoord3<float> point)			{
			::ced::STransform3									& bodyTransform					= Transforms[iBody];
			::ced::SRigidBodyFlags								& bodyFlags						= BodyFlags	[iBody];
			::ced::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
			point											-= bodyTransform.Position;	// Convert to coordinates relative to center of mass.
			bodyFrame.AccumulatedForce 						+= force;
			bodyFrame.AccumulatedTorque						+= point.Cross( force );
			bodyFlags.Active								= true;
		}
		int32_t											Integrate						(double duration)	{
			const double										durationHalfSquared				= duration * duration * 0.5;
			for(uint32_t iBody = 0; iBody < BodyFlags.size(); ++iBody) {
				::ced::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
				if(false == bodyFlags.Active)
					continue;
				::ced::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
				::ced::SForce3										& bodyForces					= Forces	[iBody];
				::ced::SMass3										& bodyMass						= Masses	[iBody];
				::ced::STransform3									& bodyTransform					= Transforms[iBody];
				::ced::integrateForces	(duration, bodyFrame, bodyForces, bodyMass);
				::ced::integratePosition(duration, durationHalfSquared, bodyFlags, bodyTransform, bodyForces);
			}
			return 0;
		}
		void											SetPosition						(uint32_t iBody, const ::gpk::SCoord3<float>& newPosition )			{
			::ced::STransform3									& bodyTransform					= Transforms[iBody];
			if( 0 == memcmp( &newPosition.x, &bodyTransform.Position.x, sizeof( ::gpk::SCoord3<float> ) ) )
				return;
			bodyTransform.Position							= newPosition;
			::ced::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.OutdatedTransform						=
			bodyFlags.OutdatedTensorWorld					= true;
		}
		void											SetOrientation					(uint32_t iBody, const ::gpk::SQuaternion<float>& newOrientation )	{
			::ced::STransform3									& bodyTransform					= Transforms[iBody];
			if( 0 == memcmp( &newOrientation.x, &bodyTransform.Orientation.x, sizeof(::gpk::SQuaternion<float>) ) )
				return;
			bodyTransform.Orientation						= newOrientation;
			::ced::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.OutdatedTransform						=
			bodyFlags.OutdatedTensorWorld					= true;
		}
	};
} // namespace

#endif // CED_RIGIDBODY_H_23487239847234
