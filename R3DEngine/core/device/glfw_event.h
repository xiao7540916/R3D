#pragma once
namespace R3D {
    class Device;
    class GLFWEvent {
    public:
        ~GLFWEvent();
        static GLFWEvent *GetInstance();
        void Init(Device *in_device);
        void Release();
        Device *GetDevice();
    private:
        GLFWEvent();
        static GLFWEvent *m_glfwEvent;
        Device *m_device;
    };
}