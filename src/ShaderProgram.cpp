#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
// program
#include "ShaderProgram.h"
#include <stdexcept>


ShaderProgram::ShaderProgram() {
    mProgramId_ = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    // todo delete
}

void ShaderProgram::addShader(const ShaderCreateInfo& shaderInfo) {
    GLenum glType;

    switch (shaderInfo.type) {
        case ShaderCreateInfo::Type::VERTEX:
            glType = GL_VERTEX_SHADER;
            break;
        case ShaderCreateInfo::Type::TESSELLATION_CONTROL:
            glType = GL_TESS_CONTROL_SHADER;
            break;
        case ShaderCreateInfo::Type::TESSELLATION_EVALUATION:
            glType = GL_TESS_EVALUATION_SHADER;
            break;
        case ShaderCreateInfo::Type::GEOMETRY:
            glType = GL_GEOMETRY_SHADER;
            break;
        case ShaderCreateInfo::Type::FRAGMENT:
            glType = GL_FRAGMENT_SHADER;
            break;
        case ShaderCreateInfo::Type::COMPUTE:
            glType = GL_COMPUTE_SHADER;
            break;
        default:
            throw std::runtime_error("Invalid Shader Type");
    }
    unsigned int shaderID = glCreateShader(glType);
    const char* sourceCStr = shaderInfo.sourceData;

    glShaderSource(shaderID, 1, &sourceCStr, nullptr);
    glCompileShader(shaderID);
    glAttachShader(mProgramId_, shaderID);
}

void ShaderProgram::linkProgram() {
    // todo call     glDeleteShader(shader);
    glLinkProgram(mProgramId_);

}

void ShaderProgram::bind() const {
    glUseProgram(mProgramId_); 
}

// utility uniform functions
void ShaderProgram::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(mProgramId_, name.c_str()), (int)value); 
}

void ShaderProgram::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(mProgramId_, name.c_str()), value); 

}

void ShaderProgram::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(mProgramId_, name.c_str()), value); 

}

void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, &value[0]); 

}

void ShaderProgram::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(mProgramId_, name.c_str()), x, y); 

}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, &value[0]); 

}

void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(mProgramId_, name.c_str()), x, y, z); 

}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, &value[0]); 

}

void ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(mProgramId_, name.c_str()), x, y, z, w); 

}

void ShaderProgram::setMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

}

void ShaderProgram::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(mProgramId_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

}

void ShaderProgram::setTexture(const std::string& uniformName, unsigned int textureId, unsigned int textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureId);

    setInt(uniformName, textureUnit);
}

/** Get the shader program Id*/
unsigned int ShaderProgram::getProgramId() const {
    return mProgramId_;
}