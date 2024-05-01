// Moddified by: Zoe Somji
// Date: 2024

#include "ECS.h"
#include "ECSComponents.h"

#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "TestScene.h"
#include <MMath.h>
#include <QMath.h>
#include <Vector>
#include "Debug.h"
#include "Body.h"
#include "skybox.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_opengl3.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
using namespace std;
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define GET_VARIABLE_NAME(Variable) (#Variable)

static int textureWidth = 1280;
static int textureHeight = 720;

// when your done with the ECS make a asset manager with shared pointers -zoe
template <typename T>
using refPtr = std::shared_ptr<T>;
static unordered_map<refPtr<Entity>, int> entityMap;

static Manager manager;
//  static bool show_app_dockspace = false;


static Entity &skull(manager.addEntity("skull"));
static Entity &eye(manager.addEntity("eye"));
static Entity &eye2(manager.addEntity("eye2"));
static Entity &shader(manager.addEntity("shader"));
static Entity &tesShader(manager.addEntity("tesShader"));
static Entity &plane(manager.addEntity("plane"));

TestScene::TestScene() : drawInWireMode(false), Skulltexture(nullptr), Eyetexture(nullptr)
{
	Debug::Info("Created TestScene: ", __FILE__, __LINE__);
}

TestScene::~TestScene()
{
	Debug::Info("Deleted TestScene: ", __FILE__, __LINE__);
}

bool TestScene::OnCreate()
{

	CreateFBO();
	GLint maxPatchVerticies;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVerticies);
	printf("maxPatchVerticies: %d\n", maxPatchVerticies);

	for (int i = 0; i < manager.getEntities().size(); ++i)
	{
		entityMap[manager.getEntities()[i]] = i;
	}
	for_each(entityMap.begin(), entityMap.end(), [](auto &p)
			 { p.first->addComponent<BodyComponent>(); });

	trackball.setWindowDimensions();

	Debug::Info("Loading assets TestScene: ", __FILE__, __LINE__);

	lightPos = Vec3(10.0f, 0.0f, 0.0f);

	skullBody = new Body();
	eyeBody = new Body();

	plane.addComponent<newMeshComponent>(nullptr, "meshes/Plane.obj");
	plane.getComponent<newMeshComponent>().OnCreate();
	plane.addComponent<newTransformComponent>(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Quaternion(-90.0f, Vec3(1.0f, 0.0f, 0.0f)));

	skull.addComponent<newMeshComponent>(nullptr, "meshes/Skull.obj");
	skull.getComponent<newMeshComponent>().OnCreate();
	skull.addComponent<newTransformComponent>(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.15f, 0.15f, 0.15f), Quaternion(1.0f, Vec3(0.0f, 1.0f, 0.0f)));
	skull.addComponent<TextureComponent>().LoadTexture("textures/skull_texture.jpg");

	eye.addComponent<newMeshComponent>(nullptr, "meshes/sphere.obj");
	eye.getComponent<newMeshComponent>().OnCreate();
	eye.addComponent<newTransformComponent>(Vec3(1.0f, 0.2f, 0.6f), Vec3(0.28f, 0.28f, 0.28f), Quaternion(-90.0f, Vec3(0.0f, 1.0f, 0.0f)));
	eye.addComponent<TextureComponent>().LoadTexture("textures/evilEye.jpg");

	eye2.addComponent<newMeshComponent>(nullptr, "meshes/sphere.obj");
	eye2.getComponent<newMeshComponent>().OnCreate();
	eye2.addComponent<newTransformComponent>(Vec3(1.0f, 0.2f, -0.6f), Vec3(0.28f, 0.28f, 0.28f), Quaternion(-90.0f, Vec3(0.0f, 1.0f, 0.0f)));
	eye2.addComponent<TextureComponent>().LoadTexture("textures/evilEye.jpg");

	shader.addComponent<newShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	tesShader.addComponent<newShaderComponent>(nullptr, "shaders/tessTexturePhoneVert.glsl", "shaders/tessTexturePhoneFrag.glsl", "shaders/tessTextureCtrl.glsl", "shaders/tessTextureEval.glsl");
	tesShader.addComponent<TextureComponent>().LoadTexture("textures/Depth/dpeth.png");
	if (shader.getComponent<newShaderComponent>().OnCreate() == false)
	{
		cout << "Shader failed ... we have a problem\n";
	}
	if (tesShader.getComponent<newShaderComponent>().OnCreate() == false)
	{
		cout << "Shader failed ... we have a problem\n";
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

void TestScene::OnDestroy()
{
	Debug::Info("Deleting assets TestScene: ", __FILE__, __LINE__);

	skull.getComponent<newMeshComponent>().OnDestroy();
	skull.OnDestroy();
	delete &skull;

	eye.getComponent<newMeshComponent>().OnDestroy();
	eye.OnDestroy();
	delete &eye;

	eye2.getComponent<newMeshComponent>().OnDestroy();
	eye2.OnDestroy();
	delete &eye2;

	shader.getComponent<newShaderComponent>().OnDestroy();
	shader.OnDestroy();
	delete &shader;

    delete Sky;

    delete skullBody;
    delete eyeBody;


	manager.clearEntities();
    entityMap.clear();


	glDeleteFramebuffers(1, &framebuffer);
    glDeleteRenderbuffers(1, &rboId);
    glDeleteTextures(1, &texture);

	// delete &trackball;
}

void TestScene::HandleEvents(const SDL_Event &sdlEvent)
{
	ImGui_ImplSDL2_ProcessEvent(&sdlEvent); 
	ImGuiIO &io = ImGui::GetIO();

	bool isClickingImGuiWindow = io.WantCaptureMouse;

	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	// Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
   //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	if (prevZoom != Zoom)
	{
		prevZoom = Zoom;
		ZoomAmount = initEyeZ + Zoom;
		cam.SetZoom(ZoomAmount);
		Sky->setZoom(ZoomAmount);
	}

	if (isClickingImGuiWindow && !isGameWindowFocused)
	{
		return;
	}

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

void TestScene::Update(const float deltaTime)
{
	if (IsPaused)
	{
		return;
	}

	HandleTheGUI();
	dt = deltaTime;

	if (ZoomAmount <= 0)
	{
		ZoomAmount = 0.1f;
	}

	static float angle = 0.0f;
	angle += 30.0f * deltaTime;

	Quaternion q = trackball.getQuat();
	Sky->setTrackball(q);

	skullMatrix = MMath::toMatrix4(q) * skull.getComponent<newTransformComponent>().transform();
	lEyeMatrix = skullMatrix * eye.getComponent<newTransformComponent>().transform();
	rEyeMatrix = skullMatrix * eye2.getComponent<newTransformComponent>().transform();
	planeMatrix = MMath::toMatrix4(q) * plane.getComponent<newTransformComponent>().transform();
}

void TestScene::HandleTheGUI()
{

	bool open = true;
	//ImGui::Begin("Frame rate", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);
	//ImGui::SetWindowPos("Frame rate", ImVec2(0, 0));
	//ImGui::Text("Zoom: %.1f ", Zoom);
	//static float values[90] = {};
	//static int values_offset = 0;
	//static double refresh_time = 0.0;
	//while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
	//{
	//	static float phase = 0.0f;
	//	values[values_offset] = ImGui::GetIO().Framerate;
	//	values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
	//	phase += 0.10f * values_offset;
	//	refresh_time += 1.0f / 60.0f;
	//}
//
	//ImGui::Text("%.1f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	//float average = 0.0f;
	//for (int n = 0; n < IM_ARRAYSIZE(values); n++)
	//	average += values[n];
	//average /= (float)IM_ARRAYSIZE(values);
	//char overlay[32];
	//sprintf_s(overlay, "avg fps %f", average);
	//ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 80.0f, ImVec2(0, 80.0f));
	//ImGui::End();

	//ImGui::Begin("Name", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
	//ImGui::Text("ZOE WAS HERE");
	//// ImGui::Text("SAMPLE BUTTONS");
	//static int clicked = 0;
	//if (ImGui::Button("Button"))
	//	clicked++;
	//if (clicked & 1)
	//{
	//	ImGui::SameLine();
	//	ImGui::Text("Thanks for clicking me!");
	//}
	//for (int i = 0; i < 7; i++)
	//{
	//	if (i > 0)
	//		ImGui::SameLine();
	//	ImGui::PushID(i);
	//	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
	//	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
	//	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
	//	ImGui::Button("Click");
	//	ImGui::PopStyleColor(3);
	//	ImGui::PopID();
	//}
	//ImGui::End();
	ImGui::Begin("SceneManager", nullptr);
	ImGui::SliderFloat("Zoom amount ", &Zoom, -10.0f, 10.0f, "%.2f");
	ImGui::SliderFloat("Tess Level ", &tesLvl, -10.0f, 10.0f, "%.2f");
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Game Objects"))
	{
		

		for (int i = 0; i < manager.getEntities().size(); ++i)
		{

			ImGui::Checkbox(manager.getEntities()[i]->getName().c_str(), &manager.getEntities()[i]->active);
			if (ImGui::IsItemHovered()) {ImGui::SetTooltip("ID: %d", manager.getEntities()[i]->getID());}
		}
		ImGui::TreePop();
		// ImGui::Separator();
		
	}
	ImGui::End();




	ImGui::ShowDemoWindow();

	//::Begin("Background color");
	// ImGui::ColorEdit3("background color", backGroundColor);
	// ImGui::End();
}

void TestScene::Render() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, textureWidth, textureHeight);

	ImGuiWindowFlags windowFlags = isTitleBarHovered ? ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse : ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse;

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

	Sky->Render(cam);
	glActiveTexture(GL_TEXTURE0);

	shader.active ? glUseProgram(shader.getComponent<newShaderComponent>().GetProgram()) : void();
	glBindTexture(GL_TEXTURE_2D, skull.getComponent<TextureComponent>().getTextureID());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("projectionMatrix"), 1, GL_FALSE, cam.GetProjectionMatrix());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("viewMatrix"), 1, GL_FALSE, cam.GetVeiwMatrix());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, skullMatrix);
	skull.active ? skull.getComponent<newMeshComponent>().Render(GL_TRIANGLES) : void();

	glUniform3fv(shader.getComponent<newShaderComponent>().GetUniformID("LightPos[0]"), 3, Lights[0]);

	glUniform4fv(shader.getComponent<newShaderComponent>().GetUniformID("ks[0]"), 3, ks[0]);
	glUniform4fv(shader.getComponent<newShaderComponent>().GetUniformID("kd[0]"), 3, kd[0]);

	glBindTexture(GL_TEXTURE_2D, eye.getComponent<TextureComponent>().getTextureID());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, lEyeMatrix);
	eye.active ? eye.getComponent<newMeshComponent>().Render(GL_TRIANGLES) : void();

	glBindTexture(GL_TEXTURE_2D, eye2.getComponent<TextureComponent>().getTextureID());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, rEyeMatrix);
	eye2.active ? eye2.getComponent<newMeshComponent>().Render(GL_TRIANGLES) : void();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, tesShader.getComponent<TextureComponent>().getTextureID());
	tesShader.active ? glUseProgram(tesShader.getComponent<newShaderComponent>().GetProgram()) : void();
	glUniformMatrix4fv(tesShader.getComponent<newShaderComponent>().GetUniformID("projectionMatrix"), 1, GL_FALSE, cam.GetProjectionMatrix());
	glUniformMatrix4fv(tesShader.getComponent<newShaderComponent>().GetUniformID("viewMatrix"), 1, GL_FALSE, cam.GetVeiwMatrix());
	glUniformMatrix4fv(tesShader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, planeMatrix);
	glUniform1f(tesShader.getComponent<newShaderComponent>().GetUniformID("tessLevelOuter"), 10.0f);
	glUniform1f(tesShader.getComponent<newShaderComponent>().GetUniformID("ppLevel"), tesLvl);
	glUniform1f(tesShader.getComponent<newShaderComponent>().GetUniformID("tessLevelInner"), 10.0f);
	plane.active ? plane.getComponent<newMeshComponent>().Render(GL_PATCHES) : void();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui::Begin("Game", nullptr, windowFlags);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	isTitleBarHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);
	isGameWindowFocused = ImGui::IsWindowFocused();

	float aspectRatio = static_cast<float>(textureWidth) / static_cast<float>(textureHeight);
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 scaledTexture;

	// Calculate scaled dimensions based on aspect ratio
	if (windowSize.x / aspectRatio <= windowSize.y)
	{
		scaledTexture.x = windowSize.x;
		scaledTexture.y = windowSize.x / aspectRatio;
	}
	else
	{
		scaledTexture.y = windowSize.y;
		scaledTexture.x = windowSize.y * aspectRatio;
	}

	// Center the image if there's empty space
	ImVec2 imagePos = ImVec2((windowSize.x - scaledTexture.x) * 0.5f, (windowSize.y - scaledTexture.y) * 0.5f);

	// Display the image with calculated dimensions
	ImGui::SetCursorPos(imagePos);
	ImGui::Image((void *)(intptr_t)texture, scaledTexture, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
}

void TestScene::CreateFBO()
{

	glGenRenderbuffers(1, &rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, textureWidth, textureHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	// magic sauce :>
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,	   // 1. fbo target: GL_FRAMEBUFFER
							  GL_DEPTH_ATTACHMENT, // 2. attachment point
							  GL_RENDERBUFFER,	   // 3. rbo target: GL_RENDERBUFFER
							  rboId);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		cerr << "Framebuffer is not complete!" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
