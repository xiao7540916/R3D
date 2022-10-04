#version 460
layout(location = 0) out vec4 FragNormal;
in vec3 viewnormal;
void main() {
    FragNormal = vec4(viewnormal, 1);
}
