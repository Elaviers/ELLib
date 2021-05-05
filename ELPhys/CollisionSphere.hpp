#pragma once
#include "CollisionShape.hpp"

class CollisionSphere : public CollisionShape
{
	float _radius;

public:
	CollisionSphere(float radius = 0.f, Vector3 translation = Vector3()) : CollisionShape(Transform(translation)), _radius(radius) {}
	virtual ~CollisionSphere() {}

	virtual CollisionShape* Clone() const override { return new CollisionSphere(*this); }

	float GetRadius() const { return _radius; }
	
	void SetRadius(float radius) { _radius = radius; _onChanged(); }

	virtual float GetBoundingRadius() const override
	{ 
		return _radius * Maths::Max(GetTransform().GetScale().GetData(), 3);
	}

	virtual bool IntersectsRay(const Ray&, RaycastHitInformation&, const Transform& = Transform()) const override;
	virtual OrientedPoint GetFarthestPointInDirection(const Vector3& axis, const Transform&) const override;
};
