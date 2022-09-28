//
// Created by Administrator on 2022/9/17.
//

#include "buffer_manage.h"
#include <device/device.h>
#include <device/scene.h>
namespace R3D {
    using std::cout;
    using std::endl;
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
                          nullptr, GL_DYNAMIC_DRAW);//DRAW代表会被用于GPU
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniBlockBaseBuffer);
        //模型数据
        glCreateBuffers(1, &m_uniBlockMeshBuffer);
        glNamedBufferData(m_uniBlockMeshBuffer, sizeof(UniformBlockMesh),
                          nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uniBlockMeshBuffer);
        //点光源数据
        glCreateBuffers(1, &m_pointLightBuffer);
        glNamedBufferData(m_pointLightBuffer, sizeof(PointLight) * POINT_LIGHT_COUNT,
                          nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_pointLightBuffer);
    }
    void BufferManage::Release() {
    }
    void BufferManage::UpdataUniBaseBuf(Scene &in_scene) {
        UniformBlockBase uniformBlockBase;
        Camera &camera = *m_device->m_camera;
        uniformBlockBase.viewproj = camera.GetProjection() * camera.GetView();
        uniformBlockBase.camerapos = camera.GetPosition();
        uniformBlockBase.dirLights[0] = in_scene.m_dirLights[0];
        uniformBlockBase.dirLights[1] = in_scene.m_dirLights[1];
        uniformBlockBase.dirLights[2] = in_scene.m_dirLights[2];
        uniformBlockBase.dirLights[3] = in_scene.m_dirLights[3];
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniBlockBaseBuffer);
        glNamedBufferSubData(m_uniBlockBaseBuffer, 0, sizeof(UniformBlockBase), &uniformBlockBase);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_pointLightBuffer);
        glNamedBufferSubData(m_pointLightBuffer, 0, sizeof(PointLight) * POINT_LIGHT_COUNT,
                             in_scene.m_pointLights.data());
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
}