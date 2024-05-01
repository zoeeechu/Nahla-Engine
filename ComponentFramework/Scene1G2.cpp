#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene1G2.h"
#include <MMath.h>
#include <QMath.h>
#include <Vector>
#include "Debug.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "Body.h"
#include "Texture.h"


Scene1G2::Scene1G2() : skull(nullptr), shader(nullptr),
drawInWireMode(false), Skulltexture(nullptr), Eyetexture(nullptr) {
	Debug::Info("Created Scene1G2: ", __FILE__, __LINE__);
}

Scene1G2::~Scene1G2() {
	Debug::Info("Deleted Scene1G2: ", __FILE__, __LINE__);
}

bool Scene1G2::OnCreate() {

	trackball.setWindowDimensions();

	Debug::Info("Loading assets Scene1G2: ", __FILE__, __LINE__);

	lightPos = Vec3(10.0f, 0.0f, 0.0f);

	skullBody = new Body();
	eyeBody = new Body();

	skull = new MeshComponent(nullptr, "meshes/Skull.obj");
	skull->OnCreate();

	Skulltexture = new Texture();
	Skulltexture->LoadImage("textures/skull_texture.jpg");

	eye = new MeshComponent(nullptr, "meshes/sphere.obj");
	eye->OnCreate();

	eye2 = new MeshComponent(nullptr, "meshes/sphere.obj");
	eye2->OnCreate();

	Eyetexture = new Texture();
	Eyetexture->LoadImage("textures/evilEye.jpg");





	shader = new ShaderComponent(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	ZoomAmount = initEyeZ + Zoom;

	//projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	//viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, ZoomAmount), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));

	cam.setVeiw(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f)), Vec3(0.0f, 0.0f, -5.0f));

	//cam.SetZoom(ZoomAmount , dt);
	skullMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	lEyeMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	rEyeMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	//lEyeMatrix = MMath::rotate(0, Vec3(0.0f, 1.0f, 0.0f));

	return true;
}

void Scene1G2::OnDestroy() {
	Debug::Info("Deleting assets Scene1G2: ", __FILE__, __LINE__);

	skull->OnDestroy();
	delete skull;

	eye->OnDestroy();
	delete eye;

	shader->OnDestroy();
	delete shader;


}

void Scene1G2::HandleEvents(const SDL_Event& sdlEvent) {

	trackball.HandleEvents(sdlEvent);


	if (sdlEvent.type == SDL_MOUSEWHEEL)
	{
		if (sdlEvent.wheel.y > 0) // scroll up
		{
			Zoom = Zoom + 0.8;
		}

		else if (sdlEvent.wheel.y < 0) // scroll down
		{
			Zoom = Zoom - 0.8;
		}


		ZoomAmount = initEyeZ + Zoom;





		//cam.SetZoom(ZoomAmount, dt);
		//float a = logf(Zoom);
		cam.SetZoom(ZoomAmount);

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

void Scene1G2::Update(const float deltaTime) {

	if (IsPaused) {
		return;
	}

	dt = deltaTime;


	if (ZoomAmount <= 0) {
		ZoomAmount = 0.1f;
	}



	//viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, ZoomAmount), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));



	static float angle = 0.0f;
	angle += 30.0f * deltaTime;

	Quaternion q = trackball.getQuat();

	skullMatrix = (MMath::toMatrix4(q)) *
		MMath::scale(Vec3(0.15f, 0.15f, 0.15f)) *
		MMath::rotate(0.0f, Vec3(0.0f, 1.0f, 0.0f));


	lEyeMatrix = skullMatrix * MMath::rotate(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *
		MMath::translate(Vec3(1.0f, 0.2f, 0.6f)) *
		MMath::scale(Vec3(0.28f, 0.28f, 0.28f));

	rEyeMatrix = skullMatrix * MMath::rotate(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *
		MMath::translate(Vec3(1.0f, 0.2f, -0.6f)) *
		MMath::scale(Vec3(0.28f, 0.28f, 0.28f));




}

void Scene1G2::Render() const {
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
	glBindTexture(GL_TEXTURE_2D, Skulltexture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam.GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam.GetVeiwMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, skullMatrix);



	glUniform3fv(shader->GetUniformID("Lights[0]"), 3, Lights[0]);

	//glUniform4fv(shader->GetUniformID("LightColors[0]"), 3, LightColors[0]);

	glUniform4fv(shader->GetUniformID("ks[0]"), 3, ks[0]);
	glUniform4fv(shader->GetUniformID("kd[0]"), 3, kd[0]);






	skull->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, Eyetexture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, lEyeMatrix);
	eye->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, Eyetexture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, rEyeMatrix);
	eye2->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}
