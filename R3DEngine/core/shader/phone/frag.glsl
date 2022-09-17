#version 460
layout(location = 0)out vec4 FragColor;
layout(binding = 0)uniform sampler2D diffTex;
layout(binding = 1)uniform sampler2D specTex;
layout(binding = 2)uniform sampler2D normalTex;
layout(binding = 3)uniform sampler2D dumpTex;
in vec2 fraguv;
void main() {
    FragColor = vec4(texture(diffTex, fraguv).rgb, 1.0);
}
