//
// Created by Administrator on 2022/9/16.
//

#include <glm/glm.hpp>
using glm::vec4;
using glm::vec3;
using glm::vec2;
using glm::mat4;
using glm::mat3;

struct UniformBlockBase {
    mat4 viewproj;
    vec3 camerapos;
    float fill0;
};
struct UniformBlockMesh{
    mat4 model;
    mat4 invmodelt;
};
