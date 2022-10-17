//
// Created by Administrator on 2022/10/14.
//

#include "bloom.h"
#include <device/device.h>
extern OptionConfig optionConfig;
namespace R3D {
    Bloom::Bloom() {
    }
    Bloom *Bloom::m_bloom = nullptr;
    Bloom *Bloom::GetInstance() {
        if (m_bloom == nullptr) {
            m_bloom = NEW Bloom();
        }
        return m_bloom;
    }
    void Bloom::Init(Device *in_device) {
        m_device = in_device;
        int width = in_device->m_windowWidth;
        int height = in_device->m_windowHeight;
        for (int i = 0;i < BLOOM_ITERATION;++i) {
            width /= 2;
            height /= 2;
            glGenTextures(1, &m_bloomTex[i]);
            glBindTexture(GL_TEXTURE_2D, m_bloomTex[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            m_bloomTexSize[i].x = width;
            m_bloomTexSize[i].y = height;
        }
    }
    void Bloom::DownSample(GLuint in_bloomSurface) {
        static Shader downSampleShader = m_device->m_shaderCache.GetShader("downsample");
        m_threshold = optionConfig.threshold;
        m_softThreshold = optionConfig.softThreshold;
        float knee = m_threshold * m_softThreshold;
        vec4 filter;
        filter.x = m_threshold;
        filter.y = filter.x - knee;
        filter.z = 2.0f * knee;
        filter.w = 0.25f / (knee + 0.00001f);
        //PASS1 提取符合阈值的颜色进行下采样
        downSampleShader.use();
        downSampleShader.setInt("uDstWidth", m_bloomTexSize[0].x);
        downSampleShader.setInt("uDstHeight", m_bloomTexSize[0].y);
        downSampleShader.setInt("uLevel", 0);
        downSampleShader.setVec4("uFilter", filter);
        downSampleShader.setFloat("uSampleRadius", 1.0f);
        glBindTextureUnit(0, in_bloomSurface);
        glBindImageTexture(1, m_bloomTex[0], 0, true, 0, GL_WRITE_ONLY, GL_RGBA32F);
        int workgroup_x = (m_bloomTexSize[0].x % TILE_SIZE) == 0 ? (m_bloomTexSize[0].x / TILE_SIZE) : (
                m_bloomTexSize[0].x / TILE_SIZE + 1);
        int workgroup_y = (m_bloomTexSize[0].y % TILE_SIZE) == 0 ? (m_bloomTexSize[0].y / TILE_SIZE) : (
                m_bloomTexSize[0].y / TILE_SIZE + 1);
        glDispatchCompute(workgroup_x, workgroup_y, 1);
        glTextureBarrier();
        for (int i = 1;i < BLOOM_ITERATION;++i) {
            downSampleShader.setInt("uDstWidth", m_bloomTexSize[i].x);
            downSampleShader.setInt("uDstHeight", m_bloomTexSize[i].y);
            downSampleShader.setInt("uLevel", i);
            downSampleShader.setFloat("uSampleRadius", 1.0f);
            glBindTextureUnit(0, m_bloomTex[i - 1]);
            glBindImageTexture(1, m_bloomTex[i], 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
            workgroup_x = (m_bloomTexSize[i].x % TILE_SIZE) == 0 ? (m_bloomTexSize[i].x / TILE_SIZE) : (
                    m_bloomTexSize[i].x / TILE_SIZE + 1);
            workgroup_y = (m_bloomTexSize[i].y % TILE_SIZE) == 0 ? (m_bloomTexSize[i].y / TILE_SIZE) : (
                    m_bloomTexSize[i].y / TILE_SIZE + 1);
            glDispatchCompute(workgroup_x, workgroup_y, 1);
            glTextureBarrier();
        }
    }
    void Bloom::UpSample() {
        static Shader upSampleShader = m_device->m_shaderCache.GetShader("upsample");
        int workgroup_x;
        int workgroup_y;
        upSampleShader.use();
        for (int i = BLOOM_ITERATION - 2;i >= 0;--i) {
            upSampleShader.setInt("uDstWidth", m_bloomTexSize[i].x);
            upSampleShader.setInt("uDstHeight", m_bloomTexSize[i].y);
            glBindTextureUnit(0, m_bloomTex[i + 1]);
            glBindImageTexture(1, m_bloomTex[i], 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
            workgroup_x = (m_bloomTexSize[i].x % TILE_SIZE) == 0 ? (m_bloomTexSize[i].x / TILE_SIZE) : (
                    m_bloomTexSize[i].x / TILE_SIZE + 1);
            workgroup_y = (m_bloomTexSize[i].y % TILE_SIZE) == 0 ? (m_bloomTexSize[i].y / TILE_SIZE) : (
                    m_bloomTexSize[i].y / TILE_SIZE + 1);
            glDispatchCompute(workgroup_x, workgroup_y, 1);
            glTextureBarrier();
        }
    }
    void Bloom::Release() {
        for (int i = 0;i < BLOOM_ITERATION;++i) {
            if (m_bloomTex[i]) {
                glDeleteTextures(1, &m_bloomTex[i]);
            }
        }
    }
    void Bloom::MergeBloom() {
        static Shader mergeBloomShader = m_device->m_shaderCache.GetShader("mergebloom");
        glBindTexture(0, m_bloomTex[0]);//绑定到着色器
        if (RenderStateManage::GetInstance()->NeedChangeState(mergeBloomShader.ID)) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDepthMask(GL_FALSE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            mergeBloomShader.use();
        }
        mergeBloomShader.setFloat("uBloomStrength", optionConfig.bloomStrength);
        static Mesh *screenbackmesh = MeshManage::GetInstance()->GetMesh("screenbackmesh");
        glBindVertexArray(screenbackmesh->VAO);
        glDrawElements(GL_TRIANGLES, screenbackmesh->m_indiceSize, GL_UNSIGNED_INT, nullptr);
        glDisable(GL_BLEND);
    }
}