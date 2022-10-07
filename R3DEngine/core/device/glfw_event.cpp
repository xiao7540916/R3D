#include "glfw_event.h"
#include <device/device.h>
#include <iostream>
namespace R3D {
    GLFWEvent::GLFWEvent() {
    }
    GLFWEvent::~GLFWEvent() {
    }
    GLFWEvent *GLFWEvent::m_glfwEvent = nullptr;
    GLFWEvent *GLFWEvent::GetInstance() {
        if (m_glfwEvent == nullptr) {
            m_glfwEvent = NEW GLFWEvent();
        }
        return m_glfwEvent;
    }
    void GLFWEvent::Init(Device *in_device) {
        GetInstance()->m_device = in_device;
        glfwSetWindowSizeCallback(Device::GetInstance()->GetWindow(), WdSize);
        glfwSetCursorPosCallback(Device::GetInstance()->GetWindow(),CursorPos);
        glfwSetWindowCloseCallback(Device::GetInstance()->GetWindow(),WdClose);
        glfwSetScrollCallback(Device::GetInstance()->GetWindow(),Scroll);
        glfwSetMouseButtonCallback(Device::GetInstance()->GetWindow(),MouseButton);
        glfwSetKeyCallback(Device::GetInstance()->GetWindow(),Key);
    }
    void GLFWEvent::Release() {
    }
    Device *GLFWEvent::GetDevice() {
        return m_device;
    }
    void GLFWEvent::WdSize(GLFWwindow *window, int width, int height) {
        GetInstance()->m_device->OnWindowSize(window, width, height);
    }
    void GLFWEvent::CursorPos(GLFWwindow *window, double xpos, double ypos) {
        GetInstance()->m_device->OnCursorPos(window, xpos, ypos);
    }
    void GLFWEvent::WdClose(GLFWwindow *window) {
        GetInstance()->m_device->OnWindowClose(window);
    }
    void GLFWEvent::Scroll(GLFWwindow *window, double xoffset, double yoffset) {
        ImGuiIO &io = ImGui::GetIO();
        if(!io.WantCaptureMouse){
            GetInstance()->m_device->OnScroll(window, xoffset, yoffset);
        }
    }
    void GLFWEvent::MouseButton(GLFWwindow *window, int button, int action, int mods) {
        ImGuiIO &io = ImGui::GetIO();
        if(!io.WantCaptureMouse){
            GetInstance()->m_device->OnMouseButton(window, button, action, mods);
        }
    }
    void GLFWEvent::Key(GLFWwindow *window, int key, int scancode, int action, int mods) {
        GetInstance()->m_device->OnKey(window, key, scancode, action, mods);
    }
}