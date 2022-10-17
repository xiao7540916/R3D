//
// Created by Administrator on 2022/10/15.
//

#pragma once
#include <structdefine.h>
namespace R3D {
    class Device;
    class DepthOfField {
    public:
        GLuint m_COCTex;
        GLuint m_halfSizeTex0;//用于降采样和模糊的目标纹理
        GLuint m_halfSizeTex1;
        float m_focus = 5;//焦距距离
        float m_focusRange = 2;//焦距范围
        float m_bokehRadius = 4;//弥散圈大小
    public:
        static DepthOfField *GetInstance();
        void Init(Device *in_device);
        void MakeCOC();
        void DownSample();
        void COCDownSample();
        void CircleSample();
        void MergeDOF();
        void Release();
    private:
        DepthOfField();
        static DepthOfField *m_depthoffiled;
        Device *m_device;
    };
}



