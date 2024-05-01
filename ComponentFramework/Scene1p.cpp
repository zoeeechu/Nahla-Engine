#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene1p.h"
#include <MMath.h>
#include <Vector>
#include "Debug.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "Body.h"
#include "Texture.h"

Scene1p::Scene1p() : shader{nullptr}, mesh{nullptr},
					 drawInWireMode{false}
{
	Debug::Info("Created Scene1P:: ", __FILE__, __LINE__);
}

Scene1p::~Scene1p()
{
	Debug::Info("Deleted Scene1p: ", __FILE__, __LINE__);
}

bool Scene1p::OnCreate()
{

	Debug::Info("Loading assets  Scene1p: ", __FILE__, __LINE__);
	Body* sphere1 = new Body();
	sphere1->OnCreate();
	sphere1->angularVel = Vec3(0, 0, -1);
	// See if this works
	// slow down the spin and make it go other way
	// sphere1->angularAccel = Vec3(0, 0, -0.1);
	sphere1->pos.set(-5.5, 2.3, -5);
	sphere1->radius = 0.5f;

	Body* sphere2 = new Body();
	sphere2->OnCreate();
	sphere2->pos.set(0, 0.6, -5);
	sphere2->radius = 0.5f;
	sphere2->mass = 1.0f;

	Vec3 scalingFactor(sphere1->radius, sphere1->radius, sphere1->radius);
	// Set up platforms
	cubeMesh = new MeshComponent(nullptr, "meshes/Cube.obj");
	cubeMesh->OnCreate();

	SphereTexture = new Texture();
	SphereTexture->LoadImage("textures/moon.jpg");

	Vec3 rampPos = Vec3(-4, 0.9, -5);
	// Stretch and squish that cube into a platform
	scalingFactor.set(2, 0.1, 0.1);
	modelMatrixPlatform1 = MMath::translate(rampPos) * MMath::rotate(-platformAngleDegrees1, Vec3(0, 0, 1)) * MMath::scale(scalingFactor);
	Vec3 flatPos = Vec3(0, 0, -5);
	modelMatrixPlatform2 = MMath::translate(flatPos) * MMath::scale(scalingFactor);
	rampPos.x *= -1;
	modelMatrixPlatform3 = MMath::translate(rampPos) *
						   MMath::rotate(platformAngleDegrees1, Vec3(0, 0, 1)) * MMath::scale(scalingFactor);

	mesh = new MeshComponent(nullptr, "meshes/Sphere.obj");
	mesh->OnCreate();

	shader = new ShaderComponent(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false)
	{
		std::cout << "Shader failed ... we have a problem\n";
	}
	lightPos = Vec3(10.0f, 0.0f, 0.0f);
	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	sphere1->modelMatrix.loadIdentity();

	Spheres.push_back(sphere1);
	Spheres.push_back(sphere2);

	return true;
}

void Scene1p::OnDestroy()
{

	Debug::Info("Deleting assets Scene1P: ", __FILE__, __LINE__);


	mesh->OnDestroy();
	delete mesh;

	shader->OnDestroy();
	delete shader;

	cubeMesh->OnDestroy();
	delete cubeMesh;
}

bool SphereSphereCollisionDetected(const Body *body1, const Body *body2)
{

	bool result = false;

	Vec3 distanceVec;

	distanceVec = body2->pos - body1->pos;

	float distance = sqrt((distanceVec.x * distanceVec.x) + (distanceVec.y * distanceVec.y) + (distanceVec.z * distanceVec.z));

	if (distance <= body1->radius + body2->radius)
	{

		result = true;
	}
	return result;
}


void SphereSphereCollisionResponse(Body *&body1, Body *&body2)
{

	Vec3 collisionNormal = VMath::normalize(body2->pos - body1->pos);
	float bouncyness = 1;
	float impulse = -(1 + bouncyness) * VMath::dot((body1->vel - body2->vel), collisionNormal) / (1 / body1->mass + 1 / body2->mass);

	body1->vel = body1->vel + impulse * collisionNormal / body1->mass;
	body2->vel = body2->vel - impulse * collisionNormal / body2->mass;
}

