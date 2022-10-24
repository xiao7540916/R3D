//
// Created by Administrator on 2022/10/18.
//

#include "stochastic_ssr.h"
#include <device/device.h>
namespace R3D {
    StochasticSSR::StochasticSSR() {
    }
    StochasticSSR *StochasticSSR::m_stochasticSsr = nullptr;
    StochasticSSR *StochasticSSR::GetInstance() {
        if (m_stochasticSsr == nullptr) {
            m_stochasticSsr = NEW StochasticSSR();
        }
        return m_stochasticSsr;
    }
    void StochasticSSR::Init(Device *in_device) {
        m_device = in_device;
        m_lastFrameTex[0].Init(m_device->m_windowWidth, m_device->m_windowHeight);
        m_lastFrameTex[1].Init(m_device->m_windowWidth, m_device->m_windowHeight);
        //depthdowntex
        {
            int width = m_device->m_windowWidth;
            int height = m_device->m_windowHeight;
            for (int i = 0;i < DEPTH_DOWN_LEVEL;++i) {
                width = width / 2;
                height = height / 2;
                glGenTextures(1, &m_downDepthTex[i]);
                glBindTexture(GL_TEXTURE_2D, m_downDepthTex[i]);
                glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, width, height);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                m_downDepthTexSize[i].x = width;
                m_downDepthTexSize[i].y = height;
            }
        }
        //sssrcoltex
        {
            int width = m_device->m_windowWidth;
            int height = m_device->m_windowHeight;
            for (int i = 0;i < 2;++i) {
                glGenTextures(1, &m_sssrColTex[i]);
                glBindTexture(GL_TEXTURE_2D, m_sssrColTex[i]);
                glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
        }
        //hitpixeltex
        {
            int width = m_device->m_windowWidth;
            int height = m_device->m_windowHeight;
            glGenTextures(1, &m_hitPixelTex);
            glBindTexture(GL_TEXTURE_2D, m_hitPixelTex);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32I, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        //debugtex
        {
            int width = m_device->m_windowWidth;
            int height = m_device->m_windowHeight;
            glGenTextures(1, &m_debugTex);
            glBindTexture(GL_TEXTURE_2D, m_debugTex);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
    }
    void StochasticSSR::Release() {
        m_lastFrameTex[0].Release();
        m_lastFrameTex[1].Release();
        for (int i = 0;i < DEPTH_DOWN_LEVEL;++i) {
            glDeleteTextures(1, &m_downDepthTex[i]);
            m_downDepthTex[i] = 0;
        }
        for (int i = 0;i < 2;++i) {
            glDeleteTextures(1, &m_sssrColTex[i]);
            m_sssrColTex[i] = 0;
        }
        glDeleteTextures(1, &m_debugTex);
    }
    void StochasticSSR::DownMinDepth() {
        static Shader depthMinShader = m_device->m_shaderCache.GetShader("depthmin");
        depthMinShader.use();
        depthMinShader.setIVec4("uSrcDstImageSize",
                                ivec4(m_device->m_windowWidth, m_device->m_windowHeight, m_downDepthTexSize[0].x,
                                      m_downDepthTexSize[0].y));
        glBindTextureUnit(0, m_device->GetActiveScreenFrame().m_depthAttach);
        glBindImageTexture(1, m_downDepthTex[0], 0, true, 0, GL_WRITE_ONLY, GL_R32F);
        int workgroup_x = (m_downDepthTexSize[0].x % TILE_SIZE) == 0 ? (m_downDepthTexSize[0].x / TILE_SIZE) : (
                m_downDepthTexSize[0].x / TILE_SIZE + 1);
        int workgroup_y = (m_downDepthTexSize[0].y % TILE_SIZE) == 0 ? (m_downDepthTexSize[0].y / TILE_SIZE) : (
                m_downDepthTexSize[0].y / TILE_SIZE + 1);
        glDispatchCompute(workgroup_x, workgroup_y, 1);
        glTextureBarrier();
        for (int i = 1;i < DEPTH_DOWN_LEVEL;++i) {
            ivec4 srcDstImageSize = ivec4(m_downDepthTexSize[i - 1].x,
                                          m_downDepthTexSize[i - 1].y,
                                          m_downDepthTexSize[i].x,
                                          m_downDepthTexSize[i].y);
            depthMinShader.setIVec4("uSrcDstImageSize", srcDstImageSize);
            glBindTextureUnit(0, m_downDepthTex[i - 1]);
            glBindImageTexture(1, m_downDepthTex[i], 0, true, 0, GL_WRITE_ONLY, GL_R32F);
            workgroup_x = (m_downDepthTexSize[i].x % TILE_SIZE) == 0 ? (m_downDepthTexSize[i].x / TILE_SIZE) : (
                    m_downDepthTexSize[i].x / TILE_SIZE + 1);
            workgroup_y = (m_downDepthTexSize[i].y % TILE_SIZE) == 0 ? (m_downDepthTexSize[i].y / TILE_SIZE) : (
                    m_downDepthTexSize[i].y / TILE_SIZE + 1);
            glDispatchCompute(workgroup_x, workgroup_y, 1);
            glTextureBarrier();
        }
    }
    void StochasticSSR::TraceHitPixel() {
        int width = m_device->m_windowWidth;
        int height = m_device->m_windowHeight;
        static Shader traceHitPixelShader = m_device->m_shaderCache.GetShader("tracehitpixel");
        traceHitPixelShader.use();
        traceHitPixelShader.setIVec4("uSrcDstImageSize",
                                     ivec4(width, height, width, height));
        glBindTextureUnit(0, m_device->GetActiveScreenFrame().m_colorAttach1);
        glBindTextureUnit(1, m_device->GetActiveScreenFrame().m_colorAttach2);
        glBindImageTexture(2, m_hitPixelTex, 0, true, 0, GL_WRITE_ONLY, GL_RGBA32I);
        glBindImageTexture(3, m_debugTex, 0, true, 0, GL_WRITE_ONLY, GL_RGBA32F);
        int workgroup_x = (width % TILE_SIZE) == 0 ? (width / TILE_SIZE) : (
                width / TILE_SIZE + 1);
        int workgroup_y = (height % TILE_SIZE) == 0 ? (height / TILE_SIZE) : (
                height / TILE_SIZE + 1);
        glDispatchCompute(workgroup_x, workgroup_y, 1);
        glTextureBarrier();
    }
    void StochasticSSR::ResloveSSSR() {
        int width = m_device->m_windowWidth;
        int height = m_device->m_windowHeight;
        static Shader resloveSssrShader = m_device->m_shaderCache.GetShader("reslovesssr");
        resloveSssrShader.use();
        glBindTextureUnit(0, m_device->GetActiveScreenFrame().m_colorAttach1);
        glBindTextureUnit(1, m_device->GetActiveScreenFrame().m_colorAttach2);
        glBindTextureUnit(2, m_device->GetActiveScreenFrame().m_colorAttach0);
        glBindTextureUnit(3, m_device->GetActiveScreenFrame().m_depthAttach);
        glBindTextureUnit(4, GetNotActiveSssrTex());
        glBindTextureUnit(5, GetNotActiveFrameColCopy().m_colorAttach0);
        glBindTextureUnit(6, GetActiveFrameColCopy().m_colorAttach0);
        glBindImageTexture(0, m_hitPixelTex, 0, true, 0, GL_READ_ONLY, GL_RGBA32I);
        glBindImageTexture(1, GetActiveSssrTex(), 0, true, 0, GL_WRITE_ONLY, GL_RGBA32F);
        int workgroup_x = (width % TILE_SIZE) == 0 ? (width / TILE_SIZE) : (
                width / TILE_SIZE + 1);
        int workgroup_y = (height % TILE_SIZE) == 0 ? (height / TILE_SIZE) : (
                height / TILE_SIZE + 1);
        glDispatchCompute(workgroup_x, workgroup_y, 1);
        glTextureBarrier();
    }
    void StochasticSSR::MergeSSSR() {
        static Shader mergeSssrShader = m_device->m_shaderCache.GetShader("mergesssr");
        if (RenderStateManage::GetInstance()->NeedChangeState(mergeSssrShader.ID)) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDepthMask(GL_FALSE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            mergeSssrShader.use();
        }
        glBindTextureUnit(0, GetActiveSssrTex());
        static Mesh *screenbackmesh = MeshManage::GetInstance()->GetMesh("screenbackmesh");
        glBindVertexArray(screenbackmesh->VAO);
        glDrawElements(GL_TRIANGLES, screenbackmesh->m_indiceSize, GL_UNSIGNED_INT, nullptr);
        glDisable(GL_BLEND);
        ExchangeSssrTexture();
    }
    void StochasticSSR::ExchangeSssrTexture() {
        m_activeSssrColIdx = (m_activeSssrColIdx + 1) % 2;
    }
    GLuint StochasticSSR::GetActiveSssrTex() {
        return m_sssrColTex[m_activeSssrColIdx];
    }
    GLuint StochasticSSR::GetNotActiveSssrTex() {
        return m_sssrColTex[(m_activeSssrColIdx + 1) % 2];
    }
    FrameBufferColHDR &StochasticSSR::GetActiveFrameColCopy() {
        return m_lastFrameTex[m_activeSssrColIdx];
    }
    FrameBufferColHDR &StochasticSSR::GetNotActiveFrameColCopy() {
        return m_lastFrameTex[(m_activeSssrColIdx + 1) % 2];
    }
}