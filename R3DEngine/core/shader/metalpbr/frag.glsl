#version 460
#define DIRECTION_LIGHT_COUNT 4
#define POINT_LIGHT_COUNT 1024
#define TILE_SIZE 16
#define TILE_LIGHT_MAX 256
layout(location = 0)out vec4 FragColor;
layout(binding = 0)uniform sampler2D albedoTex;
layout(binding = 1)uniform sampler2D normalTex;
layout(binding = 2)uniform sampler2D metalTex;
layout(binding = 3)uniform sampler2D roughnessTex;
layout(binding = 4)uniform sampler2D aoTex;

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
    int dirlightactivenum;
    DirLight dirLights[DIRECTION_LIGHT_COUNT];
    int pointlightactivenum;
    int tilepointlightmax;
    float windowwidth;
    float windowheight;
    int workgroup_x;
    float fill0;
    float fill1;
    float fill2;
};
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase block;
}ubobasedata;
struct PointLight{
    vec3 position;
    float constant;
    vec3 strength;
    float linear;
    float quadratic;
    float radius;
    float cutoff;
    float fill2;
};
layout (std430, binding = 0) buffer PointLightBuffer {
    PointLight pointLightData[POINT_LIGHT_COUNT];
};
layout (std430, binding = 1) readonly buffer visible_lights_indices {
    uint lights_indices[];
};
layout (std430, binding = 2) readonly buffer visible_lights_count {
    uint lights_count[];
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

const float PI = 3.14159265359;
vec3 GetNormalFromMap()
{
    vec3 normal = texture(normalTex, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    return normalize(fs_in.TBN*normal);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
float getAttenuation(float radius,float cutoff,float distance){
    return smoothstep(radius,cutoff,distance);
}
void main() {
    vec3 albedo     = pow(texture(albedoTex, fs_in.TexCoords).rgb, vec3(2.2));//转到线性空间
    float metallic  = texture(metalTex, fs_in.TexCoords).r;
    float roughness = texture(roughnessTex, fs_in.TexCoords).r;
    float ao        = texture(aoTex, fs_in.TexCoords).r;

    vec3 camPos = ubobasedata.block.camerapos;
    vec3 WorldPos = fs_in.FragPos;
    vec3 N = GetNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    ivec2 loc = ivec2(gl_FragCoord.xy);
    ivec2 tileID = loc / ivec2(TILE_SIZE, TILE_SIZE);
    uint index = tileID.y * ubobasedata.block.workgroup_x + tileID.x;
    uint lightcount = lights_count[index];
    uint offset = index * TILE_LIGHT_MAX;
    //点光源
    for (int i = 0; i < lightcount; ++i)
    {
        uint lightindex = lights_indices[offset + i];
        // calculate per-light radiance
        vec3 L = normalize(pointLightData[lightindex].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLightData[lightindex].position - WorldPos);
//        float attenuation = 1.0 / (pointLightData[lightindex].constant + pointLightData[lightindex].linear * distance +
//        pointLightData[lightindex].quadratic * (distance * distance));
        float attenuation = getAttenuation(pointLightData[lightindex].radius,pointLightData[lightindex].cutoff,distance);
        vec3 radiance = pointLightData[lightindex].strength * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;// 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }
    //方向光
    for (int i = 0; i < ubobasedata.block.dirlightactivenum; ++i)
    {
        // calculate per-light radiance
        vec3 L = ubobasedata.block.dirLights[i].direction;
        vec3 H = normalize(V + L);
        vec3 radiance = ubobasedata.block.dirLights[i].strength;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;// 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }


    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.0) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
//    FragColor = vec4(vec2(gl_FragCoord.xy/1600.0f),0.0, 1.0);
}
