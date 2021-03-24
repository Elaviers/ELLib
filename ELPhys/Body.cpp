#include "Body.hpp"

void PhysicsBody::ApplyForce(const Vector3& position, const Vector3& direction, float newtons)
{
	float alpha = direction.Dot((_transform.GetPosition() - position).Normalised());


}

void PhysicsBody::Integrate(float deltaSeconds)
{
	_lastTransform = _transform;
	_lastVelocity = _velocity;

	_transform.Move(_velocity * deltaSeconds + _pendingAcceleration * (deltaSeconds * deltaSeconds * .5f));
	_velocity += _pendingAcceleration * deltaSeconds;
	_pendingAcceleration = Vector3();

	if (!_lockRotation)
	{
		_transform.AddRotation(_angularVelocity * deltaSeconds * _pendingAngularAcceleration * (deltaSeconds * deltaSeconds * .5f));
		_angularVelocity += _pendingAngularAcceleration * deltaSeconds;
		_pendingAngularAcceleration = Vector3();
	}
}

void PhysicsBody::ResolveCollision(const FixedBody& body)
{
	Vector3 penetration;
	EOverlapResult result = _collider.NarrowOverlapCheck(_transform, body.Collision(), body.GetTransform(), nullptr, &penetration);

	bool overlapping = result == EOverlapResult::OVERLAPPING;
	if (result == EOverlapResult::TOUCHING || overlapping)
	{
		//Cancel forces towards fixed body

 		if (overlapping)
		{
			if (isnan(penetration.x) || isnan(penetration.y) || isnan(penetration.z))
			{
				Debug::PrintLine("NaN penetration!");
				return;
			}

			//Move out of body
			SetPosition(GetPosition() - penetration);

			Vector3 pDir = penetration.Normalised();
			
			//Alter velocity (acceleration ignored as ResolveCollision should be called after integration of acceleration)
			SetVelocity(GetVelocity() + 
				(GetVelocity().Dot(pDir) * (-Maths::Max(GetRestitution(), body.GetRestitution())) - 1.f) * pDir);

		}
	}
}

void PhysicsBody::ResolveCollision(PhysicsBody& a, PhysicsBody& b)
{
	//TODO PHYSICS BETWEEN TWO PHYSICS BODIES
	//Kind of important..
}
