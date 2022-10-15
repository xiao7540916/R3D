#version 460
layout(location = 0)out vec4 FragColor;
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
    float hdrexp;
    float fill1;
    vec3 ambient;
    float fill2;
};
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase ubobasedata;
};
layout(binding = 0)uniform sampler2D screenTex;
in VS_OUT{
    vec2 uv;
}fs_in;

const float edge_threshold_min = 0.0312;
const float edge_threshold_max = 0.125;
const float subpixel_quality = 0.875;
const int iterations = 12;
const float quality[12] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };

float luminosity(vec3 color) {
    return sqrt(dot(color, vec3(0.299, 0.587, 0.114)));
}

vec3 fxaaProcess(sampler2D image, vec2 uv) {
    vec3 center = texture(image, uv).rgb;
    vec2 inversed_size = 1.0 / textureSize(image, 0);

    float luma_center = luminosity(center);
    float luma_down   = luminosity(textureOffset(image, uv, ivec2(0, -1)).rgb);
    float luma_up     = luminosity(textureOffset(image, uv, ivec2(0, 1)).rgb);
    float luma_left   = luminosity(textureOffset(image, uv, ivec2(-1, 0)).rgb);
    float luma_right  = luminosity(textureOffset(image, uv, ivec2(1, 0)).rgb);

    float luma_min = min(luma_center, min(min(luma_down, luma_up), min(luma_left, luma_right)));
    float luma_max = max(luma_center, max(max(luma_down, luma_up), max(luma_left, luma_right)));

    float luma_range = luma_max - luma_min;

    if (luma_range < max(edge_threshold_min, luma_max * edge_threshold_max)){
        return center;
    }

    float luma_down_left  = luminosity(textureOffset(image, uv, ivec2(-1, -1)).rgb);
    float luma_up_right   = luminosity(textureOffset(image, uv, ivec2(1, 1)).rgb);
    float luma_up_left    = luminosity(textureOffset(image, uv, ivec2(-1, 1)).rgb);
    float luma_down_right = luminosity(textureOffset(image, uv, ivec2(1, -1)).rgb);

    float luma_down_up    = luma_down + luma_up;
    float luma_left_right = luma_left + luma_right;

    float luma_left_corners  = luma_down_left + luma_up_left;
    float luma_down_corners  = luma_down_left + luma_down_right;
    float luma_right_corners = luma_down_right + luma_up_right;
    float luma_up_corners    = luma_up_right + luma_up_left;

    float edge_horizontal = abs(-2.0 * luma_left + luma_left_corners) + abs(-2.0 * luma_center + luma_down_up) * 2.0 + abs(-2.0 * luma_right + luma_right_corners);
    float edge_vertical   = abs(-2.0 * luma_up + luma_up_corners) + abs(-2.0 * luma_center + luma_left_right) * 2.0  + abs(-2.0 * luma_down + luma_down_corners);

    bool isHorizontal = edge_horizontal >= edge_vertical;

    float luma1 = isHorizontal ? luma_down : luma_left;
    float luma2 = isHorizontal ? luma_up : luma_right;

    float gradient1 = luma1 - luma_center;
    float gradient2 = luma2 - luma_center;

    bool isSteepest = abs(gradient1) >= abs(gradient2);

    float gradient_scaled = 0.25 * max(abs(gradient1), abs(gradient2));

    float step_length = isHorizontal ? inversed_size.y : inversed_size.x;

    float luma_local_average = 0.0;

    if (isSteepest) {
        step_length *= -1.0;
        luma_local_average = 0.5 * (luma1 + luma_center);
    } else {
        luma_local_average = 0.5 * (luma2 + luma_center);
    }

    vec2 current_uv = uv;
    if (isHorizontal) {
        current_uv.y += step_length * 0.5;
    } else {
        current_uv.x += step_length * 0.5;
    }

    vec2 offset = isHorizontal ? vec2(inversed_size.x, 0.0) : vec2(0.0, inversed_size.y);

    vec2 uv1 = current_uv - offset;
    vec2 uv2 = current_uv + offset;

    float luma_end1 = luminosity(texture(image, uv1).rgb) - luma_local_average;
    float luma_end2 = luminosity(texture(image, uv2).rgb) - luma_local_average;

    bool reached1 = abs(luma_end1) >= gradient_scaled;
    bool reached2 = abs(luma_end2) >= gradient_scaled;
    bool reachedBoth = reached1 && reached2;

    if (!reached1) {
        uv1 -= offset;
    }

    if (!reached2) {
        uv2 += offset;
    }

    if (!reachedBoth) {
        for (int i = 2; i < iterations; ++i) {
            if (!reached1) {
                luma_end1 = luminosity(texture(image, uv1).rgb) - luma_local_average;
            }

            if (!reached2) {
                luma_end2 = luminosity(texture(image, uv2).rgb) - luma_local_average;
            }

            reached1 = abs(luma_end1) >= gradient_scaled;
            reached2 = abs(luma_end2) >= gradient_scaled;
            reachedBoth = reached1 && reached2;

            if (!reached1) {
                uv1 -= offset * quality[i];
            }

            if (!reached2) {
                uv2 += offset * quality[i];
            }

            if (reachedBoth) {
                break;
            }
        }
    }

    float distance1 = isHorizontal ? (uv.x - uv1.x) : (uv.y - uv1.y);
    float distance2 = isHorizontal ? (uv2.x - uv.x) : (uv2.y - uv.y);

    bool isDirection1 = distance1 < distance2;
    float distanceFinal = min(distance1, distance2);

    float edgeThickness = (distance1 + distance2);

    float pixelOffset = - distanceFinal / edgeThickness + 0.5;

    bool isLumaCenterSmaller = luma_center < luma_local_average;

    bool correctVariation = ((isDirection1 ? luma_end1 : luma_end2) < 0.0) != isLumaCenterSmaller;

    float finalOffset = correctVariation ? pixelOffset : 0.0;

    float lumaAverage = (1.0 / 12.0) * (2.0 * (luma_down_up + luma_left_right) + luma_left_corners + luma_right_corners);

    float subPixelOffset1 = clamp(abs(lumaAverage - luma_center) / luma_range, 0.0, 1.0);
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;

    float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * subpixel_quality;

    finalOffset = max(finalOffset, subPixelOffsetFinal);

    vec2 final_uv = uv;
    if (isHorizontal) {
        final_uv.y += finalOffset * step_length;
    } else {
        final_uv.x += finalOffset * step_length;
    }
    return texture(image, final_uv).rgb;
}
void main() {
    vec3 fxaacol = fxaaProcess(screenTex, fs_in.uv);
    //vec3 retonemapcol = pow(fxaacol, vec3(1.0/2.2));
    //vec3 retonemapcol = -log(vec3(1.0)-fxaacol)/ubobasedata.hdrexp;//反色调映射回hdr，用于其他后处理
    vec3 retonemapcol = fxaacol/max((vec3(1)-fxaacol), vec3(0.00001));
    retonemapcol = max(vec3(0),retonemapcol);
    FragColor = vec4(retonemapcol, 1.0);
}
