//
// Created by Administrator on 2022/9/23.
//

#include "frame_buffer.h"
#include <iostream>
namespace R3D {
    void FrameBufferDepth::Init(int in_width, int in_height) {
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        //深度附着
        glGenTextures(1, &m_depthAttach);
        glBindTexture(GL_TEXTURE_2D, m_depthAttach);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, in_width, in_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //指定缓冲附着
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthAttach, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: FrameBufferDepth is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//恢复到默认帧缓冲
    }
    void FrameBufferDepth::Release() {
        if (m_depthAttach) {
            glDeleteTextures(1, &m_depthAttach);
            m_depthAttach = 0;
        }
        if (m_frameBuffer) {
            glDeleteFramebuffers(1, &m_frameBuffer);
            m_frameBuffer = 0;
        }
    }
    void FrameBufferDepthNormal::Init(int in_width, int in_height) {
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        //颜色附着
        glGenTextures(1, &m_normalAttach);
        glBindTexture(GL_TEXTURE_2D, m_normalAttach);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, in_width, in_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //深度附着
        glGenTextures(1, &m_depthAttach);
        glBindTexture(GL_TEXTURE_2D, m_depthAttach);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, in_width, in_height);
        //指定缓冲附着
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_normalAttach, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthAttach, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: FrameBufferColDepthHDR is not complete!" << std::endl;
        static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};//设置启用的颜色附着，可指定不同顺序
        glDrawBuffers(1, draw_buffers);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//恢复到默认帧缓冲
    }
    void FrameBufferDepthNormal::Release() {
        if (m_depthAttach) {
            glDeleteTextures(1, &m_depthAttach);
            m_depthAttach = 0;
        }
        if (m_normalAttach) {
            glDeleteTextures(1, &m_normalAttach);
            m_normalAttach = 0;
        }
        if (m_frameBuffer) {
            glDeleteFramebuffers(1, &m_frameBuffer);
            m_frameBuffer = 0;
        }
    }
    void FrameBufferColDepthNorRoughHDR::Init(int in_width, int in_height) {
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        //颜色附着0
        glGenTextures(1, &m_colorAttach0);
        glBindTexture(GL_TEXTURE_2D, m_colorAttach0);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, in_width, in_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //颜色附着1
        glGenTextures(1, &m_colorAttach1);
        glBindTexture(GL_TEXTURE_2D, m_colorAttach1);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, in_width, in_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //颜色附着2
        glGenTextures(1, &m_colorAttach2);
        glBindTexture(GL_TEXTURE_2D, m_colorAttach2);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, in_width, in_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //深度附着
        glGenTextures(1, &m_depthAttach);
        glBindTexture(GL_TEXTURE_2D, m_depthAttach);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, in_width, in_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //指定缓冲附着
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorAttach0, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_colorAttach1, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_colorAttach2, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthAttach, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: FrameBufferColDepthHDR is not complete!" << std::endl;
        //初始化时仅启用一个着色附着，在程序中控制需要的fbo开启更多附着
        GLuint draw_buffers[1] = {GL_COLOR_ATTACHMENT0};//设置启用的颜色附着，可指定不同顺序
        glDrawBuffers(1, draw_buffers);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//恢复到默认帧缓冲
    }
    void FrameBufferColDepthNorRoughHDR::Release() {
        if (m_depthAttach) {
            glDeleteTextures(1, &m_depthAttach);
            m_depthAttach = 0;
        }
        if (m_colorAttach0) {
            glDeleteTextures(1, &m_colorAttach0);
            m_colorAttach0 = 0;
        }
        if (m_colorAttach1) {
            glDeleteTextures(1, &m_colorAttach1);
            m_colorAttach1 = 0;
        }
        if (m_colorAttach2) {
            glDeleteTextures(1, &m_colorAttach2);
            m_colorAttach2 = 0;
        }
        if (m_frameBuffer) {
            glDeleteFramebuffers(1, &m_frameBuffer);
            m_frameBuffer = 0;
        }
    }
    void FrameBufferColHDR::Init(int in_width, int in_height) {
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        //颜色附着
        glGenTextures(1, &m_colorAttach0);
        glBindTexture(GL_TEXTURE_2D, m_colorAttach0);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, in_width, in_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //指定缓冲附着
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorAttach0, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: FrameBufferColDepthHDR is not complete!" << std::endl;
        static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};//设置启用的颜色附着，可指定不同顺序
        glDrawBuffers(1, draw_buffers);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//恢复到默认帧缓冲
    }
    void FrameBufferColHDR::Release() {
        if (m_colorAttach0) {
            glDeleteTextures(1, &m_colorAttach0);
            m_colorAttach0 = 0;
        }
        if (m_frameBuffer) {
            glDeleteFramebuffers(1, &m_frameBuffer);
            m_frameBuffer = 0;
        }
    }
    void FrameBufferAO::Init(int in_width, int in_height) {
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        //颜色附着
        glGenTextures(1, &m_colorAttach0);
        glBindTexture(GL_TEXTURE_2D, m_colorAttach0);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16F, in_width, in_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //指定缓冲附着
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorAttach0, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: FrameBufferColDepthHDR is not complete!" << std::endl;
        static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};//设置启用的颜色附着，可指定不同顺序
        glDrawBuffers(1, draw_buffers);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//恢复到默认帧缓冲

        glGenTextures(1, &m_blurDstTexture);
        glBindTexture(GL_TEXTURE_2D, m_blurDstTexture);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16F, in_width, in_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    void FrameBufferAO::Release() {
        if (m_blurDstTexture) {
            glDeleteTextures(1, &m_blurDstTexture);
            m_blurDstTexture = 0;
        }
        if (m_colorAttach0) {
            glDeleteTextures(1, &m_colorAttach0);
            m_colorAttach0 = 0;
        }
        if (m_frameBuffer) {
            glDeleteFramebuffers(1, &m_frameBuffer);
            m_frameBuffer = 0;
        }
    }
}