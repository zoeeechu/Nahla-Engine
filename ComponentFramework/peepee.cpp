
// Moddified by: Zoe Somji
// Date: 2024

#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "peepee.h"
#include <MMath.h>
#include <QMath.h>
#include <Vector>
#include "Debug.h"
#include "Body.h"
#include "skybox.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "TestScene.cpp"

//when your done with the ECS make a asset manager with shared pointers -zoe
template <typename T>
using ref = std::shared_ptr<T>;

#include "ECS.h"
#include "ECSComponents.h"

Manager manager;
auto &skull(manager.addEntity());
auto &eye(manager.addEntity());
auto &eye2(manager.addEntity());
auto &shader(manager.addEntity());

peepee::peepee() : drawInWireMode(false), Skulltexture(nullptr), Eyetexture(nullptr)
{
	Debug::Info("Created peepee: ", __FILE__, __LINE__);
}

peepee::~peepee()
{
	Debug::Info("Deleted peepee: ", __FILE__, __LINE__);
}

bool peepee::OnCreate()
{

	trackball.setWindowDimensions();

	Debug::Info("Loading assets peepee: ", __FILE__, __LINE__);

	lightPos = Vec3(10.0f, 0.0f, 0.0f);

	skullBody = new Body();
	eyeBody = new Body();

	skull.addComponent<newMeshComponent>(nullptr, "meshes/Skull.obj");
	skull.getComponent<newMeshComponent>().OnCreate();
	skull.addComponent<newTransformComponent>(Vec3(0.0f, 0.0f, 0.0f),  Vec3(0.15f, 0.15f, 0.15f), Quaternion(1.0f, Vec3(0.0f, 1.0f, 0.0f)));
	skull.addComponent<TextureComponent>().LoadTexture("textures/skull_texture.jpg");

	eye.addComponent<newMeshComponent>(nullptr, "meshes/sphere.obj");
	eye.getComponent<newMeshComponent>().OnCreate();
	eye.addComponent<newTransformComponent>(Vec3(1.0f, 0.2f, 0.6f),  Vec3(0.28f, 0.28f, 0.28f), Quaternion(-90.0f, Vec3(0.0f, 1.0f, 0.0f)));
	eye.addComponent<TextureComponent>().LoadTexture("textures/evilEye.jpg");

	eye2.addComponent<newMeshComponent>(nullptr, "meshes/sphere.obj");
	eye2.getComponent<newMeshComponent>().OnCreate();
	eye2.addComponent<newTransformComponent>(Vec3(1.0f, 0.2f, -0.6f),  Vec3(0.28f, 0.28f, 0.28f), Quaternion(-90.0f, Vec3(0.0f, 1.0f, 0.0f)));
	eye2.addComponent<TextureComponent>().LoadTexture("textures/evilEye.jpg");


	shader.addComponent<newShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader.getComponent<newShaderComponent>().OnCreate() == false)
	{
		std::cout << "Shader failed ... we have a problem\n";
	}

	ZoomAmount = initEyeZ + Zoom;
	prevZoom = Zoom;

	// Sky = new skybox("textures/skyBox/Underwater Box_Posx.png", "textures/skyBox/Underwater Box_Negx.png",
	//				 "textures/skyBox/Underwater Box_Posy.png", "textures/skyBox/Underwater Box_Negy.png",
	//				 "textures/skyBox/Underwater Box_Posz.png", "textures/skyBox/Underwater Box_Negz.png");

	Sky = new skybox("textures/skyBox/posx.jpg", "textures/skyBox/negx.jpg",
					 "textures/skyBox/posy.jpg", "textures/skyBox/negy.jpg",
					 "textures/skyBox/posz.jpg", "textures/skyBox/negz.jpg");
	Sky->OnCreate();

	cam.setVeiw(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f)), Vec3(0.0f, 0.0f, -5.0f));
	

	return true;
	
}

void peepee::OnDestroy()
{
	Debug::Info("Deleting assets peepee: ", __FILE__, __LINE__);

	skull.getComponent<newMeshComponent>().OnDestroy();
	delete &skull;

	eye.getComponent<newMeshComponent>().OnDestroy();
	delete &eye;

	eye2.getComponent<newMeshComponent>().OnDestroy();
	delete &eye2;

	shader.getComponent<newShaderComponent>().OnDestroy();
	delete &shader;
}

void peepee::HandleEvents(const SDL_Event &sdlEvent)
{

	ImGuiIO &io = ImGui::GetIO();
	bool isClickingImGuiWindow = io.WantCaptureMouse;

	if (prevZoom != Zoom)
	{
		prevZoom = Zoom;
		ZoomAmount = initEyeZ + Zoom;
		cam.SetZoom(ZoomAmount);
		Sky->setZoom(ZoomAmount);
	}

	if (isClickingImGuiWindow && !isGameWindowFocused) { return; }

	if (!isTitleBarHovered)
	{
		trackball.HandleEvents(sdlEvent);
	}

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

		cam.SetZoom(ZoomAmount);
		Sky->setZoom(ZoomAmount);
	}

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

