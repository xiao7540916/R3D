//
// Created by Administrator on 2022/9/20.
//

#pragma once
#include <resource/shader.h>
#include <unordered_map>
#include <string>
namespace R3D {
    using std::unordered_map;
    using std::string;
    class ShaderCache {
    public:
        unordered_map<string, Shader> m_shaders;
        void Init();//初始化所有需要的Shader
        void Release();
        Shader GetShader(string in_shadertype);
    };
}



