//
// Created by Administrator on 2022/9/15.
//

#ifndef R3D_STRUCTDEFINE_H
#define R3D_STRUCTDEFINE_H
#include <glm/glm.hpp>
#include <unordered_map>
#include <glad/glad.h>
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
namespace R3D {
    using glm::vec4;
    using glm::vec3;
    using glm::vec2;
    using glm::mat3;
    using glm::mat4;
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
}
#endif //R3D_STRUCTDEFINE_H
