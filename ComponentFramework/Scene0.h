#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
using namespace MATH;


/// Forward declarations 
union SDL_Event;
class Body;
//class Mesh;
class MeshComponent;
//class Shader;
class ShaderComponent;
// class Texture
class Texture;


class Scene0 : public Scene {
private:
	Body* sphere;
	Body* mario;
	//Shader* shader;
	ShaderComponent* shader;
	//Mesh* mesh;
	//Mesh* mario;
	MeshComponent* mesh;
	MeshComponent* mario_mesh;
	Texture* texture;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 modelMatrix;
	bool drawInWireMode;
	bool IsPaused = false;
	Vec3 lightPos;


	Vec3 Lights[4] {
		Vec3(0.0f,5.0f,5.0f),
		Vec3(5.0f,-5.0f,5.0f),
		Vec3(-5.0f,-5.0f,5.0f),
		Vec3(-5.0f,-5.0f,0.0f)
	};



	Vec4 kd[4]{
	Vec4(0.3f,0.0f,0.0f, 0.0f),
	Vec4(0.0f,0.3f,0.0f, 0.0f),
	Vec4(0.0f,0.0f,0.3f, 0.0f),
	Vec4(0.0f,0.3f,0.3f, 0.0f)
	};

	Vec4 ks[4]{
	Vec4(0.3f,0.0f,0.0f, 0.0f),
	Vec4(0.0f,0.3f,0.0f, 0.0f),
	Vec4(0.0f,0.0f,0.3f, 0.0f),
	Vec4(0.0f,0.3f,0.3f, 0.0f)
	};


public:
	explicit Scene0();
	virtual ~Scene0();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};


#endif // SCENE0_H