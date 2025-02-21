// standard lib
#include <iostream>
#include <fstream>
// third party
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// project
#include "core/application/Resources.h"


FileData Resources::loadFileToMemory(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    auto buffer = std::make_unique<unsigned char[]>(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.get()), fileSize)) {
        throw std::runtime_error("Failed to read file: " + filePath);
    }

    return {std::move(buffer), static_cast<std::size_t>(fileSize)}; 
}

ImageData fileDataToImageData(const FileData& imageFile) {
    ImageData imageData;
    imageData.pixels = stbi_load_from_memory(
        imageFile.data.get(), 
        imageFile.size,
        &imageData.width, 
        &imageData.height, 
        &imageData.channels, 
        0
    );

    if (imageData.pixels == nullptr) {
        const char* errorMessage = stbi_failure_reason();
        std::cout << errorMessage << std::endl;
        throw std::runtime_error("Texture load failed");
    }

    return imageData;
}

template<typename T>
void Resources::loadResource(const std::vector<std::string>& resourceInfo, const std::string& resourceName) {
    if constexpr(std::is_same_v<T, Texture>) {
        FileData fileData = loadFileToMemory(resourceInfo[0]);
        ImageData imageData = fileDataToImageData(fileData);
        auto texture = std::make_unique<Texture>(imageData);
        mTextures_[resourceName] = std::move(texture);
    } else if constexpr (std::is_same_v<T, ShaderProgram>) {
        auto shaderProgram = std::make_unique<ShaderProgram>();

        for (const auto& eachPath: resourceInfo) {
            const size_t lastColon =  eachPath.find_last_of(':');
            const std::string shaderPath = eachPath.substr(0, lastColon);
            const std::string shaderType = eachPath.substr(lastColon + 1);

            FileData fileData = loadFileToMemory(shaderPath);

            ShaderProgram::ShaderCreateInfo::Type shaderTypeEnum;

            if (shaderType == "VERTEX") {
                shaderTypeEnum = ShaderProgram::ShaderCreateInfo::Type::VERTEX;
            } else if (shaderType == "TESSELLATION_CONTROL") {
                shaderTypeEnum = ShaderProgram::ShaderCreateInfo::Type::TESSELLATION_CONTROL;
            } else if (shaderType == "TESSELLATION_EVALUATION") {
                shaderTypeEnum = ShaderProgram::ShaderCreateInfo::Type::TESSELLATION_EVALUATION;
            } else if (shaderType == "GEOMETRY") {
                shaderTypeEnum = ShaderProgram::ShaderCreateInfo::Type::GEOMETRY;
            } else if (shaderType == "FRAGMENT") {
                shaderTypeEnum = ShaderProgram::ShaderCreateInfo::Type::FRAGMENT;
            } else if (shaderType == "COMPUTE") {
                shaderTypeEnum = ShaderProgram::ShaderCreateInfo::Type::COMPUTE;
            } else {
                throw std::runtime_error("Invalid shader path suffix");
            }

            shaderProgram->addShader({
                shaderTypeEnum,
                std::string(reinterpret_cast<char*>(fileData.data.get()), fileData.size).c_str(),
                fileData.size
            });
        }

        shaderProgram->linkProgram();
        mShaders_[resourceName] = std::move(shaderProgram);
    }
}

template<typename T>
T* Resources::getResource(const std::string& resourceName) {
    if constexpr(std::is_same_v<T, Texture>) {
        auto it = mTextures_.find(resourceName);
        if (it != mTextures_.end()) {
            return it->second.get();
        }
    } else if constexpr (std::is_same_v<T, ShaderProgram>) {
        auto it = mShaders_.find(resourceName);
        if (it != mShaders_.end()) {
            return it->second.get();
        }
    }

    // Return nullptr if resource not found or type mismatch
    return nullptr;
}

// Explicit instantiate template for expected types
template void Resources::loadResource<Texture>(const std::vector<std::string>& resourceInfo, const std::string& resourceName);
template void Resources::loadResource<ShaderProgram>(const std::vector<std::string>& resourceInfo, const std::string& resourceName);

template Texture* Resources::getResource(const std::string& resourceName);
template ShaderProgram* Resources::getResource(const std::string& resourceName);

