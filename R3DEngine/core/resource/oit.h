//
// Created by Administrator on 2022/10/11.
//

#pragma once
#include <structdefine.h>
#include <resource/shader.h>
/*初始化计数器count为0
 * //m_headPointTex 存储逐像素链表head。每帧开始时，初始化为0xFF.采用头插法。像素为透明时，将对应位置的head数值更新到
 * 记录所有数据buffer的count处，对应位置的head更新为count，然后count++；
 * */
namespace R3D {
    class Device;
    class OIT {
    public:
        GLuint m_headInitBuffer = 0;
        GLuint m_headPointTex = 0;
        GLuint m_atomicCountBuffer = 0;
        GLuint m_fragmentStorageBuffer = 0;
        GLuint m_fragmentStorageTex = 0;
    public:
        void Init(Device* in_device);
        void PrapareData();//重置链表头，计数器
        static OIT *GetInstance();
        void Resolve();
        void Release();
    private:
        OIT();
        static OIT *m_OIT;
        Device* m_device;
        Shader m_resolveShader;
    };
}



