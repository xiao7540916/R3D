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
uniform float TanBias = tan(30.0 * M_PI / 180.0);
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
    vec4 viewPos = ubobasedata.invproj * clipPos;
    viewPos /= viewPos.w;
    return viewPos.rgb;
}

//距离衰减 NegInvR2为负值，距离平方越大，衰减越强
float Falloff(float DistanceSquare)
{
    return DistanceSquare * aoconfigdata.NegInvR2 + 1.0;
}

float PowVec3_2(in vec3 v){
    return dot(v,v);
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

void main()
{
    float depth = texture(depthTex, texUV).r;
    vec3 ViewPosition = FetchViewPos(texUV);
    vec3 ViewNormal = normalize(texture(viewNormalTex, texUV).xyz);
    float RadiusPixels = aoconfigdata.RadiusToScreen / abs(ViewPosition.z);
    vec4 Rand = GetJitter();

    float StepSizePixels = RadiusPixels/(NUM_STEPS+1);
    const float StepAngle = 2.0 * M_PI / NUM_DIRECTIONS;
    float R2 = aoconfigdata.R*aoconfigdata.R;
    float AO = 0;
    for (float DirectionIndex = 0; DirectionIndex < NUM_DIRECTIONS; ++DirectionIndex)
    {
        float Angle = StepAngle * DirectionIndex+M_PI *2.0*Rand.x;
        //深度图中的前进方向
        vec2 Direction = vec2(cos(Angle), sin(Angle));
        //随机第一步步长
        float RayPixels = (Rand.y * StepSizePixels + 1.0);
        //切向角
        vec3 h = vec3(Direction, 0);
        vec3 B = normalize(cross(ViewNormal, h));
        vec3 T = normalize(cross(B,ViewNormal));

        vec3 hightPos = ViewPosition;
        float tanH = BiasedTangent(T);
        float sinH = TanToSin(tanH);
        float tanS;
        float d2;
        vec3 S;
        for (float StepIndex = 0; StepIndex < NUM_STEPS; ++StepIndex)
        {
            //步进点采样uv
            vec2 SnappedUV = RayPixels * Direction * aoconfigdata.InvFullResolution + texUV;
            S = FetchViewPos(SnappedUV);//步进点相机空间位置
            RayPixels += StepSizePixels;
            tanS = Tangent(ViewPosition,S);
            vec3 dv = S-ViewPosition;
            d2 = dot(dv,dv);
            if(tanS>tanH&&d2<R2){
                float sinS = TanToSin(tanS);
                AO += clamp(Falloff(d2),0,1) * (sinS - sinH);
                tanH = tanS;
                sinH = sinS;
            }
        }
    }
    AO/=NUM_DIRECTIONS;
    AO = 1.0-AO;
    if (abs(depth-1.0f)<0.00001f){
        AO = 1.0f;
    }
    FragColor = vec4(AO);
}
