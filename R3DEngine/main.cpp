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
    Camera camera;
    camera.SetLens(radians(70.0f), (float) 800 / (float) 600, 0.5f, 100.0f);
    vec3 campos(0, 1, 5);
    vec3 camtarget(0, 0, 0);
    vec3 worldup(0, 1, 0);
    camera.SetPosition(campos);
    camera.LookAt(campos, camtarget, worldup);
    camera.UpdateViewMatrix();
    Device *device = Device::GetInstance();
    device->Init("windowtest", 800, 600, true);
    Gui *gui = Gui::GetInstance();
    TextureManage *textureManage = TextureManage::GetInstance();
    BufferManage *bufferManage = BufferManage::GetInstance();
    Shader phone;
    vector<ShaderCreateDesc> phonedesc = {
            {SHADER_DIR + "phone/vert.glsl", GL_VERTEX_SHADER},
            {SHADER_DIR + "phone/frag.glsl", GL_FRAGMENT_SHADER}
    };
    phone.lodShader(phonedesc);
    MaterialPhone *materialPhone = new MaterialPhone();
    materialPhone->m_shader = phone;
    materialPhone->m_diffTexUrl = CURRENT_SOURCE_DIR + "Data/image/Box_Diffuse.png";
    materialPhone->InitResource();
    Mesh box;
    MeshCreate::CreateBox(box, 1, 1, 1, VERT_POS_NOR_TAN_UV);
    box.SetMaterial(materialPhone);
    mat4 I = mat4(1.0f);
    bufferManage->m_uniBlockBase.model = I;
    bufferManage->m_uniBlockBase.viewproj = camera.GetProjection()*camera.GetView();
    glNamedBufferSubData(bufferManage->m_uniBlockBaseBuffer, 0, sizeof(UniformBlockBase), &bufferManage->m_uniBlockBase);
    while (device->Run()) {
        glfwPollEvents();
        gui->Begin();
        guiMake();
        gui->End();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        box.Remder();
        box.Remder();
        box.Remder();
        gui->Render();
        glfwSwapBuffers(device->GetWindow());
        while (!device->m_eventInfo.empty()) {
            device->m_eventInfo.pop();
        }
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