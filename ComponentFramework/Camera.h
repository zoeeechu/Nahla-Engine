#pragma once

#include <Matrix.h>
#include <Quaternion.h>
#include <Vector.h>
#include <MMath.h>
#include <QMath.h>
#include <VMath.h>

using namespace MATH;


class Camera {
private:

	Quaternion orient; // Orientation
	Vec3 pos; //			   Position
	Matrix4 proj; //	   Projection
	Matrix4 view; //	   View Matrix

public: 

	Camera();
	~Camera();


	void SetZoom(float _zoom);
	void setVeiw(Quaternion _q, Vec3 _vec);
	Matrix4 GetVeiwMatrix() const;
	Matrix4 GetProjectionMatrix() const;


};