void Scene1p::HandleEvents(const SDL_Event &sdlEvent)
{
	switch (sdlEvent.type)
	{
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode)
		{
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

void Scene1p::Update(const float deltaTime)
{

	if (IsPaused)
	{
		return;
	}

	float xMinMaxPlatform1[2] = {-5.8f, -2.0f};
	float xMinMaxPlatform2[2] = {-2.0f, 2.0f};
	float xMinMaxPlatform3[2] = {2.0f, 5.8f};

	for (int i = 0; i < Spheres.size(); i++)
	{

		bool onPlatform1 = false, onPlatform2 = false, onPlatform3 = false;

		if (Spheres[i]->pos.x > xMinMaxPlatform1[0] && Spheres[i]->pos.x < xMinMaxPlatform1[1])
		{
			onPlatform1 = true;

			platformAngleDegrees1 = 25;

			i == 0 ? torque1.z = Spheres[i]->mass * -9.8 * Spheres[i]->radius * sin(platformAngleDegrees1 * DEGREES_TO_RADIANS) : torque2.z = Spheres[i]->mass * -9.8 * Spheres[i]->radius * sin(platformAngleDegrees1 * DEGREES_TO_RADIANS);
		}
		if (Spheres[i]->pos.x > xMinMaxPlatform2[0] && Spheres[i]->pos.x < xMinMaxPlatform2[1])
		{
			onPlatform2 = true;

			platformAngleDegrees1 = 0;

			i == 0 ? torque1.z = Spheres[i]->mass * -9.8 * Spheres[i]->radius * sin(0.0f * DEGREES_TO_RADIANS) : torque2.z = Spheres[i]->mass * -9.8 * Spheres[i]->radius * sin(0.0f * DEGREES_TO_RADIANS);
		}
		if (Spheres[i]->pos.x > xMinMaxPlatform3[0] && Spheres[i]->pos.x < xMinMaxPlatform3[1])
		{
			onPlatform3 = true;

			platformAngleDegrees1 = -25;

			i == 0 ? torque1.z = Spheres[i]->mass * -9.8 * Spheres[i]->radius * sin(platformAngleDegrees1 * DEGREES_TO_RADIANS) : torque2.z = Spheres[i]->mass * -9.8 * Spheres[i]->radius * sin(platformAngleDegrees1 * DEGREES_TO_RADIANS);
		}

		if (Spheres[i]->pos.x > xMinMaxPlatform3[1])
		{

			Spheres[i]->pos.x = xMinMaxPlatform3[1];
			Spheres[i]->pos.y = 2.38f;
		}

		if (Spheres[i]->pos.x < xMinMaxPlatform1[0])
		{

			Spheres[i]->pos.x = xMinMaxPlatform1[0];
			Spheres[i]->pos.y = 2.38f;
		}

		Spheres[i]->UpdateAngularVel(deltaTime);
		Spheres[i]->UpdateOrientation(deltaTime);

		Vec3 rVectorDir(sin(platformAngleDegrees1 * DEGREES_TO_RADIANS), cos(platformAngleDegrees1 * DEGREES_TO_RADIANS), 0.0f);
		Vec3 rVector = rVectorDir * Spheres[i]->radius;

		Spheres[i]->ApplyTorque(i == 0 ? torque1 : torque2);
		Spheres[i]->vel = VMath::cross(Spheres[i]->angularVel, rVector);
		Spheres[i]->Update(deltaTime);

		// You will learn this in math 2
		Matrix4 R = MMath::toMatrix4(Spheres[i]->orientation);
		Matrix4 T = MMath::translate(Spheres[i]->pos);
		Vec3 scalingFactor(Spheres[i]->radius, Spheres[i]->radius, Spheres[i]->radius);
		Matrix4 S = MMath::scale(scalingFactor);
		Spheres[i]->modelMatrix = T * R * S;
	}

	bool hasCollided = SphereSphereCollisionDetected(Spheres[0], Spheres[1]);

	if (hasCollided && !collisionEnd)
	{

		SphereSphereCollisionResponse(Spheres[0], Spheres[1]);
		collisionEnd = true;

		Spheres[0]->angularVel.z = -Spheres[0]->vel.x / Spheres[0]->radius;
		Spheres[1]->angularVel.z = -Spheres[1]->vel.x / Spheres[1]->radius;
		std::cout << "word";
	}

	if (!hasCollided)
	{
		collisionEnd = false;
	}
}

void Scene1p::Render() const
{

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (drawInWireMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	// glUniform3fv(shader->GetUniformID("lightPos"), 1, lightPos);
	glBindTexture(GL_TEXTURE_2D, SphereTexture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, Spheres[0]->modelMatrix);
	mesh->Render(GL_TRIANGLES);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, Spheres[1]->modelMatrix);
	mesh->Render(GL_TRIANGLES);

	glUniform3fv(shader->GetUniformID("Lights[0]"), 3, Lights[0]);
	glUniform4fv(shader->GetUniformID("ks[0]"), 3, ks[0]);
	glUniform4fv(shader->GetUniformID("kd[0]"), 3, kd[0]);

	// Send the platform model matrices and render the cube
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, modelMatrixPlatform1);
	cubeMesh->Render(GL_TRIANGLES);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, modelMatrixPlatform2);
	cubeMesh->Render(GL_TRIANGLES);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, modelMatrixPlatform3);
	cubeMesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
}
