#pragma once
#include "CollisionShape.hpp"
#include <ELMaths/Box.hpp>

class CollisionBox : public CollisionShape
{
public:
	CollisionBox(const Transform &transform = Transform()) : CollisionShape(transform) {}
	CollisionBox(const Box& box) : CollisionShape(Transform(box.centre, Rotation(), box.halfSize)) {}
	virtual ~CollisionBox() {}

	COLLISIONSHAPE_FUNCS(CollisionBox)

	float GetBoundingRadius() const override { return GetTransform().GetScale().Length(); }

	virtual bool IntersectsRay(const Ray&, RaycastHitInformation&, const Transform& = Transform()) const override;
	virtual OrientedPoint GetFarthestPointInDirection(const Vector3& axis, const Transform&) const override;
};
