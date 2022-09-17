#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <device/glfw_event.h>
#include <device/gui.h>
#include <resource/texture_manage.h>
#include <resource/buffer_manage.h>
#include <resource/render_state_manage.h>
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

