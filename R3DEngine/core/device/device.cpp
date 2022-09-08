#include "device.h"
#include<iostream>
namespace R3D {
    Device::Device() {
    }
    Device::~Device() {
    }
    Device *Device::m_device = nullptr;
    Device *Device::GetInstance() {
        if (m_device == nullptr) {
            m_device = new Device();
        }
        return m_device;
    }
    Device *Device::Init(const char *in_appname, int in_width, int in_height, bool in_vsync) {
        RLog::Init();
        static bool onceInit = true;
        Device *device = GetInstance();

        if (onceInit) {
            device->runable = true;
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            //glfwWindowHint(GLFW_DECORATED, GL_FALSE);//窗口边框

            device->m_window = glfwCreateWindow(in_width, in_height, in_appname, nullptr, nullptr);
            if (device->m_window == nullptr) {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return nullptr;
            }
            glfwMakeContextCurrent(device->m_window);
            glfwSwapInterval(in_vsync ? 1 : 0);//是否启用垂直同步
            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD" << std::endl;
            }
            glViewport(0, 0, in_width, in_height);
            device->InitControlSystem();
            onceInit = false;
        }
        return device;
    }
    void Device::InitControlSystem() {
        Device *device = GetInstance();
        device->m_eventControl = GLFWEvent::GetInstance();
        device->m_eventControl->Init(device);
    }
    bool Device::Run() {
        Device *device = GetInstance();
        device->runable = !glfwWindowShouldClose(device->GetWindow());
        return device->runable;
    }
    void Device::Release() {
        Device *device = GetInstance();
        device->m_eventControl->Release();
        delete device->m_eventControl;
    }
    GLFWwindow *Device::GetWindow() {
        return m_window;
    }
    void Device::SetAppName(const char *in_appname) {
        glfwSetWindowTitle(m_window, in_appname);
    }
}
