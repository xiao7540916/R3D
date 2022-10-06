#version 460
#define DIRECTION_LIGHT_COUNT 4
#define PI 3.1415926
/*
在相机空间上，以处理点为中心，处理点的z值平面为地平面，测试周围点的相机空间z值。选取仰角最大的点。
*/
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
};
layout(std140, binding = 1) uniform AoConfigBuffer {
    AOConfig block;
}aoconfigdata;

layout (binding = 0) uniform sampler2D depthTex;
layout (binding = 1) uniform sampler2D viewNormalTex;
layout (binding = 2) uniform sampler2D blueNoiseTex;
layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec2 texUV;
float rand(vec2 val)
{
    return fract(sin(dot(val, vec2(12.9898f, 78.233f))) * 43758.5453f);
}
vec3 getViewPos(float depth)
{
    vec4 clipPos = vec4(texUV * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewPos = ubobasedata.block.invproj * clipPos;
    viewPos /= viewPos.w;

    return viewPos.rgb;
}
void main() {
    float fragDepth = texture(depthTex, texUV).r;
    vec3 fragViewNormal = normalize(texture(viewNormalTex, texUV).xyz);
    //生成随机方向
    float currAngle = rand(texUV)*2.0*PI;
    float angleStep = 2.0 * PI / aoconfigdata.block.dirCount;
    float radiusStep = aoconfigdata.block.radiusScale / aoconfigdata.block.stepCount;

    vec3 viewPos = getViewPos(fragDepth);//处理点在相机空间的位置
    vec3 viewDx = dFdx(viewPos);//处理点在相机空间位置x方向的变化率
    vec3 viewDy = dFdy(viewPos);//处理点在相机空间位置y方向的变化率

    float AO = 0;
    for (int i = 0; i < aoconfigdata.block.dirCount; i++)
    {
        //在深度图上选取方向，计算单位像素dx，dy方向像素
        vec2 dir = vec2(cos(currAngle), sin(currAngle));
        vec3 currTan = dir.x * viewDx + dir.y * viewDy;

        float currRad = radiusStep;//当前前进距离
        vec3 highestPos = viewPos;
        bool occluded = false;
        float WAO = 0.0f;
        float tanAngle = atan(currTan.z / length(currTan.xy)) + radians(aoconfigdata.block.angleBias);
        float sintan = sin(tanAngle);
        for (int j = 0; j < aoconfigdata.block.stepCount; j++)
        {
            vec3 currPos = viewPos + currRad * vec3(dir, 0);//z等于处理点z的面上，步进单步距离后的相机空间坐标
            vec4 ndcPos = ubobasedata.block.proj * vec4(currPos, 1.0);
            vec2 newUV = (ndcPos.xy / ndcPos.w + vec2(1.0)) / 2.0;//求出采样点在深度图中的uv

            float currDepth = texture(depthTex, newUV).r;
            vec3 currViewPos = getViewPos(currDepth);//采样点在相机空间的坐标
            //如果还没有遮挡点，只要测试点的相机空间z大于相机空间最高点(初始值为处理点)的z，则最高点赋值成测试点
            if (!occluded){
                if (currViewPos.z > highestPos.z){
                    highestPos = currViewPos;
                    occluded = true;
                    vec3 horVec = highestPos - viewPos;
                    float finalAtt = max(1.0 - pow(length(horVec) / aoconfigdata.block.radiusScale, 2),0);
                    float horAngle = atan(horVec.z / length(horVec.xy));
                    float sinhigh = sin(horAngle);
                    float AOresult = sinhigh - sintan;
                    sintan = sinhigh;
                    WAO+=finalAtt*AOresult;
                    //
                }
            } else {
                //如果已经有遮挡点，则需要比较tan值来确定是否更新最高点
                 if (currViewPos.z > highestPos.z){
                     vec3 dlthighest = highestPos-viewPos;
                     float tanhighest2 = (dlthighest.z*dlthighest.z)/(dlthighest.x*dlthighest.x+dlthighest.y*dlthighest.y);
                     vec3 dltcurr = currViewPos-viewPos;
                     float tancurr2 = (dltcurr.z*dltcurr.z)/(dltcurr.x*dltcurr.x+dltcurr.y*dltcurr.y);
                     if (tancurr2>tanhighest2){
                         highestPos = currViewPos;
                         vec3 horVec = highestPos - viewPos;
                         float finalAtt = max(1.0 - pow(length(horVec) / aoconfigdata.block.radiusScale, 2),0);
                         float horAngle = atan(horVec.z / length(horVec.xy));
                         float sinhigh = sin(horAngle);
                         float AOresult = sinhigh - sintan;
                         sintan = sinhigh;
                         WAO+=finalAtt*AOresult;
                     }
                 }
            }
            currRad += radiusStep;
        }

        currAngle += angleStep;

        if (highestPos == viewPos) continue;

        vec3 horVec = highestPos - viewPos;
        float horAngle = atan(horVec.z / length(horVec.xy));

        float finalAtt = 1.0 - pow(length(horVec) / aoconfigdata.block.radiusScale, 2);
        float AOresult = (sin(horAngle) - sintan)*clamp(finalAtt,0,1);
        if (AOresult < 0.0) continue;
        AO += WAO;
    }

    AO /= aoconfigdata.block.dirCount;
    AO = 1.0-AO;
    if (fragDepth>0.999){
        AO = 1.0f;
    }
    //    AO = 1.0 - pow(AO, aoconfigdata.block.scaleAO);
    FragColor = vec4(AO, AO, AO, 1.0);
}
