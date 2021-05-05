#pragma once
#include <ELCore/Function.hpp>
#include <ELMaths/Transform.hpp>

struct Ray;
struct RaycastHitInformation;

#define COLLISIONSHAPE_FUNCS(TYPENAME) virtual CollisionShape* Clone() const override { return new TYPENAME(*this); }

class CollisionShape
{
protected:
	friend class Collider;

	Transform _transform;

	Callback _onChanged;

	CollisionShape(const Transform& transform) : _transform(transform) {}

public:
	virtual ~CollisionShape() {}

	const Transform& GetTransform() const { return _transform; }
	const Vector3& GetPosition() const { return _transform.GetPosition(); }
	const Rotation& GetRotation() const { return _transform.GetRotation(); }
	const Vector3& GetScale() const { return _transform.GetScale(); }

	void SetTransform(const Transform& transform) { _transform = transform; _onChanged(); }
	void SetPosition(const Vector3& position) { _transform.SetPosition(position); _onChanged(); }
	void SetRotation(const Rotation& rotation) { _transform.SetRotation(rotation); _onChanged(); }
	void SetScale(const Vector3& scale) { _transform.SetScale(scale); _onChanged(); }

	virtual CollisionShape* Clone() const = 0;

	virtual float GetBoundingRadius() const = 0;

	virtual bool IntersectsRay(const Ray&, RaycastHitInformation&, const Transform& transform) const = 0;
	
	struct OrientedPoint
	{
		Vector3 position;
		Vector3 normal;
	};

	virtual OrientedPoint GetFarthestPointInDirection(const Vector3& axis, const Transform&) const = 0;
};
