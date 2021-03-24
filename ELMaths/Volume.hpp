#pragma once
#include "Transform.hpp"

struct Frustum;

class Volume
{
protected:
	Transform _transform;

	Volume(const Transform& transform) : _transform(transform) {}

public:
	virtual ~Volume() {}

	const Transform& GetTransform() const { return _transform; }
	const Vector3& GetPosition() const { return _transform.GetPosition(); }
	const Rotation& GetRotation() const { return _transform.GetRotation(); }
	const Vector3& GetScale() const { return _transform.GetScale(); }

	void SetTransform(const Transform& transform) { _transform = transform; }
	void SetPosition(const Vector3& position) { _transform.SetPosition(position); }
	void SetRotation(const Rotation& rotation) { _transform.SetRotation(rotation); }
	void SetScale(const Vector3& scale) { _transform.SetScale(scale); }

	virtual void Bounds_AABB(const Transform& parent, Vector3& min, Vector3& max) const = 0;
	virtual void Bounds_Sphere(const Transform& parent, Vector3& centre, float& radius) const = 0;

	virtual bool OverlapsFrustum(const Transform& parent, const Frustum& frustum) const = 0;
};

//Box at idWorldObject scaling is 1x1x1
class VBox : public Volume
{
public:
	VBox(const Transform& transform = Transform()) : Volume(transform) {}
	virtual ~VBox() {}

	virtual void Bounds_AABB(const Transform& parent, Vector3& min, Vector3& max) const override;
	virtual void Bounds_Sphere(const Transform& parent, Vector3& centre, float& radius) const override;

	virtual bool OverlapsFrustum(const Transform& parent, const Frustum& frustum) const override;
};

class VSphere : public Volume
{
	float _radius;

public:
	VSphere(const Transform& transform = Transform(), float radius = 1.f) : Volume(transform), _radius(radius) {}
	virtual ~VSphere() {}

	float GetRadius() const { return _radius; }
	void SetRadius(float radius) { _radius = radius; }

	virtual void Bounds_AABB(const Transform& parent, Vector3& min, Vector3& max) const override;
	virtual void Bounds_Sphere(const Transform& parent, Vector3& centre, float& radius) const override;

	virtual bool OverlapsFrustum(const Transform& parent, const Frustum& frustum) const override;
};
