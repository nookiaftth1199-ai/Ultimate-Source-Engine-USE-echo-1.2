#pragma once
#include "stdafx.h"

class AssetBrowser {
public:
    AssetBrowser();
    void Draw();

private:
    void ScanAssets(const std::string& directory);
    void DrawFileList();

    struct AssetEntry {
        std::string name;
        std::string path;
        std::string type; // "model", "texture", "material", "audio", "script"
    };
    std::vector<AssetEntry> m_assets;
    std::string m_currentDirectory;
    bool m_showFileDialog;
    std::string m_selectedAsset;
};