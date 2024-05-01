#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene3p.h"
#include <MMath.h>
#include "Debug.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "Body.h"
#include <QMath.h>
#include "Texture.h"


Scene3p::Scene3p() {
	Debug::Info("Created Scene3p: ", __FILE__, __LINE__);
}

Scene3p::~Scene3p() {
	Debug::Info("Deleted Scene3p: ", __FILE__, __LINE__);
}

bool Scene3p::OnCreate() {
	Debug::Info("Loading assets Scene3p: ", __FILE__, __LINE__);

	mesh = new MeshComponent(nullptr, "meshes/Sphere.obj");
	mesh->OnCreate();

	skull = new MeshComponent(nullptr, "meshes/Skull.obj");
	skull->OnCreate();

	SphereTexture = new Texture();
	SphereTexture->LoadImage("textures/moon.jpg");

	SkullTexture = new Texture();
	SkullTexture->LoadImage("textures/skull_texture.jpg");
	
	jellyfishHead = new Body();
	jellyfishHead->OnCreate();
	jellyfishHead->pos = Vec3(-1.5, 4, -25);
	jellyfishHead->radius = 3.0f;

	const int numAnchors = 10;
	Vec3 anchorPos(-6.0f, 0.0f, -25);
	for (int i = 0; i < numAnchors; i++) {
		anchors.push_back(new Body());
		anchors[i]->pos = anchorPos;
		anchors[i]->radius = 0.5f;
								

		// Novel says we should put another loop in here for the tentacle spheres
		// TODO: Figure this out properly (sketch it out)
		// Umer will just do one tentacle for now
		
			for (int j = 0; j < numSpheresPerAnchor; j++) {
				tentacleSpheres.push_back(new Body());
				tentacleSpheres[j + i*10]->pos = anchorPos + Vec3(0, -(j+1) * spacing, 0);
				tentacleSpheres[j + i*10]->radius = 0.2f;

			}
	
		// Move the anchor position for the next swing through this loop
		anchorPos += Vec3(spacing, 0, 0);
	}

	shader = new ShaderComponent(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	return true;
}

void Scene3p::OnDestroy() {
	Debug::Info("Deleting assets Scene3p: ", __FILE__, __LINE__);

	jellyfishHead->OnDestroy();
	delete jellyfishHead;

	for (Body* anchor : anchors) {
		anchor->OnDestroy();
		delete anchor;
	}

	for (Body* tentacleSphere : tentacleSpheres) {
		tentacleSphere->OnDestroy();
		delete tentacleSphere;
	}

	mesh->OnDestroy();
	delete mesh;

	skull->OnDestroy();
	delete skull;

	shader->OnDestroy();
	delete shader;

}

void Scene3p::HandleEvents(const SDL_Event &sdlEvent) {
	//trackball.HandleEvents(sdlEvent);
	
	float speed = 5.0f;

	switch( sdlEvent.type ) {
		case SDL_KEYDOWN:
			switch (sdlEvent.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					
					jellyfishHead->vel.y = speed;
					for (Body* anchor : anchors) {
						anchor->vel.y = speed;
					}
					break;

				case SDL_SCANCODE_S:
					jellyfishHead->vel.y = -speed;
					for (Body* anchor : anchors) {
						anchor->vel.y = -speed;
					}
					break;

				case SDL_SCANCODE_D:
					jellyfishHead->vel.x = speed;
					for (Body* anchor : anchors) {
						anchor->vel.x = speed;
					}
					break;

				case SDL_SCANCODE_A:
					jellyfishHead->vel.x = -speed;
					for (Body* anchor : anchors) {
						anchor->vel.x = -speed;
					}
					break;

				case SDL_SCANCODE_UP:
					jellyfishHead->vel.z = -speed;
					for (Body* anchor : anchors) {
						anchor->vel.z = -speed;
					}
					break;

				case SDL_SCANCODE_DOWN:
					jellyfishHead->vel.z = speed;
					for (Body* anchor : anchors) {
						anchor->vel.z = speed;
					}
					break;
			}
			break;
		case SDL_KEYUP:
			jellyfishHead->vel = Vec3(0, 0, 0);
			for (Body* anchor : anchors) {
				anchor->vel = Vec3(0, 0, 0);
			}
			break;
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

void Scene3p::Update(const float deltaTime) {

	// Orbit camera around origin using
	// view = Translate * rotate
	//viewMatrix = MMath::translate(Vec3(0, 0, -5)) * MMath::toMatrix4(trackball.getQuat());
	viewMatrix = MMath::translate(Vec3(0, 0, -5));

	// Start with unconstrained motion
	// Drop those tentacle spheres to the ocean floor (sad)
	for (int i = 0; i < 100; i++ ) {
		Vec3 gravityForce = Vec3(0, -9.8f * tentacleSpheres[i]->mass, 0);
		Vec3 dragForce = -5.0f * tentacleSpheres[i]->vel;
		Vec3 netForce = gravityForce + dragForce;
		tentacleSpheres[i]->ApplyForce(netForce);
		// Calculate initial estimate of velocity from gravity & drag
		tentacleSpheres[i]->UpdateVel(deltaTime);
		// Constraint goes here:

		if (i%10 == 0) {
			tentacleSpheres[i]->RodConstraint(deltaTime, anchors[i/10]->pos, tentacleSpheres[i]->rodLength);

		}
		else{
			tentacleSpheres[i]->RodConstraint(deltaTime, tentacleSpheres[i-1]->pos, tentacleSpheres[i]->rodLength);

		}
		
		tentacleSpheres[i]->UpdatePos(deltaTime);
	}
	jellyfishHead->UpdatePos(deltaTime);
	for (Body* anchor : anchors) {
		anchor->UpdatePos(deltaTime);
	}

	
	
}

void Scene3p::Render() const {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                                                        

	if(drawInWireMode){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, SkullTexture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, jellyfishHead->GetModelMatrix());
	skull->Render(GL_TRIANGLES);
	
	glUniform3fv(shader->GetUniformID("Lights[0]"), 3, Lights[0]);
	glUniform4fv(shader->GetUniformID("ks[0]"), 3, ks[0]);
	glUniform4fv(shader->GetUniformID("kd[0]"), 3, kd[0]);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, SphereTexture->getTextureID());
	for(Body* anchor : anchors) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, anchor->GetModelMatrix());
		mesh->Render(GL_TRIANGLES);
	}

	for (Body* tentacleSphere : tentacleSpheres) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, tentacleSphere->GetModelMatrix());
		mesh->Render(GL_TRIANGLES);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
}