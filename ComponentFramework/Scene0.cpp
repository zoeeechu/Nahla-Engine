#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene0.h"
#include <MMath.h>
#include <Vector>
#include "Debug.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "Body.h"
#include "Texture.h"


Scene0::Scene0() : mario(nullptr), shader(nullptr), mesh(nullptr),
drawInWireMode(false), texture(nullptr) {
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0::~Scene0() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}

bool Scene0::OnCreate() {

	Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
	mario = new Body();
	mario->OnCreate();
	lightPos = Vec3(10.0f, 0.0f, 0.0f);

	mesh = new MeshComponent(nullptr, "meshes/mario.obj");
	//mesh = new MeshComponent(nullptr, "meshes/sphere.obj");
	mesh->OnCreate();

	texture = new Texture();
	texture->LoadImage("textures/mario_main.png");

	shader = new ShaderComponent(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	return true;
}

void Scene0::OnDestroy() {
	Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
	mario->OnDestroy();
	delete mario;

	mesh->OnDestroy();
	delete mesh;

	shader->OnDestroy();
	delete shader;


}

void Scene0::HandleEvents(const SDL_Event& sdlEvent) {
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

void Scene0::Update(const float deltaTime) {

	if(IsPaused) {
		return;
	}
	static float angle = 0.0f;
	angle += 30.0f * deltaTime; 
	modelMatrix = MMath::rotate(angle, Vec3(0.0f, 1.0f, 0.0f));



}

void Scene0::Render() const {
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


	
	glUniform3fv(shader->GetUniformID("Lights[0]"), 3, Lights[0]);

	//glUniform4fv(shader->GetUniformID("LightColors[0]"), 3, LightColors[0]);

	glUniform4fv(shader->GetUniformID("ks[0]"), 3, ks[0]);
	glUniform4fv(shader->GetUniformID("kd[0]"), 3, kd[0]);
	





	mesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}
