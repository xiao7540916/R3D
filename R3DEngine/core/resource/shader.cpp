//
// Created by Administrator on 2022/9/15.
//

#include "shader.h"
namespace R3D {
    Shader::Shader() {}
    Shader::Shader(const string &vertexPath, const string &fragmentPath) {
        // 1.从路径中获取顶点，片元着色器源代码
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        //确保 ifstream 对象可以抛出异常：
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            //打开文件
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            //将文件读入流
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            //关闭文件句柄
            vShaderFile.close();
            fShaderFile.close();
            //将流转换为string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        //获得着色器字符串
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    void Shader::lodShader(vector<ShaderCreateDesc> &shaderCreateDescs) {
        size_t shadercount = shaderCreateDescs.size();
        // 1.从路径中获取顶点，片元着色器源代码
        vector<string> shadercode(shadercount);
        vector<std::ifstream> shaderfile(shadercount);
        for (int i = 0;i < shadercount;++i) {
            shaderfile[i].exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                //打开文件
                shaderfile[i].open(shaderCreateDescs[i].shaderpath);
                std::stringstream shaderStream;
                //将文件读入流
                shaderStream << shaderfile[i].rdbuf();
                //关闭文件句柄
                shaderfile[i].close();
                //将流转换为string
                shadercode[i] = shaderStream.str();
            }
            catch (std::ifstream::failure e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            }
        }
        vector<unsigned int> shaderid(shadercount);
        for (int i = 0;i < shadercount;++i) {
            const char *shaderCode = shadercode[i].c_str();
            shaderid[i] = glCreateShader(shaderCreateDescs[i].shaderstage);
            glShaderSource(shaderid[i], 1, &shaderCode, nullptr);
            glCompileShader(shaderid[i]);
            checkCompileErrors(shaderid[i], "");
        }
        // shader Program
        ID = glCreateProgram();
        for (int i = 0;i < shadercount;++i) {
            glAttachShader(ID, shaderid[i]);
        }
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        //删除shader
        for (int i = 0;i < shadercount;++i) {
            glDeleteShader(shaderid[i]);
        }
    }
//绑定uniform块,uniformBlockIndex指着色器中的uniform块绑定值，uniformBlockBinding指opengl状态机中的值
    void Shader::bindUniformBlock(uint32_t uniformBlockIndex, uint32_t uniformBlockBinding) {
        glUniformBlockBinding(ID, uniformBlockIndex, uniformBlockBinding);
    }
    void Shader::bindUniformBlock(const std::string &name, uint32_t uniformBlockBinding) {
        glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name.c_str()), uniformBlockBinding);
    }
// activate the shader
// ------------------------------------------------------------------------
    void Shader::use() {
        glUseProgram(ID);
    }
// utility uniform functions
// ------------------------------------------------------------------------
    void Shader::setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
    }
// ------------------------------------------------------------------------
    void Shader::setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
// ------------------------------------------------------------------------
    void Shader::setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
//--------------------------------------------------------------------------
    void Shader::setMat4(const std::string &name, float value[]) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
    }
    void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
// ------------------------------------------------------------------------
    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
// ------------------------------------------------------------------------
    void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec4(const std::string &name, float x, float y, float z, float w) {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
// ------------------------------------------------------------------------
    void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
// ------------------------------------------------------------------------
    void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
// ------------------------------------------------------------------------
    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
//通过ID直接设置uniform
// utility uniform functions
// ------------------------------------------------------------------------
    void Shader::setBool(uint32_t uniformID, bool value) const {
        glUniform1i(uniformID, (int) value);
    }
// ------------------------------------------------------------------------
    void Shader::setInt(uint32_t uniformID, int value) const {
        glUniform1i(uniformID, value);
    }
// ------------------------------------------------------------------------
    void Shader::setFloat(uint32_t uniformID, float value) const {
        glUniform1f(uniformID, value);
    }
//--------------------------------------------------------------------------
    void Shader::setMat4(uint32_t uniformID, float value[]) const {
        glUniformMatrix4fv(uniformID, 1, GL_FALSE, value);
    }
    void Shader::setVec2(uint32_t uniformID, const glm::vec2 &value) const {
        glUniform2fv(uniformID, 1, &value[0]);
    }
    void Shader::setVec2(uint32_t uniformID, float x, float y) const {
        glUniform2f(uniformID, x, y);
    }
// ------------------------------------------------------------------------
    void Shader::setVec3(uint32_t uniformID, const glm::vec3 &value) const {
        glUniform3fv(uniformID, 1, &value[0]);
    }
    void Shader::setVec3(uint32_t uniformID, float x, float y, float z) const {
        glUniform3f(uniformID, x, y, z);
    }
// ------------------------------------------------------------------------
    void Shader::setVec4(uint32_t uniformID, const glm::vec4 &value) const {
        glUniform4fv(uniformID, 1, &value[0]);
    }
    void Shader::setVec4(uint32_t uniformID, float x, float y, float z, float w) {
        glUniform4f(uniformID, x, y, z, w);
    }
// ------------------------------------------------------------------------
    void Shader::setMat2(uint32_t uniformID, const glm::mat2 &mat) const {
        glUniformMatrix2fv(uniformID, 1, GL_FALSE, &mat[0][0]);
    }
// ------------------------------------------------------------------------
    void Shader::setMat3(uint32_t uniformID, const glm::mat3 &mat) const {
        glUniformMatrix3fv(uniformID, 1, GL_FALSE, &mat[0][0]);
    }
// ------------------------------------------------------------------------
    void Shader::setMat4(uint32_t uniformID, const glm::mat4 &mat) const {
        glUniformMatrix4fv(uniformID, 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
    Shader::~Shader() {
        if (ID != 0) {
            glDeleteProgram(ID);
        }
    }
}