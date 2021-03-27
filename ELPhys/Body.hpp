#pragma once
#include <ELPhys/Collider.hpp>

class FixedBody
{
protected:
	const uint32 _uid;

	Collider _collider;
	Transform _transform;

	float _friction;
	float _restitution;

	//todo- THIS IS A VOID POINTER!!!!!!!!!! AHHHHHHHHH!!!!
	void* _owner;

public:
	FixedBody(uint32 uid) : _uid(uid), _friction(.5f), _restitution(.5f), _owner(nullptr) {}
	
	FixedBody(uint32 uid, const FixedBody& other) : 
		_uid(uid), 
		_collider(other._collider),
		_transform(other._transform),
		_friction(other._friction), 
		_restitution(other._restitution), 
		_owner(other._owner) 
	{}
	
	FixedBody(const FixedBody&) = delete;
	FixedBody(FixedBody&&) = delete;

	FixedBody& operator=(const FixedBody&) = delete;
	FixedBody& operator=(FixedBody&) = delete;

	virtual ~FixedBody() {}

	Collider& Collision() { return _collider; }
	const Collider& Collision() const { return _collider; }
	const Transform& GetTransform() const { return _transform; }
	const Vector3& GetPosition() const { return _transform.GetPosition(); }
	const Rotation& GetRotation() const { return _transform.GetRotation(); }
	const Vector3& GetScale() const { return _transform.GetScale(); }
	float GetFriction() const { return _friction; }
	float GetRestitution() const { return _restitution; }
	void* GetUserData() const { return _owner; }

	void SetTransform(const Transform& transform) { _transform = transform; }
	void SetPosition(const Vector3& position) { _transform.SetPosition(position); }
	void SetRotation(const Rotation& rotation) { _transform.SetRotation(rotation); }
	void SetScale(const Vector3& scale) { _transform.SetScale(scale); }
	void SetFriction(float friction) { _friction = friction; }
	void SetRestitution(float restitution) { _restitution = restitution; }
	void SetOwner(void* userData) { _owner = userData; }

	bool operator==(const FixedBody& other) const { return _uid == other._uid; }
};

class PhysicsBody : public FixedBody
{
	Vector3 _velocity;
	Vector3 _angularVelocity;

	Vector3 _pendingAcceleration;
	Vector3 _pendingAngularAcceleration;

	Transform _lastTransform;
	Vector3 _lastVelocity;

	Transform _iTransform;
	Vector3 _iVelocity;

	float _mass;

	bool _lockRotation;

public:
	PhysicsBody(uint32 uid) : FixedBody(uid), _mass(1.f) {}

	PhysicsBody(uint32 uid, const PhysicsBody& other) :
		FixedBody(uid, other),
		_velocity(other._velocity),
		_angularVelocity(other._angularVelocity),
		_pendingAcceleration(other._pendingAcceleration),
		_pendingAngularAcceleration(other._pendingAngularAcceleration),
		_lastTransform(other._lastTransform),
		_lastVelocity(other._lastVelocity),
		_mass(other._mass),
		_lockRotation(false)
	{}

	PhysicsBody(const PhysicsBody&) = delete;
	PhysicsBody(PhysicsBody&&) = delete;

	PhysicsBody& operator=(const PhysicsBody&) = delete;
	PhysicsBody& operator=(PhysicsBody&) = delete;

	const Transform& GetITransform() const { return _iTransform; }
	const Transform& GetLastTransform() const { return _lastTransform; }
	const Vector3& GetLastPosition() const { return _lastTransform.GetPosition(); }
	const Rotation& GetLastRotation() const { return _lastTransform.GetRotation(); }
	const Vector3& GetLastScale() const { return _lastTransform.GetScale(); }
	const Vector3& GetIVelocity() const { return _iVelocity; }
	const Vector3& GetVelocity() const { return _velocity; }
	const Vector3& GetLastVelocity() const { return _lastVelocity; }
	const Vector3& GetAngularVelocity() const { return _angularVelocity; }
	const Vector3& GetPendingAcceleration() const { return _pendingAcceleration; }
	const Vector3& GetPendingAngularAcceleration() const { return _pendingAngularAcceleration; }
	float GetMass() const { return _mass; }
	bool IsRotationLocked() const { return _lockRotation; }

	void SetVelocity(const Vector3& velocity) { _velocity = velocity; }
	void SetAngularVelocity(const Vector3& angularVelociy) { _angularVelocity = angularVelociy; }
	void SetPendingAcceleration(const Vector3& pendingAcceleration) { _pendingAcceleration = pendingAcceleration; }
	void SetPendingAngularAcceleration(const Vector3& pendingAngularAcceleration) { _pendingAngularAcceleration = pendingAngularAcceleration; }
	void SetMass(float mass) { _mass = mass; }
	void SetRotationLocked(bool lockRotation) { _lockRotation = lockRotation; }

	void ApplyAccelerationAtLocation(const Vector3& position, const Vector3& direction, float newtons);

	void Integrate(float deltaSeconds);
	void UpdateInterpolatedData(float alpha);

	void ResolveCollision(const FixedBody& body, float minBounceVel);
	static void ResolveCollision(PhysicsBody& bodyA, PhysicsBody& bodyB, float minBounceVel);
};
