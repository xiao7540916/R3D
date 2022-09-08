#include "glfw_event.h"
#include <iostream>
namespace R3D {
    GLFWEvent::GLFWEvent() {
    }
    GLFWEvent::~GLFWEvent() {
    }
    GLFWEvent *GLFWEvent::m_glfwEvent = nullptr;
    GLFWEvent *GLFWEvent::GetInstance() {
        if (m_glfwEvent == nullptr) {
            m_glfwEvent = new GLFWEvent();
        }
        return m_glfwEvent;
    }
    void GLFWEvent::Init(Device *in_device) {
        m_device = in_device;
    }
    void GLFWEvent::Release() {
    }
    Device *GLFWEvent::GetDevice() {
        return m_device;
    }
}