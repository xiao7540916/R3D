//
// Created by Administrator on 2022/9/15.
//

#ifndef R3D_STRUCTDEFINE_H
#define R3D_STRUCTDEFINE_H
#include <glm/glm.hpp>
#include <unordered_map>
#include <glad/glad.h>
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
}
#endif //R3D_STRUCTDEFINE_H
