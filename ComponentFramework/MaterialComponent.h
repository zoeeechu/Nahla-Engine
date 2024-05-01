#pragma once
#include "Component.h"
class MaterialComponent : public Component {
public:
	MaterialComponent(Component* parent_);
	virtual ~MaterialComponent();
	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime_);
	virtual void Render()const;
};