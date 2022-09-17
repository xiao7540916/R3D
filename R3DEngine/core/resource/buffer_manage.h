//
// Created by Administrator on 2022/9/17.
//

#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <shader/glsldefine.h>
namespace R3D {
    class BufferManage {
    public:
        UniformBlockBase m_uniBlockBase;
        GLuint m_uniBlockBaseBuffer;
        ~BufferManage();
        BufferManage(const BufferManage &) = delete;
        BufferManage &operator=(const BufferManage &) = delete;
        static BufferManage *GetInstance();
        void Init();
        void Release();
    private:
        BufferManage();
        static BufferManage *m_bufferManage;
    };
}



