//
// Created by Administrator on 2022/9/15.
//

#pragma once
#include <resource/shader.h>
#include <resource/texture_manage.h>
#include <glm/glm.hpp>
#include <structdefine.h>
namespace R3D {
    class Material {
    public:
        Material();
        Shader m_shader;
        TextureManage *m_textureManage;
        virtual void InitResource() = 0;
        virtual void BindResource() = 0;
    };
    class MaterialPhone : public Material {
    public:
        string m_diffTexUrl = {};
        string m_specTexUrl = {};
        string m_normalTexUrl = {};
        string m_dumpTexUrl = {};
        GLuint m_diffTex;
        GLuint m_specTex;
        GLuint m_normalTex;
        GLuint m_dumpTex;
        void InitResource()override;
        void BindResource()override;
    };
}



