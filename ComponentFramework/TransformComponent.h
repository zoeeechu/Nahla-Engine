#pragma once
#include "Component.h"
#include <Vector.h>
#include <Matrix.h>
#include <Quaternion.h>

using namespace MATH;


class TransformComponent : public Component {

private:

	Quaternion orientation;
	Vec3 translate;
	Vec3 scale; 
	Matrix4 modelMatrix;

public:

	TransformComponent(Component* parent_, Quaternion orientation_ , Vec3 translate_, Vec3 scale_);
	~TransformComponent();
	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const;

	Matrix4 getModelMatrix() {
		return modelMatrix;
	}


	Vec3 GetPosition() { // idk if this is right???
		return translate;
	}

	Quaternion GetOrientation() {
		return orientation;
	}

};
