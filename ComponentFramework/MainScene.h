#ifndef MainScene_H
#define MainScene_H


#include "Scene.h"
#include "Vector.h"
#include "Trackball.h"
#include "Camera.h"
#include "Debug.h"
#include "Body.h"
#include "skybox.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_internal.h"


#include "ECS.h"
#include "ECSComponents.h"


#include <Matrix.h>
#include <vector>
#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include <QMath.h>
#include <Vector>
#include <unordered_map>
#include <map>
#include <string>
#include <memory>
#include <vector>

using namespace MATH;

/// Forward declarations
union SDL_Event;
class Body;
class Texture;

class skybox;

class MainScene : public Scene
{
private:
	Body *skullBody;
	Body *eyeBody;
    Body *cubeBody;

	GLuint framebuffer;
	GLuint texture;
	GLuint rboId;
	Texture *Skulltexture;
	Texture *Eyetexture;
    Texture *cubeTexture;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 skullMatrix;
    Matrix4 cubeMatrix;
	Matrix4 planeMatrix;
	Matrix4 lEyeMatrix, rEyeMatrix;
	Matrix4 RCMatrix;

    Vec3 translationOffset = Vec3(0.0f, 0.0f, 0.0f);



	bool drawInWireMode;
	bool IsPaused = false;
    bool IsHoldingShift = false; 
	mutable bool isGameWindowFocused = false;
	// mutable bool isGameWindowHovered = false;
	mutable bool isTitleBarHovered = false;
	Vec3 lightPos;

	skybox *Sky;

	float dt;
	float tesLvl = 0.1f;

	float fogDensity = 0.2f;
	float fogGradient = 1.2f;

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
	explicit MainScene();
	virtual ~MainScene();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
	void HandleTheGUI();
	void CreateFBO();
	void AssignUniqueColors();
	void DepthBufferFBO();
	void renderObjectWithID(Entity *object);

};

#endif // Scene2G_H