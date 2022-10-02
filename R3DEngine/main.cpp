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
    bool LightShowRender = false;
    bool LightRadiusRender = false;
    int PointLightCount = POINT_LIGHT_COUNT;
};
OptionConfig optionConfig{};
void guiMake();
int main() {
    Device *device = Device::GetInstance();
    device->Init("windowtest", 1600, 900, true);
    device->SetCamera(vec3(-8, 3, -8), vec3(0, 0, 0), radians(70.0f),
                      float(device->m_windowWidth) / float(device->m_windowHeight),
                      0.1f, 100.0f);
    BufferManage::GetInstance()->CreateTileClipBuffer(*device->m_camera, device->m_windowWidth, device->m_windowHeight,
                                                      TILE_SIZE);
    Gui *gui = Gui::GetInstance();
    BufferManage *bufferManage = BufferManage::GetInstance();
    MeshManage *meshManage = MeshManage::GetInstance();
    ShaderCache &shaderCache = device->m_shaderCache;
    MaterialManage &materialManage = *device->m_materialManage;
    Scene scene;
    scene.Init(device);
    scene.SetLightCount(0, optionConfig.PointLightCount, TILE_LIGHT_MAX);
    scene.m_dirLights[0].direction = glm::normalize(vec3(-8, 16, -8));
    scene.m_dirLights[0].strength = vec3(10, 10, 10);
    scene.m_dirLights[1].direction = glm::normalize(vec3(-1, 5, 1));
    scene.m_dirLights[1].strength = vec3(0, 0, 0);
    for (int i = 0;i < scene.m_pointLights.size();++i) {
        PointLight *pointLight = &scene.m_pointLights[0];
        pointLight[i].position = vec3(RandomFloat(-16, 16), RandomFloat(0, 16), RandomFloat(-16, 16));
        if (i % 100 == 0) {
            pointLight[i].strength = vec3(RandomFloat(0.0, 8), RandomFloat(0.0, 8), RandomFloat(0.0, 8));
        } else {
            pointLight[i].strength = vec3(RandomFloat(0.0, 3), RandomFloat(0.0, 3), RandomFloat(0.0, 3));
        }
        pointLight[i].constant = 2.0f;
        pointLight[i].linear = 2.0f;
        pointLight[i].quadratic = 2.0f;
//        float bright = glm::dot(vec3(0.3, 0.6, 0.1), pointLight[i].strength);
        float bright = glm::dot(vec3(0.33, 0.33, 0.33), pointLight[i].strength);
        /*float minbright = 0.001f;
        float a = pointLight[i].quadratic;
        float b = pointLight[i].linear;
        float c = pointLight[i].constant - bright / minbright;
        pointLight[i].radius = 0.5f * (sqrt(b * b - 4.0f * a * c) - b) / a;*/
        pointLight[i].radius = bright*3;
        pointLight[i].cutoff = pointLight[i].radius*0.2f;
    }
    Object *rootplane = new Object("rootplane", nullptr, vec3(0), 0, 0, 0, true);
    scene.SetRoot(rootplane);
    rootplane->SetMesh(meshManage->GetMesh("planemesh"));
    rootplane->SetMaterial(materialManage.GetMaterial("metalpbr_floor"));
    rootplane->SetUvConfig(vec2(0), vec2(16));
    rootplane->Scale(16.0f);
    //墙面
    {
        Object *wall0 = new Object("wall0", rootplane, vec3(0), 0, 0, 0, true);
        wall0->SetMesh(meshManage->GetMesh("boxwallmesh"));
        wall0->SetMaterial(materialManage.GetMaterial("metalpbr_stonewall"));
        wall0->SetUvConfig(vec2(0), vec2(2));
        wall0->Scale(16.0f);
        wall0->Move(vec3(0.0f, 8.0f, 8.0f));
        Object *wall1 = new Object("wall1", rootplane, vec3(0), 0, 0, 0, true);
        wall1->SetMesh(meshManage->GetMesh("boxwallmesh"));
        wall1->SetMaterial(materialManage.GetMaterial("metalpbr_stonewall"));
        wall1->SetUvConfig(vec2(0), vec2(2));
        wall1->Scale(16.0f);
        wall1->RotationYaw(PI_DIV_2);
        wall1->Move(vec3(8.0f, 8.0f, 0.0f));
    }
    {
        //i=0
        {
            int i = 0;
            Object *matballsub0 = new Object("matballmesh0" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
            matballsub0->SetMesh(meshManage->GetMesh("matballmesh0"));
            matballsub0->SetMaterial(materialManage.GetMaterial("metalpbr_silver"));
            matballsub0->RotationYaw(PI * 0.75f);
            matballsub0->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.0f, (i / 4 - 1.5) * 2.0f));
            matballsub0->SetActionFunc([&]() {
                matballsub0->RotationYaw(device->m_gameTime.TotalTime());
            });
            Object *matballsub1 = new Object("matballsub1" + IntToString(i), matballsub0, vec3(0), 0, 0, 0, true);
            matballsub1->SetMesh(meshManage->GetMesh("matballmesh1"));
            matballsub1->SetMaterial(materialManage.GetMaterial("metalpbr_rusted_iron"));
        }
        //i=1
        {
            int i = 1;
            Object *matballsub0 = new Object("matballmesh0" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
            matballsub0->SetMesh(meshManage->GetMesh("matballmesh0"));
            matballsub0->SetMaterial(materialManage.GetMaterial("metalpbr_silver"));
            matballsub0->RotationYaw(PI * 0.75f);
            matballsub0->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.0f, (i / 4 - 1.5) * 2.0f));
            matballsub0->SetActionFunc([&]() {
                matballsub0->RotationYaw(-device->m_gameTime.TotalTime() * 0.5f);
            });
            Object *matballsub1 = new Object("matballsub1" + IntToString(i), matballsub0, vec3(0), 0, 0, 0, true);
            matballsub1->SetMesh(meshManage->GetMesh("matballmesh1"));
            matballsub1->SetMaterial(materialManage.GetMaterial("metalpbr_forestbrown"));
        }
        //i=2
        {
            int i = 2;
            Object *matballsub0 = new Object("matballmesh0" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
            matballsub0->SetMesh(meshManage->GetMesh("matballmesh0"));
            matballsub0->SetMaterial(materialManage.GetMaterial("metalpbr_rusted_iron"));
            matballsub0->RotationYaw(PI * 0.75f);
            matballsub0->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.0f, (i / 4 - 1.5) * 2.0f));
            matballsub0->SetActionFunc([&]() {
                matballsub0->RotationYaw(device->m_gameTime.TotalTime() * 0.25f);
            });
            Object *matballsub1 = new Object("matballsub1" + IntToString(i), matballsub0, vec3(0), 0, 0, 0, true);
            matballsub1->SetMesh(meshManage->GetMesh("matballmesh1"));
            matballsub1->SetMaterial(materialManage.GetMaterial("metalpbr_gold"));
        }
        //i=3
        {
            int i = 3;
            Object *matballsub0 = new Object("matballmesh0" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
            matballsub0->SetMesh(meshManage->GetMesh("matballmesh0"));
            matballsub0->SetMaterial(materialManage.GetMaterial("metalpbr_forestbrown"));
            matballsub0->RotationYaw(PI * 0.75f);
            matballsub0->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.0f, (i / 4 - 1.5) * 2.0f));
            matballsub0->SetActionFunc([&]() {
                matballsub0->RotationYaw(-device->m_gameTime.TotalTime() * 0.125f);
            });
            Object *matballsub1 = new Object("matballsub1" + IntToString(i), matballsub0, vec3(0), 0, 0, 0, true);
            matballsub1->SetMesh(meshManage->GetMesh("matballmesh1"));
            matballsub1->SetMaterial(materialManage.GetMaterial("metalpbr_silver"));
        }
    }
    for (int i = 4;i < 8;++i) {
        Object *box = new Object("box" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
        box->SetMesh(meshManage->GetMesh("boxmesh"));
        box->SetMaterial(materialManage.GetMaterial("metalpbr_aluminiumfoil"));
        box->Scale(1.0f);
        box->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.5, (i / 4 - 0.5) * 2.0f));
    }
    //体现父子关系的球绕立方体运动
    {
        Object *boxfather0 = new Object("boxfather0", rootplane, vec3(0), 0, 0, 0, true);
        boxfather0->SetMesh(meshManage->GetMesh("boxmesh"));
        boxfather0->SetMaterial(materialManage.GetMaterial("metalpbr_floor"));
        Object *spherechild0 = new Object("spherechild0", boxfather0, vec3(2, 0, 0), 0, 0, 0, true);
        boxfather0->MoveTo(vec3(0.0f, 2.0f, 0.0f));
        boxfather0->SetActionFunc([&]() {
            boxfather0->MoveTo(vec3(0, 2, sinf(device->m_gameTime.TotalTime()) * 2.0f));
            boxfather0->RotationYaw(device->m_gameTime.TotalTime() * 0.5f);
        });
        spherechild0->SetMesh(meshManage->GetMesh("geospheremesh"));
        spherechild0->SetMaterial(materialManage.GetMaterial("metalpbr_gold"));
        spherechild0->Scale(0.4f);
        spherechild0->SetActionFunc([&]() {
            spherechild0->MoveTo(vec3(0.9f + abs(sinf(device->m_gameTime.TotalTime() * 2.0f)) * 2.0f, 0, 0));
        });
    }

    //----------------
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
        scene.SetLightCount(0, optionConfig.PointLightCount, TILE_LIGHT_MAX);
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
        //光源剔除
        scene.CullLight();
/*        void *lightcountptr = glMapNamedBuffer(bufferManage->m_tileLightsNumBuffer, GL_READ_ONLY);
        uint32_t *lightcount = (uint32_t *) (lightcountptr);
        for (int i = 0;i < 10;++i) {
            cout<<lightcount[i]<<" ";
        }        cout << endl;
        glUnmapNamedBuffer(bufferManage->m_tileLightsNumBuffer);*/
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
        if (optionConfig.LightShowRender) {
            scene.RenderLightShow();
        }
        if (optionConfig.LightRadiusRender) {
            scene.RenderLightRadius();
        }
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
    ImGui::Checkbox("LightShowRender", &optionConfig.LightShowRender);
    ImGui::Checkbox("LightRadiusRender", &optionConfig.LightRadiusRender);
    ImGui::SliderInt("PointLightCount", &optionConfig.PointLightCount, 0, 1024);
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
