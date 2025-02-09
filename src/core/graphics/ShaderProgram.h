#pragma once
// standard lib
#include <cstdint>
#include <string>
// third party
#include <glm/glm.hpp>

class ShaderProgram {
public:

    struct ShaderCreateInfo {
        enum class Type : uint8_t {
            VERTEX,
            TESSELLATION_CONTROL,
            TESSELLATION_EVALUATION,
            GEOMETRY,
            FRAGMENT,
            COMPUTE
        } type;
        const char* sourceData;
        size_t sourceSize;
    };


    ShaderProgram();

    ~ShaderProgram();

    void addShader(const ShaderCreateInfo& shaderInfo);

    void linkProgram();

    void bind() const;

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    void setTexture(const std::string& uniformName, unsigned int textureId, unsigned int textureUnit) const;

    /** Get the shader program Id*/
    unsigned int getProgramId() const;

private:
    unsigned int mProgramId_;


};