#version 460
layout(location = 0)out vec4 FragColor;
layout(binding = 0)uniform sampler2D diffTex;
layout(binding = 1)uniform sampler2D specTex;
layout(binding = 2)uniform sampler2D normalTex;
layout(binding = 3)uniform sampler2D dumpTex;
struct UniformBlockBase {
    mat4 viewproj;
    vec3 camerapos;
    float fill0;
};
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase block;
}ubobasedata;
in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;
const float INVPI = 0.31831f;
void main() {
    vec3 LightPos = vec3(2,2,-2);
    vec3 normal = texture(normalTex, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    normal = fs_in.TBN*normal;

    // get diffuse color
    vec3 color = texture(diffTex, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.0 * color;
    // diffuse
    vec3 lightDir = normalize(LightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(ubobasedata.block.camerapos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    float specstrength = texture(specTex,fs_in.TexCoords).r;
    vec3 specular = vec3(specstrength) * spec;
    FragColor = vec4(INVPI*(ambient + diffuse + specular), 1.0);
}
