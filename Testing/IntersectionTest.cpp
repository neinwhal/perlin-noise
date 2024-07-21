//#pragma once
//#include "IntersectionTest.h"
//
//bool isPointInAABB(const glm::vec3& point, const glm::vec3& center, const glm::vec3& halfExtent)
//{
//    glm::vec3 aabbMin = center - halfExtent;
//    glm::vec3 aabbMax = center + halfExtent;
//
//    if (point.x < aabbMin.x || point.x > aabbMax.x)
//        return false;
//    if (point.y < aabbMin.y || point.y > aabbMax.y)
//        return false;
//    if (point.z < aabbMin.z || point.z > aabbMax.z)
//        return false;
//
//    return true;
//}
//
//std::string vec3ToString(const glm::vec3& vec)
//{
//    std::ostringstream oss;
//    oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
//    return oss.str();
//}
//
//bool isRayIntersectAABB(const Ray& ray, const glm::vec3& center, const glm::vec3& halfExtent)
//{
//    glm::vec3 aabbMin = center - halfExtent;
//    glm::vec3 aabbMax = center + halfExtent;
//
//    float tmin = std::numeric_limits<float>::lowest();
//    float tmax = std::numeric_limits<float>::max();
//
//    for (int i = 0; i < 3; ++i)
//    {
//        float invD = 1.0f / ray.m_Direction[i];
//        float t1 = (aabbMin[i] - ray.m_Start[i]) * invD;
//        float t2 = (aabbMax[i] - ray.m_Start[i]) * invD;
//
//        if (invD < 0.0f)
//            std::swap(t1, t2);
//
//        tmin = std::max(tmin, t1);
//        tmax = std::min(tmax, t2);
//
//        if (tmin > tmax || tmax < 0)
//        {
//            return false;
//        }
//    }
//    return true;
//}
//
//bool isAABBIntersectAABB(const AABB& aabb1, const AABB& aabb2)
//{
//    glm::vec3 aabb1Min = aabb1.m_Center - aabb1.m_HalfExtents;
//    glm::vec3 aabb1Max = aabb1.m_Center + aabb1.m_HalfExtents;
//
//    glm::vec3 aabb2Min = aabb2.m_Center - aabb2.m_HalfExtents;
//    glm::vec3 aabb2Max = aabb2.m_Center + aabb2.m_HalfExtents;
//
//    // Early rejection: check if the AABBs are separated on any axis
//    if (aabb1Max.x < aabb2Min.x || aabb1Min.x > aabb2Max.x)
//        return false;
//
//    if (aabb1Max.y < aabb2Min.y || aabb1Min.y > aabb2Max.y)
//        return false;
//
//    if (aabb1Max.z < aabb2Min.z || aabb1Min.z > aabb2Max.z)
//        return false;
//
//    return true;
//}
//bool isPlaneIntersectAABB(const Plane& plane, const AABB& aabb) {
//    glm::vec3 normal = (glm::vec3(plane.m_Normal));
//
//
//    glm::vec3 p0 = aabb.m_Center + glm::vec3(
//        aabb.m_HalfExtents.x,
//        aabb.m_HalfExtents.y,
//        aabb.m_HalfExtents.z
//    );
//
//    glm::vec3 p1 = aabb.m_Center - glm::vec3(
//        aabb.m_HalfExtents.x,
//        aabb.m_HalfExtents.y,
//        aabb.m_HalfExtents.z
//    );
//
//
//    float distanceP0 = glm::dot(normal, p0);
//    float distanceP1 = glm::dot(normal, p1);
//    const float epsilon = 0.05f;
//
//
//    bool forP0 = distanceP0 >= epsilon;
//    bool forP1 = distanceP1 <= epsilon;
//
//    bool intersects = (forP0 && forP1);
//
//
//
//    return intersects;
//}
//
//bool isPointInPlane(const glm::vec3& point, const Plane& plane) {
//    float distance = glm::dot(glm::vec3(plane.m_Normal), point) + plane.m_Normal.w;
//    const float epsilon = 0.05f;
//
//    return std::abs(distance) < epsilon;
//}
//
//bool isRayIntersectPlane(const Ray& ray, const Plane& plane) {
//    glm::vec3 rayStart      = ray.m_Start;
//    glm::vec3 rayDirection  = ray.m_Direction * 5.f;
//    glm::vec3 planeNormal   = glm::vec3(plane.m_Normal);
//    float planeDistance     = plane.m_Normal.w;
//
//    float denominator = glm::dot(rayDirection, planeNormal);
//    float numerator   = -glm::dot(planeNormal, rayStart) - planeDistance;
//    float t = numerator / denominator;
//
//    if (t >= 0.0f && t <= 1.0f) {
//        return true;
//    }
//    return false;
//}
//
//bool isPointCoplanar(const glm::vec3& pt, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
//{
//    // Compute the normal of the triangle
//    glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
//
//    // Compute the dot product between the point and the normal
//    float dotProduct = glm::dot(normal, pt - v1);
//
//    // Check if the dot product is close to zero (epsilon to account for floating-point precision)
//    return fabs(dotProduct) < 1e-6;
//}
//bool isPointInTriangle(const glm::vec3& pt, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
//{
//    // First check if the point is coplanar with the triangle
//    if (!isPointCoplanar(pt, v1, v2, v3)) {
//        return false;
//    }
//
//    // Compute vectors
//    glm::vec3 v2_v1 = v2 - v1;
//    glm::vec3 v3_v1 = v3 - v1;
//    glm::vec3 pt_v1 = pt - v1;
//
//    // Compute dot products
//    float dot00 = glm::dot(v3_v1, v3_v1);
//    float dot01 = glm::dot(v3_v1, v2_v1);
//    float dot02 = glm::dot(v3_v1, pt_v1);
//    float dot11 = glm::dot(v2_v1, v2_v1);
//    float dot12 = glm::dot(v2_v1, pt_v1);
//
//    // Compute barycentric coordinates
//    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
//    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
//    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
//
//    // Check if point is in triangle
//    return (u >= 0) && (v >= 0) && (u + v < 1.0f);
//}
//
//bool isRayIntersectTriangle(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
//    // Perform Ray vs. Plane intersection
//    glm::vec3 planeNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
//    Plane plane(glm::vec4(planeNormal, -glm::dot(planeNormal, v0)));
//    glm::vec3 rayStart = ray.m_Start;
//    glm::vec3 rayDirection = ray.m_Direction * 5.f;
//    float planeDistance = plane.m_Normal.w;
//
//    float denominator = glm::dot(rayDirection, planeNormal);
//    float numerator = -glm::dot(planeNormal, rayStart) - planeDistance;
//    float t = numerator / denominator;
//    glm::vec3 intersectionPoint = rayStart + t * rayDirection;
//    std::cout << "t: "  <<t << std::endl;
//
//    if (!isRayIntersectPlane(ray, plane)) {
//        return false; // No intersection with the plane
//    }
//
//
//
//    // Check if intersection point is inside the triangle
//    return isPointInTriangle(intersectionPoint, v0, v1, v2);
//}
//
//bool isSphereIntersectPoint(const Sphere& sphere, const glm::vec3& point)
//{
//    float d = glm::distance(sphere.m_Center, point);
//
//    float r_sq = sphere.m_Radius * sphere.m_Radius;
//
//    return  ((d * d) <= r_sq);
//}
//
//bool isSphereIntersectSphere(const Sphere& sphere1, const Sphere& sphere2)
//{
//    //Sphere tmp(sphere1.m_Center, sphere2.m_Radius + sphere1.m_Radius, 20, 20);
//    //return isSphereIntersectPoint(tmp, sphere2.m_Center);
//}
//
//bool isSphereIntersectRay(const Sphere& sphere, const Ray& ray)
//{
//    float a = glm::dot(ray.m_Direction, ray.m_Direction);
//    float b = 2.0f * glm::dot(ray.m_Direction, ray.m_Start - sphere.m_Center);
//    float c = glm::dot(ray.m_Start - sphere.m_Center, ray.m_Start - sphere.m_Center) - sphere.m_Radius * sphere.m_Radius;
//
//    float discriminant = b * b - 4 * a * c;
//
//    if (discriminant < 0.0f) {
//        return false;
//    }
//
//    if (discriminant == 0.0f) {
//        float t = -b / (2.0f * a);
//
//        if (t >= 0.0f) {
//            return true;
//        }
//        return false;
//    }
//
//    float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
//    float t2 = (-b + sqrt(discriminant)) / (2.0f * a);
//
//    if (t1 >= 0.0f || t2 >= 0.0f) {
//        return true;
//    }
//    return false;
//}
//
//
//bool isSphereIntersectAABB(const Sphere& sphere, const AABB& aabb)
//{
//    float sqDist = 0.0f;
//
//    for (int i = 0; i < 3; i++)
//    {
//        float v = sphere.m_Center[i];
//        float min = aabb.m_Center[i] - aabb.m_HalfExtents[i];
//        float max = aabb.m_Center[i] + aabb.m_HalfExtents[i];
//
//        if (v < min) sqDist += (min - v) * (min - v);
//        if (v > max) sqDist += (v - max) * (v - max);
//    }
//
//    return sqDist <= (sphere.m_Radius * sphere.m_Radius);
//}
//
//bool isPlaneIntersectSphere(const Plane& plane, const Sphere& sphere)
//{
//    float distance = glm::dot(glm::vec3(plane.m_Normal), sphere.m_Center) + plane.m_Normal.w;
//
//    return std::abs(distance) <= sphere.m_Radius;
//}
//
//bool isAABBIntersectSphere(const AABB& aabb, const Sphere& sphere)
//{
//    return isSphereIntersectAABB(sphere, aabb);
//}
