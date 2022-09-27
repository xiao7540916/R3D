#define STB_IMAGE_IMPLEMENTATION
#include<iostream>
#include<device/device.h>
#include <resource/mesh.h>
#include <resource/mesh_create.h>
#include <resource/material.h>
#include <math/sphere.h>
#include <resource/object.h>
#include <device/scene.h>
using namespace std;
using namespace R3D;
string CURRENT_SOURCE_DIR = "../../";
string SHADER_DIR = "../../R3DEngine/core/shader/";
struct OptionConfig {
    uint32_t FPS;
    uint32_t OpaqueRenderCount;
    bool SphereRender = false;
};
OptionConfig optionConfig{};
void guiMake();
int main() {
    Device *device = Device::GetInstance();
    device->Init("windowtest", 1600, 900, true);
    device->SetCamera(vec3(-8, 3, -8), vec3(0, 0, 0), radians(70.0f),
                      float(device->m_windowWidth) / float(device->m_windowHeight),
                      0.1f, 100.0f);
    Gui *gui = Gui::GetInstance();
    BufferManage *bufferManage = BufferManage::GetInstance();
    MeshManage *meshManage = MeshManage::GetInstance();
    ShaderCache &shaderCache = device->m_shaderCache;
    MaterialManage &materialManage = *device->m_materialManage;
    Scene scene;
    scene.Init(device);
    scene.m_dirLights[0].direction = glm::normalize(vec3(-8, 3, -8));
    scene.m_dirLights[0].strength = vec3(1, 1, 1);
    scene.m_dirLights[1].direction = glm::normalize(vec3(-1, 5, 1));
    scene.m_dirLights[1].strength = vec3(0, 0, 0);
    for (int i = 0;i < scene.m_pointLights.size();++i) {
        scene.m_pointLights[i].position = vec3(RandomFloat(-8, 8), RandomFloat(0, 3), RandomFloat(-8, 8));
        if(i%100 == 0){
            scene.m_pointLights[i].strength = vec3(RandomFloat(0.0, 5), RandomFloat(0.0, 5), RandomFloat(0.0, 5));
        } else{
            scene.m_pointLights[i].strength = vec3(RandomFloat(0.0, 1), RandomFloat(0.0, 1), RandomFloat(0.0, 1));
        }
        scene.m_pointLights[i].constant = 1.0f;
        scene.m_pointLights[i].linear = 0.5f;
        scene.m_pointLights[i].quadratic = 0.2f;
    }
    Object *rootplane = new Object("rootplane", nullptr, vec3(0), 0, 0, 0, true);
    scene.SetRoot(rootplane);
    rootplane->SetMesh(meshManage->GetMesh("planemesh"));
    rootplane->SetMaterial(materialManage.GetMaterial("metalpbr_dirtground"));
    rootplane->SetUvConfig(vec2(0), vec2(2));
    rootplane->Scale(16.0f);
    for (int i = 0;i < 4;++i) {
        Object *matballsub0 = new Object("matballmesh0" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
        matballsub0->SetMesh(meshManage->GetMesh("matballmesh0"));
        matballsub0->SetMaterial(materialManage.GetMaterial("metalpbr_gold"));
        matballsub0->RotationYaw(PI * 0.75f);
        matballsub0->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.0f, (i / 4 - 1.5) * 2.0f));
        if (i == 0) {
            matballsub0->SetActionFunc([&]() {
                static float subroty = 0.0f;
                subroty = device->m_gameTime.TotalTime();
                matballsub0->RotationYaw(subroty);
            });
        }
        Object *matballsub1 = new Object("matballsub1" + IntToString(i), matballsub0, vec3(0), 0, 0, 0, true);
        matballsub1->SetMesh(meshManage->GetMesh("matballmesh1"));
        matballsub1->SetMaterial(materialManage.GetMaterial("metalpbr_rusted_iron"));
    }
    for (int i = 4;i < 8;++i) {
        Object *box = new Object("box" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
        box->SetMesh(meshManage->GetMesh("boxmesh"));
        box->SetMaterial(materialManage.GetMaterial("metalpbr_rusted_iron"));
        box->Scale(1.0f);
        box->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.5, (i / 4 - 1.5) * 2.0f));
    }
    for (int i = 8;i < 12;++i) {
        Object *box = new Object("box" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
        box->SetMesh(meshManage->GetMesh("boxmesh"));
        box->SetMaterial(materialManage.GetMaterial("metalpbr_wood"));
        box->Scale(1.0f);
        box->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.5, (i / 4 - 1.5) * 2.0f));
    }
    rootplane->UpdataSubSceneGraph(true);
    rootplane->UpdataBoundSphere(rootplane);
    scene.GatherDynamic(rootplane);
    scene.MakeRenderList();
    scene.m_opaqueList.Sort();
    while (device->Run()) {
        //数据准备
        device->Tick();
        glfwPollEvents();
        while (!device->m_eventInfo.empty()) {
            EventInfo &eventInfo = device->m_eventInfo.front();
            device->UpdataAppInfo(eventInfo);
            device->UpdataInputInfo(eventInfo);
            scene.UpdataAnimate(device->m_gameTime.DeltaTime(), eventInfo);
            device->m_eventInfo.pop();
        }
        EventInfo eventInfo{};
        eventInfo.type = EVENT_NONE;
        scene.UpdataAnimate(device->m_gameTime.DeltaTime(), eventInfo);
        scene.UpdataTransBound();
        scene.MakeRenderList();
        scene.SortRenderList();
        optionConfig.OpaqueRenderCount = scene.m_opaqueList.m_objectList.size();
        bufferManage->UpdataUniBaseBuf(scene);
        gui->Begin();
        guiMake();
        gui->End();
        //渲染开始
        //深度预渲染
        glBindFramebuffer(GL_FRAMEBUFFER, device->m_preDepthFBO.m_frameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        scene.m_opaqueList.RenderDepth();

        //拷贝pre深度缓冲到后台缓冲，使用深度相等渲染
        glBindFramebuffer(GL_READ_FRAMEBUFFER, device->m_preDepthFBO.m_frameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, device->m_backHDRFBO.m_frameBuffer);
        glBlitFramebuffer(0, 0, device->m_windowWidth, device->m_windowHeight,
                          0, 0, device->m_windowWidth, device->m_windowHeight,
                          GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        //后台HDR缓冲渲染
        glBindFramebuffer(GL_FRAMEBUFFER, device->m_backHDRFBO.m_frameBuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        scene.m_opaqueList.Render();
        scene.RenderLight();
        if (optionConfig.SphereRender) {
            scene.m_opaqueList.RenderBndSphere();
        }
        //windowframe
        glBindFramebuffer(GL_READ_FRAMEBUFFER, device->m_backHDRFBO.m_frameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, device->m_windowWidth, device->m_windowHeight,
                          0, 0, device->m_windowWidth, device->m_windowHeight,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        gui->Render();
        //渲染结束
        glfwSwapBuffers(device->GetWindow());
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
    ImGui::Text("OpaqueCount:%d", int(optionConfig.OpaqueRenderCount));
    ImGui::Checkbox("SphereRender", &optionConfig.SphereRender);
    ImGui::End();
    ImGui::Begin("DepthTex");
    //翻转y轴使图像于屏幕匹配
    ImGui::Image((ImTextureID) Device::GetInstance()->m_preDepthFBO.m_depthAttach, ImVec2(800, 450), ImVec2(0, 1),
                 ImVec2(1, 0));
    ImGui::End();
    ImGui::Begin("BackColTex");
    //翻转y轴使图像于屏幕匹配
    ImGui::Image((ImTextureID) Device::GetInstance()->m_backHDRFBO.m_colorAttach0, ImVec2(800, 450), ImVec2(0, 1),
                 ImVec2(1, 0));
    ImGui::End();
};

/*
 *
 *shader.setInt("speculartex", 1);//等同于layout(binding = 1) uniform sample2D speculartex
 *glActiveTexture(GL_TEXTURE1);
 *glBindTexture(GL_TEXTURE_2D, specularMap);
 * */
