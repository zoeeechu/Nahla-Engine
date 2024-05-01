#ifndef SCENE5G_H
#define SCENE5G_H
#include "Scene.h"
#include "Vector.h"
#include "Trackball.h"
#include "Camera.h"
#include <Matrix.h>
#include <vector>

#include "ECS.h"
#include "ECSComponents.h"

using namespace MATH;

/// Forward declarations
union SDL_Event;
class Body;
class Texture;

class skybox;

class Scene5G : public Scene
{
private:
	Body *skullBody;
	Body *eyeBody;

	GLuint framebuffer;
	GLuint texture;
	GLuint rboId;
	Texture *Skulltexture;
	Texture *Eyetexture;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 skullMatrix;
	Matrix4 planeMatrix;
	Matrix4 lEyeMatrix, rEyeMatrix;
	Matrix4 RCMatrix;
	bool drawInWireMode;
	bool IsPaused = false;
	mutable bool isGameWindowFocused = false;
	// mutable bool isGameWindowHovered = false;
	mutable bool isTitleBarHovered = false;
	Vec3 lightPos;

	skybox *Sky;

	float dt;
	float tesLvl = 1.0f;

	Camera cam;

	float Zoom;
	float initEyeZ = -5.0f;
	float ZoomAmount;
	float prevZoom;

	Trackball trackball = Trackball();

	Vec3 Lights[4]{
		Vec3(0.0f, 8.0f, 5.0f),
		Vec3(0.0f, -10.0f, 5.0f),
	};

	Vec4 kd[4]{
		// color
		Vec4(0.5f, 0.5f, 0.8f, 0.0f), // rgba
		Vec4(0.3f, 0.3f, 0.3f, 0.0f),
	};

	Vec4 ks[4]{
		// specular

		Vec4(0.05f, 0.05f, 0.05f, 0.0f), // rgba
		Vec4(0.06f, 0.06f, 0.03f, 0.0f),

	};

public:
	explicit Scene5G();
	virtual ~Scene5G();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
	void HandleTheGUI();
	void CreateFBO();
};

#endif // Scene2G_H