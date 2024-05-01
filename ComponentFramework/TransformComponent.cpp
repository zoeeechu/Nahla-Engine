#include "TransformComponent.h"

TransformComponent::TransformComponent(Component* parent_, Quaternion orientation_, Vec3 translate_, Vec3 scale_) :
	Component(parent_), orientation(orientation_), translate(translate_), scale(scale_) {

	

}

TransformComponent::~TransformComponent() {}

bool TransformComponent::OnCreate() {
	return true;
}

void TransformComponent::OnDestroy() {}

void TransformComponent::Update(const float deltaTime) {}

void TransformComponent::Render() const {}