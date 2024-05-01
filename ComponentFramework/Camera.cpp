#include "Camera.h"


Camera::Camera()
{
    proj = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
  //  view = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
   // orient = Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f));
   // pos = Vec3(0.0f, 0.0f, -5.0f);
}

Camera::~Camera()
{
}


void Camera::SetZoom(float _zoom)
{
  pos.z =  _zoom;
   //pos = VMath::lerp(pos, Vec3(pos.x, pos.y, _zoom), t);
}

void Camera::setVeiw(Quaternion _q, Vec3 _vec)
{
    orient = _q;
    pos = _vec;
}

Matrix4 Camera::GetVeiwMatrix() const
{
    return MMath::translate(pos) * MMath::toMatrix4(orient);
}

Matrix4 Camera::GetProjectionMatrix() const
{
    return proj;
}
