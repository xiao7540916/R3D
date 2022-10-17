//
// Created by Administrator on 2022/9/15.
//

#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
namespace R3D {
    using std::string;
    using std::vector;
    struct ShaderCreateDesc {
        std::string shaderpath;
        GLenum shaderstage;
    };
    class Shader {
    public:
        GLuint ID;
        ~Shader();
        Shader();
        void Release();
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        Shader(const string &vertexPath, const string &fragmentPath);
        void loadShader(vector<ShaderCreateDesc> &shaderCreateDescs);
        void loadShader(const string &vertexPath, const string &fragmentPath);
        void loadCompShader(const string &compPath);
        //绑定uniform块,uniformBlockIndex指着色器中的uniform块绑定值，uniformBlockBinding指opengl状态机中的值
        void bindUniformBlock(uint32_t uniformBlockIndex, uint32_t uniformBlockBinding);
        void bindUniformBlock(const std::string &name, uint32_t uniformBlockBinding);
        // activate the shader
        // ------------------------------------------------------------------------
        void use();
        // utility uniform functions
        // ------------------------------------------------------------------------
        void setBool(const std::string &name, bool value) const;
        // ------------------------------------------------------------------------
        void setInt(const std::string &name, int value) const;
        // ------------------------------------------------------------------------
        void setFloat(const std::string &name, float value) const;
        //--------------------------------------------------------------------------
        void setMat4(const std::string &name, float value[]) const;
        void setVec2(const std::string &name, const glm::vec2 &value) const;
        void setVec2(const std::string &name, float x, float y) const;
        // ------------------------------------------------------------------------
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec3(const std::string &name, float x, float y, float z) const;
        // ------------------------------------------------------------------------
        void setVec4(const std::string &name, const glm::vec4 &value) const;
        void setVec4(const std::string &name, float x, float y, float z, float w);
        // ------------------------------------------------------------------------
        void setMat2(const std::string &name, const glm::mat2 &mat) const;
        // ------------------------------------------------------------------------
        void setMat3(const std::string &name, const glm::mat3 &mat) const;
        // ------------------------------------------------------------------------
        void setMat4(const std::string &name, const glm::mat4 &mat) const;
        //通过ID直接设置uniform
        // utility uniform functions
        // ------------------------------------------------------------------------
        void setBool(uint32_t uniformID, bool value) const;
        // ------------------------------------------------------------------------
        void setInt(uint32_t uniformID, int value) const;
        // ------------------------------------------------------------------------
        void setFloat(uint32_t uniformID, float value) const;
        //--------------------------------------------------------------------------
        void setMat4(uint32_t uniformID, float value[]) const;
        void setVec2(uint32_t uniformID, const glm::vec2 &value) const;
        void setVec2(uint32_t uniformID, float x, float y) const;
        // ------------------------------------------------------------------------
        void setVec3(uint32_t uniformID, const glm::vec3 &value) const;
        void setVec3(uint32_t uniformID, float x, float y, float z) const;
        // ------------------------------------------------------------------------
        void setVec4(uint32_t uniformID, const glm::vec4 &value) const;
        void setVec4(uint32_t uniformID, float x, float y, float z, float w);
        // ------------------------------------------------------------------------
        void setMat2(uint32_t uniformID, const glm::mat2 &mat) const;
        // ------------------------------------------------------------------------
        void setMat3(uint32_t uniformID, const glm::mat3 &mat) const;
        // ------------------------------------------------------------------------
        void setMat4(uint32_t uniformID, const glm::mat4 &mat) const;
    private:
        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void checkCompileErrors(unsigned int shader, std::string type);
    };
}



