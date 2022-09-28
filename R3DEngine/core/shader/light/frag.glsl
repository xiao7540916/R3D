#version 460
layout(location = 0)out vec4 FragColor;
layout(binding = 0)uniform sampler2D albedoTex;
layout(binding = 1)uniform sampler2D normalTex;
layout(binding = 2)uniform sampler2D metalTex;
layout(binding = 3)uniform sampler2D roughnessTex;
layout(binding = 4)uniform sampler2D aoTex;

#define DIRECTION_LIGHT_COUNT 4
struct DirLight {
    vec3 direction;
    float fill0;
    vec3 strength;
    float fill1;
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
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase block;
}ubobasedata;


const float PI = 3.14159265359;
in vec3 lightcolor;
void main() {
    FragColor = vec4(lightcolor, 1.0);
}
