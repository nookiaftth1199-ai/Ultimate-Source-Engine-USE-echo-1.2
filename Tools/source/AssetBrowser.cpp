#include "AssetBrowser.h"
#include "ImGuiFileDialog.h" // optional, we'll use simple file list

AssetBrowser::AssetBrowser() : m_showFileDialog(false) {
    m_currentDirectory = "./ASSETS/Game";
    ScanAssets(m_currentDirectory);
}

void AssetBrowser::ScanAssets(const std::string& directory) {
    m_assets.clear();
    // Use engine's FileSystem to list files (simplified: use FindFirstFile on Windows)
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA((directory + "/*").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            std::string name = fd.cFileName;
            if (name == "." || name == "..") continue;
            std::string fullPath = directory + "/" + name;
            AssetEntry entry;
            entry.name = name;
            entry.path = fullPath;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                entry.type = "directory";
            } else {
                std::string ext = name.substr(name.find_last_of('.') + 1);
                if (ext == "fbx" || ext == "gltf" || ext == "obj") entry.type = "model";
                else if (ext == "png" || ext == "jpg" || ext == "tga") entry.type = "texture";
                else if (ext == "json") entry.type = "material";
                else if (ext == "wav" || ext == "ogg") entry.type = "audio";
                else if (ext == "lua" || ext == "py") entry.type = "script";
                else entry.type = "other";
            }
            m_assets.push_back(entry);
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
}

void AssetBrowser::Draw() {
    ImGui::Begin("Asset Browser");
    ImGui::Text("Current: %s", m_currentDirectory.c_str());
    if (ImGui::Button("Up")) {
        size_t pos = m_currentDirectory.find_last_of("/\\");
        if (pos != std::string::npos) {
            m_currentDirectory = m_currentDirectory.substr(0, pos);
            ScanAssets(m_currentDirectory);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Refresh")) ScanAssets(m_currentDirectory);

    DrawFileList();

    // Preview selected asset
    if (!m_selectedAsset.empty()) {
        ImGui::Separator();
        ImGui::Text("Preview: %s", m_selectedAsset.c_str());
        // Simple preview based on type
        // For models/textures you could load and display using engine API
    }

    ImGui::End();
}

void AssetBrowser::DrawFileList() {
    ImGui::Columns(2, "assetcols");
    ImGui::Text("Name"); ImGui::NextColumn();
    ImGui::Text("Type"); ImGui::NextColumn();
    ImGui::Separator();

    for (auto& asset : m_assets) {
        ImGui::PushID(asset.name.c_str());
        if (ImGui::Selectable(asset.name.c_str(), m_selectedAsset == asset.path, ImGuiSelectableFlags_SpanAllColumns)) {
            m_selectedAsset = asset.path;
            if (asset.type == "directory") {
                m_currentDirectory = asset.path;
                ScanAssets(m_currentDirectory);
            }
        }
        ImGui::NextColumn();
        ImGui::Text("%s", asset.type.c_str()); ImGui::NextColumn();
        ImGui::PopID();
    }
    ImGui::Columns(1);
}