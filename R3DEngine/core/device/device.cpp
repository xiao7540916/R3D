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
            device->InitShaderCache();
            device->InitMeshManage();
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
        device->m_meshManage->Release();
        delete m_meshManage;
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
    void Device::InitMeshManage() {
        Device *device = GetInstance();
        device->m_meshManage = MeshManage::GetInstance();
        device->m_meshManage->Init();
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
        m_mouseInfo.lastX = float(in_width >> 1);
        m_mouseInfo.lastY = float(in_height >> 1);
        m_gameTime.Start();
        glViewport(0, 0, in_width, in_height);
    }
    void Device::InitBufferManage() {
        Device *device = GetInstance();
        device->m_bufferManege = BufferManage::GetInstance();
        device->m_bufferManege->Init(device);
    }
    void Device::InitRenderStateManage() {
        Device *device = GetInstance();
        device->m_renderStateManage = RenderStateManage::GetInstance();
    }
    void Device::InitShaderCache() {
        m_shaderCache.Init();
    }
    void
    Device::SetCamera(vec3 in_position, vec3 in_target, float in_fovy, float in_aspect, float in_zn, float in_zf) {
        if (m_camera == nullptr) {
            m_camera = new Camera();
            m_camera->SetLens(in_fovy, in_fovy, in_zn, in_zf);
            m_camera->SetPosition(in_position);
            m_camera->LookAt(in_position, in_target, vec3(0, 1, 0));
            m_camera->UpdateViewMatrix();
        } else {
            delete m_camera;
            m_camera = new Camera();
            m_camera->SetLens(in_fovy, in_fovy, in_zn, in_zf);
            m_camera->SetPosition(in_position);
            m_camera->LookAt(in_position, in_target, vec3(0, 1, 0));
            m_camera->UpdateViewMatrix();
        }
    }
    void Device::UpdataInputInfo(EventInfo &in_eventInfo) {
        if (in_eventInfo.type == EVENT_MOUSE_BUTTON) {
            {
                if (in_eventInfo.button == GLFW_MOUSE_BUTTON_LEFT && in_eventInfo.action == GLFW_PRESS) {
                    m_mouseInfo.leftdown = true;
                }
                if (in_eventInfo.button == GLFW_MOUSE_BUTTON_LEFT && in_eventInfo.action == GLFW_RELEASE) {
                    m_mouseInfo.leftdown = false;
                    m_mouseInfo.firstMouse = true;
                }
            }
        }
        if (in_eventInfo.type == EVENT_CURSOR_POS) {
            m_mouseInfo.x = in_eventInfo.xpos;
            m_mouseInfo.y = in_eventInfo.ypos;
            if (m_mouseInfo.leftdown) {
                if (m_mouseInfo.firstMouse) {
                    m_mouseInfo.lastX = in_eventInfo.xpos;
                    m_mouseInfo.lastY = in_eventInfo.ypos;
                    m_mouseInfo.firstMouse = false;
                }
                m_mouseInfo.xoffset += m_mouseInfo.x - m_mouseInfo.lastX;
                m_mouseInfo.yoffset += m_mouseInfo.lastY - m_mouseInfo.y;
                m_mouseInfo.lastX = in_eventInfo.xpos;
                m_mouseInfo.lastY = in_eventInfo.ypos;
            }
        } else if (in_eventInfo.type == EVENT_KEY) {
            if (in_eventInfo.action == GLFW_PRESS) {
                switch (in_eventInfo.key) {
                    case GLFW_KEY_W: {
                        m_cameraKeyInfo.forward = true;
                        break;
                    }
                    case GLFW_KEY_S: {
                        m_cameraKeyInfo.back = true;
                        break;
                    }
                    case GLFW_KEY_A: {
                        m_cameraKeyInfo.left = true;
                        break;
                    }
                    case GLFW_KEY_D: {
                        m_cameraKeyInfo.right = true;
                        break;
                    }
                    case GLFW_KEY_Q: {
                        m_cameraKeyInfo.up = true;
                        break;
                    }
                    case GLFW_KEY_E: {
                        m_cameraKeyInfo.down = true;
                        break;
                    }
                }
            } else if (in_eventInfo.action == GLFW_RELEASE) {
                switch (in_eventInfo.key) {
                    case GLFW_KEY_W: {
                        m_cameraKeyInfo.forward = false;
                        break;
                    }
                    case GLFW_KEY_S: {
                        m_cameraKeyInfo.back = false;
                        break;
                    }
                    case GLFW_KEY_A: {
                        m_cameraKeyInfo.left = false;
                        break;
                    }
                    case GLFW_KEY_D: {
                        m_cameraKeyInfo.right = false;
                        break;
                    }
                    case GLFW_KEY_Q: {
                        m_cameraKeyInfo.up = false;
                        break;
                    }
                    case GLFW_KEY_E: {
                        m_cameraKeyInfo.down = false;
                        break;
                    }
                }
            }
        }
    }
    void Device::UpdataCamera() const {
        Camera &_camera = *m_camera;
        //旋转
        {
            if (!(ZERO(m_mouseInfo.xoffset) && ZERO(m_mouseInfo.yoffset))) {
                float sensitivity = 0.05;
                float xoffset = m_mouseInfo.xoffset;
                float yoffset = m_mouseInfo.yoffset;
                xoffset *= sensitivity;
                yoffset *= sensitivity;
                _camera.yaw += xoffset;
                _camera.pitch += yoffset;
                if (_camera.pitch > 89.0f)
                    _camera.pitch = 89.0f;
                if (_camera.pitch < -89.0f)
                    _camera.pitch = -89.0f;
                glm::vec3 front;
                front.x = cos(glm::radians(_camera.yaw)) * cos(glm::radians(_camera.pitch));
                front.y = sin(glm::radians(_camera.pitch));
                front.z = sin(glm::radians(_camera.yaw)) * cos(glm::radians(_camera.pitch));
                front = glm::normalize(front);
                _camera.SetFront(front);
                _camera.UpdateViewMatrix();
            }
        }
        //平移
        {
            float deltime = m_gameTime.DeltaTime();
            if (m_cameraKeyInfo.forward) {
                _camera.Walk(_camera.speed * deltime);
                _camera.UpdateViewMatrix();
            } else if (m_cameraKeyInfo.back) {
                _camera.Walk(-_camera.speed * deltime);
                _camera.UpdateViewMatrix();
            } else {
            }
            if (m_cameraKeyInfo.left) {
                _camera.Strafe(_camera.speed * deltime);
                _camera.UpdateViewMatrix();
            } else if (m_cameraKeyInfo.right) {
                _camera.Strafe(-_camera.speed * deltime);
                _camera.UpdateViewMatrix();
            } else {
            }
            if (m_cameraKeyInfo.up) {
                _camera.GoUp(-_camera.speed * deltime);
                _camera.UpdateViewMatrix();
            } else if (m_cameraKeyInfo.down) {
                _camera.GoUp(_camera.speed * deltime);
                _camera.UpdateViewMatrix();
            } else {
            }
        }
    }
    void Device::UpdataAppInfo(EventInfo &in_eventInfo) {
        if (in_eventInfo.type == EVENT_KEY) {
            if (in_eventInfo.action == GLFW_RELEASE) {
                if (in_eventInfo.key == GLFW_KEY_ESCAPE) {
                    glfwSetWindowShouldClose(in_eventInfo.window, true);
                }
            }
        }
    }
    void Device::Tick() {
        m_gameTime.Tick();
        m_mouseInfo.xoffset = 0.0f;
        m_mouseInfo.yoffset = 0.0f;
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }
    void Device::Tock() {
        UpdataCamera();
    }
}

