// Moddified by: Zoe Somji
// Date: 2024

#include "MainScene.h"

///// these must stay in cpp or else it wont compile -zoe ///////
#include "filebrowser.h"
#include <filesystem>
#include <windows.h>
#include "skybox.h"
/////////////////////////////////////////////////////////////////

using namespace std;
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define GET_VARIABLE_NAME(Variable) (#Variable)

static string currentScene = "MainScene";

static int textureWidth = 1280;
static int textureHeight = 720;

// when your done with the ECS make a asset manager with shared pointers -zoe
template <typename T>
using refPtr = std::shared_ptr<T>;
static unordered_map<refPtr<Entity>, int> entityMap;
static map<uint32_t, int> colorToEntity;

static Manager manager;
//  static bool show_app_dockspace = false;

static Entity &defaultCube(manager.addEntity("defaultCube"));
static Entity &shader(manager.addEntity("shader"));
static Entity &tesShader(manager.addEntity("tesShader"));
static Entity &plane(manager.addEntity("plane"));
static Entity &normalMap(manager.addEntity("normalMap"));
static Entity &textureMap(manager.addEntity("TextureMap"));
static Entity &heightMap(manager.addEntity("heightMap"));

static Entity &texture2D(manager.addEntity("texture2D"));

static filebrowser fb;

MainScene::MainScene() : drawInWireMode(false), cubeTexture(nullptr)
{
    Debug::Info("Created MainScene: ", __FILE__, __LINE__);
}

MainScene::~MainScene()
{
    Debug::Info("Deleted MainScene: ", __FILE__, __LINE__);
}

bool MainScene::OnCreate()
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

    Debug::Info("Loading assets MainScene: ", __FILE__, __LINE__);

    lightPos = Vec3(10.0f, 0.0f, 0.0f);

    cubeBody = new Body();

    plane.addComponent<newMeshComponent>(nullptr, "meshes/Plane.obj");
    plane.getComponent<newMeshComponent>().OnCreate();
    plane.addComponent<newTransformComponent>(Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 1.0f, 1.0f), Quaternion(-90.0f, Vec3(1.0f, 0.0f, 0.0f)));

    defaultCube.addComponent<newMeshComponent>(nullptr, "meshes/Cube.obj");
    defaultCube.getComponent<newMeshComponent>().OnCreate();
    defaultCube.addComponent<newTransformComponent>(Vec3(0.0f, 0.0f, 3.0f), Vec3(0.15f, 0.15f, 0.15f), Quaternion(1.0f, Vec3(0.0f, 1.0f, 0.0f)));
    defaultCube.addComponent<TextureComponent>().LoadTexture("textures/cube_texture.jpg");

    shader.addComponent<newShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
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

    Sky = new skybox("textures/skyBox/defaultSky/negx.jpg", "textures/skyBox/defaultSky/posx.jpg",
                     "textures/skyBox/defaultSky/posy.jpg", "textures/skyBox/defaultSky/negy.jpg",
                     "textures/skyBox/defaultSky/posz.jpg", "textures/skyBox/defaultSky/negz.jpg");
    Sky->OnCreate();

    cam.setVeiw(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f)), Vec3(0.0f, -0.2f, -6.0f));

    AssignUniqueColors();

    return true;
}

void MainScene::OnDestroy()
{
    Debug::Info("Deleting assets MainScene: ", __FILE__, __LINE__);

    defaultCube.getComponent<newMeshComponent>().OnDestroy();
    defaultCube.OnDestroy();
    delete &defaultCube;

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

void MainScene::HandleEvents(const SDL_Event &sdlEvent)
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
        case SDL_SCANCODE_TAB:
            drawInWireMode = !drawInWireMode;
            break;
        case SDL_SCANCODE_P:
            IsPaused = !IsPaused;
            break;
        case SDL_SCANCODE_UP:

            if (IsHoldingShift)
            {
                translationOffset.z += 0.1f;
            }
            else
            {
                translationOffset.y -= 0.1f;
            }

            break;
        case SDL_SCANCODE_DOWN:

            if (IsHoldingShift)
            {
                translationOffset.z -= 0.1f;
            }
            else
            {
                translationOffset.y += 0.1f;
            }

            break;
        case SDL_SCANCODE_LEFT:
            translationOffset.x -= 0.1f;
            break;
        case SDL_SCANCODE_RIGHT:
            translationOffset.x += 0.1f;
            break;
        case SDL_SCANCODE_W:

            if (IsHoldingShift)
            {
                translationOffset.z += 0.1f;
            }
            else
            {
                translationOffset.y -= 0.1f;
            }

            break;
        case SDL_SCANCODE_S:

            if (IsHoldingShift)
            {
                translationOffset.z -= 0.1f;
            }
            else
            {
                translationOffset.y += 0.1f;
            }

            break;
        case SDL_SCANCODE_A:
            translationOffset.x -= 0.1f;
            break;
        case SDL_SCANCODE_D:
            translationOffset.x += 0.1f;
            break;
        case SDL_SCANCODE_LSHIFT:
            IsHoldingShift = true;
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

    case SDL_KEYUP:
        switch (sdlEvent.key.keysym.scancode)
        {
        case SDL_SCANCODE_LSHIFT:
            IsHoldingShift = false;
            break;
        }


        break;

    default:
        break;
    }
}

