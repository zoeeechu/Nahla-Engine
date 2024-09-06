// Moddified by: Zoe Somji
// Date: 2024

#include "ECS.h"
#include "ECSComponents.h"

#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene6G.h"
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
#include <unordered_map>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>
#include <windows.h>
#include "filebrowser.h"
// #include "picker.h"

using namespace std;
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define GET_VARIABLE_NAME(Variable) (#Variable)

static string currentScene = "Scene6G";

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
static Entity &normalMap(manager.addEntity("normalMap"));
static Entity &textureMap(manager.addEntity("TextureMap"));
static Entity &heightMap(manager.addEntity("heightMap"));

static Entity &texture2D(manager.addEntity("texture2D"));

static filebrowser fb;

Scene6G::Scene6G() : drawInWireMode(false), Skulltexture(nullptr), Eyetexture(nullptr)
{
	Debug::Info("Created Scene6G: ", __FILE__, __LINE__);
}

Scene6G::~Scene6G()
{
	Debug::Info("Deleted Scene6G: ", __FILE__, __LINE__);
}

bool Scene6G::OnCreate()
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

	Debug::Info("Loading assets Scene6G: ", __FILE__, __LINE__);

	lightPos = Vec3(10.0f, 0.0f, 0.0f);

	skullBody = new Body();
	eyeBody = new Body();

	plane.addComponent<newMeshComponent>(nullptr, "meshes/Plane.obj");
	plane.getComponent<newMeshComponent>().OnCreate();
	plane.addComponent<newTransformComponent>(Vec3(0.0f, 0.0f, 2.8f), Vec3(1.0f, 1.0f, 1.0f), Quaternion(-90.0f, Vec3(1.0f, 0.0f, 0.0f)));

	skull.addComponent<newMeshComponent>(nullptr, "meshes/Skull.obj");
	skull.getComponent<newMeshComponent>().OnCreate();
	skull.addComponent<newTransformComponent>(Vec3(0.0f, 0.0f, 3.0f), Vec3(0.15f, 0.15f, 0.15f), Quaternion(1.0f, Vec3(0.0f, 1.0f, 0.0f)));
	skull.addComponent<TextureComponent>().LoadTexture("textures/skull_texture.jpg");

	eye.addComponent<newMeshComponent>(nullptr, "meshes/sphere.obj");
	eye.getComponent<newMeshComponent>().OnCreate();
	eye.addComponent<newTransformComponent>(Vec3(1.0f, 0.2f, 0.6f), Vec3(0.28f, 0.28f, 0.28f), Quaternion(-90.0f, Vec3(0.0f, 1.0f, 0.0f)));
	eye.addComponent<TextureComponent>().LoadTexture("textures/evilEye.jpg");

	eye2.addComponent<newMeshComponent>(nullptr, "meshes/sphere.obj");
	eye2.getComponent<newMeshComponent>().OnCreate();
	eye2.addComponent<newTransformComponent>(Vec3(1.0f, 0.2f, -0.6f), Vec3(0.28f, 0.28f, 0.28f), Quaternion(-90.0f, Vec3(0.0f, 1.0f, 0.0f)));
	eye2.addComponent<TextureComponent>().LoadTexture("textures/evilEye.jpg");

	shader.addComponent<newShaderComponent>(nullptr, "shaders/FogtexturePhongVert.glsl", "shaders/FogtexturePhongFrag.glsl");
	tesShader.addComponent<newShaderComponent>(nullptr, "shaders/tessTexturePhoneVert.glsl", "shaders/tessTexturePhoneFrag.glsl", "shaders/tessTextureCtrl.glsl", "shaders/tessTextureEval.glsl");

	heightMap.addComponent<TextureComponent>().LoadTexture("textures/Depth/terrainHeight.png");
	textureMap.addComponent<TextureComponent>().LoadTexture("textures/Depth/terrainDiffuse.png");
	normalMap.addComponent<TextureComponent>().LoadTexture("textures/Depth/terrainNormal.png");

	bool ret = texture2D.addComponent<Texture2DComponent>().LoadTexture("textures/nahla.png");
	IM_ASSERT(ret);

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

void Scene6G::OnDestroy()
{
	Debug::Info("Deleting assets Scene6G: ", __FILE__, __LINE__);

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

void Scene6G::HandleEvents(const SDL_Event &sdlEvent)
{
	ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
	ImGuiIO &io = ImGui::GetIO();

	bool isClickingImGuiWindow = io.WantCaptureMouse;

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
		if (sdlEvent.button.button == SDL_BUTTON_LEFT)
		{
			int mouseX = sdlEvent.button.x;
			int mouseY = sdlEvent.button.y;

			// int pickedID = picker.GetPickedObjectID(mouseX, mouseY, textureHeight);
			// int pickedID = GetPickedObjectID(mouseX, mouseY);
			// if (pickedID >= 0 && pickedID < manager.getEntities().size())
			//{
			//	auto pickedEntity = manager.getEntities()[pickedID];
			//	std::cout << "Clicked on entity: " << pickedEntity->getName() << std::endl;
			//}
		}
		break;

	case SDL_MOUSEBUTTONUP:
		break;

	default:
		break;
	}
}

void Scene6G::Update(const float deltaTime)
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

void Scene6G::HandleTheGUI()
{

	bool open = true;

	// ImGui::Begin("Game", nullptr, windowFlags);

	// ImGui::Begin("SceneName", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);
	// ImGui::SetWindowPos("SceneName", ImVec2(0, 15));
	// ImGui::Text("Current Scene: %s ", currentScene.c_str());
	// ImGui::End();
	// ImGui::Begin("Frame rate", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);
	// ImGui::SetWindowPos("Frame rate", ImVec2(0, 0));
	// ImGui::Text("Zoom: %.1f ", Zoom);
	// static float values[90] = {};
	// static int values_offset = 0;
	// static double refresh_time = 0.0;
	// while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
	//{
	//	static float phase = 0.0f;
	//	values[values_offset] = ImGui::GetIO().Framerate;
	//	values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
	//	phase += 0.10f * values_offset;
	//	refresh_time += 1.0f / 60.0f;
	// }
	//
	// ImGui::Text("%.1f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	// float average = 0.0f;
	// for (int n = 0; n < IM_ARRAYSIZE(values); n++)
	//	average += values[n];
	// average /= (float)IM_ARRAYSIZE(values);
	// char overlay[32];
	// sprintf_s(overlay, "avg fps %f", average);
	// ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 80.0f, ImVec2(0, 80.0f));
	// ImGui::End();

	// ImGui::Begin("Name", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
	// ImGui::Text("ZOE WAS HERE");
	//// ImGui::Text("SAMPLE BUTTONS");
	// static int clicked = 0;
	// if (ImGui::Button("Button"))
	//	clicked++;
	// if (clicked & 1)
	//{
	//	ImGui::SameLine();
	//	ImGui::Text("Thanks for clicking me!");
	// }
	// for (int i = 0; i < 7; i++)
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
	// }
	// ImGui::End();
	ImGui::Begin("SceneManager", nullptr);
	ImGui::SliderFloat("ZoomAmount ", &Zoom, -10.0f, 10.0f, "%.2f");
	ImGui::SliderFloat("TessLevel ", &tesLvl, -10.0f, 10.0f, "%.2f");
	ImGui::SliderFloat("fogDensity", &fogDensity, -10.0f, 10.0f, "%.2f");
	ImGui::SliderFloat("fogGradient", &fogGradient, -10.0f, 10.0f, "%.2f");
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Game Objects"))
	{

		for (int i = 0; i < manager.getEntities().size(); ++i)
		{

			ImGui::Checkbox(manager.getEntities()[i]->getName().c_str(), &manager.getEntities()[i]->active);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("ID: %d", manager.getEntities()[i]->getID());
			}
		}
		ImGui::TreePop();
		// ImGui::Separator();
	}
	ImGui::End();

	fb.ShowFileBrowser(".");

	ImGui::Begin("Image Render Test");

	Texture2DComponent &textureComponent = texture2D.getComponent<Texture2DComponent>();
	int imageWidth = textureComponent.getImageWidth();
	int imageHeight = textureComponent.getImageHeight();
	GLuint my_image_texture = textureComponent.getTexture();

	float aspectRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);

	ImVec2 windowSize = ImGui::GetWindowSize();

	ImVec2 scaledTexture;
	if (windowSize.y * aspectRatio <= windowSize.x)
	{

		scaledTexture.y = windowSize.y;
		scaledTexture.x = windowSize.y * aspectRatio;
	}
	else
	{

		scaledTexture.x = windowSize.x;
		scaledTexture.y = windowSize.x / aspectRatio;
	}

	// Display image
	ImGui::Image((void *)(intptr_t)my_image_texture, scaledTexture);

	ImGui::End();

	ImGui::ShowDemoWindow();

	//::Begin("Background color");
	// ImGui::ColorEdit3("background color", backGroundColor);
	// ImGui::End();
}

