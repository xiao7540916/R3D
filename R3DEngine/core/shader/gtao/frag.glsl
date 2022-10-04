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
    UniformBlockBase block;
}ubobasedata;
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
    float R;
    float NegInvR2;
    vec2 InvQuarterResolution;
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
float rand(vec2 val)
{
    return fract(sin(dot(val, vec2(12.9898f, 78.233f))) * 43758.5453f);
}
vec3 FetchViewPos(vec2 UV)
{
    float depth = texture(depthTex, UV).r;
    vec4 clipPos = vec4(UV * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewPos = ubobasedata.block.invproj * clipPos;
    viewPos /= viewPos.w;
    return viewPos.rgb;
}


vec3 MinDiff(vec3 P, vec3 Pr, vec3 Pl)
{
    vec3 V1 = Pr - P;
    vec3 V2 = P - Pl;
    return (dot(V1, V1) < dot(V2, V2)) ? V1 : V2;
}

vec3 ReconstructNormal(vec2 UV, vec3 P)
{
    vec3 Pr = FetchViewPos(UV + vec2(aoconfigdata.block.InvFullResolution.x, 0));
    vec3 Pl = FetchViewPos(UV + vec2(-aoconfigdata.block.InvFullResolution.x, 0));
    vec3 Pt = FetchViewPos(UV + vec2(0, aoconfigdata.block.InvFullResolution.y));
    vec3 Pb = FetchViewPos(UV + vec2(0, -aoconfigdata.block.InvFullResolution.y));
    return normalize(cross(MinDiff(P, Pr, Pl), MinDiff(P, Pt, Pb)));
}

//距离衰减 NegInvR2为负值，距离平方越大，衰减越强
float Falloff(float DistanceSquare)
{
    return DistanceSquare * aoconfigdata.block.NegInvR2 + 1.0;
}

//P 处理点的相机空间位置，N 处理点相机空间法线S 采样点相机空间位置
float ComputeAO(vec3 P, vec3 N, vec3 S)
{
    vec3 V = S - P;//处理点指向采样点的位置向量
    float VdotV = dot(V, V);
    float NdotV = dot(N, V) * 1.0/sqrt(VdotV);//法线在采样点方向上投影越大，说明越陡峭
    return clamp(NdotV - aoconfigdata.block.NDotVBias, 0, 1) * clamp(Falloff(VdotV), 0, 1);
}

vec2 RotateDirection(vec2 Dir, vec2 CosSin)
{
    return vec2(Dir.x*CosSin.x - Dir.y*CosSin.y,
    Dir.x*CosSin.y + Dir.y*CosSin.x);
}

vec4 GetJitter()
{
    return textureLod(noiseTex, (gl_FragCoord.xy / AO_RANDOMTEX_SIZE), 0);
}

float ComputeCoarseAO(vec2 FullResUV, float RadiusPixels, vec4 Rand, vec3 ViewPosition, vec3 ViewNormal)
{
    // Divide by NUM_STEPS+1 so that the farthest samples are not fully attenuated
    float StepSizePixels = RadiusPixels / (NUM_STEPS + 1);

    const float Alpha = 2.0 * M_PI / NUM_DIRECTIONS;
    float AO = 0;

    for (float DirectionIndex = 0; DirectionIndex < NUM_DIRECTIONS; ++DirectionIndex)
    {
        float Angle = Alpha * DirectionIndex;
        //深度图中的前进方向
        vec2 Direction = RotateDirection(vec2(cos(Angle), sin(Angle)), Rand.xy);
        //随机第一步步长
        float RayPixels = (Rand.z * StepSizePixels + 1.0);

        for (float StepIndex = 0; StepIndex < NUM_STEPS; ++StepIndex)
        {
            //步进点采样uv
            vec2 SnappedUV = round(RayPixels * Direction) * aoconfigdata.block.InvFullResolution + FullResUV;
            vec3 S = FetchViewPos(SnappedUV);//步进点相机空间位置
            RayPixels += StepSizePixels;

            AO += ComputeAO(ViewPosition, ViewNormal, S);
        }
    }

    AO *= aoconfigdata.block.AOMultiplier / (NUM_DIRECTIONS * NUM_STEPS);
    return clamp(1.0 - AO * 2.0, 0, 1);
}

void main()
{
    vec2 uv = texUV;
    float depth = texture(depthTex, uv).r;
    vec3 ViewPosition = FetchViewPos(uv);
    vec3 ViewNormal = texture(viewNormalTex, uv).xyz;
    float RadiusPixels = aoconfigdata.block.RadiusToScreen / (ViewPosition.z);
    vec4 Rand = GetJitter();
    float AO = ComputeCoarseAO(uv, RadiusPixels, Rand, ViewPosition, ViewNormal);
    if(abs(depth-1.0f)<0.00001f){
        AO = 1.0f;
    }
    FragColor = vec4(pow(AO, aoconfigdata.block.PowExponent));
}