void MainScene::Update(const float deltaTime)
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

    cubeMatrix = MMath::toMatrix4(q) * MMath::translate(translationOffset) * defaultCube.getComponent<newTransformComponent>().transform();
    planeMatrix = MMath::toMatrix4(q) * MMath::translate(translationOffset) * plane.getComponent<newTransformComponent>().transform();
}

void MainScene::HandleTheGUI()
{

    bool open = true;

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

void MainScene::Render() const
{

    // object picker test -zoe
    // GLint depth;
    // glGetIntegerv (GL_MODELVIEW_STACK_DEPTH, &depth);
    // int capacity = manager.getEntities().size() * 4 * depth;
    // glBindBuffer(GL_UNIFORM_BUFFER, capacity);
    // glSelectBuffer(capacity, nullptr);
    // glRenderMode(GL_SELECT);
    //
    //
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    //
    ////Restrict region to pick object only in this region
    // gluPickMatrix();    //x, y, width, height is the picking area
    //
    ////Load the projection matrix
    // glMultMatrixf(projection, 0);
    /////////

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, textureWidth, textureHeight);

    ImGuiWindowFlags windowFlags = isTitleBarHovered ? ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse : ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);


    drawInWireMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

    glBindTexture(GL_TEXTURE_2D, defaultCube.getComponent<TextureComponent>().getTextureID());
    glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("projectionMatrix"), 1, GL_FALSE, cam.GetProjectionMatrix());
    glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("viewMatrix"), 1, GL_FALSE, cam.GetVeiwMatrix());
    glUniformMatrix4fv(shader.getComponent<newShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, cubeMatrix);
    defaultCube.active ? defaultCube.getComponent<newMeshComponent>().Render(GL_TRIANGLES) : void();

    glUniform3fv(shader.getComponent<newShaderComponent>().GetUniformID("LightPos[0]"), 3, Lights[0]);

    glUniform4fv(shader.getComponent<newShaderComponent>().GetUniformID("ks[0]"), 3, ks[0]);
    glUniform4fv(shader.getComponent<newShaderComponent>().GetUniformID("kd[0]"), 3, kd[0]);
    glUniform1f(shader.getComponent<newShaderComponent>().GetUniformID("fogDensity"), fogDensity);
    glUniform1f(shader.getComponent<newShaderComponent>().GetUniformID("fogGradient"), fogGradient);
    ;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    heightMap.active ? glActiveTexture(GL_TEXTURE0) : void();
    heightMap.active ? glBindTexture(GL_TEXTURE_2D, heightMap.getComponent<TextureComponent>().getTextureID()) : void();

    normalMap.active ? glActiveTexture(GL_TEXTURE1) : void();
    normalMap.active ? glBindTexture(GL_TEXTURE_2D, normalMap.getComponent<TextureComponent>().getTextureID()) : void();

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

void MainScene::CreateFBO()
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
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,      // 1. fbo target: GL_FRAMEBUFFER
                              GL_DEPTH_ATTACHMENT, // 2. attachment point
                              GL_RENDERBUFFER,     // 3. rbo target: GL_RENDERBUFFER
                              rboId);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cerr << "Framebuffer is not complete!" << endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MainScene::DepthBufferFBO()
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

void MainScene::renderObjectWithID(Entity *object)
{
    GLuint id = object->getID();
    glUniform1i(object->getComponent<newShaderComponent>().GetUniformID("objectID"), id);

    // Render the object here (e.g., using object->render())
}

void MainScene::AssignUniqueColors()
{
    size_t numEntities = entityMap.size();
    for (size_t i = 0; i < numEntities; ++i)
    {
        if (!manager.getEntities()[i]->hasComponent<newMeshComponent>())
            continue;

        uint8_t r = ((i + 1) * 123) % 256;
        uint8_t g = ((i + 1) * 321) % 256;
        uint8_t b = ((i + 1) * 213) % 256;

        // Combine r, g, and b into a single colorID
        uint32_t colorID = (r << 16) | (g << 8) | b;

        // Ensure colorID is unique by checking the colorToEntity map
        while (colorToEntity.find(colorID) != colorToEntity.end())
        {
            // If the colorID is already used, adjust the colors slightly
            r = (r + 17) % 256;
            g = (g + 31) % 256;
            b = (b + 29) % 256;
            colorID = (r << 16) | (g << 8) | b;
        }

        // Map the unique colorID to the entity
        colorToEntity[colorID] = i;

        // Set the color on the mesh component
        Vec3 color = Vec3(r / 255.0f, g / 255.0f, b / 255.0f);
        manager.getEntities()[i]->getComponent<newMeshComponent>().setMappedColors(color);

        // Output the entity and its color
        std::cout << "Entity: " << manager.getEntities()[i]->getName().c_str()
                  << " Color: " << int(r) << ", " << int(g) << ", " << int(b)
                  << " (" << std::hex << colorID << ")" << std::endl;
    }
}
