#version 460
#define DIRECTION_LIGHT_COUNT 4
#define M_PI 3.14159265f
#define AO_RANDOMTEX_SIZE 256
const float  NUM_STEPS = 4;
const float  NUM_DIRECTIONS = 8;
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
    UniformBlockBase ubobasedata;
};
struct AOConfig{
    float radiusScale;//采样周围多大的距离
    float angleBias;
    int dirCount;//trace光线在周围的切分数量
    int stepCount;//采样所需距离的步数
    float attenuation;
    float scaleAO;
    int blurPass;
    float bfSpace;
    float bfRang;
    float fill0;
    float fill1;
    float fill2;
    vec4 projInfo;
    vec2 InvFullResolution;
    vec2 InvQuarterResolution;
    float R;
    float NegInvR2;
    float RadiusToScreen;
    float PowExponent;
    float NDotVBias;
    float AOMultiplier;
    float fill3;
    float fill4;
};
layout(std140, binding = 1) uniform AoConfigBuffer {
    AOConfig block;
}aoconfigdata;

layout (binding = 0) uniform sampler2D depthTex;
layout (binding = 1) uniform sampler2D viewNormalTex;
layout (binding = 2) uniform sampler2D noiseTex;
layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec2 texUV;
const float PI = 3.14159265;
uniform float u_Near = 0.1;
uniform float u_Far = 100.0f;
uniform float u_Fov = radians(70.0f);
uniform float u_WindowWidth = 1600;
uniform float u_WindowHeight = 900;
uniform vec2 u_FocalLen;
uniform float u_AOStrength = 1.9;
uniform float R = 0.3;
uniform float R2 = 0.3 * 0.3;
uniform float NegInvR2 = - 1.0 / (0.3 * 0.3);
uniform float TanBias = tan(30.0 * PI / 180.0);
uniform float MaxRadiusPixels = 50.0;

uniform int NumDirections = 6;
uniform int NumSamples = 3;

float ViewSpaceZFromDepth(float d)
{
    d = d * 2.0 - 1.0;
    //视线坐标系看向的z轴负方向，因此要求视觉空间的z值应该要把线性深度变成负值
    return -(2.0 * u_Near * u_Far) / (u_Far + u_Near - d * (u_Far - u_Near));
}

vec3 UVToViewSpace(vec2 uv, float z)
{
    uv = uv * 2.0 - 1.0;
    uv.x = uv.x * tan(u_Fov / 2.0) * u_WindowWidth / u_WindowHeight  * z ;
    uv.y = uv.y * tan(u_Fov / 2.0)  * z ;
    return vec3(-uv, z);
}

vec3 GetViewPos(vec2 uv)
{
    float z = ViewSpaceZFromDepth(texture(depthTex, uv).r);
    return UVToViewSpace(uv, z);
}

float TanToSin(float x)
{
    return x * inversesqrt(x*x + 1.0);
}

float InvLength(vec2 V)
{
    return inversesqrt(dot(V,V));
}

float Tangent(vec3 V)
{
    return V.z * InvLength(V.xy);
}

float BiasedTangent(vec3 V)
{
    return V.z * InvLength(V.xy) + TanBias;
}

float Tangent(vec3 P, vec3 S)
{
    return Tangent(S - P);
}

float Length2(vec3 V)
{
    return dot(V,V);
}

vec3 MinDiff(vec3 P, vec3 Pr, vec3 Pl)
{
    vec3 V1 = Pr - P;
    vec3 V2 = P - Pl;
    return (Length2(V1) < Length2(V2)) ? V1 : V2;
}

vec2 SnapUVOffset(vec2 uv)
{
    return round(uv * vec2(u_WindowWidth,u_WindowHeight)) * vec2(1.0 / u_WindowWidth,1.0 / u_WindowHeight);
}

float Falloff(float d2)
{
    return d2 * NegInvR2 + 1.0f;
}

float HorizonOcclusion(	vec2 deltaUV,
vec3 P,
vec3 dPdu,
vec3 dPdv,
float randstep,
float numSamples)
{
    float ao = 0;
    vec2 uv = TexCoord + SnapUVOffset(randstep*deltaUV);
    deltaUV = SnapUVOffset(deltaUV);
    vec3 T = deltaUV.x * dPdu + deltaUV.y * dPdv;
    float tanH = BiasedTangent(T);
    float sinH = TanToSin(tanH);
    float tanS;
    float d2;
    vec3 S;
    for(float s = 1; s <= numSamples; ++s)
    {
        uv += deltaUV;
        S = GetViewPos(uv);
        tanS = Tangent(P, S);
        d2 = Length2(S - P);
        if(d2 < R2 && tanS > tanH)
        {
            float sinS = TanToSin(tanS);
            ao += Falloff(d2) * (sinS - sinH);
            tanH = tanS;
            sinH = sinS;
        }
    }

    return ao;
}

vec2 RotateDirections(vec2 Dir, vec2 CosSin)
{
    return vec2(Dir.x*CosSin.x - Dir.y*CosSin.y,
    Dir.x*CosSin.y + Dir.y*CosSin.x);
}

void ComputeSteps(inout vec2 stepSizeUv, inout float numSteps, float rayRadiusPix, float rand)
{
    numSteps = min(NumSamples, rayRadiusPix);

    float stepSizePix = rayRadiusPix / (numSteps + 1);

    float maxNumSteps = MaxRadiusPixels / stepSizePix;
    if (maxNumSteps < numSteps)
    {
        numSteps = floor(maxNumSteps + rand);
        numSteps = max(numSteps, 1);
        stepSizePix = MaxRadiusPixels / numSteps;
    }

    stepSizeUv = stepSizePix * vec2(1.0 / u_WindowWidth,1.0 / u_WindowHeight);;
}

void main(void)
{
    vec2 NoiseScale = vec2(u_WindowWidth/4.0f, u_WindowHeight/4.0f);
    float numDirections = NumDirections;

    vec3 P, Pr, Pl, Pt, Pb;
    P 	= GetViewPos(TexCoord);
    Pr 	= GetViewPos(TexCoord + vec2( 1.0 / u_WindowWidth, 0));
    Pl 	= GetViewPos(TexCoord + vec2(-1.0 / u_WindowWidth, 0));
    Pt 	= GetViewPos(TexCoord + vec2( 0, 1.0 / u_WindowHeight));
    Pb 	= GetViewPos(TexCoord + vec2( 0,-1.0 / u_WindowHeight));
    vec3 dPdu = MinDiff(P, Pr, Pl);
    vec3 dPdv = MinDiff(P, Pt, Pb) * (u_WindowHeight * 1.0 / u_WindowWidth);
    vec3 random = texture(noiseTex, texUV.xy * NoiseScale).rgb;
    vec2 rayRadiusUV = 0.5 * R * u_FocalLen / -P.z;
    float rayRadiusPix = rayRadiusUV.x * u_WindowWidth;
    float ao = 1.0;
    float numSteps;
    vec2 stepSizeUV;
    if(rayRadiusPix > 1.0)
    {
        ao = 0.0;
        ComputeSteps(stepSizeUV, numSteps, rayRadiusPix, random.z);
        float alpha = 2.0 * PI / numDirections;
        for(float d = 0; d < numDirections; ++d)
        {
            float theta = alpha * d;
            vec2 dir = RotateDirections(vec2(cos(theta), sin(theta)), random.xy);
            vec2 deltaUV = dir * stepSizeUV;
            ao += HorizonOcclusion(deltaUV,P,dPdu,dPdv,random.z,numSteps);
        }
        ao = 1.0 - ao / numDirections * u_AOStrength;
    }
    FragColor = vec4(ao);
}