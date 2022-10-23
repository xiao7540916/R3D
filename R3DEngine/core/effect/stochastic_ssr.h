//
// Created by Administrator on 2022/10/18.
//

#pragma once
#include <structdefine.h>
#include <array>
#define DEPTH_DOWN_LEVEL 8
namespace R3D {
    using std::array;
    class Device;
    class StochasticSSR {
    public:
        array<GLuint, DEPTH_DOWN_LEVEL> m_downDepthTex{0};
        array<glm::ivec2, DEPTH_DOWN_LEVEL> m_downDepthTexSize;
        array<GLuint, 2> m_sssrColTex{0};
        GLuint m_hitPixelTex;//存储hit点的pixel坐标
        GLuint m_debugTex;
        static StochasticSSR *GetInstance();
        void Init(Device *in_device);
        void Release();
        void DownMinDepth();
        void TraceHitPixel();
    private:
        StochasticSSR();
        static StochasticSSR *m_stochasticSsr;
        Device *m_device;
    };
}



