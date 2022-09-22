#define STB_IMAGE_IMPLEMENTATION
#include<iostream>
#include<device/device.h>
#include <resource/mesh.h>
#include <resource/mesh_create.h>
#include <resource/material.h>
#include <math/sphere.h>
#include <resource/object.h>
using namespace std;
using namespace R3D;
string CURRENT_SOURCE_DIR = "../../";
string SHADER_DIR = "../../R3DEngine/core/shader/";
void guiMake();
int main() {
    Device *device = Device::GetInstance();
    device->Init("windowtest", 1200, 900, true);
    device->SetCamera(vec3(0, 0, 5), vec3(0, 0, 0), radians(70.0f),
                      float(device->m_windowWidth) / float(device->m_windowHeight),
                      0.1f, 100.0f);
    Gui *gui = Gui::GetInstance();
    BufferManage *bufferManage = BufferManage::GetInstance();
    MeshManage *meshManage = MeshManage::GetInstance();
    ShaderCache &shaderCache = device->m_shaderCache;
    MaterialPhone *materialPhone = new MaterialPhone();
    materialPhone->m_shader = shaderCache.GetShader("phone");
    materialPhone->m_diffTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/diffuse.png";
    materialPhone->m_specTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/spec.png";
    materialPhone->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/phone/circlebox/normal.png";
    materialPhone->InitResource();
    MaterialMetalPbr *materialMetalPbr = new MaterialMetalPbr();
    materialMetalPbr->m_shader = shaderCache.GetShader("metalpbr");
    materialMetalPbr->m_albedoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/bathroomtile/albedo.png";
    materialMetalPbr->m_normalTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/bathroomtile/normal.png";
    materialMetalPbr->m_roughnessTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/bathroomtile/roughness.png";
    materialMetalPbr->m_aoTexUrl = CURRENT_SOURCE_DIR + "Data/image/pbr/bathroomtile/ao.png";
    materialMetalPbr->InitResource();
    MaterialGreen *materialGreen = new MaterialGreen();
    materialGreen->m_shader = shaderCache.GetShader("green");
    Object *rootplane = new Object("rootplane", nullptr, vec3(0), 0, 0, 0, true);
    device->m_opaqueList.m_objectList.push_back(rootplane);
    rootplane->SetMesh(meshManage->GetMesh("planemesh"));
    rootplane->SetMaterial(materialMetalPbr);
    rootplane->m_bndSphMaterial = materialGreen;
    rootplane->Scale(10.0f);
    Object *box = new Object("box", rootplane, vec3(0), 0, 0, 0, true);
    device->m_opaqueList.m_objectList.push_back(box);
    box->SetMesh(meshManage->GetMesh("boxmesh"));
    box->SetMaterial(materialPhone);
    box->m_bndSphMaterial = materialGreen;
    box->Scale(1.0f);
    box->MoveTo(vec3(0, 0.5, 0));
    rootplane->UpdataSubSceneGraph(true);
    rootplane->UpdataBoundSphere(rootplane);
    device->m_opaqueList.Sort();
    for (int i = 0;i < device->m_opaqueList.m_objectList.size();++i) {
        cout << device->m_opaqueList.m_objectList[i]->GetMaterial()->m_shader.ID << endl;
    }
    while (device->Run()) {
        device->Tick();
        glfwPollEvents();
        bufferManage->UpdataUniBaseBuf();
        gui->Begin();
        guiMake();
        gui->End();
        device->m_opaqueList.Render();
        device->m_opaqueList.RenderBndSphere();
        gui->Render();
        glfwSwapBuffers(device->GetWindow());
        while (!device->m_eventInfo.empty()) {
            EventInfo &eventInfo = device->m_eventInfo.front();
            device->UpdataAppInfo(eventInfo);
            device->UpdataInputInfo(eventInfo);
            device->m_eventInfo.pop();
        }
        device->Tock();
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


/*
 *     Sphere sp(1,vec3(0,0,1.1));
    Camera cam;
    cam.SetLens(30,1,0.1,6);
    vec3 campos = vec3(0,6,6);
    vec3 target = vec3(0,0,0);
    cam.SetPosition(campos);
    cam.LookAt(campos, target, vec3(0, 1, 0));
    cam.UpdateViewMatrix();

    vec3 spcenterviewpos = cam.GetView()*vec4(sp.GetCenter(),1.0f);
    PrintVec3(spcenterviewpos);
    Sphere spview(sp.GetRadius(),spcenterviewpos);
    cout<<spview.Intersects(cam.m_frustum)<<endl;
    */