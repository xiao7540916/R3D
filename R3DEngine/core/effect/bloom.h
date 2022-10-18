//
// Created by Administrator on 2022/10/14.
//

#pragma once
#include <structdefine.h>
#include <array>
namespace R3D {
    using std::array;
    using glm::ivec2;
    class Device;
    class FrameBuffer;
    class Bloom {
    public:
        array<GLuint, BLOOM_ITERATION> m_bloomTex;
        array<ivec2, BLOOM_ITERATION> m_bloomTexSize;
        float m_threshold = 1.0f;//bloom阈值
        float m_softThreshold = 0.5;//用于柔和bloom阈值的控制项
    public:
        static Bloom *GetInstance();
        void Init(Device *in_device);
        void DownSample();
        void UpSample();
        void MergeBloom();
        void Release();
    private:
        Bloom();
        Device *m_device;
        static Bloom *m_bloom;
    };
}