void peepee::Update(const float deltaTime)
{
	if (IsPaused){ return; }
	HandleTheGUI();
	dt = deltaTime;
	if (ZoomAmount <= 0) { ZoomAmount = 0.1f; }

	static float angle = 0.0f;
	angle += 30.0f * deltaTime;

	Quaternion q = trackball.getQuat();
	Sky->setTrackball(q);

	skullMatrix = MMath::toMatrix4(q) * skull.getComponent<newTransformComponent>().transform();
	lEyeMatrix = skullMatrix * eye.getComponent<newTransformComponent>().transform();
	rEyeMatrix = skullMatrix * eye2.getComponent<newTransformComponent>().transform();

}

void peepee::HandleTheGUI()
{
	bool open = true;
	ImGui::Begin("Frame rate", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos("Frame rate", ImVec2(0, 0));
	ImGui::Text("Zoom: %.1f ", Zoom);
	static float values[90] = {};
	static int values_offset = 0;
	static double refresh_time = 0.0;
	while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
	{
		static float phase = 0.0f;
		values[values_offset] = ImGui::GetIO().Framerate;
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		phase += 0.10f * values_offset;
		refresh_time += 1.0f / 60.0f;
	}

	ImGui::Text("%.1f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	float average = 0.0f;
	for (int n = 0; n < IM_ARRAYSIZE(values); n++)
		average += values[n];
	average /= (float)IM_ARRAYSIZE(values);
	char overlay[32];
	sprintf_s(overlay, "avg fps %f", average);
	ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 80.0f, ImVec2(0, 80.0f));
	ImGui::End();

	ImGui::Begin("Name", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
	ImGui::Text("ZOE WAS HERE");
	// ImGui::Text("SAMPLE BUTTONS");
	static int clicked = 0;
	if (ImGui::Button("Button"))
		clicked++;
	if (clicked & 1)
	{
		ImGui::SameLine();
		ImGui::Text("Thanks for clicking me!");
	}
	for (int i = 0; i < 7; i++)
	{
		if (i > 0)
			ImGui::SameLine();
		ImGui::PushID(i);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
		ImGui::Button("Click");
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	}
	ImGui::End();

	ImGui::SliderFloat("Zoom amount ", &Zoom, -10.0f, 10.0f, "%.2f");
	if (ImGui::TreeNode("Game Objects"))
	{
		// HelpMarker("The same contents can be accessed in 'Tools->Style Editor' or by calling the ShowStyleEditor() function.");
		// ImGui::ShowStyleEditor();
		ImGui::TreePop();
		// ImGui::Separator();
	}

	ImGui::ShowDemoWindow();

	//::Begin("Background color");
	// ImGui::ColorEdit3("background color", backGroundColor);
	// ImGui::End();
}

void peepee::Render() const
{

	ImGuiWindowFlags windowFlags = isTitleBarHovered ? ImGuiWindowFlags_NoBackground : ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove;
	ImGui::Begin("Game", nullptr, windowFlags);

	isTitleBarHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);
	// isGameWindowHovered = ImGui::IsWindowHovered();
	isGameWindowFocused = ImGui::IsWindowFocused();

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	float sceneWidth = 1280.0f;
	float sceneHeight = 720.0f;

	// Calculate the position offset to center the rendered scene within the ImGui window
	float xOffset = (windowSize.x - sceneWidth) / 2.0f;
	float yOffset = (windowSize.y - sceneHeight) / 2.0f;

	// Set OpenGL viewport to match the ImGui window size and position
	glViewport(static_cast<int>(windowPos.x), static_cast<int>(ImGui::GetIO().DisplaySize.y - (windowPos.y + windowSize.y)),
			   static_cast<int>(windowSize.x), static_cast<int>(windowSize.y));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glEnable(GL_DEPTH_TEST);

	if (drawInWireMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	glPushMatrix();
	glTranslatef(xOffset, yOffset, 0.0f);

	Sky->Render(cam);

	glUseProgram(shader.getComponent<newShaderComponent>().GetProgram());
	glBindTexture(GL_TEXTURE_2D, skull.getComponent<TextureComponent>().getTextureID());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("projectionMatrix"), 1, GL_FALSE, cam.GetProjectionMatrix());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("viewMatrix"), 1, GL_FALSE, cam.GetVeiwMatrix());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, skullMatrix);
	skull.getComponent<newMeshComponent>().Render(GL_TRIANGLES);

	glUniform3fv(shader.getComponent<newShaderComponent>().GetUniformID("LightPos[0]"), 3, Lights[0]);


	glUniform4fv(shader.getComponent<newShaderComponent>().GetUniformID("ks[0]"), 3, ks[0]);
	glUniform4fv(shader.getComponent<newShaderComponent>().GetUniformID("kd[0]"), 3, kd[0]);

	glBindTexture(GL_TEXTURE_2D, eye.getComponent<TextureComponent>().getTextureID());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, lEyeMatrix);
	eye.getComponent<newMeshComponent>().Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D,  eye2.getComponent<TextureComponent>().getTextureID());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, rEyeMatrix);
	eye2.getComponent<newMeshComponent>().Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	glPopMatrix();

	ImGui::End();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
}