void Scene6G::Render() const
{


	// object picker test -zoe
    //GLint depth;
  	//glGetIntegerv (GL_MODELVIEW_STACK_DEPTH, &depth);
    //int capacity = manager.getEntities().size() * 4 * depth;
	//glBindBuffer(GL_UNIFORM_BUFFER, capacity);
	//glSelectBuffer(capacity, nullptr);
    //glRenderMode(GL_SELECT);
//
//
	//glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
   //
    ////Restrict region to pick object only in this region
    //gluPickMatrix();    //x, y, width, height is the picking area
//
    ////Load the projection matrix
    //glMultMatrixf(projection, 0);
	/////////
	



	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, textureWidth, textureHeight);

	ImGuiWindowFlags windowFlags = isTitleBarHovered ? ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse : ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse;

	glClearColor(0.8f, 0.5f, 0.2f, 0.0f);
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
	glUniform1f(shader.getComponent<newShaderComponent>().GetUniformID("fogDensity"), fogDensity);
	glUniform1f(shader.getComponent<newShaderComponent>().GetUniformID("fogGradient"), fogGradient);
	;

	glBindTexture(GL_TEXTURE_2D, eye.getComponent<TextureComponent>().getTextureID());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, lEyeMatrix);
	eye.active ? eye.getComponent<newMeshComponent>().Render(GL_TRIANGLES) : void();

	glBindTexture(GL_TEXTURE_2D, eye2.getComponent<TextureComponent>().getTextureID());
	glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, rEyeMatrix);
	eye2.active ? eye2.getComponent<newMeshComponent>().Render(GL_TRIANGLES) : void();

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	heightMap.active ? glActiveTexture(GL_TEXTURE0) : void();
	heightMap.active ? glBindTexture(GL_TEXTURE_2D, heightMap.getComponent<TextureComponent>().getTextureID()) : void();

	normalMap.active ? glActiveTexture(GL_TEXTURE1) : void();
	normalMap.active ? glBindTexture(GL_TEXTURE_2D, normalMap.getComponent<TextureComponent>().getTextureID()) : void();

	textureMap.active ? glActiveTexture(GL_TEXTURE2) : void();
	textureMap.active ? glBindTexture(GL_TEXTURE_2D, textureMap.getComponent<TextureComponent>().getTextureID()) : void();

	tesShader.active ? glUseProgram(tesShader.getComponent<newShaderComponent>().GetProgram()) : void();
	glUniformMatrix4fv(tesShader.getComponent<newShaderComponent>().GetUniformID("projectionMatrix"), 1, GL_FALSE, cam.GetProjectionMatrix());
	glUniformMatrix4fv(tesShader.getComponent<newShaderComponent>().GetUniformID("viewMatrix"), 1, GL_FALSE, cam.GetVeiwMatrix());
	glUniformMatrix4fv(tesShader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, planeMatrix);
	glUniform1f(tesShader.getComponent<newShaderComponent>().GetUniformID("tessLevelOuter"), 10.0f);
	glUniform1f(tesShader.getComponent<newShaderComponent>().GetUniformID("ppLevel"), tesLvl);
	glUniform1f(tesShader.getComponent<newShaderComponent>().GetUniformID("tessLevelInner"), 10.0f);
	glUniform3fv(tesShader.getComponent<newShaderComponent>().GetUniformID("LightsPos[0]"), 3, Lights[0]);
	glUniform4fv(tesShader.getComponent<newShaderComponent>().GetUniformID("ks[0]"), 3, ks[0]);
	glUniform4fv(tesShader.getComponent<newShaderComponent>().GetUniformID("kd[0]"), 3, kd[0]);
	glUniform1f(tesShader.getComponent<newShaderComponent>().GetUniformID("fogDensity"), fogDensity);
	glUniform1f(tesShader.getComponent<newShaderComponent>().GetUniformID("fogGradient"), fogGradient);
	plane.active ? plane.getComponent<newMeshComponent>().Render(GL_PATCHES) : void();

	heightMap.active ? glActiveTexture(GL_TEXTURE0) : void();
	heightMap.active ? glBindTexture(GL_TEXTURE_2D, 0) : void();
	normalMap.active ? glActiveTexture(GL_TEXTURE1) : void();
	normalMap.active ? glBindTexture(GL_TEXTURE_2D, 0) : void();
	textureMap.active ? glActiveTexture(GL_TEXTURE2) : void();
	textureMap.active ? glBindTexture(GL_TEXTURE_2D, 0) : void();
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

void Scene6G::CreateFBO()
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

void Scene6G::DepthBufferFBO()
{
	GLuint fbo, idTexture;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create a texture for storing object IDs
	glGenTextures(1, &idTexture);
	glBindTexture(GL_TEXTURE_2D, idTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, textureWidth, textureHeight, 0, GL_RED_INTEGER, GL_INT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, idTexture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Scene6G::renderObjectWithID(Entity *object) {
    GLuint id = object->getID();
	glUniform1i(object->getComponent<newShaderComponent>().GetUniformID("objectID"), id);


    // Render the object here (e.g., using object->render())
}



