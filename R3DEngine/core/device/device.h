#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<device/glfw_event.h>
#include<util/r_log.h>
namespace R3D {
    class Device {
    public:
        ~Device();
        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;
        static Device *GetInstance();
        Device *Init(const char *in_appname, int in_width, int in_height,bool in_vsync = true);
        void Release();
        void InitControlSystem();
        bool Run();
        GLFWwindow *GetWindow();
        void SetAppName(const char *in_appname);
    private:
        static Device *m_device;
        GLFWwindow *m_window = nullptr;
        GLFWEvent *m_eventControl = nullptr;
        bool runable = false;
    private:
        Device();
    };
}

