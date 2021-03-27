#include "Body.hpp"

void PhysicsBody::ApplyAccelerationAtLocation(const Vector3& position, const Vector3& direction, float acceleration)
{
	Vector3 delta = _transform.GetPosition() - position;

	float alpha = 1.f; 
	if (delta.LengthSquared() > 0.1f) 
		alpha = direction.Dot(delta) / delta.Length();

	if (alpha >= 1.f)
		_pendingAcceleration += direction * acceleration;
	else if (alpha <= 0.f)
		_pendingAngularAcceleration;
	else
	{
		_pendingAcceleration += (1.f - alpha) * direction * acceleration;
	}
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

void PhysicsBody::UpdateInterpolatedData(float alpha)
{
	_iVelocity = Maths::Lerp(_lastVelocity, _velocity, alpha);
	_iTransform.SetPosition(Maths::Lerp(GetLastPosition(), GetPosition(), alpha));
}

void PhysicsBody::ResolveCollision(const FixedBody& body, float minBounceVel)
{
	Vector3 penetration;
	EOverlapResult result = _collider.NarrowOverlapCheck(_transform, body.Collision(), body.GetTransform(), nullptr, &penetration);

	bool overlapping = result == EOverlapResult::OVERLAPPING;
	bool touching = result == EOverlapResult::TOUCHING;
	
	if (overlapping || touching)
	{
		if (isnan(penetration.x) || isnan(penetration.y) || isnan(penetration.z))
		{
			Debug::PrintLine("NaN penetration!");
			return;
		}

		Vector3 pDir = touching ? penetration : penetration.Normalised();

		//Alter velocity (acceleration ignored as ResolveCollision should be called after integration of acceleration)
		float directionalVelocity = GetVelocity().Dot(pDir);

		if (directionalVelocity > 0.f)
		{
			_velocity -= pDir * directionalVelocity;

			if (directionalVelocity > minBounceVel)
			{
				if (_lockRotation)
					_velocity -= pDir * (directionalVelocity * Maths::Max(GetRestitution(), body.GetRestitution()));
				else
				{
					//todo

				}
			}
		}
	}

	if (overlapping)
	{
		//Move out of body
		SetPosition(GetPosition() - penetration);
	}
}

void PhysicsBody::ResolveCollision(PhysicsBody& a, PhysicsBody& b, float minVel)
{
	//TODO PHYSICS BETWEEN TWO PHYSICS BODIES
	//Kind of important..
}
