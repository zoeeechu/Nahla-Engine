#ifndef BODY_H
#define BODY_H
#include <Vector.h> /// This is in GameDev
#include <Quaternion.h>
#include "MMath.h"
using namespace MATH; 

/// Just forward declair these classes so I can define a pointer to them
/// Used later in the course
class Mesh;
class Texture;

class Body {
public:
    Body();
    ~Body();
	Vec3 pos;
	Vec3 vel;
	Vec3 accel;
	float mass = 1.0f;
	// New stuff for physics 2
	Vec3 angularVel;
	Quaternion orientation;
	Vec3 angularAccel;

	Matrix4 modelMatrix;
	Matrix3 iMatrix;
	float radius = 0.5f;
	float rodLength = 1.0f;
private: /// Graphics stuff 
	Mesh *mesh;
	Texture *texture;
	
public:
	bool OnCreate();
	void OnDestroy();
	void Update(float deltaTime);
	void UpdateVel(float deltaTime);
	void UpdatePos(float deltaTime);
	void Render() const;
	void ApplyForce(Vec3 force);
	void UpdateOrientation(float deltaTime);
	void setAccel(const Vec3 &accel_) { accel = accel_;}
	void UpdateAngularVel(float deltaTime);
	void ApplyTorque(Vec3 torque);

		Matrix4 GetModelMatrix() const {
		Matrix4 R = MMath::toMatrix4(orientation);
		Matrix4 T = MMath::translate(pos);
		Vec3 scalingFactor(radius, radius, radius);
		Matrix4 S = MMath::scale(scalingFactor);
		return T * R * S;
	}
	void RodConstraint(float deltaTime, Vec3 anchorPoint, float rodLength);
};

#endif
