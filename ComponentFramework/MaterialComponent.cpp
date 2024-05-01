#include "MaterialComponent.h"

MaterialComponent::MaterialComponent(Component* parent_) :Component(parent_) {}

MaterialComponent::~MaterialComponent() {}

bool MaterialComponent::OnCreate() {
	return true;
}

void MaterialComponent::OnDestroy() {}

void MaterialComponent::Update(const float deltaTime) {}

void MaterialComponent::Render() const {}