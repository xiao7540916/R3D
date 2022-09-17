//
// Created by Administrator on 2022/9/17.
//

#include "buffer_manage.h"
namespace R3D {
    BufferManage::BufferManage() {
    }
    BufferManage::~BufferManage() {
    }
    BufferManage *BufferManage::m_bufferManage = nullptr;
    BufferManage *BufferManage::GetInstance() {
        if (m_bufferManage == nullptr) {
            m_bufferManage = new BufferManage();
        }
        return m_bufferManage;
    }
    void BufferManage::Init() {
        glCreateBuffers(1, &m_uniBlockBaseBuffer);
        glNamedBufferData(m_uniBlockBaseBuffer, sizeof(UniformBlockBase),
                          nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniBlockBaseBuffer);
    }
    void BufferManage::Release() {
    }
}