#pragma once

#include "Component.h"

#include "glew.h"
#include <unordered_map>
#include <string>

class ShaderComponent : public Component {
	ShaderComponent(const ShaderComponent&) = delete;
	ShaderComponent(ShaderComponent&&) = delete;
	ShaderComponent& operator = (const ShaderComponent&) = delete;
	ShaderComponent& operator = (ShaderComponent&&) = delete;
private:
	const char* vsFilename;
	const char* fsFilename;
	unsigned int shaderID;
	unsigned int vertShaderID;
	unsigned int fragShaderID;
	std::unordered_map<std::string, GLuint > uniformMap;

	/// Private helper methods
	char* ReadTextFile(const char* fileName);
	bool CompileAttach();
	bool Link();
	void SetUniformLocations();


public:
	ShaderComponent(Component* parent_, const char* vsFilename_, const char* fsFilename_);
	~ShaderComponent();
	bool OnCreate() override;
	void OnDestroy() override;

	void Update(float deltaTime) override; //just cause 

	inline GLuint GetProgram() const { return shaderID; }
	GLuint GetUniformID(std::string name);

	virtual void Render()const;
};