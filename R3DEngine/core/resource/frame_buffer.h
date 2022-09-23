//
// Created by Administrator on 2022/9/23.
//

#pragma once
#include <structdefine.h>
namespace R3D {
    class FrameBuffer {
    public:
        GLuint m_frameBuffer;
        ~FrameBuffer();
    };
    class FrameBufferDepth : public FrameBuffer {
    public:
        GLuint m_depthAttach;
        void Init(int in_width, int in_height);
    };
}



