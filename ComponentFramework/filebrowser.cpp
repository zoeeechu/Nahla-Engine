// https://github.com/TheCherno/Hazel/blob/master/Hazelnut/src/Panels/ContentBrowserPanel.cpp

#include "filebrowser.h"


#include "ECS.h"
#include "ECSComponents.h"

#include "imgui/imgui.h"

static Manager manager;
namespace fs = std::filesystem;

static Entity &DirectoryIcon(manager.addEntity("DirectoryIcon"));
static Entity &FileIcon(manager.addEntity("FileIcon"));

filebrowser::filebrowser() : m_BaseDirectory(fs::current_path()), m_CurrentDirectory(m_BaseDirectory) {}
filebrowser::~filebrowser() {}

bool filebrowser::OnCreate()
{
    return true;
}

void filebrowser::OpenFileWithDefaultProgram(const std::string &filename)
{
    ShellExecuteA(nullptr, "open", filename.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void filebrowser::ShowFileBrowser(const std::string &directory)
{

    
    bool ret = DirectoryIcon.addComponent<Texture2DComponent>().LoadTexture("textures/DirectoryIcon.png");
    IM_ASSERT(ret);

    ret = FileIcon.addComponent<Texture2DComponent>().LoadTexture("textures/FileIcon.png");
    IM_ASSERT(ret);


    ImGui::Begin("Files Browser");

    if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
    {
        if (ImGui::Button("<-"))
        {
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }
    }

    static float padding = 16.0f;
    static float thumbnailSize = 70.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
    {
        const auto &path = directoryEntry.path();
        std::string filename = path.filename().string();
        std::string ext = filename.length() > 11 ? "..." : "";  
        std::string filenameString = filename.substr(0, 9) + ext;

                 
        ImGui::PushID(filenameString.c_str());
        Texture2DComponent &icon = directoryEntry.is_directory() ? DirectoryIcon.getComponent<Texture2DComponent>() : FileIcon.getComponent<Texture2DComponent>();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::ImageButton((ImTextureID)icon.getTexture(), {thumbnailSize, thumbnailSize});

        if (ImGui::BeginDragDropSource())
        {
            std::filesystem::path relativePath(path);
            const wchar_t *itemPath = relativePath.c_str();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (directoryEntry.is_directory()){m_CurrentDirectory /= path.filename();}
            if (directoryEntry.is_regular_file()){OpenFileWithDefaultProgram(path.string());}
            
        }
        ImGui::TextWrapped(filenameString.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    //ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    //ImGui::SliderFloat("Padding", &padding, 0, 32);

    // TODO: status bar
    ImGui::End();

   
}

void filebrowser::OnDestroy()
{
}
void filebrowser::Update(const float deltaTime)
{
}

void filebrowser::Render() const
{
}