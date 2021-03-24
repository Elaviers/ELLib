#pragma once
#include "Body.hpp"
#include <ELCore/List.hpp>
#include <ELCore/Pool.hpp>

class PhysSimulation
{
	float _stepTime;
	float _timeSinceLastStep;

	Vector3 _gravity;

	List<FixedBody> _fixedBodies;
	List<PhysicsBody> _physicsBodies;

	typedef MultiPool<byte, sizeof(Pair<PhysicsBody*, FixedBody*>) * 64> PFPoolType;
	typedef MultiPool<byte, sizeof(Pair<PhysicsBody*>) * 64> PPPoolType;
	PFPoolType _pfPool;
	PPPoolType _ppPool;
	List<Pair<PhysicsBody*, FixedBody*>> _pfCollisions;
	List<Pair<PhysicsBody*>> _ppCollisions;

	uint32 _nextUID;

public:
	PhysSimulation() :
		_stepTime(1.f / 60.f), _timeSinceLastStep(0.f),
		_gravity(0.f, -9.8f, 0.f),
		_pfCollisions(NewHandler(_pfPool, &PFPoolType::NewArray), DeleteHandler(_pfPool, &PFPoolType::DeleteHandler)),
		_ppCollisions(NewHandler(_ppPool, &PPPoolType::NewArray), DeleteHandler(_ppPool, &PPPoolType::DeleteHandler)),
		_nextUID(1)
	{}

	PhysSimulation(const PhysSimulation&) = delete;
	PhysSimulation(PhysSimulation&&) = delete;

	float GetStepTime() const { return _stepTime; }
	const Vector3& GetGravity() const { return _gravity; }
	const List<FixedBody>& GetFixedBodies() const { return _fixedBodies; }
	const List<PhysicsBody>& GetPhysicsBodies() const { return _physicsBodies; }

	void SetStepTime(float stepTime) { _stepTime = stepTime; }
	void SetGravity(const Vector3& gravity) { _gravity = gravity; }
	FixedBody& NewFixedBody() { return *_fixedBodies.Emplace(_nextUID++); }
	FixedBody& NewFixedBody(const FixedBody& other) { return *_fixedBodies.Emplace(_nextUID++, other); }
	PhysicsBody& NewPhysicsBody() { return *_physicsBodies.Emplace(_nextUID++); }
	PhysicsBody& NewPhysicsBody(const PhysicsBody& other) { return *_physicsBodies.Emplace(_nextUID++, other); }
	void RemoveBody(const FixedBody& body) { _fixedBodies.Remove(body); }
	void RemoveBody(const PhysicsBody& body) { _physicsBodies.Remove(body); }

	void Simulate(float deltaSeconds, int maxSteps = 10);

	float GetInterpAlpha() const { return _timeSinceLastStep / _stepTime; }

	template <typename T>
	struct OverlapInformation
	{
		const T* body;
		EOverlapResult type;
		Vector3 penetration;
	};

	struct FindOverlapsResult
	{
		Buffer<OverlapInformation<FixedBody>> fixedBodies;
		Buffer<OverlapInformation<PhysicsBody>> physicsBodies;
	};

	int FindOverlaps(FindOverlapsResult& result, const Collider& collider, const Transform& transform, bool calculatePenetration) const;

	bool Raycast(Buffer<struct RaycastHitInformation>& outHits, const Ray& ray, ECollisionChannels channels) const;
};
