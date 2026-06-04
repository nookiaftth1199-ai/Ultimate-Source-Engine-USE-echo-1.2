#pragma once
class AssetCompiler {
public:
    void CompileAssets(const char* sourceDir, const char* outputDir) {
        // Recursively process source directory
        ProcessDirectory(sourceDir, outputDir);
    }
    
private:
    void ProcessDirectory(const std::string& source, const std::string& dest) {
        // Convert FBX → .msh
        // Convert PNG → DDS (with mipmaps)
        // Compile materials to .mat (JSON)
        // Package into .pak files
    }
};