#pragma once
#include "Body.hpp"
#include <ELCore/List.hpp>
#include <ELCore/IteratorUtils.hpp>
#include <ELCore/PagedMemory.hpp>

class PhysSimulation
{
	template <typename T>
	using _PageAllocedList = List<T, PagedMemory<>::Allocator<T>>;

	float _stepTime;
	float _timeSinceLastStep;

	Vector3 _gravity;

	List<FixedBody> _fixedBodies;
	List<PhysicsBody> _physicsBodies;

	PagedMemory<> _pfPool;
	PagedMemory<> _ppPool;

	_PageAllocedList<Pair<PhysicsBody*, FixedBody*>> _pfCollisions;
	_PageAllocedList<Pair<PhysicsBody*>> _ppCollisions;

	uint32 _nextUID;

public:
	PhysSimulation() :
		_stepTime(1.f / 60.f), _timeSinceLastStep(0.f),
		_gravity(0.f, -9.8f, 0.f),
		_pfCollisions(_pfPool.GetAllocator<Pair<PhysicsBody*, FixedBody*>>()),
		_ppCollisions(_ppPool.GetAllocator<Pair<PhysicsBody*>>()),
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
	FixedBody& NewFixedBody() { return _fixedBodies.EmplaceBack(_nextUID++); }
	FixedBody& NewFixedBody(const FixedBody& other) { return _fixedBodies.EmplaceBack(_nextUID++, other); }
	PhysicsBody& NewPhysicsBody() { return _physicsBodies.EmplaceBack(_nextUID++); }
	PhysicsBody& NewPhysicsBody(const PhysicsBody& other) { return _physicsBodies.EmplaceBack(_nextUID++, other); }
	void RemoveBody(const FixedBody& body) { _fixedBodies.Remove(IteratorUtils::FirstEqualPosition(_fixedBodies.begin(), _fixedBodies.end(), body)); }
	void RemoveBody(const PhysicsBody& body) { _physicsBodies.Remove(IteratorUtils::FirstEqualPosition(_physicsBodies.begin(), _physicsBodies.end(), body)); }

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
