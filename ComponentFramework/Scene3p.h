#ifndef SCENE3P_H
#define SCENE3P_H
#include "Scene.h"
#include <Vector.h>
#include <Matrix.h>
#include <vector>
#include "Trackball.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class MeshComponent;
class ShaderComponent;
class Texture;



class Scene3p : public Scene {
private:
	Body* jellyfishHead = nullptr;
	std::vector<Body*> anchors;
	std::vector<Body*> tentacleSpheres;
	// These variables will tells us how long each tentacle will be
	// and the spacing between individual spheres 
	const int numSpheresPerAnchor = 10;
	const float spacing = 1.0f;

	ShaderComponent* shader = nullptr;
	MeshComponent* mesh = nullptr;
	MeshComponent* skull = nullptr;

	Texture* SphereTexture;
	Texture* SkullTexture;


	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool drawInWireMode = false;
	float time = 0;
	Trackball trackball;

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
	explicit Scene3p();
	virtual ~Scene3p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};


#endif 