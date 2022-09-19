#define STB_IMAGE_IMPLEMENTATION
#include<iostream>
#include<device/device.h>
#include <resource/texture_manage.h>
#include <resource/mesh.h>
#include <resource/mesh_create.h>
#include <resource/material.h>
#include <math/camera.h>
using namespace std;
using namespace R3D;
string CURRENT_SOURCE_DIR = "C:/CppProjects/R3D/";
string SHADER_DIR = "C:/CppProjects/R3D/R3DEngine/core/shader/";
void guiMake();
int main() {
    Device *device = Device::GetInstance();
    device->Init("windowtest", 800, 600, true);
    device->SetCamera(vec3(0, 0, 5), vec3(0, 0, 0), radians(70.0f),
                      float(device->m_windowWidth) / float(device->m_windowHeight),
                      0.1f, 100.0f);
    Gui *gui = Gui::GetInstance();
    TextureManage *textureManage = TextureManage::GetInstance();
    BufferManage *bufferManage = BufferManage::GetInstance();
    Shader phone;
    vector<ShaderCreateDesc> phonedesc = {
            {SHADER_DIR + "phone/vert.glsl", GL_VERTEX_SHADER},
            {SHADER_DIR + "phone/frag.glsl", GL_FRAGMENT_SHADER}
    };
    phone.lodShader(phonedesc);
    Shader metalpbr;
    vector<ShaderCreateDesc> metalpbrdesc = {
            {SHADER_DIR + "metalpbr/vert.glsl", GL_VERTEX_SHADER},
            {SHADER_DIR + "metalpbr/frag.glsl", GL_FRAGMENT_SHADER}
    };
    metalpbr.lodShader(metalpbrdesc);
    MaterialPhone *materialPhone = new MaterialPhone();
    materialPhone->m_shader = phone;
    materialPhone->m_diffTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/diffuse.png";
    materialPhone->m_specTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/spec.png";
    materialPhone->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/normal.png";
    materialPhone->InitResource();
    MaterialMetalPbr *materialMetalPbr = new MaterialMetalPbr();
    materialMetalPbr->m_shader = metalpbr;
    materialMetalPbr->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/albedo.png";
    materialMetalPbr->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/normal.png";
    materialMetalPbr->m_metallicTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/metallic.png";
    materialMetalPbr->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/roughness.png";
    materialMetalPbr->m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/rusted_iron/ao.png";
    materialMetalPbr->InitResource();
    Mesh box;
    MeshCreate::CreateBox(box, 1, 1, 1, VERT_POS_NOR_TAN_UV);
    box.SetMaterial(materialMetalPbr);
    mat4 I = mat4(1.0f);
    UniformBlockMesh uniformBlockMesh;
    uniformBlockMesh.model = I;
    uniformBlockMesh.invmodelt = I;
    bufferManage->UpdataUniBaseBuf();
    glNamedBufferSubData(bufferManage->m_uniBlockMeshBuffer, 0, sizeof(UniformBlockMesh), &uniformBlockMesh);
    while (device->Run()) {
        device->m_gameTime.Tick();
        glfwPollEvents();
        bufferManage->UpdataUniBaseBuf();
        gui->Begin();
        guiMake();
        gui->End();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        box.Remder();
        gui->Render();
        glfwSwapBuffers(device->GetWindow());
        device->m_mouseInfo.xoffset = 0.0f;
        device->m_mouseInfo.yoffset = 0.0f;
        while (!device->m_eventInfo.empty()) {
            EventInfo& eventInfo = device->m_eventInfo.front();
            device->UpdataAppInfo(eventInfo);
            device->UpdataInputInfo(eventInfo);
            device->m_eventInfo.pop();
        }
        device->UpdataCamera();
    }
    glfwTerminate();
    device->Release();
    delete device;
    return 0;
}
void guiMake() {
//    ImGui::Begin("Hello, world!");
//    ImGui::End();
//    ImGui::Begin("Hello, world2!");
//    ImGui::End();
};

/*
 *
 *shader.setInt("speculartex", 1);//等同于layout(binding = 1) uniform sample2D speculartex
 *glActiveTexture(GL_TEXTURE1);
 *glBindTexture(GL_TEXTURE_2D, specularMap);
 * */