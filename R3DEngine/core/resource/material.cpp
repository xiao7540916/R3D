//
// Created by Administrator on 2022/9/15.
//

#include "material.h"
#include <iostream>
extern string CURRENT_SOURCE_DIR;
namespace R3D {
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
        if(m_textureManage->m_textureBindCache[0]!=m_diffTex){
            glBindTextureUnit(0, m_diffTex);
        }
        if(m_textureManage->m_textureBindCache[1]!=m_specTex){
            glBindTextureUnit(1, m_specTex);
        }
        if(m_textureManage->m_textureBindCache[2]!=m_normalTex){
            glBindTextureUnit(2, m_normalTex);
        }
        if(m_textureManage->m_textureBindCache[3]!=m_dumpTex){
            glBindTextureUnit(3, m_dumpTex);
        }
    }
    Material::Material() {
        m_textureManage = TextureManage::GetInstance();
    }
}