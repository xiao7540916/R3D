//
// Created by Administrator on 2022/9/15.
//

#ifndef R3D_STRUCTDEFINE_H
#define R3D_STRUCTDEFINE_H
#include <glm/glm.hpp>
#include <unordered_map>
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#endif
using std::cout;
using std::endl;
using std::unordered_map;
using std::string;
//数学常量
#define PI 3.14159265f
#define PI2 6.2831852f
#define PI_DIV_2 1.5707963f
#define PI_DIV_4 0.78539815f
#define PI_INV 0.318309886f

//计算精度
#define EPSILON_E6 ((float)(1E-6))
//角度弧度转换
#define Deg_TO_Rad(deg) (deg*0.0174533f)
#define Rad_TO_Deg(rad) (rad*57.2957796f)
#define OFFSET(structure, member) ((size_t)&((structure*)0)->member)
#define ZERO(member) (abs(member)<EPSILON_E6)
#define EVENT_COUNT_MAX 10
#define DIRECTION_LIGHT_COUNT 4
#define SPOT_LIGHT_COUNT 4
#define POINT_LIGHT_COUNT 1024
#define TILE_LIGHT_MAX 256
#define TILE_SIZE 16
#define OIT_FRAME_WIDTH 2048
#define OIT_FRAME_HEIGHT 2048
#define BLOOM_ITERATION 5
//-----------------------------------------

extern bool g_isMainLoop;
extern uint64_t g_newCount;
extern unordered_map<void *, string> g_dstToString;
inline void *operator new(size_t size, const char *filename, int line) {
    ++g_newCount;
    void *data = malloc(size);
    g_dstToString[data] = "file:" + string(filename) + " line:" + std::to_string(line);
    return data;
}
inline void operator delete(void *p, const char *filename, int line) {}
inline void *operator new(size_t size) {
    return malloc(size);
}
inline void operator delete(void *p) {
    if (g_isMainLoop) {
        if (g_dstToString.find(p) != g_dstToString.end()) {
            --g_newCount;
            g_dstToString.erase(p);
        }
    }
    free(p);
}
#define NEW new(__FILE__,__LINE__)
//-----------------------------------------

namespace R3D {
    using glm::vec4;
    using glm::vec3;
    using glm::vec2;
    using glm::ivec4;
    using glm::ivec3;
    using glm::ivec2;
    using glm::uvec4;
    using glm::uvec3;
    using glm::uvec2;
    using glm::mat3;
    using glm::mat4;

    enum EventType {
        EVENT_NONE = 0,
        EVENT_WINDOW_SIZE,
        EVENT_CURSOR_POS,
        EVENT_WINDOW_CLOSE,
        EVENT_SCROLL,
        EVENT_MOUSE_BUTTON,
        EVENT_KEY
    };
    struct EventInfo {
        EventType type;
        GLFWwindow *window;
        int width;
        int height;
        double xpos;
        double ypos;
        double xoffset;
        double yoffset;
        int button;
        int action;
        int mods;
        int key;
        int scancode;
    };
    enum VertexLayout {
        VERT_POS,
        VERT_POS_COL,
        VERT_POS_NOR,
        VERT_POS_NOR_UV,
        VERT_POS_NOR_TAN_UV
    };
    struct VertexPos {
        vec3 position;
    };
    struct VertexPosCol {
        vec3 position;
        vec3 color;
    };
    struct VertexPosNor {
        vec3 position;
        vec3 normal;
    };
    struct VertexPosNorUv {
        vec3 position;
        vec3 normal;
        vec2 uv;
    };
    struct VertexPosNorTanUv {
        vec3 position;
        vec3 normal;
        vec3 tangent;
        vec2 uv;
        bool operator==(const VertexPosNorTanUv &rhs) const {
            return position == rhs.position && normal == rhs.normal && uv == rhs.uv;
        }
    };
    enum SurfaceType {
        SURFACE_NULL = 0,
        SURFACE_PHONE = 1,
        SURFACE_METALPBR = 2,
        SURFACE_GREEN = 3
    };
    enum MtrQueue {
        MTRQUEUE_BACKGROUND = 1000,
        MTRQUEUE_GEOMETRY = 2000,
        MTRQUEUE_GEOMETRYREF = 2100,
        MTRQUEUE_ALPHATEST = 2450,
        MTRQUEUE_TRANSPARENT = 3000,
        MTRQUEUE_OVERLAY = 4000
    };
    struct MaterialInfo {
        SurfaceType surfaceType;//表面模型
        bool castShadow;//投射阴影
        bool receiveShadow;
    };
    struct AABB {
        vec3 min;
        vec3 max;
    };
}
template<>
struct std::hash<R3D::VertexPosNorTanUv> {
    size_t operator()(R3D::VertexPosNorTanUv const &vertex) const {
        return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
               (hash<glm::vec2>()(vertex.uv) << 1);
    }
};
struct OptionConfig {
    uint32_t FPS;
    uint32_t OpaqueRenderCount;
    bool SphereRender = false;
    bool LightShowRender = false;
    bool LightRadiusRender = false;
    int PointLightCount = 256;
    float radiusScale = 1.5f;//采样周围多大的距离
    float angleBias = 30.0f;
    int dirCount = 10;//trace光线在周围的切分数量
    int stepCount = 5;//采样所需距离的步数
    float attenuation = 0.64f;
    float scaleAO = 1.4f;
    int blurPass = 1;
    float bfSpace = 2.0f;
    float bfRang = 1.0f;
    float powExponent = 2.0f;
    float nDotVBias = 0.1f;
    float lightPosOffset = 10.0f;
    float depthbias = 0.05f;
    float normalbias = 0.05f;
    float hdrExp = 1.5f;
    float bloomStrength = 0.5f;
    float threshold = 3.0f;//bloom阈值
    float softThreshold = 0.5;//用于柔和bloom阈值的控制项
    float focus = 8.0f;//焦距
    float focusRange = 16.0f;//焦距范围
    float bokehRadius = 2.0f;//弥散圈大小
};
#endif //R3D_STRUCTDEFINE_H
