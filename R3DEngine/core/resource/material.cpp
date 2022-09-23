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
    void MaterialPhone::InitResource() {
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
        m_textureManage->LoadTexture(m_diffTexUrl);
        m_textureManage->LoadTexture(m_specTexUrl);
        m_textureManage->LoadTexture(m_normalTexUrl);
        m_textureManage->LoadTexture(m_dumpTexUrl);
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
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            m_shader.use();
        }
        BindResource();
    }
    //---------------------------------------------
    MaterialMetalPbr::MaterialMetalPbr() {
        m_mtrQueue = MTRQUEUE_GEOMETRY;
    }
    void MaterialMetalPbr::InitResource() {
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
        m_textureManage->LoadTexture(m_albedoTexUrl);
        m_textureManage->LoadTexture(m_normalTexUrl);
        m_textureManage->LoadTexture(m_metallicTexUrl);
        m_textureManage->LoadTexture(m_roughnessTexUrl);
        m_textureManage->LoadTexture(m_aoTexUrl);
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
    void MaterialGreen::InitResource() {
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
    void MaterialDepth::InitResource() {
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
            glPolygonMode(GL_FRONT, GL_FILL);
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
        MaterialPhone *phone_circlebox = new MaterialPhone();
        phone_circlebox->m_shader = shaderCache.GetShader("phone");
        phone_circlebox->m_diffTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/diffuse.png";
        phone_circlebox->m_specTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/spec.png";
        phone_circlebox->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/normal.png";
        phone_circlebox->InitResource();
        AddMaterial("phone_circlebox", phone_circlebox);
        //----------------------------------------------
        MaterialMetalPbr *metalpbr_bathroomtile = new MaterialMetalPbr();
        metalpbr_bathroomtile->m_shader = shaderCache.GetShader("metalpbr");
        metalpbr_bathroomtile->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/bathroomtile/albedo.png";
        metalpbr_bathroomtile->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/bathroomtile/normal.png";
        metalpbr_bathroomtile->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/bathroomtile/roughness.png";
        metalpbr_bathroomtile->m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/bathroomtile/ao.png";
        metalpbr_bathroomtile->InitResource();
        AddMaterial("metalpbr_bathroomtile", metalpbr_bathroomtile);
        //----------------------------------------------
        MaterialGreen *green = new MaterialGreen();
        green->m_shader = shaderCache.GetShader("green");
        AddMaterial("green", green);
        //----------------------------------------------
        MaterialDepth *depth = new MaterialDepth();
        depth->m_shader = shaderCache.GetShader("depth");
        AddMaterial("depth", depth);
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