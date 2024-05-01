// Modify SceneManager.h

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>

class Scene;
extern Scene* currentScene;
extern Scene* scenes[];

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    void Run();
    bool Initialize(std::string name_, int width_, int height_);
    void HandleEvents();
    void ChangeScene(Scene* newScene);
    void GoBack();
    int getWidth();
    int getHeight();


private:
    class Timer* timer;
    class Window* window;

    int width, height;


    unsigned int fps;
    bool isRunning;
    bool fullScreen;
};

#endif // SCENEMANAGER_H
