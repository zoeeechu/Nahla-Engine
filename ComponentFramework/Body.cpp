#include "Body.h"
#include <QMath.h>
Body::Body(): pos{}, vel{}, accel{}, mass{1.0f}, mesh{nullptr},texture{nullptr} {
}

Body::~Body() {}

void Body::Update(float deltaTime) {
	/// From 1st semester physics class
	pos += vel * deltaTime + 0.5f * accel * deltaTime * deltaTime;
	vel += accel * deltaTime;
}

void Body::ApplyForce(Vec3 force) {
	accel = force / mass;
}

void Body::UpdateOrientation(float deltaTime)
{
	UpdateAngularVel(deltaTime);

	// Pull out angle from angular velocity
	float angleDegrees = VMath::mag(angularVel) * deltaTime * RADIANS_TO_DEGREES;
	// if angle is zero get outta here
	if (angleDegrees < VERY_SMALL) {
		return;
	}
	// Axis of rotation
	Vec3 axis = VMath::normalize(angularVel);
	// Build a quaternion
	Quaternion rotation = QMath::angleAxisRotation(angleDegrees, axis);
	// Update the orientation
	orientation = orientation * rotation;
}

void Body::UpdateAngularVel(float deltaTime) {
	angularVel += angularAccel * deltaTime;

}

void Body::ApplyTorque(Vec3 torque)
{
	float I = (2.0f / 5.0f) * mass * (radius * radius) + mass * (radius * radius);

	iMatrix = MMath::scale(I, I, I);

	angularAccel = MMath::inverse(iMatrix) * torque;
}

void Body::UpdateVel(float deltaTime)
{
	vel += accel * deltaTime;
}

void Body::UpdatePos(float deltaTime)
{
	// For constrained motion, skip the accel part
	pos += vel * deltaTime;
}

void Body::RodConstraint(float deltaTime, Vec3 anchorPoint, float rodLength)
{
	Vec3 rodVector = anchorPoint - pos;
	float positionConstraint = VMath::mag(rodVector) - rodLength;

	float JV = VMath::dot(rodVector, vel) / MATH::VMath::mag(rodVector);
	const float baumgarteStabilizationParameter = 0.15; // Try tuning this number
	float b = -(baumgarteStabilizationParameter / deltaTime) * positionConstraint;
	float lambda = -mass * (JV + b);

	Vec3 Jtransposed = rodVector / VMath::mag(rodVector);
	Vec3 deltaVel = Jtransposed * lambda / mass;
	vel += deltaVel;
}



bool Body::OnCreate() {
	return true;
}

void Body::OnDestroy() {
}

void Body::Render() const {
}


