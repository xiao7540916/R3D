//
// Created by Administrator on 2022/10/11.
//

#include "oit.h"
#include <device/device.h>
namespace R3D {
    OIT::OIT() {
    }
    OIT *OIT::m_OIT = nullptr;
    OIT *OIT::GetInstance() {
        if (m_OIT == nullptr) {
            m_OIT = NEW OIT();
        }
        return m_OIT;
    }
    void OIT::Init(Device *in_device) {
        m_device = in_device;
        m_resolveShader = in_device->m_shaderCache.GetShader("oitresolve");
        size_t totalpixels = OIT_FRAME_WIDTH * OIT_FRAME_HEIGHT;
        //初始化head
        glGenTextures(1, &m_headPointTex);
        glBindTexture(GL_TEXTURE_2D, m_headPointTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, OIT_FRAME_WIDTH, OIT_FRAME_HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_INT,
                     nullptr);
        //初始化head初始数据，使用Pixel Buffer Object，PBO 仅用于执行像素传输不连接到纹理，且与FBO（帧缓冲区对象）无关
        glGenBuffers(1, &m_headInitBuffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_headInitBuffer);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, totalpixels * sizeof(GLuint), nullptr, GL_STATIC_DRAW);
        void *data = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        memset(data, 0x00, totalpixels * sizeof(GLuint));
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        //创建原子计数器缓冲
        glGenBuffers(1, &m_atomicCountBuffer);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicCountBuffer);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_COPY);
        //创建存储片源信息缓冲
        glGenBuffers(1, &m_fragmentStorageBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, m_fragmentStorageBuffer);
        glBufferData(GL_TEXTURE_BUFFER, 2 * totalpixels * sizeof(vec4), nullptr, GL_DYNAMIC_COPY);
    }
    void OIT::PrapareData() {
        //绑定了GL_PIXEL_UNPACK_BUFFER时，glTexSubImage2D的数据来源变为所绑定的缓冲，通过指针位置设置偏移
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_headInitBuffer);
        glBindTexture(GL_TEXTURE_2D, m_headPointTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, OIT_FRAME_WIDTH, OIT_FRAME_HEIGHT, GL_RED_INTEGER, GL_UNSIGNED_INT,
                        nullptr);
        glBindImageTexture(0, m_headPointTex, 0, false, 0, GL_READ_WRITE, GL_R32UI);//绑定到着色器
        //重置计数器
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_atomicCountBuffer);//绑定到着色器
        const GLuint zero = 0;
        glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
        //
        glBindImageTexture(1, m_fragmentStorageBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32UI);//绑定到着色器
    }
    void OIT::Release() {
        if (m_headInitBuffer) {
            glDeleteBuffers(1, &m_headInitBuffer);
            m_headInitBuffer = 0;
        }
        if (m_headPointTex) {
            glDeleteTextures(1, &m_headPointTex);
            m_headPointTex = 0;
        }
        if (m_atomicCountBuffer) {
            glDeleteBuffers(1, &m_atomicCountBuffer);
            m_atomicCountBuffer = 0;
        }
        if (m_fragmentStorageBuffer) {
            glDeleteBuffers(1, &m_fragmentStorageBuffer);
            m_fragmentStorageBuffer = 0;
        }
    }
    void OIT::Resolve() {
        glBindImageTexture(1, m_fragmentStorageBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32UI);//绑定到着色器
        if (RenderStateManage::GetInstance()->NeedChangeState(m_resolveShader.ID)) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDepthMask(GL_FALSE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            m_resolveShader.use();
        }
        static Mesh *screenbackmesh = MeshManage::GetInstance()->GetMesh("screenbackmesh");
        glBindVertexArray(screenbackmesh->VAO);
        glDrawElements(GL_TRIANGLES, screenbackmesh->m_indiceSize, GL_UNSIGNED_INT, nullptr);
    }
}