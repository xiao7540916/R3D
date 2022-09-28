//
// Created by Administrator on 2022/9/25.
//

#pragma once
#include <structdefine.h>
#include <unordered_map>
#include <string>
#include <device/render_list.h>
#include <math/hexahedron.h>
#include <math/sphere.h>
#include <shader/glsldefine.h>
#include <array>
namespace R3D {
    using std::unordered_map;
    using std::string;
    using std::array;
    class Object;
    class Device;
    class Camera;
    class Scene {
    public:
        void Init(Device *in_device);
        void SetRoot(Object *in_root);
        void UpdataAnimate(float in_deltaTime, EventInfo &in_eventInfo);//更新动态数据
        void UpdataTransBound();
        void ClearDynamic();
        void DeleteDymaic(Object *in_object);
        void GatherDynamic(Object *in_object);//收集所有动态物体
        void MakeRenderList();
        void SortRenderList();
        void RenderLight();
        void RenderLightRadius();
    public:
        RenderList m_opaqueList;
        RenderList m_transparent;
    public:
        array<DirLight, DIRECTION_LIGHT_COUNT> m_dirLights;
        vector<PointLight> m_pointLights;
    private:
        Object *m_root;
        Device *m_device;
        unordered_map<string, Object *> m_dynamicObjects;
        void ObjectCull(Object *in_object);
        static bool HexahedSphereTest(Hexahedron &in_hexahedron, Sphere &in_sphere);
        static bool CameraSphereTest(const Camera &in_camera, const Sphere &in_sphere);
    };
}



