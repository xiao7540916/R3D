#include "device.h"
#include<iostream>
namespace R3D {
    Device::Device() {
    }
    Device::~Device() {
    }
    void onresize(GLFWwindow *window, int width, int height) {
        std::cout << width << std::endl;
        glViewport(0, 0, width, height);
    }
    Device *Device::m_device = nullptr;
    Device *Device::GetInstance() {
        if (m_device == nullptr) {
            m_device = new Device();
        }
        return m_device;
    }
    Device *Device::Init(const char *in_appname, int in_width, int in_height, bool in_vsync) {
        static bool onceInit = true;
        Device *device = GetInstance();
        if (onceInit) {
            RLog::Init();
            device->InitGlfw(in_appname, in_width, in_height, in_vsync);
            device->InitControlSystem();
            device->InitGui();
            device->InitTextureManage();
            device->InitBufferManage();
            device->InitRenderStateManage();
            onceInit = false;
        }
        return device;
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
        device->m_textureManage->Release();
        delete m_textureManage;
        device->m_bufferManege->Release();
        delete m_bufferManege;
    }
    GLFWwindow *Device::GetWindow() {
        return m_window;
    }
    void Device::SetAppName(const char *in_appname) {
        glfwSetWindowTitle(m_window, in_appname);
    }
    void Device::OnWindowSize(GLFWwindow *window, int width, int height) {
        if (GetInstance()->m_eventInfo.size() < EVENT_COUNT_MAX) {
            EventInfo info{};
            info.type = EVENT_WINDOW_SIZE;
            info.window = window;
            info.width = width;
            info.height = height;
            GetInstance()->m_eventInfo.push(info);
        }
    }
    void Device::OnCursorPos(GLFWwindow *window, double xpos, double ypos) {
        if (GetInstance()->m_eventInfo.size() < EVENT_COUNT_MAX) {
            EventInfo info{};
            info.type = EVENT_CURSOR_POS;
            info.window = window;
            info.xpos = xpos;
            info.ypos = ypos;
            GetInstance()->m_eventInfo.push(info);
        }
    }
    void Device::OnWindowClose(GLFWwindow *window) {
        if (GetInstance()->m_eventInfo.size() < EVENT_COUNT_MAX) {
            EventInfo info{};
            info.type = EVENT_WINDOW_CLOSE;
            info.window = window;
            GetInstance()->m_eventInfo.push(info);
        }
    }
    void Device::OnScroll(GLFWwindow *window, double xoffset, double yoffset) {
        if (GetInstance()->m_eventInfo.size() < EVENT_COUNT_MAX) {
            EventInfo info{};
            info.type = EVENT_SCROLL;
            info.window = window;
            info.xoffset = xoffset;
            info.yoffset = yoffset;
            GetInstance()->m_eventInfo.push(info);
        }
    }
    void Device::OnMouseButton(GLFWwindow *window, int button, int action, int mods) {
        if (GetInstance()->m_eventInfo.size() < EVENT_COUNT_MAX) {
            EventInfo info{};
            info.type = EVENT_MOUSE_BUTTON;
            info.window = window;
            info.button = button;
            info.action = action;
            GetInstance()->m_eventInfo.push(info);
        }
    }
    void Device::OnKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (GetInstance()->m_eventInfo.size() < EVENT_COUNT_MAX) {
            EventInfo info{};
            info.type = EVENT_KEY;
            info.window = window;
            info.key = key;
            info.scancode = scancode;
            info.action = action;
            info.mods = mods;
            GetInstance()->m_eventInfo.push(info);
        }
    }
    void Device::InitControlSystem() {
        Device *device = GetInstance();
        device->m_eventControl = GLFWEvent::GetInstance();
        device->m_eventControl->Init(device);
    }
    void Device::InitGui() {
        Device *device = GetInstance();
        device->m_gui = Gui::GetInstance();
        device->m_gui->Init(device);
    }
    void Device::InitTextureManage() {
        Device *device = GetInstance();
        device->m_textureManage = TextureManage::GetInstance();
        device->m_textureManage->Init();
    }
    void Device::InitGlfw(const char *in_appname, int in_width, int in_height, bool in_vsync) {
        Device *device = GetInstance();
        device->runable = true;
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_DECORATED, GL_FALSE);//窗口边框
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);//隐藏窗口

        device->m_windowWidth = in_width;
        device->m_windowHeight = in_height;
        device->m_window = glfwCreateWindow(in_width, in_height, in_appname, nullptr, nullptr);
        if (device->m_window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        //窗口居中
        GLFWmonitor *wmonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *wvidmode = glfwGetVideoMode(wmonitor);
        device->m_screenWidth = wvidmode->width;
        device->m_screenHeight = wvidmode->height;
        glfwSetWindowPos(device->m_window,
                         (device->m_screenWidth - device->m_windowWidth) / 2,
                         (device->m_screenHeight - device->m_windowHeight) / 2);
        glfwShowWindow(device->m_window);
        glfwSetWindowSizeCallback(device->m_window, onresize);
        glfwMakeContextCurrent(device->m_window);
        glfwSwapInterval(in_vsync ? 1 : 0);//是否启用垂直同步
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
        glViewport(0, 0, in_width, in_height);
    }
    void Device::InitBufferManage() {
        Device *device = GetInstance();
        device->m_bufferManege = BufferManage::GetInstance();
        device->m_bufferManege->Init();
    }
    void Device::InitRenderStateManage() {
        Device *device = GetInstance();
        device->m_renderStateManage = RenderStateManage::GetInstance();
    }
}
