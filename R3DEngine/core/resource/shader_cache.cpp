//
// Created by Administrator on 2022/9/20.
//

#include "shader_cache.h"
extern std::string SHADER_DIR;
namespace R3D {
    void ShaderCache::Init() {
        Shader phone;
        phone.loadShader(SHADER_DIR + "phone/vert.glsl", SHADER_DIR + "phone/frag.glsl");
        m_shaders.insert({"phone", phone});
        Shader metalpbr;
        metalpbr.loadShader(SHADER_DIR + "metalpbr/vert.glsl", SHADER_DIR + "metalpbr/frag.glsl");
        m_shaders.insert({"metalpbr", metalpbr});
        Shader green;
        green.loadShader(SHADER_DIR + "green/vert.glsl", SHADER_DIR + "green/frag.glsl");
        m_shaders.insert({"green", green});
    }
    Shader ShaderCache::GetShader(string in_shadertype) {
        if (m_shaders.find(in_shadertype) == m_shaders.end()) {
            std::cout << in_shadertype << " shader is not find" << std::endl;
            return m_shaders["phone"];
        } else {
            return m_shaders[in_shadertype];
        }
    }
}