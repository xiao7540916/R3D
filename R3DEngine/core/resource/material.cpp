//
// Created by Administrator on 2022/9/15.
//

#include "material.h"
#include "render_state_manage.h"
#include <device/device.h>
#include <iostream>
extern string CURRENT_SOURCE_DIR;
namespace R3D {
    Material::Material() {
        m_textureManage = TextureManage::GetInstance();
    }
    //---------------------------------------------
    MaterialPhone::MaterialPhone() {
        m_mtrQueue = MTRQUEUE_GEOMETRY;
    }
    void MaterialPhone::InitResource(GLint in_param, GLint in_mipmapinfo) {
        if (m_diffTexUrl.empty()) {
            m_diffTexUrl = CURRENT_SOURCE_DIR + "Data/image/white.png";
        }
        if (m_specTexUrl.empty()) {
            m_specTexUrl = CURRENT_SOURCE_DIR + "Data/image/black.png";
        }
        if (m_normalTexUrl.empty()) {
            m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/normal.png";
        }
        if (m_dumpTexUrl.empty()) {
            m_dumpTexUrl = CURRENT_SOURCE_DIR + "Data/image/black.png";
        }
        m_textureManage->LoadTexture(m_diffTexUrl, in_param, in_mipmapinfo);
        m_textureManage->LoadTexture(m_specTexUrl, in_param, in_mipmapinfo);
        m_textureManage->LoadTexture(m_normalTexUrl, in_param, in_mipmapinfo);
        m_textureManage->LoadTexture(m_dumpTexUrl, in_param, in_mipmapinfo);
        m_diffTex = m_textureManage->GetTextureByUrl(m_diffTexUrl);
        m_specTex = m_textureManage->GetTextureByUrl(m_specTexUrl);
        m_normalTex = m_textureManage->GetTextureByUrl(m_normalTexUrl);
        m_dumpTex = m_textureManage->GetTextureByUrl(m_dumpTexUrl);
    }
    void MaterialPhone::BindResource() {
        if (m_textureManage->m_textureBindCache[0] != m_diffTex) {
            glBindTextureUnit(0, m_diffTex);
        }
        if (m_textureManage->m_textureBindCache[1] != m_specTex) {
            glBindTextureUnit(1, m_specTex);
        }
        if (m_textureManage->m_textureBindCache[2] != m_normalTex) {
            glBindTextureUnit(2, m_normalTex);
        }
        if (m_textureManage->m_textureBindCache[3] != m_dumpTex) {
            glBindTextureUnit(3, m_dumpTex);
        }
    }
    void MaterialPhone::RenderPrepare() {
        if (RenderStateManage::GetInstance()->NeedChangeState(m_shader.ID)) {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDepthFunc(GL_EQUAL);
            glPolygonMode(GL_FRONT, GL_FILL);
            m_shader.use();
        }
        BindResource();
    }
    //---------------------------------------------
    MaterialMetalPbr::MaterialMetalPbr() {
        m_mtrQueue = MTRQUEUE_GEOMETRY;
    }
    void MaterialMetalPbr::InitResource(GLint in_param, GLint in_mipmapinfo) {
        if (m_albedoTexUrl.empty()) {
            m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/white.png";
        }
        if (m_normalTexUrl.empty()) {
            m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/normal.png";
        }
        if (m_metallicTexUrl.empty()) {
            m_metallicTexUrl = CURRENT_SOURCE_DIR + "Data/image/black.png";
        }
        if (m_roughnessTexUrl.empty()) {
            m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/white.png";
        }
        if (m_aoTexUrl.empty()) {
            m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/white.png";
        }
        m_textureManage->LoadTexture(m_albedoTexUrl, in_param, in_mipmapinfo);
        m_textureManage->LoadTexture(m_normalTexUrl, in_param, in_mipmapinfo);
        m_textureManage->LoadTexture(m_metallicTexUrl, in_param, in_mipmapinfo);
        m_textureManage->LoadTexture(m_roughnessTexUrl, in_param, in_mipmapinfo);
        m_textureManage->LoadTexture(m_aoTexUrl, in_param, in_mipmapinfo);
        m_albedoTex = m_textureManage->GetTextureByUrl(m_albedoTexUrl);
        m_normalTex = m_textureManage->GetTextureByUrl(m_normalTexUrl);
        m_metallicTex = m_textureManage->GetTextureByUrl(m_metallicTexUrl);
        m_roughnessTex = m_textureManage->GetTextureByUrl(m_roughnessTexUrl);
        m_aoTex = m_textureManage->GetTextureByUrl(m_aoTexUrl);
    }
    void MaterialMetalPbr::BindResource() {
        if (m_textureManage->m_textureBindCache[0] != m_albedoTex) {
            glBindTextureUnit(0, m_albedoTex);
        }
        if (m_textureManage->m_textureBindCache[1] != m_normalTex) {
            glBindTextureUnit(1, m_normalTex);
        }
        if (m_textureManage->m_textureBindCache[2] != m_metallicTex) {
            glBindTextureUnit(2, m_metallicTex);
        }
        if (m_textureManage->m_textureBindCache[3] != m_roughnessTex) {
            glBindTextureUnit(3, m_roughnessTex);
        }
        if (m_textureManage->m_textureBindCache[4] != m_aoTex) {
            glBindTextureUnit(4, m_aoTex);
        }
    }
    void MaterialMetalPbr::RenderPrepare() {
        if (RenderStateManage::GetInstance()->NeedChangeState(m_shader.ID)) {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDepthFunc(GL_EQUAL);
            glPolygonMode(GL_FRONT, GL_FILL);
            m_shader.use();
        }
        BindResource();
    }
    //---------------------------------------------
    MaterialGreen::MaterialGreen() {
        m_mtrQueue = MTRQUEUE_GEOMETRY;
    }
    void MaterialGreen::InitResource(GLint in_param, GLint in_mipmapinfo) {
    }
    void MaterialGreen::BindResource() {
    }
    void MaterialGreen::RenderPrepare() {
        if (RenderStateManage::GetInstance()->NeedChangeState(m_shader.ID)) {
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glDepthFunc(GL_LESS);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            m_shader.use();
        }
    }
    //----------------------------------------------
    MaterialDepth::MaterialDepth() {
        m_mtrQueue = MTRQUEUE_BACKGROUND;
    }
    void MaterialDepth::InitResource(GLint in_param, GLint in_mipmapinfo) {
    }
    void MaterialDepth::BindResource() {
    }
    void MaterialDepth::RenderPrepare() {
        if (RenderStateManage::GetInstance()->NeedChangeState(m_shader.ID)) {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDepthFunc(GL_LESS);
            //注：此处GL_FRONT_AND_BACK对应恢复包围球pass中的状态，不可仅恢复FRONT状态，造成渲染错误
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            m_shader.use();
        }
    }
    //----------------------------------------------
    MaterialLightShow::MaterialLightShow() {
        m_mtrQueue = MTRQUEUE_GEOMETRY;
    }
    void MaterialLightShow::InitResource(GLint in_param, GLint in_mipmapinfo) {
    }
    void MaterialLightShow::BindResource() {
    }
    void MaterialLightShow::RenderPrepare() {
        if (RenderStateManage::GetInstance()->NeedChangeState(m_shader.ID)) {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDepthFunc(GL_LESS);
            //注：此处GL_FRONT_AND_BACK对应恢复包围球pass中的状态，不可仅恢复FRONT状态，造成渲染错误
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            m_shader.use();
        }
    }
    //----------------------------------------------
    MaterialLightRadius::MaterialLightRadius() {
        m_mtrQueue = MTRQUEUE_GEOMETRY;
    }
    void MaterialLightRadius::InitResource(GLint in_param, GLint in_mipmapinfo) {
    }
    void MaterialLightRadius::BindResource() {
    }
    void MaterialLightRadius::RenderPrepare() {
        if (RenderStateManage::GetInstance()->NeedChangeState(m_shader.ID)) {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDepthFunc(GL_LESS);
            //注：此处GL_FRONT_AND_BACK对应恢复包围球pass中的状态，不可仅恢复FRONT状态，造成渲染错误
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            m_shader.use();
        }
    }
    //----------------------------------------------
    MaterialManage::MaterialManage() {
    }
    MaterialManage *MaterialManage::m_materialManage = nullptr;
    MaterialManage *MaterialManage::GetInstance() {
        if (m_materialManage == nullptr) {
            m_materialManage = new MaterialManage();
        }
        return m_materialManage;
    }
    void MaterialManage::Init() {
        ShaderCache &shaderCache = Device::GetInstance()->m_shaderCache;
/*        MaterialPhone *phone_circlebox = new MaterialPhone();
        phone_circlebox->m_shader = shaderCache.GetShader("phone");
        phone_circlebox->m_diffTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/diffuse.png";
        phone_circlebox->m_specTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/spec.png";
        phone_circlebox->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/normal.png";
        phone_circlebox->InitResource();
        AddMaterial("phone_circlebox", phone_circlebox);*/
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_rusted_iron = new MaterialMetalPbr();
        metalpbr_rusted_iron->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_rusted_iron->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/albedo.png";
        metalpbr_rusted_iron->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/normal.png";
        metalpbr_rusted_iron->m_metallicTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/metallic.png";
        metalpbr_rusted_iron->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/roughness.png";
        metalpbr_rusted_iron->m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/ao.png";
        metalpbr_rusted_iron->InitResource(GL_CLAMP_TO_EDGE);
        AddMaterial("metalpbr_rusted_iron", metalpbr_rusted_iron);
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_gold = new MaterialMetalPbr();
        metalpbr_gold->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_gold->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/gold/albedo.png";
        metalpbr_gold->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/gold/normal.png";
        metalpbr_gold->m_metallicTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/gold/metallic.png";
        metalpbr_gold->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/gold/roughness.png";
        metalpbr_gold->m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/gold/ao.png";
        metalpbr_gold->InitResource();
        AddMaterial("metalpbr_gold", metalpbr_gold);
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_forestbrown = new MaterialMetalPbr();
        metalpbr_forestbrown->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_forestbrown->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/forestbrown/albedo.png";
        metalpbr_forestbrown->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/forestbrown/normal.png";
        metalpbr_forestbrown->m_metallicTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/forestbrown/metallic.png";
        metalpbr_forestbrown->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/forestbrown/roughness.png";
        metalpbr_forestbrown->m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/forestbrown/ao.png";
        metalpbr_forestbrown->InitResource();
        AddMaterial("metalpbr_forestbrown", metalpbr_forestbrown);
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_silver = new MaterialMetalPbr();
        metalpbr_silver->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_silver->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/silver/albedo.jpg";
        metalpbr_silver->m_metallicTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/silver/metallic.jpg";
        metalpbr_silver->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/silver/roughness.jpg";
        metalpbr_silver->InitResource();
        AddMaterial("metalpbr_silver", metalpbr_silver);
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_aluminiumfoil = new MaterialMetalPbr();
        metalpbr_aluminiumfoil->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_aluminiumfoil->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/aluminiumfoil/albedo.png";
        metalpbr_aluminiumfoil->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/aluminiumfoil/normal.png";
        metalpbr_aluminiumfoil->m_metallicTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/aluminiumfoil/metallic.png";
        metalpbr_aluminiumfoil->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/aluminiumfoil/roughness.png";
        metalpbr_aluminiumfoil->m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/aluminiumfoil/ao.png";
        metalpbr_aluminiumfoil->InitResource();
        AddMaterial("metalpbr_aluminiumfoil", metalpbr_aluminiumfoil);
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_stonewall = new MaterialMetalPbr();
        metalpbr_stonewall->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_stonewall->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/stonewall/albedo.png";
        metalpbr_stonewall->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/stonewall/normal.png";
        metalpbr_stonewall->m_metallicTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/stonewall/metallic.png";
        metalpbr_stonewall->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/stonewall/roughness.png";
        metalpbr_stonewall->m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/stonewall/ao.png";
        metalpbr_stonewall->InitResource();
        AddMaterial("metalpbr_stonewall", metalpbr_stonewall);
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_floor = new MaterialMetalPbr();
        metalpbr_floor->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_floor->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/floor/albedo.png";
        metalpbr_floor->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/floor/normal.png";
        metalpbr_floor->m_metallicTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/floor/metallic.png";
        metalpbr_floor->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/floor/roughness.png";
        metalpbr_floor->m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/floor/ao.png";
        metalpbr_floor->InitResource();
        AddMaterial("metalpbr_floor", metalpbr_floor);
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_check = new MaterialMetalPbr();
        metalpbr_check->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_check->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/check/albedo.png";
        metalpbr_check->InitResource();
        AddMaterial("metalpbr_check", metalpbr_check);
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_base = new MaterialMetalPbr();
        metalpbr_base->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_base->InitResource();
        AddMaterial("metalpbr_base", metalpbr_base);
        //----------------------------------------------
        MaterialGreen *green = new MaterialGreen();
        green->m_shader = shaderCache.GetShader("green");
        AddMaterial("green", green);
        //----------------------------------------------
        MaterialDepth *depth = new MaterialDepth();
        depth->m_shader = shaderCache.GetShader("depth");
        AddMaterial("depth", depth);
        //----------------------------------------------
        MaterialLightShow *lightshow = new MaterialLightShow();
        lightshow->m_shader = shaderCache.GetShader("lightshow");
        AddMaterial("lightshow", lightshow);
        //----------------------------------------------
        MaterialLightRadius *lightradius = new MaterialLightRadius();
        lightradius->m_shader = shaderCache.GetShader("lightradius");
        AddMaterial("lightradius", lightradius);
    }
    void MaterialManage::Release() {
        for (auto item = m_nameToMtr.begin();item != m_nameToMtr.end();item++) {
            delete item->second;
        }
        m_nameToMtr.clear();
        //m_nameToMtr自身占用空间仍需要清理
    }
    void MaterialManage::AddMaterial(const string &in_name, Material *in_material) {
        if (m_nameToMtr.find(in_name) != m_nameToMtr.end()) {
            std::cout << "material " << in_name << " has add!" << std::endl;
            return;
        }
        m_nameToMtr.insert({in_name, in_material});
    }
    Material *MaterialManage::GetMaterial(const string &in_name) {
        if (m_nameToMtr.find(in_name) != m_nameToMtr.end()) {
            return m_nameToMtr[in_name];
        }
        std::cout << "can not find " << in_name << " material" << std::endl;
        return m_nameToMtr["green"];
    }
}