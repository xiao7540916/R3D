//
// Created by Administrator on 2022/10/7.
//

#pragma once
#include <structdefine.h>
#include <vector>
#include <array>
#include <resource/frame_buffer.h>
#include <math/camera.h>
#include <resource/shader.h>
#include <device/render_list.h>
namespace R3D {
    class Scene;
    using std::vector;
    using std::array;
    struct CSMInfo {
        int csmLayerCount;//层数
        int shadowMapSize;//shadowmap大小
    };
    struct CSMData {
        OrthoDate orthoDate;
        mat4 viewproj;
    };
    class CascadedShadowMap {
    public:
        vector<FrameBufferDepth> m_shadowMapFBO;
        vector<CSMData> m_CSMData;
        vector<mat4> m_lightViewProjs;//保存变换矩阵和它的逆矩阵
        vector<float> m_splitPoint;
        vector<array<vec3, 8>> m_viewconepointsW;
        vector<RenderList> m_renderList;
        vector<GLuint64> m_shadowmapHandles; //纹理句柄
        int m_csmLayerCount;
        Shader m_shader;
    public:
        void Init(CSMInfo &in_csmInfo);
        void PrepareRenderData(Camera &in_camera, Scene &in_scene);
        void UpdataLightViewProj(Camera &in_camera, Scene &in_scene);//计算
        void MakeRenderList(Scene &in_scene);
        void OpaqueObjectCull(Object *in_object, RenderList &in_renderList);
        void MakeShadowMap(Scene &in_scene);
        void Release();
    };
}



