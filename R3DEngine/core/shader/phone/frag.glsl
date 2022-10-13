#version 460
layout(location = 0)out vec4 FragColor;
layout(binding = 0)uniform sampler2D diffTex;
layout(binding = 1)uniform sampler2D specTex;
layout(binding = 2)uniform sampler2D normalTex;
layout(binding = 3)uniform sampler2D dumpTex;
#define DIRECTION_LIGHT_COUNT 4
struct DirLight {
    vec3 direction;
    float fill0;
    vec3 strength;
    float fill1;
};
struct UniformBlockBase {
    mat4 view;
    mat4 proj;
    mat4 invproj;
    mat4 viewproj;
    vec3 camerapos;
    int dirlightactivenum;//平行光启用数目
    DirLight dirLights[DIRECTION_LIGHT_COUNT];
    int pointlightactivenum;//点光源启用数目
    int tilepointlightmax;//单个块最多点光源数目
    float windowwidth;
    float windowheight;
    int workgroup_x;//用于灯光剔除的横向组数
    float znear;
    float zfar;
    int csmlayercount;
    mat4 lightviewprojdata[6];
    float depthbias;
    float normalbias;
    float fill0;
    float fill1;
    vec3 ambient;
    float fill2;
};
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase ubobasedata;
};
in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;
const float INVPI = 0.31831f;
void main() {

    vec3 normal = texture(normalTex, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);// this normal is in tangent space
    normal = fs_in.TBN*normal;

    // get diffuse color
    vec3 color = texture(diffTex, fs_in.TexCoords).rgb;
    vec3 Lo = vec3(0);
    // ambient
    vec3 ambient = 0.0 * color;
    //点光源
    {
        vec3 LightPos = vec3(2, 2, -2);
        vec3 lightstrength = vec3(0);
        // diffuse
        vec3 lightDir = normalize(LightPos - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * color*lightstrength;
        // specular
        vec3 viewDir = normalize(ubobasedata.camerapos - fs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        float specstrength = texture(specTex, fs_in.TexCoords).r;
        vec3 specular = vec3(specstrength) * spec*lightstrength;
        Lo+=diffuse;
        Lo+=specular;
    }
    //方向光
    for (int i = 0; i < DIRECTION_LIGHT_COUNT; ++i)
    {
        // diffuse
        vec3 lightDir = ubobasedata.dirLights[i].direction;
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * color * ubobasedata.dirLights[i].strength;
        // specular
        vec3 viewDir = normalize(ubobasedata.camerapos - fs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        float specstrength = texture(specTex, fs_in.TexCoords).r;
        vec3 specular = vec3(specstrength) * spec * ubobasedata.dirLights[i].strength;
        Lo+=diffuse;
        Lo+=specular;
    }
    FragColor = vec4(Lo, 1.0);
}
