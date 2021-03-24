#include "Volume.hpp"
#include "Frustum.hpp"

void VBox::Bounds_AABB(const Transform& parent, Vector3& min, Vector3& max) const
{
	Transform t = _transform * parent;
	const Vector3& s = t.GetScale();
	Vector3 halfSz = ((t.GetForwardVector() * s.z).Abs() + (t.GetRightVector() * s.x).Abs() + (t.GetUpVector() * s.y).Abs()) / 2.f;

	min = t.GetPosition() - halfSz;
	max = t.GetPosition() + halfSz;
}

void VBox::Bounds_Sphere(const Transform& parent, Vector3& centre, float& radius) const
{
	centre = (Vector4(_transform.GetPosition()) * parent.GetMatrix()).GetXYZ();
	radius = (_transform.GetScale() * parent.GetScale()).Abs().Length() / 2.f;
}

bool VBox::OverlapsFrustum(const Transform& parent, const Frustum& frustum) const
{
	Vector3 min, max;
	Bounds_AABB(parent, min, max);
	return frustum.OverlapsAABB(min, max);
}

void VSphere::Bounds_AABB(const Transform& parent, Vector3& min, Vector3& max) const
{
	Transform t = _transform * parent;
	const Vector3& s = t.GetScale() * parent.GetScale();
	Vector3 extent = (t.GetForwardVector() * s.z).Abs() + (t.GetRightVector() * s.x).Abs() + (t.GetUpVector() * s.y).Abs();

	min = t.GetPosition() - extent;
	max = t.GetPosition() + extent;
}

void VSphere::Bounds_Sphere(const Transform& parent, Vector3& centre, float& radius) const
{
	centre = (Vector4(_transform.GetPosition()) * parent.GetMatrix()).GetXYZ();
	radius = _radius * Maths::Max((_transform.GetScale() * parent.GetScale()).GetData(), 3);
}

bool VSphere::OverlapsFrustum(const Transform& parent, const Frustum& frustum) const
{
	Vector3 centre;
	float radius;
	Bounds_Sphere(parent, centre, radius);
	return frustum.OverlapsSphere(centre, radius);
}
