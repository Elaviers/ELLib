#include "Frustum.hpp"

Frustum& Frustum::operator=(const Frustum& other)
{
	normalNear = other.normalNear;
	normalTop = other.normalTop;
	normalBottom = other.normalBottom;
	normalLeft = other.normalLeft;
	normalRight = other.normalRight;
	normalFar = other.normalFar;
	nearBtmLeft = other.nearBtmLeft;
	nearTopLeft = other.nearTopLeft;
	nearBtmRight = other.nearBtmRight;
	farBtmLeft = other.farBtmLeft;
	farBtmRight = other.farBtmRight;
	farTopLeft = other.farTopLeft;
	farTopRight = other.farTopRight;
	nearTopRight = other.nearTopRight;

	return *this;
}

__forceinline bool _SphereOutsidePlane(const Vector3& point, const Vector3& planePoint2Point, const Vector3& planeNormal, float r)
{
	float dot = planePoint2Point.Dot(planeNormal);
	return dot > 0.f && dot > r;
}

bool Frustum::OverlapsSphere(const Vector3& centre, float radius) const
{
	const Vector3 ToFTL = centre - farTopLeft;
	const Vector3 ToFBR = centre - farBtmRight;

	if ((position - centre).LengthSquared() <= radius * radius) return true;

	if (_SphereOutsidePlane(centre, ToFTL, normalFar, radius)) return false;
	if (_SphereOutsidePlane(centre, ToFBR, normalRight, radius)) return false;
	if (_SphereOutsidePlane(centre, ToFTL, normalLeft, radius)) return false;
	if (_SphereOutsidePlane(centre, ToFTL, normalTop, radius)) return false;
	if (_SphereOutsidePlane(centre, ToFBR, normalBottom, radius)) return false;

	return true;
}

bool Frustum::OverlapsAABB(const Vector3& min, const Vector3& max) const
{
	Vector3 aabb[8] = { 
		min, 
		Vector3(min.x, min.y, max.z), 
		Vector3(min.x, max.y, min.z), 
		Vector3(min.x, max.y, max.z),
		Vector3(max.x, min.y, min.z), 
		Vector3(max.x, min.y, max.z), 
		Vector3(max.x, max.y, min.z), 
		max 
	};
	
	for (int i = 0; i < 6; ++i)
	{
		const Vector3& n = normals[i];
		const Vector3& p = points[i];

		int outside = 0;
		for (int j = 0; j < 8; ++j)
			if ((aabb[j] - p).Dot(n) > 0.f) ++outside;

		if (outside == 8) return false;
	}
	
	int outside;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].x > max.x) ++outside; if (outside == 8) return false;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].x < min.x) ++outside; if (outside == 8) return false;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].y > max.y) ++outside; if (outside == 8) return false;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].y < min.y) ++outside; if (outside == 8) return false;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].z > max.z) ++outside; if (outside == 8) return false;

	return true;
}
