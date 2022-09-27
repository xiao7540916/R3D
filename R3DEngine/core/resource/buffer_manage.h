//
// Created by Administrator on 2022/9/17.
//

#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <shader/glsldefine.h>
namespace R3D {
    class Device;
    class Scene;
    class BufferManage {
    public:
        GLuint m_uniBlockBaseBuffer;
        GLuint m_uniBlockMeshBuffer;
        GLuint m_pointLightBuffer;
        ~BufferManage();
        BufferManage(const BufferManage &) = delete;
        BufferManage &operator=(const BufferManage &) = delete;
        static BufferManage *GetInstance();
        void Init(Device *in_device);
        void Release();
        void UpdataUniBaseBuf(Scene &in_scene);
    private:
        BufferManage();
        static BufferManage *m_bufferManage;
        Device *m_device;
    };
}



