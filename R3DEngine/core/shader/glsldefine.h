//
// Created by Administrator on 2022/9/16.
//
#pragma once
#include <glm/glm.hpp>
using glm::vec4;
using glm::vec3;
using glm::vec2;
using glm::mat4;
using glm::mat3;

enum LightType {
    LIGHT_AMBIENT = 0,
    LIGHT_DIRECTION = 1,
    LIGHT_POINT = 2,
    LIGHT_SPOT = 3
};
struct DirLight {
    vec3 direction;
    float fill0;
    vec3 strength;
    float fill1;
};
struct UniformBlockBase {
    mat4 viewproj;
    vec3 camerapos;
    float fill0;
    DirLight dirLight0;
    DirLight dirLight1;
    DirLight dirLight2;
    DirLight dirLight3;
};
struct UniformBlockMesh {
    mat4 model;
    mat4 invmodelt;
    vec2 uvoffset;
    vec2 uvscale;
};
