#pragma once
#include <filesystem>
#include <windows.h>
#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_internal.h"
#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <memory>
#include <vector>
#include <stack>

#include "ECS.h"
#include "ECSComponents.h"

union SDL_Event;

class filebrowser
{
public:
	filebrowser();
	~filebrowser();
	void OpenFileWithDefaultProgram(const std::string &filename);
	void ShowFileBrowser(const std::string &directory);
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime);
	void Render() const;

private:
	std::filesystem::path m_BaseDirectory;
	std::filesystem::path m_CurrentDirectory;
};
