//
// Created by Administrator on 2022/9/15.
//

#ifndef R3D_STRUCTDEFINE_H
#define R3D_STRUCTDEFINE_H
#include <glm/glm.hpp>
#include <unordered_map>
#include <glad/glad.h>
#define OFFSET(structure, member) ((size_t)&((structure*)0)->member)
#define ZERO(member) (abs(member)<0.00001)
namespace R3D {
    using glm::vec3;
    using glm::vec2;
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
        SYRFACE_PHONE = 1
    };
    struct MaterialInfo {
        SurfaceType surfaceType;//表面模型
        bool castShadow;//投射阴影
        bool receiveShadow;
    };
}
#endif //R3D_STRUCTDEFINE_H
