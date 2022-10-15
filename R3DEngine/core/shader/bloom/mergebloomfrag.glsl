#version 460
layout (binding = 0) uniform sampler2D srcTex;
layout(location = 0) out vec4 FragColor;
uniform float uBloomStrength;
in vec2 uv;
vec3 Sample(vec2 uv){
    return texture(srcTex, uv).rgb;
}

vec3 SampleBox(vec2 uv, vec2 texelSize){
    float radius = 0.5f;
    vec4 o = texelSize.xyxy*vec2(-radius, radius).xxyy;
    vec3 colxy = Sample(uv+o.xy);
    vec3 colzy = Sample(uv+o.zy);
    vec3 colxw = Sample(uv+o.xw);
    vec3 colzw = Sample(uv+o.zw);
    vec3 s = colxy+colzy+colxw+colzw;
    return s*0.25f;
}
void main() {
    vec2 texelSize = 1.0 / textureSize(srcTex, 0);
    vec3 col = uBloomStrength*SampleBox(uv, texelSize);
    FragColor = vec4(col, 1.0f);
}
