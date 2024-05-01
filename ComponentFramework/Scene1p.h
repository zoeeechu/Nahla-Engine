#ifndef SCENE1P_H
#define SCENE1P_H
#include "Scene.h"
#include <Vector.h>
#include <Matrix.h>
#include <vector>
using namespace MATH;

union SDL_Event;
class Body;
//class Mesh;
class MeshComponent;
//class Shader;
class ShaderComponent;
// class Texture
class Texture;


class Scene1p : public Scene {
private:

	std::vector<Body*> Spheres;
	Texture* SphereTexture;
	ShaderComponent* shader;
	MeshComponent* mesh;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool drawInWireMode;
	bool collisionEnd = false;
	Vec3 lightPos;
	bool IsPaused = false;

	MeshComponent* cubeMesh = nullptr;

	Matrix4 modelMatrixPlatform1;
	Matrix4 modelMatrixPlatform2;
	Matrix4 modelMatrixPlatform3;

	float platformAngleDegrees1 = 25.0f;
	float platformAngleDegrees2 = 25.0f;
	Vec3 torque1;
	Vec3 torque2;
	

	Vec3 Lights[4]{
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
	explicit Scene1p();
	virtual ~Scene1p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // SCENE1G_H