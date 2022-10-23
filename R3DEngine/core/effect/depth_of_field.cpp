//
// Created by Administrator on 2022/10/15.
//

#include "depth_of_field.h"
#include <device/device.h>
extern OptionConfig optionConfig;
namespace R3D {
    DepthOfField::DepthOfField() {
    }
    DepthOfField *DepthOfField::m_depthoffiled = nullptr;
    DepthOfField *DepthOfField::GetInstance() {
        if (m_depthoffiled == nullptr) {
            m_depthoffiled = NEW DepthOfField();
        }
        return m_depthoffiled;
    }
    void DepthOfField::Init(Device *in_device) {
        m_device = in_device;
        int width = in_device->m_windowWidth;
        int height = in_device->m_windowHeight;
        glGenTextures(1, &m_COCTex);
        glBindTexture(GL_TEXTURE_2D, m_COCTex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenTextures(1, &m_halfSizeTex0);
        glBindTexture(GL_TEXTURE_2D, m_halfSizeTex0);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width / 2, height / 2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenTextures(1, &m_halfSizeTex1);
        glBindTexture(GL_TEXTURE_2D, m_halfSizeTex1);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width / 2, height / 2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    void DepthOfField::MakeCOC() {
        int width = m_device->m_windowWidth;
        int height = m_device->m_windowHeight;
        Shader cocShader = m_device->m_shaderCache.GetShader("depthtococ");
        m_focus = optionConfig.focus;
        m_focusRange = optionConfig.focusRange;
        m_bokehRadius = optionConfig.bokehRadius;
        cocShader.use();
        cocShader.setFloat("uFocus", m_focus);
        cocShader.setFloat("uFocusRange", m_focusRange);
        cocShader.setFloat("uBokehRadius",m_bokehRadius);
        glBindTextureUnit(0, m_device->m_preDepthFBO.m_depthAttach);
        glBindImageTexture(1, m_COCTex, 0, false, 0, GL_WRITE_ONLY, GL_R32F);
        int workgroup_x = (width % TILE_SIZE) == 0 ? (width / TILE_SIZE) : (
                width / TILE_SIZE + 1);
        int workgroup_y = (height % TILE_SIZE) == 0 ? (height / TILE_SIZE) : (
                height / TILE_SIZE + 1);
        glDispatchCompute(workgroup_x, workgroup_y, 1);
        glTextureBarrier();
    }
    void DepthOfField::Release() {
        if(m_COCTex){
            glDeleteTextures(1, &m_COCTex);
            m_COCTex = 0;
        }
        if(m_halfSizeTex0){
            glDeleteTextures(1, &m_halfSizeTex0);
            m_halfSizeTex0 = 0;
        }
        if(m_halfSizeTex1){
            glDeleteTextures(1, &m_halfSizeTex1);
            m_halfSizeTex1 = 0;
        }
    }
    void DepthOfField::DownSample() {
        int width = m_device->m_windowWidth / 2;
        int height = m_device->m_windowHeight / 2;
        static Shader downSampleShader = m_device->m_shaderCache.GetShader("downsample");
        //PASS1 提取符合阈值的颜色进行下采样
        downSampleShader.use();
        downSampleShader.setInt("uDstWidth", width);
        downSampleShader.setInt("uDstHeight", height);
        downSampleShader.setInt("uLevel", 1);
        downSampleShader.setFloat("uSampleRadius", 1.0f);
        glBindTextureUnit(0, m_device->GetActiveScreenFrame().m_colorAttach0);
        glBindImageTexture(1, m_halfSizeTex0, 0, true, 0, GL_WRITE_ONLY, GL_RGBA32F);
        int workgroup_x = (width % TILE_SIZE) == 0 ? (width / TILE_SIZE) : (
                width / TILE_SIZE + 1);
        int workgroup_y = (height % TILE_SIZE) == 0 ? (height / TILE_SIZE) : (
                height / TILE_SIZE + 1);
        glDispatchCompute(workgroup_x, workgroup_y, 1);
        glTextureBarrier();
    }
    void DepthOfField::COCDownSample() {
        int width = m_device->m_windowWidth / 2;
        int height = m_device->m_windowHeight / 2;
        static Shader COCDownSampleShader = m_device->m_shaderCache.GetShader("cocdownsample");
        //PASS1 提取符合阈值的颜色进行下采样
        COCDownSampleShader.use();
        COCDownSampleShader.setInt("uDstWidth", width);
        COCDownSampleShader.setInt("uDstHeight", height);
        COCDownSampleShader.setFloat("uSampleRadius", 0.5f);
        glBindTextureUnit(0, m_COCTex);
        glBindImageTexture(1, m_halfSizeTex0, 0, true, 0, GL_READ_WRITE, GL_RGBA32F);
        int workgroup_x = (width % TILE_SIZE) == 0 ? (width / TILE_SIZE) : (
                width / TILE_SIZE + 1);
        int workgroup_y = (height % TILE_SIZE) == 0 ? (height / TILE_SIZE) : (
                height / TILE_SIZE + 1);
        glDispatchCompute(workgroup_x, workgroup_y, 1);
        glTextureBarrier();
    }
    void DepthOfField::CircleSample() {
        int width = m_device->m_windowWidth / 2;
        int height = m_device->m_windowHeight / 2;
        //环形采样
        {
            m_bokehRadius = optionConfig.bokehRadius;
            static Shader circleSampleShader = m_device->m_shaderCache.GetShader("circlesample");
            //PASS1 提取符合阈值的颜色进行下采样
            circleSampleShader.use();
            circleSampleShader.setInt("uDstWidth", width);
            circleSampleShader.setInt("uDstHeight", height);
            circleSampleShader.setFloat("uBokehRadius", m_bokehRadius);
            glBindTextureUnit(0, m_halfSizeTex0);
            glBindImageTexture(1, m_halfSizeTex1, 0, true, 0, GL_WRITE_ONLY, GL_RGBA32F);
            int workgroup_x = (width % TILE_SIZE) == 0 ? (width / TILE_SIZE) : (
                    width / TILE_SIZE + 1);
            int workgroup_y = (height % TILE_SIZE) == 0 ? (height / TILE_SIZE) : (
                    height / TILE_SIZE + 1);
            glDispatchCompute(workgroup_x, workgroup_y, 1);
            glTextureBarrier();
        }
        //模糊
        {
            static Shader downSampleShader = m_device->m_shaderCache.GetShader("downsample");
            //PASS1 提取符合阈值的颜色进行下采样
            downSampleShader.use();
            downSampleShader.setInt("uDstWidth", width);
            downSampleShader.setInt("uDstHeight", height);
            downSampleShader.setInt("uLevel", 1);
            downSampleShader.setFloat("uSampleRadius", 0.5f);
            glBindTextureUnit(0, m_halfSizeTex1);
            glBindImageTexture(1, m_halfSizeTex0, 0, true, 0, GL_WRITE_ONLY, GL_RGBA32F);
            int workgroup_x = (width % TILE_SIZE) == 0 ? (width / TILE_SIZE) : (
                    width / TILE_SIZE + 1);
            int workgroup_y = (height % TILE_SIZE) == 0 ? (height / TILE_SIZE) : (
                    height / TILE_SIZE + 1);
            glDispatchCompute(workgroup_x, workgroup_y, 1);
            glTextureBarrier();
        }
    }
    void DepthOfField::MergeDOF() {
        static Shader mergeDOFShader = m_device->m_shaderCache.GetShader("mergedof");
        m_device->ExangeActiveScreenFrame();
        glBindFramebuffer(GL_FRAMEBUFFER, m_device->GetActiveScreenFrame().m_frameBuffer);
        glBindTextureUnit(0, m_device->GetNotActiveScreenFrame().m_colorAttach0);//绑定到着色器
        glBindTextureUnit(1, m_COCTex);//绑定到着色器
        glBindTextureUnit(2, m_halfSizeTex0);//绑定到着色器
        if (RenderStateManage::GetInstance()->NeedChangeState(mergeDOFShader.ID)) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDepthMask(GL_FALSE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            mergeDOFShader.use();
        }
        static Mesh *screenbackmesh = MeshManage::GetInstance()->GetMesh("screenbackmesh");
        glBindVertexArray(screenbackmesh->VAO);
        glDrawElements(GL_TRIANGLES, screenbackmesh->m_indiceSize, GL_UNSIGNED_INT, nullptr);
    }
}