#define STB_IMAGE_IMPLEMENTATION
#include<iostream>
#include<device/device.h>
#include <resource/mesh.h>
#include <resource/mesh_create.h>
#include <resource/material.h>
#include <math/sphere.h>
#include <resource/object.h>
#include <device/scene.h>
#include <resource/cascaded_shadow_map.h>
using namespace std;
using namespace R3D;
string CURRENT_SOURCE_DIR = "../../";
string SHADER_DIR = "../../R3DEngine/core/shader/";
OptionConfig optionConfig{};
bool isMainLoop = true;
uint64_t gNewCount = 0;
unordered_map<void *, string> dstToString;
void guiMake();
int main() {
    Device &device = *Device::GetInstance();
    device.Init("windowtest", 1600, 900, 3, true);
    device.SetCamera(vec3(-8, 3, -8), vec3(0, 0, 0), radians(70.0f),
                     float(device.m_windowWidth) / float(device.m_windowHeight),
                     0.1f, 50.0f);
    BufferManage::GetInstance()->CreateTileClipBuffer(*device.m_camera, device.m_windowWidth, device.m_windowHeight,
                                                      TILE_SIZE);
    Gui &gui = *Gui::GetInstance();
    BufferManage &bufferManage = *BufferManage::GetInstance();
    MeshManage &meshManage = *MeshManage::GetInstance();
    OIT &oit = *OIT::GetInstance();
    ShaderCache &shaderCache = device.m_shaderCache;
    MaterialManage &materialManage = *device.m_materialManage;
    Scene scene;
    scene.Init(&device);
    scene.SetAABB(vec3(-9, -1, -9), vec3(9, 17, 9));
    scene.SetLightCount(1, optionConfig.PointLightCount, TILE_LIGHT_MAX);
    scene.m_dirLights[0].direction = glm::normalize(vec3(-8, 16, -8));
    scene.m_dirLights[0].strength = vec3(6, 6, 6);
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
        pointLight[i].radius = bright * 3;
        pointLight[i].cutoff = pointLight[i].radius * 0.2f;
    }
    Object *rootplane = NEW Object("rootplane", nullptr, vec3(0), 0, 0, 0, true);
    scene.SetRoot(rootplane);
    rootplane->SetMesh(meshManage.GetMesh("planemesh"));
    rootplane->SetMaterial(materialManage.GetMaterial("metalpbr_floor"));
    rootplane->SetUvConfig(vec2(0), vec2(16));
    rootplane->Scale(16.0f);
    //墙面
    {
        Object *wall0 = NEW Object("wall0", rootplane, vec3(0), 0, 0, 0, true);
        wall0->SetMesh(meshManage.GetMesh("boxwallmesh"));
        wall0->SetMaterial(materialManage.GetMaterial("metalpbr_stonewall"));
        wall0->SetUvConfig(vec2(0), vec2(2));
        wall0->Scale(16.0f);
        wall0->Move(vec3(0.0f, 8.0f, 8.0f));
        Object *wall1 = NEW Object("wall1", rootplane, vec3(0), 0, 0, 0, true);
        wall1->SetMesh(meshManage.GetMesh("boxwallmesh"));
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
            Object *matballsub0 = NEW Object("matballmesh0" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
            matballsub0->SetMesh(meshManage.GetMesh("matballmesh0"));
            matballsub0->SetMaterial(materialManage.GetMaterial("metalpbr_silver"));
            matballsub0->RotationYaw(PI * 0.75f);
            matballsub0->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.0f, (i / 4 - 1.5) * 2.0f));
            matballsub0->SetActionFunc([&]() {
                matballsub0->RotationYaw(device.m_gameTime.TotalTime());
            });
            Object *matballsub1 = NEW Object("matballsub1" + IntToString(i), matballsub0, vec3(0), 0, 0, 0, true);
            matballsub1->SetMesh(meshManage.GetMesh("matballmesh1"));
            matballsub1->SetMaterial(materialManage.GetMaterial("metalpbr_rusted_iron"));
        }
        //i=1
        {
            int i = 1;
            Object *matballsub0 = NEW Object("matballmesh0" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
            matballsub0->SetMesh(meshManage.GetMesh("matballmesh0"));
            matballsub0->SetMaterial(materialManage.GetMaterial("metalpbr_silver"));
            matballsub0->RotationYaw(PI * 0.75f);
            matballsub0->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.0f, (i / 4 - 1.5) * 2.0f));
            matballsub0->SetActionFunc([&]() {
                matballsub0->RotationYaw(-device.m_gameTime.TotalTime() * 0.5f);
            });
            Object *matballsub1 = NEW Object("matballsub1" + IntToString(i), matballsub0, vec3(0), 0, 0, 0, true);
            matballsub1->SetMesh(meshManage.GetMesh("matballmesh1"));
            matballsub1->SetMaterial(materialManage.GetMaterial("metalpbr_forestbrown"));
        }
        //i=2
        {
            int i = 2;
            Object *matballsub0 = NEW Object("matballmesh0" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
            matballsub0->SetMesh(meshManage.GetMesh("matballmesh0"));
            matballsub0->SetMaterial(materialManage.GetMaterial("metalpbr_rusted_iron"));
            matballsub0->RotationYaw(PI * 0.75f);
            matballsub0->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.0f, (i / 4 - 1.5) * 2.0f));
            matballsub0->SetActionFunc([&]() {
                matballsub0->RotationYaw(device.m_gameTime.TotalTime() * 0.25f);
            });
            Object *matballsub1 = NEW Object("matballsub1" + IntToString(i), matballsub0, vec3(0), 0, 0, 0, true);
            matballsub1->SetMesh(meshManage.GetMesh("matballmesh1"));
            matballsub1->SetMaterial(materialManage.GetMaterial("metalpbr_gold"));
        }
        //i=3
        {
            int i = 3;
            Object *matballsub0 = NEW Object("matballmesh0" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
            matballsub0->SetMesh(meshManage.GetMesh("matballmesh0"));
            matballsub0->SetMaterial(materialManage.GetMaterial("metalpbr_forestbrown"));
            RouteAction *routeAction = NEW RouteAction();
            float sqlen = 4.5;
            Path path0 = {PATH_CURVED, vec3(-sqlen, 1, -sqlen), vec3(sqlen, 0, -sqlen), vec3(-sqlen, 1, -8),
                          vec3(sqlen, 0, -8)};
            Path path1 = {PATH_STRAIGHT, vec3(sqlen, 0, -sqlen), vec3(sqlen, 0, sqlen)};
            Path path2 = {PATH_CURVED, vec3(sqlen, 0, sqlen), vec3(-sqlen, 0, sqlen), vec3(sqlen, 0, 7),
                          vec3(-sqlen, 0, 7)};
            Path path3 = {PATH_CURVED, vec3(-sqlen, 0, sqlen), vec3(-sqlen, 1, -sqlen), vec3(-sqlen, 0, 0),
                          vec3(-sqlen, 1, -2)};
            routeAction->AddPath(path0);
            routeAction->AddPath(path1);
            routeAction->AddPath(path2);
            routeAction->AddPath(path3);
            routeAction->m_speed = 4.0f;
            routeAction->Init();
            matballsub0->m_routeAction = routeAction;
            matballsub0->SetActionFunc([&]() {
                matballsub0->m_routeAction->m_actionTime += device.m_gameTime.DeltaTime();
                matballsub0->UpdataRouteAction();
            });
            Object *matballsub1 = NEW Object("matballsub1" + IntToString(i), matballsub0, vec3(0), 0, 0, 0, true);
            matballsub1->SetMesh(meshManage.GetMesh("matballmesh1"));
            matballsub1->SetMaterial(materialManage.GetMaterial("metalpbr_silver"));
        }
    }
    for (int i = 4;i < 8;++i) {
        Object *box = NEW Object("box" + IntToString(i), rootplane, vec3(0), 0, 0, 0, true);
        box->SetMesh(meshManage.GetMesh("boxmesh"));
        box->SetMaterial(materialManage.GetMaterial("metalpbr_aluminiumfoil"));
        box->Scale(1.0f);
        box->MoveTo(vec3((i % 4 - 1.5) * 2.0f, 0.5, (i / 4 - 0.5) * 2.0f));
    }
    //体现父子关系的球绕立方体运动
    {
        Object *boxfather0 = NEW Object("boxfather0", rootplane, vec3(0), 0, 0, 0, true);
        boxfather0->SetMesh(meshManage.GetMesh("boxmesh"));
        boxfather0->SetMaterial(materialManage.GetMaterial("metalpbr_floor"));
        Object *spherechild0 = NEW Object("spherechild0", boxfather0, vec3(2, 0, 0), 0, 0, 0, true);
        boxfather0->MoveTo(vec3(0.0f, 2.0f, 0.0f));
        boxfather0->SetActionFunc([&]() {
            boxfather0->MoveTo(vec3(0, 2, sinf(device.m_gameTime.TotalTime()) * 2.0f));
            boxfather0->RotationYaw(device.m_gameTime.TotalTime() * 0.5f);
        });
        spherechild0->SetMesh(meshManage.GetMesh("geospheremesh"));
        spherechild0->SetMaterial(materialManage.GetMaterial("metalpbr_gold"));
        spherechild0->Scale(0.4f);
        spherechild0->SetActionFunc([&]() {
            spherechild0->MoveTo(vec3(0.9f + abs(sinf(device.m_gameTime.TotalTime() * 2.0f)) * 2.0f, 0, 0));
        });
    }
    //透明物体
    {
        Object *boxoit = NEW Object("boxoit" + IntToString(0), rootplane, vec3(0), 0, 0, 0, true);
        boxoit->SetMesh(meshManage.GetMesh("boxmesh"));
        boxoit->SetMaterial(materialManage.GetMaterial("oitgreen"));
        boxoit->Scale(1.0f);
        boxoit->MoveTo(vec3(0, 4, 0));
    }

    //----------------
    rootplane->UpdataSubSceneGraph(true);
    rootplane->UpdataBoundSphere(rootplane);
    scene.GatherDynamic(rootplane);
    while (device.Run()) {
        //数据准备
        device.Tick();
        glfwPollEvents();
        while (!device.m_eventInfo.empty()) {
            EventInfo &eventInfo = device.m_eventInfo.front();
            device.UpdataAppInfo(eventInfo);
            device.UpdataInputInfo(eventInfo);
            scene.UpdataAnimate(device.m_gameTime.DeltaTime(), eventInfo);
            device.m_eventInfo.pop();
        }
        EventInfo eventInfo{};
        eventInfo.type = EVENT_NONE;
        scene.UpdataAnimate(device.m_gameTime.DeltaTime(), eventInfo);
        scene.SetLightCount(1, optionConfig.PointLightCount, TILE_LIGHT_MAX);
        scene.UpdataTransBound();
        scene.MakeRenderList();
        scene.SortRenderList();
        optionConfig.OpaqueRenderCount = scene.m_opaqueList.m_objectList.size();
        //更新shadowmap
        device.PrepareCSM(scene);
        bufferManage.UpdataUniBaseBuf(scene);
        //更新GUI
        gui.Begin();
        guiMake();
        gui.End();
        //渲染开始
        //shadowmap
        glViewport(0, 0, 1024, 1024);
        device.UpdataCSM(scene);
        glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);
        glViewport(0, 0, device.m_windowWidth, device.m_windowHeight);
        //深度预渲染
        glBindFramebuffer(GL_FRAMEBUFFER, device.m_preDepthFBO.m_frameBuffer);
        glClearColor(-1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, BufferManage::GetInstance()->m_uniBlockBaseBuffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, 15, BufferManage::GetInstance()->m_uniCSMHandleBuffer);
        scene.m_opaqueList.RenderDepth();
        //光源剔除
        scene.CullLight();
        glBindFramebuffer(GL_FRAMEBUFFER, device.m_AOFBO.m_frameBuffer);
        scene.MakeAO();
        //拷贝pre深度缓冲到后台缓冲，使用深度相等渲染
        glBindFramebuffer(GL_READ_FRAMEBUFFER, device.m_preDepthFBO.m_frameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, device.m_backHDRFBO.m_frameBuffer);
        glBlitFramebuffer(0, 0, device.m_windowWidth, device.m_windowHeight,
                          0, 0, device.m_windowWidth, device.m_windowHeight,
                          GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        //后台HDR缓冲渲染
        glBindFramebuffer(GL_FRAMEBUFFER, device.m_backHDRFBO.m_frameBuffer);
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
            scene.m_transparent.RenderBndSphere();
        }
        //准备透明渲染所需数据
        oit.PrapareData();
        scene.m_transparent.Render();
        glDepthMask(GL_TRUE);
        oit.Resolve();
        glDepthMask(GL_TRUE);
        //windowframe
        glBindFramebuffer(GL_READ_FRAMEBUFFER, device.m_backHDRFBO.m_frameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, device.m_windowWidth, device.m_windowHeight,
                          0, 0, device.m_windowWidth, device.m_windowHeight,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        gui.Render();
        //渲染结束
        glfwSwapBuffers(device.GetWindow());
        device.Tock();
    }
    glfwTerminate();
    scene.Release();
    device.Release();
    delete &device;
    for (auto &item:dstToString) {
        cout << item.first << " " << item.second << endl;
    }
    cout << gNewCount << endl;
    isMainLoop = false;
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
    ImGui::SliderFloat("DepthBias", &optionConfig.depthbias, 0.0, 0.2);
    ImGui::SliderFloat("NormalBias", &optionConfig.normalbias, 0.0, 0.2);
    ImGui::End();
    ImGui::Begin("DepthTex");
    //翻转y轴使图像于屏幕匹配
    ImGui::Image((ImTextureID) Device::GetInstance()->m_preDepthFBO.m_depthAttach, ImVec2(400, 225), ImVec2(0, 1),
                 ImVec2(1, 0));
    ImGui::End();
    ImGui::Begin("BackColTex");
    //翻转y轴使图像于屏幕匹配
    ImGui::Image((ImTextureID) Device::GetInstance()->m_backHDRFBO.m_colorAttach0, ImVec2(400, 225), ImVec2(0, 1),
                 ImVec2(1, 0));
    ImGui::End();
    ImGui::Begin("ShadowMap0");
    //翻转y轴使图像于屏幕匹配
    ImGui::Image((ImTextureID) Device::GetInstance()->m_cascadedShadowMap.m_shadowMapFBO[0].m_depthAttach, ImVec2(400, 400), ImVec2(0, 1),
                 ImVec2(1, 0));
    ImGui::End();
    ImGui::Begin("AO Config");
    ImGui::SliderFloat("RadiusScale", &optionConfig.radiusScale, 0.01, 2.0);
    ImGui::SliderFloat("PowExponent", &optionConfig.powExponent, 0.1, 3.0);
    ImGui::End();
};

/*
 *
 *shader.setInt("speculartex", 1);//等同于layout(binding = 1) uniform sample2D speculartex
 *glActiveTexture(GL_TEXTURE1);
 *glBindTexture(GL_TEXTURE_2D, specularMap);
 * */
