#version 460
layout (binding = 0) uniform sampler2D srcTex;
layout (binding = 1) uniform sampler2D cocTex;
layout (binding = 2) uniform sampler2D dofTex;
layout(location = 0) out vec4 FragColor;
in vec2 uv;
vec3 SampleBox(vec2 uv, vec2 texelSize){
    vec4 source = texture(srcTex, uv);
    float coc = texture(cocTex, uv).r;
    vec4 dof = texture(dofTex, uv);
    float dofStrangth = smoothstep(0.1f, 1.0f, abs(coc));
    vec3 col = mix(source.rgb, dof.rgb, dofStrangth);
    return col;
}
void main() {
    vec2 texelSize = 1.0 / textureSize(srcTex, 0);
    vec3 col = SampleBox(uv, texelSize);
    FragColor = vec4(col, 1.0f);
}
