#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "demoScene.h"
#include <MMath.h>
#include <Vector>
#include "Debug.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "Body.h"
#include "Texture.h"





demoScene::demoScene() : mario(nullptr), shader(nullptr), mesh(nullptr),
drawInWireMode(false), texture(nullptr) {
	Debug::Info("Created demoScene: ", __FILE__, __LINE__);
}

demoScene::~demoScene() {
	Debug::Info("Deleted demoScene: ", __FILE__, __LINE__);
}

bool demoScene::OnCreate() {
	trackball.setWindowDimensions();
	Debug::Info("Loading assets demoScene: ", __FILE__, __LINE__);
	mario = new Body();
	mario->OnCreate();
	lightPos = Vec3(10.0f, 0.0f, 0.0f);

	//mesh = new MeshComponent(nullptr, "meshes/mario.obj");
	mesh = new MeshComponent(nullptr, "meshes/eevee.obj");
	mesh->OnCreate();

	texture = new Texture();
	texture->LoadImage("textures/eevee.png");


	moon = new MeshComponent(nullptr, "meshes/sphere.obj");
	moon->OnCreate();

	moon_texture = new Texture();
	moon_texture->LoadImage("textures/moon.jpg");

	



	shader = new ShaderComponent(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	ZoomAmount = initEyeZ + Zoom;

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, ZoomAmount), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));

	moon_modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(20.0f, 0.0f, 0.0)));
	return true;
}

void demoScene::OnDestroy() {
	Debug::Info("Deleting assets demoScene: ", __FILE__, __LINE__);
	mario->OnDestroy();
	delete mario;

	mesh->OnDestroy();
	delete mesh;

	moon->OnDestroy();
	delete moon;

	shader->OnDestroy();
	delete shader;

	delete texture;
	delete moon_texture;


}

void demoScene::HandleEvents(const SDL_Event& sdlEvent) {

	trackball.HandleEvents(sdlEvent);


	if (sdlEvent.type == SDL_MOUSEWHEEL)
	{
		if (sdlEvent.wheel.y > 0) // scroll up
		{
			Zoom = Zoom + 1;
		}
		else if (sdlEvent.wheel.y < 0) // scroll down
		{
			Zoom = Zoom - 1;
		}
	}

	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_P:
			IsPaused = !IsPaused;
			break;
		}
	
	case SDL_MOUSEMOTION:
		break;

	case SDL_MOUSEBUTTONDOWN:
		break;

	case SDL_MOUSEBUTTONUP:
		break;

	default:
		break;
	}
}

void demoScene::Update(const float deltaTime) {

	if (IsPaused) {
		return;
	}

	ZoomAmount = initEyeZ + Zoom;

	if (ZoomAmount <= 0) {
		ZoomAmount = 0.1f;
	}


	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, ZoomAmount), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));


	static float angle = 0.0f;
	angle += 30.0f * deltaTime;

	Quaternion q = trackball.getQuat();

	modelMatrix = MMath::toMatrix4(q) * MMath::rotate(angle, Vec3(0.0f, 1.0f, 0.0f));


	
	 moon_modelMatrix = (MMath::toMatrix4(q)) * MMath::rotate(angle, Vec3(0.0f, 1.0f, 0.0f)) *
		MMath::translate(Vec3(-1.0f, 0.0f, 0.0f)) *
		MMath::scale(Vec3(0.15f, 0.15f, 0.15f)) *
		MMath::rotate(90.0f, Vec3(0.0f, -1.0f, 0.0f));
	
	 //moon_modelMatrix *= MMath::toMatrix4(q);
	


}

void demoScene::Render() const {
	glEnable(GL_DEPTH_TEST);
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, texture->getTextureID());

	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);


	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, modelMatrix);
	mesh->Render(GL_TRIANGLES);


	glBindTexture(GL_TEXTURE_2D, moon_texture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, moon_modelMatrix);



	glUniform3fv(shader->GetUniformID("Lights[0]"), 3, Lights[0]);
	glUniform4fv(shader->GetUniformID("ks[0]"), 3, ks[0]);
	glUniform4fv(shader->GetUniformID("kd[0]"), 3, kd[0]);

	moon->Render(GL_TRIANGLES);
	

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D, 1);
	glUseProgram(0);
}
