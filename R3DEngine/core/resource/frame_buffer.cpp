//
// Created by Administrator on 2022/9/23.
//

#include "frame_buffer.h"
#include <iostream>
namespace R3D {
    FrameBuffer::~FrameBuffer() {
        if (m_frameBuffer) {
            glDeleteFramebuffers(1, &m_frameBuffer);
        }
    }
    void FrameBufferDepth::Init(int in_width, int in_height) {
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        //深度附着
        glGenTextures(1, &m_depthAttach);
        glBindTexture(GL_TEXTURE_2D, m_depthAttach);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, in_width, in_height);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthAttach, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: FrameBufferDepth is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//恢复到默认帧缓冲
    }
}