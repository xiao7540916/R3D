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
    class Camera;
    class BufferManage {
    public:
        GLuint m_uniBlockBaseBuffer;
        GLuint m_uniBlockMeshBuffer;
        GLuint m_uniBlockAoCfgBuffer;
        GLuint m_pointLightBuffer;
        GLuint m_tileLightsIdxBuffer;
        GLuint m_tileLightsNumBuffer;
        GLuint m_tileClipPlaneBuffer;
        GLuint m_uniCSMBaseBuffer;
        GLuint m_uniCSMMeshBuffer;
        GLuint m_uniCSMHandleBuffer;
        ~BufferManage();
        BufferManage(const BufferManage &) = delete;
        BufferManage &operator=(const BufferManage &) = delete;
        static BufferManage *GetInstance();
        void Init(Device *in_device);
        void CreateTileLightBuffer(int in_windowwidth,int in_windowheight,int in_tilesize);
        void CreateTileClipBuffer(Camera& in_camera,int in_windowwidth,int in_windowheight,int in_tilesize);
        void Release();
        void UpdataUniBaseBuf(Scene &in_scene);
    private:
        BufferManage();
        static BufferManage *m_bufferManage;
        Device *m_device;
    };
}



