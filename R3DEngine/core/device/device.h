#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <device/glfw_event.h>
#include <device/gui.h>
#include <device/game_time.h>
#include <resource/texture_manage.h>
#include <resource/buffer_manage.h>
#include <resource/render_state_manage.h>
#include <resource/shader_cache.h>
#include <resource/mesh_manage.h>
#include <resource/material.h>
#include <resource/frame_buffer.h>
#include <device/render_list.h>
#include <effect/oit.h>
#include <effect/bloom.h>
#include <effect/depth_of_field.h>
#include <math/camera.h>
#include <util/r_log.h>
#include <queue>
#include <effect/cascaded_shadow_map.h>
namespace R3D {
    class Bloom;
    struct MouseInfo {
        float x = 0;
        float y = 0;
        float lastX;
        float lastY;
        bool leftdown = false;
        bool rightdown = false;
        bool middledown = false;
        bool firstMouse = true;//是否是首次点击
        float xoffset = 0.0f;
        float yoffset = 0.0f;
    };
    struct CameraKeyInfo {
        bool forward = false;
        bool back = false;
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
    };
    class Device {
    public:
        ~Device();
        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;
        static Device *GetInstance();
        Device *Init(const char *in_appname, int in_width, int in_height, int in_csmlayercount, bool in_vsync = true);
        void Release();
        void InitGlfw(const char *in_appname, int in_width, int in_height, bool in_vsync);
        void InitControlSystem();
        void InitGui();
        void InitTextureManage();
        void InitBufferManage();
        void InitCSM(int in_csmlayercount);
        void InitRenderStateManage();
        void InitShaderCache();
        void InitMeshManage();
        void InitMaterialManage();
        void InitOIT();
        void InitBloom();
        void InitDepthOfField();
        void InitFrameBuffers(int in_width, int in_height);
        void SetCamera(vec3 in_position, vec3 in_target, float in_fovy, float in_aspect, float in_zn, float in_zf);
        void UpdataAppInfo(EventInfo &in_eventInfo);
        void UpdataInputInfo(EventInfo &in_eventInfo);
        void UpdataCamera() const;
        void PrepareCSM(Scene &in_scene);
        void UpdataCSM(Scene &in_scene);
        void HDRToLow();
        void FXAA();
        void BloomSurface();
        void DepthOfFieldSurface();
        void HDRToGama();
        bool Run();
        void Tick();//每帧初始时运行
        void Tock();//每帧结束前运行
        GLFWwindow *GetWindow();
        void SetAppName(const char *in_appname);
        FrameBufferColDepthHDR &GetActiveScreenFrame();
        FrameBufferColDepthHDR &GetNotActiveScreenFrame();
        void ExangeActiveScreenFrame();
    public:
        void OnWindowSize(GLFWwindow *window, int width, int height);
        void OnCursorPos(GLFWwindow *window, double xpos, double ypos);
        void OnWindowClose(GLFWwindow *window);
        void OnScroll(GLFWwindow *window, double xoffset, double yoffset);
        void OnMouseButton(GLFWwindow *window, int button, int action, int mods);
        void OnKey(GLFWwindow *window, int key, int scancode, int action, int mods);
    public:
        int m_windowWidth;
        int m_windowHeight;
        int m_screenWidth;
        int m_screenHeight;
        std::queue<EventInfo> m_eventInfo;
        MouseInfo m_mouseInfo;
        CameraKeyInfo m_cameraKeyInfo;
        GameTime m_gameTime;
        Camera *m_camera = nullptr;
        ShaderCache m_shaderCache;
        MaterialManage *m_materialManage;
        MeshManage *m_meshManage = nullptr;
        RenderList m_opaqueList;
        FrameBufferDepthNormal m_preDepthFBO;
        FrameBufferColDepthHDR m_backHDRFBO;
        FrameBufferColDepthHDR m_postHDRFBO;
        FrameBufferAO m_AOFBO;
        CascadedShadowMap m_cascadedShadowMap;
    private:
        static Device *m_device;
        GLFWwindow *m_window = nullptr;
        GLFWEvent *m_eventControl = nullptr;
        Gui *m_gui = nullptr;
        TextureManage *m_textureManage = nullptr;
        BufferManage *m_bufferManege = nullptr;
        RenderStateManage *m_renderStateManage = nullptr;
        OIT *m_OIT = nullptr;
        Bloom *m_bloom = nullptr;
        DepthOfField *m_depthOfField = nullptr;
        bool runable = false;
        bool m_activeMainFrame = true;
    private:
        Device();
    };
}

