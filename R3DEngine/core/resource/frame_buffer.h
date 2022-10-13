//
// Created by Administrator on 2022/9/23.
//

#pragma once
#include <structdefine.h>
namespace R3D {
    class FrameBuffer {
    public:
        GLuint m_frameBuffer = 0;
    };
    class FrameBufferDepth : public FrameBuffer {
    public:
        GLuint m_depthAttach = 0;
        void Init(int in_width, int in_height);
        void Release();
    };
    class FrameBufferDepthNormal : public FrameBuffer {
    public:
        GLuint m_depthAttach = 0;
        GLuint m_normalAttach = 0;//视空间normal
        void Init(int in_width, int in_height);
        void Release();
    };
    class FrameBufferColDepthHDR : public FrameBuffer {
    public:
        GLuint m_depthAttach = 0;
        GLuint m_colorAttach0 = 0;
        void Init(int in_width, int in_height);
        void Release();
    };
    class FrameBufferColHDR : public FrameBuffer {
    public:
        GLuint m_colorAttach0 = 0;
        void Init(int in_width, int in_height);
        void Release();
    };
    class FrameBufferAO : public FrameBuffer {
    public:
        GLuint m_colorAttach0 = 0;
        GLuint m_blurDstTexture = 0;
        void Init(int in_width, int in_height);
        void Release();
    };
}



