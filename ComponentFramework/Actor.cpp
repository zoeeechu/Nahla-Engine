#include "Actor.h"

Actor::Actor(Component* parent_) :Component(parent_) {}

/*
* TODO ==========================
* Make these get/set part of a Actor
*	Shader Componenet
*	Tranform Componenet 
*	Mesh Componenet
*	Shader Componenet
*
* Then render based on a Actor 
* ==============================
*/

Actor::~Actor() {}

bool Actor::OnCreate() {
	return true;
}

void Actor::OnDestroy() {
}

void Actor::Update(const float deltaTime) {
}

void Actor::Render() const {
}

void Actor::ListComponents() const {
	/// typeid is a RTTI operator
	std::cout << typeid(*this).name() << " contain the following components:\n";
	for (auto component : components) {
		std::cout << typeid(*component).name() << std::endl;
	}
}


void Actor::RemoveAllComponents() {
	for (auto component : components) {
		component->OnDestroy();
		delete component;
	}
	components.clear();
}
