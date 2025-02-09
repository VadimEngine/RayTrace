#pragma once
// standard lib
#include <string>
#include <optional>
// Project
#include "core/graphics/ShaderProgram.h"
#include "core/graphics/Texture.h"
#include "core/Utils.h"


class Resources {
public:
    struct ResourceInfo {
        std::string path;
        std::optional<ShaderProgram::ShaderCreateInfo::Type> shaderType; 
    };

    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> mShaders_;

    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures_;

    FileData loadFileToMemory(const std::string& filePath);

    template<typename T>
    void loadResource(const std::vector<ResourceInfo>& resourceInfo, const std::string& resourceName);

    template<typename T>
    T* getResource(const std::string& resourceName);
};
