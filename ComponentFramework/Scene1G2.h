#ifndef SCENE1G2_H
#define SCENE1G2_H
#include "Scene.h"
#include "Vector.h"
#include "Trackball.h"
#include "Camera.h"
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


class Scene1G2 : public Scene {
private:

	Body* skullBody;
	Body* eyeBody;
	ShaderComponent* shader;
	MeshComponent* skull;
	MeshComponent* eye;
	MeshComponent* eye2;
	Texture* Skulltexture;
	Texture* Eyetexture;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 skullMatrix;
	Matrix4 lEyeMatrix, rEyeMatrix;
	bool drawInWireMode;
	bool IsPaused = false;
	Vec3 lightPos;

	float dt;

	Camera cam;


	float Zoom;
	float initEyeZ = -5.0f;
	float ZoomAmount;

	Trackball trackball = Trackball();


	Vec3 Lights[4]{
		Vec3(0.0f,0.0f,5.0f),

	};



	Vec4 kd[4]{
	Vec4(0.3f,0.3f,0.3f, 0.0f),

	};

	Vec4 ks[4]{
	Vec4(0.6f,0.6f,0.3f, 0.0f),

	};


public:
	explicit Scene1G2();
	virtual ~Scene1G2();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // Scene2G_H