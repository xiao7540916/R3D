#include "r_device.h"
#include<iostream>
namespace R3D {
    RDevice::RDevice() {
    }
    RDevice::~RDevice() {
    }
    RDevice *RDevice::m_device = nullptr;
    RDevice *RDevice::GetDevice() {
        if (m_device == nullptr) {
            m_device = new RDevice();
        }
        return m_device;
    }
    RDevice *RDevice::Init(const char *in_appname, int in_width, int in_height) {
        RLog::Init();
        static bool onceInit = true;
        RDevice *device = GetDevice();
        if (onceInit) {
            device->runable = true;
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            device->m_window = glfwCreateWindow(in_width, in_height, in_appname, nullptr, nullptr);
            if (device->m_window == nullptr) {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
            }
            glfwMakeContextCurrent(device->m_window);
            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD" << std::endl;
            }
            glViewport(0, 0, in_width, in_height);
            device->InitControlSystem();
            onceInit = false;
        }
        return device;
    }
    void RDevice::InitControlSystem() {
        RDevice *device = GetDevice();
        device->m_eventControl = GLFWEvent::GetInstance();
        device->m_eventControl->Init(device);
    }
    bool RDevice::Run() {
        RDevice *device = GetDevice();
        device->runable = !glfwWindowShouldClose(device->GetWindow());
        return device->runable;
    }
    void RDevice::Release() {
        RDevice *device = GetDevice();
        device->m_eventControl->Release();
        delete device->m_eventControl;
    }
    GLFWwindow *RDevice::GetWindow() {
        return m_window;
    }
    void RDevice::SetAppName(const char *in_appname) {
        glfwSetWindowTitle(m_window, in_appname);
    }
}
