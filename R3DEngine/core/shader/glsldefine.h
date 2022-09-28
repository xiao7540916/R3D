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
#define DIRECTION_LIGHT_COUNT 4
#define SPOT_LIGHT_COUNT 4
#define POINT_LIGHT_COUNT 1024
#define TILE_LIGHT_MAX 128
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
struct PointLight {
    vec3 position;
    float constant;
    vec3 strength;
    float linear;
    float quadratic;
    float radius;
    float fill1;
    float fill2;
};
struct UniformBlockBase {
    mat4 viewproj;
    vec3 camerapos;
    int dirlightenable;
    DirLight dirLights[DIRECTION_LIGHT_COUNT];
    int pointlightenable;
    int tilepointlightmax;
    float fill1;
    float fill2;
};
struct UniformBlockMesh {
    mat4 model;
    mat4 invmodelt;
    vec2 uvoffset;
    vec2 uvscale;
};
