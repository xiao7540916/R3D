#version 460
layout (binding = 0) uniform sampler2D srcTex;
layout (location = 0) out vec4 FragColor;
in vec2 uv;
void main() {
    vec3 col = texture(srcTex,uv).rgb;
    FragColor = vec4(col, 1.0f);
}
