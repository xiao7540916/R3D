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
uint32_t FPS = 0;
void guiMake();
int main() {
    Device *device = Device::GetInstance();
    device->Init("windowtest", 1200, 900, false);
    device->SetCamera(vec3(0, 1, 5), vec3(0, 0, 0), radians(70.0f),
                      float(device->m_windowWidth) / float(device->m_windowHeight),
                      0.1f, 100.0f);
    Gui *gui = Gui::GetInstance();
    BufferManage *bufferManage = BufferManage::GetInstance();
    MeshManage *meshManage = MeshManage::GetInstance();
    ShaderCache &shaderCache = device->m_shaderCache;
    MaterialManage &materialManage = *device->m_materialManage;
    Object *rootplane = new Object("rootplane", nullptr, vec3(0), 0, 0, 0, true);
    device->m_opaqueList.m_objectList.push_back(rootplane);
    rootplane->SetMesh(meshManage->GetMesh("planemesh"));
    rootplane->SetMaterial(materialManage.GetMaterial("metalpbr_bathroomtile"));
    rootplane->SetUvConfig(vec2(0), vec2(5));
    rootplane->Scale(10.0f);
    Object *box = new Object("box", rootplane, vec3(0), 0, 0, 0, true);
    device->m_opaqueList.m_objectList.push_back(box);
    box->SetMesh(meshManage->GetMesh("boxmesh"));
    box->SetMaterial(materialManage.GetMaterial("phone_circlebox"));
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
    ImGui::Begin("Info");
    static uint32_t fpsFrameCount = 0;
    static GameTime &gametime = Device::GetInstance()->m_gameTime;
    static float deltatime = gametime.DeltaTime() * 100.0f;
    static float lasttime = gametime.TotalTime();
    static float nowtime;
    if (fpsFrameCount < 100) {
        fpsFrameCount++;
    } else {
        fpsFrameCount = 0;
        nowtime = gametime.TotalTime();
        deltatime = nowtime - lasttime;
        lasttime = nowtime;
    }
    ImGui::Text("FPS:%d", int(100.0f / deltatime));
    ImGui::End();
    ImGui::Begin("DepthTex");
    ImGui::Image((ImTextureID)TextureManage::GetInstance()->m_urlToTexture["../../Data/image/phone/circlebox/diffuse.png"],ImVec2(512,512));
    ImGui::End();
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