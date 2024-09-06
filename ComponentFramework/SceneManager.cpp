// Moddified by: Zoe Somji
// Date: 2024-02-29
// Sorry scott!

#include <SDL.h>
#include <stack> // Sorry scott!
#include "SceneManager.h"
#include "Timer.h"
#include "Window.h"
#include "Scene.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

// untill i make a way to choose a scene here are all the scenes -zoe
#include "Scene0.h"
#include "Scene1p.h"
#include "Scene3p.h"
#include "demoScene.h"
#include "Scene2G.h"
#include "Scene3G.h"
#include "Scene1G2.h"
#include "Scene4G.h"
#include "TestScene.h"
#include "Scene5G.h"
#include "Scene6G.h"
#include "Scene6G2.h"
#include "MainScene.h"

////////////////////////////////////////////////////////////////////////

std::stack<Scene *> sceneStack;

Scene *currentScene = nullptr;

extern Scene *scenes[] = {
    new MainScene,
    new Scene6G,
    new Scene3p,
    new Scene6G2, 
    new Scene1p,
    new Scene5G,
    new TestScene,
    new Scene4G, // problem scene since it doesnt use the framebuffer it messes with the sizing of the other scenes
   // to fix we need to use frame buffer in Scene4G just like the other scenes will do when I have time -zoe
    new Scene0,
    new Scene1p,
    new demoScene,
    new Scene2G,
    new Scene3G,
    new Scene1G2,
    //...

};
extern int scenenum = std::max(0, scenenum - 1);

SceneManager::SceneManager() : window{nullptr}, timer{nullptr},
    fps(60), isRunning{false}, fullScreen{false}{}

SceneManager::~SceneManager()
{

    Debug::Info("Deleting the SceneManager", __FILE__, __LINE__);

    if (currentScene)
    {
        currentScene->OnDestroy();
        delete currentScene;
        currentScene = nullptr;
    }
    // Delete timer and window
    if (timer)
    {
        delete timer;
        timer = nullptr;
    }
    if (window)
    {
        delete window;
        window = nullptr;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

bool SceneManager::Initialize(std::string name_, int width_, int height_)
{
    width = width_;
    height = height_;

    window = new Window();
    if (!window->OnCreate(name_, width_, height_))
    {
        Debug::FatalError("Failed to initialize Window object", __FILE__, __LINE__);
        return false;
    }

    timer = new Timer();
    if (timer == nullptr)
    {
        Debug::FatalError("Failed to initialize Timer object", __FILE__, __LINE__);
        return false;
    }

    /********************************   Default first scene   ***********************/
    ChangeScene(scenes[scenenum]);
    /********************************************************************************/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();                         //(void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
   // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows // BREAKS SHIT !!!! - zoe
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleThemed();
    ImGui_ImplSDL2_InitForOpenGL(window->getWindow(), window->getContext());
    ImGui_ImplOpenGL3_Init("#version 450");
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Arial.ttf", 18.0f);
    return true;
}

// void SceneManager::ChangeScene(Scene* newScene) {
//
//     // Delete the current scene if it exists
//     if (currentScene != nullptr) {
//         currentScene->OnDestroy();
//         delete currentScene;
//         currentScene = nullptr;
//     }
//
//     // Set the new scene
//     currentScene = newScene;
//     // Call OnCreate of the new scene
//     currentScene->OnCreate();

void SceneManager::ChangeScene(Scene *newScene)
{   
    // Push the current scene onto the stack
    if (currentScene != nullptr)
    {
        sceneStack.push(currentScene);
    }
    // Set the new scene
    currentScene = newScene;
    // Call OnCreate of the new scene
    currentScene->OnCreate();
}

void SceneManager::GoBack()
{
    // Pop the top scene from the stack if it's not empty
    if (!sceneStack.empty())
    {
        if (currentScene != nullptr)
        {
            currentScene->OnDestroy();
            // delete currentScene;
        }
        currentScene = sceneStack.top();
        sceneStack.pop();
        currentScene->OnCreate();
    }
}

void SceneManager::Run()
{
    timer->Start();
    isRunning = true;
    while (isRunning)
    {
        HandleEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        timer->UpdateFrameTicks();
        currentScene->Update(timer->GetDeltaTime());
        currentScene->Render();
        ImGui::Render();
      //  ImGui::UpdatePlatformWindows();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
         if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault(NULL, (void*)SDL_GL_GetProcAddress);
        }
        SDL_GL_SwapWindow(window->getWindow());
        SDL_Delay(timer->GetSleepTime(fps));
    }
}

void SceneManager::HandleEvents()
{

    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent))
    {
        // Handle SDL events
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        if (sdlEvent.type == SDL_EventType::SDL_QUIT)
        {
            isRunning = false;
            return;
        }
        else if (sdlEvent.type == SDL_KEYDOWN)
        {
            switch (sdlEvent.key.keysym.scancode)
            {
                [[fallthrough]]; /// C17 Prevents switch/case fallthrough warnings
            case SDL_SCANCODE_ESCAPE:
            case SDL_SCANCODE_Q:
                isRunning = false;
                return;
            case SDL_SCANCODE_F1:

                scenenum--;
                // Debug::Info(std::to_string(scenenum), __FILE__, __LINE__);
                // ChangeScene(scenes[scenenum]);
                GoBack();
                break;

            case SDL_SCANCODE_F2:
                if (scenenum < sizeof(scenes) / sizeof(scenes[0]) - 1)
                {
                    scenenum++;
                    Debug::Info(std::to_string(scenenum), __FILE__, __LINE__);
                    ChangeScene(scenes[scenenum]);
                }
                break;

            default:
                break;
            }
        }
        if (currentScene == nullptr)
        { /// Just to be careful
            Debug::FatalError("No currentScene", __FILE__, __LINE__);
            isRunning = false;
            return;
        }
        // Handle other events
        currentScene->HandleEvents(sdlEvent);
    }
}
