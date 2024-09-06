#include "ray.h"

ray::ray(){}
ray::~ray(){}


Vec3 GetRayFromMouse(int mouseX, int mouseY, const Matrix4 projectionMatrix, const Matrix4 viewMatrix, static int windowWidth, static int windowHeight) {
    // Normalize mouse coordinates to [-1, 1] range
    float x = (2.0f * mouseX) / windowWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / windowHeight;
    Vec4 rayClip(x, y, -1.0f, 1.0f); // Ray in clip space

    // Convert to eye space (camera space)
    Vec4 rayEye = MMath::inverse(projectionMatrix) * rayClip;
    rayEye = Vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Set w to 0 for direction vector

    // Convert to world space
    Vec3 rayWorld = Vec3(MMath::inverse(viewMatrix) * rayEye);
    rayWorld = VMath::normalize(rayWorld); // Normalize the direction

    return rayWorld;
}

//sphere collision
bool RayIntersectsSphere(const Vec3& rayOrigin, const Vec3& rayDirection, const Vec3& sphereCenter, float sphereRadius) {
    Vec3 oc = rayOrigin - sphereCenter;
    float a = VMath::dot(rayDirection, rayDirection);
    float b = 2.0f * VMath::dot(oc, rayDirection);
    float c = VMath::dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

// ray intersection with mesh
bool RayIntersectsTriangle(const Vec3& rayOrigin, const Vec3& rayDirection, 
                           const Vec3& v0, const Vec3& v1, const Vec3& v2, 
                           float& t) {
    const float EPSILON = 0.0000001;
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 h = VMath::cross(rayDirection, edge2);
    float a = VMath::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false; // Ray is parallel to triangle

    float f = 1.0 / a;
    Vec3 s = rayOrigin - v0;
    float u = f * VMath::dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;

    Vec3 q = VMath::cross(s, edge1);
    float v = f * VMath::dot(rayDirection, q);
    if (v < 0.0 || u + v > 1.0)
        return false;

    t = f * VMath::dot(edge2, q); // Ray intersection distance
    return t > EPSILON; // Intersection with triangle
}




