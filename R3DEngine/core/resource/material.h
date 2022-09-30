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
        MtrQueue m_mtrQueue;
        TextureManage *m_textureManage;
        virtual void InitResource(GLint in_param,GLint in_mipmapinfo) = 0;
        virtual void BindResource() = 0;
        virtual void RenderPrepare() = 0;
    };
    class MaterialPhone : public Material {
    public:
        MaterialPhone();
        string m_diffTexUrl = {};
        string m_specTexUrl = {};
        string m_normalTexUrl = {};
        string m_dumpTexUrl = {};
        GLuint m_diffTex{};
        GLuint m_specTex{};
        GLuint m_normalTex{};
        GLuint m_dumpTex{};
        void InitResource(GLint in_param = GL_REPEAT,GLint in_mipmapinfo = GL_LINEAR_MIPMAP_LINEAR) override;
        void BindResource() override;
        void RenderPrepare() override;
    };
    class MaterialMetalPbr : public Material {
    public:
        MaterialMetalPbr();
        string m_albedoTexUrl = {};
        string m_normalTexUrl = {};
        string m_metallicTexUrl = {};
        string m_roughnessTexUrl = {};
        string m_aoTexUrl = {};
        GLuint m_albedoTex{};
        GLuint m_normalTex{};
        GLuint m_metallicTex{};
        GLuint m_roughnessTex{};
        GLuint m_aoTex{};
        void InitResource(GLint in_param = GL_REPEAT,GLint in_mipmapinfo = GL_LINEAR_MIPMAP_LINEAR) override;
        void BindResource() override;
        void RenderPrepare() override;
    };
    class MaterialGreen : public Material {
    public:
        MaterialGreen();
        void InitResource(GLint in_param = GL_REPEAT,GLint in_mipmapinfo = GL_LINEAR_MIPMAP_LINEAR) override;
        void BindResource() override;
        void RenderPrepare() override;
    };
    class MaterialDepth : public Material {
    public:
        MaterialDepth();
        void InitResource(GLint in_param = GL_REPEAT,GLint in_mipmapinfo = GL_LINEAR_MIPMAP_LINEAR) override;
        void BindResource() override;
        void RenderPrepare() override;
    };
    class MaterialLightShow : public Material {
    public:
        MaterialLightShow();
        void InitResource(GLint in_param = GL_REPEAT,GLint in_mipmapinfo = GL_LINEAR_MIPMAP_LINEAR) override;
        void BindResource() override;
        void RenderPrepare() override;
    };
    class MaterialLightRadius : public Material {
    public:
        MaterialLightRadius();
        void InitResource(GLint in_param = GL_REPEAT,GLint in_mipmapinfo = GL_LINEAR_MIPMAP_LINEAR) override;
        void BindResource() override;
        void RenderPrepare() override;
    };
    //材质管理(主要用于初始化一些材质，减少主程序工作量,实例运行需要在贴图和着色器加载完成后)
    class MaterialManage {
    public:
        unordered_map<string,Material*> m_nameToMtr;
        void Init();
        void Release();
        void AddMaterial(const string& in_name,Material* in_material);
        Material* GetMaterial(const string& in_name);
        static MaterialManage* GetInstance();
    private:
        MaterialManage();
        static MaterialManage* m_materialManage;
    };
}



