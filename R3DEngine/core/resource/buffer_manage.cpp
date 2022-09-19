//
// Created by Administrator on 2022/9/17.
//

#include "buffer_manage.h"
#include <device/device.h>
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
    void BufferManage::Init(Device *in_device) {
        m_device = in_device;
        glCreateBuffers(1, &m_uniBlockBaseBuffer);
        glNamedBufferData(m_uniBlockBaseBuffer, sizeof(UniformBlockBase),
                          nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniBlockBaseBuffer);
        //模型数据
        glCreateBuffers(1, &m_uniBlockMeshBuffer);
        glNamedBufferData(m_uniBlockMeshBuffer, sizeof(UniformBlockMesh),
                          nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uniBlockMeshBuffer);
    }
    void BufferManage::Release() {
    }
    void BufferManage::UpdataUniBaseBuf() {
        UniformBlockBase uniformBlockBase;
        Camera& camera = *m_device->m_camera;
        uniformBlockBase.viewproj = camera.GetProjection() * camera.GetView();
        uniformBlockBase.camerapos = camera.GetPosition();
        glNamedBufferSubData(m_uniBlockBaseBuffer, 0, sizeof(UniformBlockBase), &uniformBlockBase);
    }
}