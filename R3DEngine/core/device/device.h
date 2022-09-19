#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <device/glfw_event.h>
#include <device/gui.h>
#include <device/game_time.h>
#include <resource/texture_manage.h>
#include <resource/buffer_manage.h>
#include <resource/render_state_manage.h>
#include <math/camera.h>
#include <util/r_log.h>
#include <queue>
namespace R3D {
#define EVENT_COUNT_MAX 10
    enum EventType {
        EVENT_NONE = 0,
        EVENT_WINDOW_SIZE,
        EVENT_CURSOR_POS,
        EVENT_WINDOW_CLOSE,
        EVENT_SCROLL,
        EVENT_MOUSE_BUTTON,
        EVENT_KEY
    };
    struct EventInfo {
        EventType type;
        GLFWwindow *window;
        int width;
        int height;
        double xpos;
        double ypos;
        double xoffset;
        double yoffset;
        int button;
        int action;
        int mods;
        int key;
        int scancode;
    };
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
        Device *Init(const char *in_appname, int in_width, int in_height, bool in_vsync = true);
        void Release();
        void InitGlfw(const char *in_appname, int in_width, int in_height, bool in_vsync);
        void InitControlSystem();
        void InitGui();
        void InitTextureManage();
        void InitBufferManage();
        void InitRenderStateManage();
        void SetCamera(vec3 in_position, vec3 in_target, float in_fovy, float in_aspect, float in_zn, float in_zf);
        void UpdataAppInfo(EventInfo &in_eventInfo);
        void UpdataInputInfo(EventInfo &in_eventInfo);
        void UpdataCamera() const;
        bool Run();
        GLFWwindow *GetWindow();
        void SetAppName(const char *in_appname);
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
    private:
        static Device *m_device;
        GLFWwindow *m_window = nullptr;
        GLFWEvent *m_eventControl = nullptr;
        Gui *m_gui = nullptr;
        TextureManage *m_textureManage = nullptr;
        BufferManage *m_bufferManege = nullptr;
        RenderStateManage *m_renderStateManage = nullptr;
        bool runable = false;
    private:
        Device();
    };
}
