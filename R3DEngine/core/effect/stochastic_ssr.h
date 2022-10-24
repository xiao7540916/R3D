//
// Created by Administrator on 2022/10/18.
//

#pragma once
#include <structdefine.h>
#include <resource/frame_buffer.h>
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
        int m_activeSssrColIdx = 0;
        GLuint m_hitPixelTex;//存储hit点的pixel坐标
        GLuint m_debugTex;
        array<FrameBufferColHDR, 2> m_lastFrameTex;//保存上一帧未处理过的颜色缓冲，用于与当前帧对比，缓解动态物体sssr时出现的采样不准确问题
        static StochasticSSR *GetInstance();
        void Init(Device *in_device);
        void Release();
        void DownMinDepth();
        void TraceHitPixel();
        void ResloveSSSR();
        void MergeSSSR();
        void ExchangeSssrTexture();
        GLuint GetActiveSssrTex();
        GLuint GetNotActiveSssrTex();
        FrameBufferColHDR& GetActiveFrameColCopy();
        FrameBufferColHDR& GetNotActiveFrameColCopy();
    private:
        StochasticSSR();
        static StochasticSSR *m_stochasticSsr;
        Device *m_device;
    };
}



