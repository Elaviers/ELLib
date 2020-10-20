#include "CollisionCapsule.hpp"
#include <ELMaths/Ray.hpp>
#include <ELMaths/Vector2.hpp>

//Basically just the ray intersection test from CollisionSphere.cpp :/
inline bool SphereRayEntryTime(Vector3 rayStart, const Vector3& rayDir, float sphereRadius, float sphereTranslationY, float &outT)
{
    rayStart.y -= sphereTranslationY;

    float distanceSq = rayStart.LengthSquared() - sphereRadius * sphereRadius;
    float dot = rayStart.Dot(rayDir);

    if (distanceSq > 0.f && dot > 0.f)
        return false;

    float discriminant = dot * dot - distanceSq;
    if (discriminant < 0.f)
        return false;

    outT = -dot - Maths::SquareRoot(discriminant);
    return true;
}


/*
    Excerpt from CollisionSphere.cpp:


                            discriminant
                        -------------------------
                        |						|
    t = -S.D +|- sqrt( (S.D)^2 - (D.D)(S.S - R^2) )
        -----------------------------------------
                            D.D
*/

bool CollisionCapsule::IntersectsRay(const Ray& ray, RaycastResult& result, const Transform& worldTransform) const
{
    Matrix4 inv = (_transform * worldTransform).GetInverseTransformationMatrix();
    Vector3 transformedRayOrigin = (Vector4(ray.origin, 1.f) * inv).GetXYZ();
    Vector4 transformedRayDirection4 = Vector4(ray.direction, 0.f) * inv;
    Vector3 transformedRayDirection = Vector3(transformedRayDirection4.x, transformedRayDirection4.y, transformedRayDirection4.z);
    transformedRayDirection.Normalise();

    //The transformed space has the capsule at the origin with its radius and half height
    float dot = transformedRayOrigin.Dot(transformedRayDirection);

    if (transformedRayOrigin.LengthSquared() > _halfHeight * _halfHeight && dot > 0.f) 
        return false;   //We are not close enough to the capsule and are not facing it

    float dot_S_D_2D = transformedRayOrigin.x * transformedRayDirection.x + transformedRayOrigin.z * transformedRayDirection.z;
    float dot_S_S_2D = transformedRayOrigin.x * transformedRayOrigin.x + transformedRayOrigin.z * transformedRayOrigin.z;
    float dot_D_D_2D = transformedRayDirection.x * transformedRayDirection.x + transformedRayDirection.z * transformedRayDirection.z;

    //(S.D)^2 - (D.D)(S.S - R^2)
    float cylinderDiscriminant = dot_S_D_2D * dot_S_D_2D - dot_D_D_2D * (dot_S_S_2D - _radius * _radius);
    if (cylinderDiscriminant < 0.f)
        return false;   //Ray does not intersect infinite cylinder

    float cylinderEntry = (-dot_S_D_2D - Maths::SquareRoot(cylinderDiscriminant)) / dot_D_D_2D;
    float cylinderEntryY = transformedRayOrigin.y + cylinderEntry * transformedRayDirection.y;
    float cylinderHalfHeight = _halfHeight - _radius;

    if (cylinderEntryY > cylinderHalfHeight)
    {
        float t;
        if (!SphereRayEntryTime(transformedRayOrigin, transformedRayDirection, _radius, cylinderHalfHeight, t))
            return false;

        result.entryTime = t;
    }
    else if (cylinderEntryY < -cylinderHalfHeight)
    {
        float t;
        if (!SphereRayEntryTime(transformedRayOrigin, transformedRayDirection, _radius, -cylinderHalfHeight, t))
            return false;

        result.entryTime = t;
    }
    else
        result.entryTime = cylinderEntry;

    if (result.entryTime < 0.f)
        result.entryTime = 0.f;

    return true;
}

Vector3 CollisionCapsule::GetNormalForPoint(const Vector3& point, const Transform& worldTransform) const
{
    return Vector3();
}

inline Vector3 SphereRayExitPoint(float startY, const Vector3& dir,  float radius)
{
    //D.D = 1
    //t = -S.D +|- sqrt( (S.D)^2 - (S.S - R^2) )

    Vector3 rayStart(0.f, startY, 0.f);
    float dot = rayStart.Dot(dir);
    float sphereDiscriminant = dot * dot - (rayStart.LengthSquared() - radius * radius);

    Debug::Assert(sphereDiscriminant >= 0.f);

    float sphereExit = -dot + Maths::SquareRoot(sphereDiscriminant);
    return dir * sphereExit;
}

CollisionShape::OrientedPoint CollisionCapsule::GetFarthestPointInDirection(const Vector3& axisIn, const Transform& worldTransform) const
{
    Transform ft = _transform * worldTransform;
    Matrix4 transform = ft.GetTransformationMatrix();
    Vector3 axis = (Vector4(axisIn, 0.f) * ft.GetInverseTransformationMatrix()).GetXYZ().Normalised();

    float xz2 = axis.x * axis.x + axis.z * axis.z;
    if (xz2 == 0.f)
    {
        CollisionShape::OrientedPoint point;

        if (axis.y > 0.f)
            point.position = (Vector4(0.f, _halfHeight, 0.f, 1.f) * transform).GetXYZ();
        else 
            point.position = (Vector4(0.f, -_halfHeight, 0.f, 1.f) * transform).GetXYZ();

        point.normal = (point.position - worldTransform.GetPosition()).Normalise();
        return point;
    }

    /*
        S = [0, 0]
        
        t = +|- sqrt(D.D * R^2 )
            --------------------
                    D.D
    */

    
    float cylT = Maths::SquareRoot(xz2 * _radius * _radius) / xz2;
    Vector3 cylPoint = axis * cylT;

    if (cylPoint.y > _halfHeight - _radius)
    {
        //top hemi
        CollisionShape::OrientedPoint point;
        point.position = (Vector4(SphereRayExitPoint(_radius - _halfHeight, axis, _radius), 1.f) * transform).GetXYZ();
        point.normal = point.position - (worldTransform.GetPosition() + worldTransform.GetUpVector() * (_halfHeight - _radius));
        point.normal.Normalise();
        return point;
    }
    else if (cylPoint.y < _radius - _halfHeight)
    {
        //bottom hemi
        CollisionShape::OrientedPoint point;
        point.position = (Vector4(SphereRayExitPoint(_halfHeight - _radius, axis, _radius), 1.f) * transform).GetXYZ();
        point.normal = point.position - (worldTransform.GetPosition() - worldTransform.GetUpVector() * (_halfHeight - _radius));
        point.normal.Normalise();
        return point;
    }

    //on cylinder
    return { (Vector4(cylPoint, 1.f) * transform).GetXYZ(), (Vector4(axis.x, 0.f, axis.z, 0.f) * transform).GetXYZ().Normalised() };
}
