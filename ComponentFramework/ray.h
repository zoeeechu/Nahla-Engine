#ifndef RAY_H
#define RAY_H

#include <glew.h>
#include <iostream>
#include <ECS.h>
#include "ECSComponents.h"
#include <unordered_map>
#include <Vector.h>
#include <VMath.h>
#include <Matrix.h>
#include <MMath.h>

using namespace MATH;

class ray
{
public:
    ray();
    ~ray();
    Vec3 GetRayFromMouse(int mouseX, int mouseY, const Matrix4 projectionMatrix, const Matrix4 viewMatrix, static int windowWidth, static int windowHeight);
};

#endif