#include "PhysSimulation.hpp"
#include "RaycastHitInformation.hpp"

void PhysSimulation::Simulate(float deltaSeconds, int maxSteps)
{
	_timeSinceLastStep += deltaSeconds;

	int i = 0;
	while (_timeSinceLastStep > _stepTime && i++ < maxSteps)
	{
		for (PhysicsBody& bodyA : _physicsBodies)
		{
			bodyA.SetPendingAcceleration(bodyA.GetPendingAcceleration() + _gravity);

			for (FixedBody& bodyB : _fixedBodies)
				if (bodyA.Collision().BroadOverlapCheck(bodyA.GetTransform(), bodyB.Collision(), bodyB.GetTransform()))
					_pfCollisions.Emplace(&bodyA, &bodyB);

			for (PhysicsBody& bodyB : _physicsBodies)
				if (bodyA != bodyB && bodyA.Collision().BroadOverlapCheck(bodyA.GetTransform(), bodyB.Collision(), bodyB.GetTransform()))
					if (!_ppCollisions.FirstWhere([&bodyA, &bodyB](const Pair<PhysicsBody*>& p) { return p.first == &bodyB && p.second == &bodyA; }).IsValid())
						_ppCollisions.Emplace(&bodyA, &bodyB);
		}

		for (Pair<PhysicsBody*, FixedBody*>& collision : _pfCollisions)
			collision.first->ResolveCollision(*collision.second);

		for (Pair<PhysicsBody*>& collision : _ppCollisions)
			PhysicsBody::ResolveCollision(*collision.first, *collision.second);

		for (PhysicsBody& body : _physicsBodies)
		{
			body.Integrate(_stepTime);
		}

		_pfCollisions.Clear(); _ppCollisions.Clear();
		_pfPool.Clear(); _pfPool.Clear();

		_timeSinceLastStep -= _stepTime;
	}
}

template <typename T>
void _FindOverlapsForSet(Buffer<PhysSimulation::OverlapInformation<T>>& results,
	const List<T>& set, 
	int& count, 
	const Collider& collider, 
	const Transform& transform, 
	bool calculatePenetration)
{
	PhysSimulation::OverlapInformation<T> info;

	for (const T& body : set)
	{
		if (collider.BroadOverlapCheck(transform, body.Collision(), body.GetTransform()))
		{
			info.type = collider.NarrowOverlapCheck(transform, body.Collision(), body.GetTransform(), nullptr, calculatePenetration ? &info.penetration : nullptr);

			if (info.type != EOverlapResult::SEPERATE)
			{
				++count;
				info.body = &body;
				results.Add(std::move(info));
				info.type = EOverlapResult::SEPERATE;
			}
		}
	}
}

int PhysSimulation::FindOverlaps(FindOverlapsResult& result, const Collider& collider, const Transform& transform, bool calculatePenetration) const
{
	int overlaps = 0;
	_FindOverlapsForSet(result.fixedBodies, _fixedBodies, overlaps, collider, transform, calculatePenetration);
	_FindOverlapsForSet(result.physicsBodies, _physicsBodies, overlaps, collider, transform, calculatePenetration);

	return overlaps;
}

bool PhysSimulation::Raycast(Buffer<RaycastHitInformation>& outHits, const Ray& ray, ECollisionChannels channels) const
{
	bool success = false;
	RaycastHitInformation info;

	for (const FixedBody& fb : _fixedBodies)
		if (fb.Collision().IntersectsRay(fb.GetTransform(), ray, channels, info))
		{
			info.fBody = &fb;
			outHits.Add(std::move(info));
			success = true;
		}

	for (const PhysicsBody& pb : _physicsBodies)
		if (pb.Collision().IntersectsRay(pb.GetTransform(), ray, channels, info))
		{
			info.pBody = &pb;
			outHits.Add(std::move(info));
			success = true;
		}

	return success;
}
