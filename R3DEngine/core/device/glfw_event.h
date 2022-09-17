#pragma once
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
namespace R3D {
    class Device;
    class GLFWEvent {
    public:
        ~GLFWEvent();
        static GLFWEvent *GetInstance();
        void Init(Device *in_device);
        void Release();
        Device *GetDevice();
        static void WdSize(GLFWwindow* window, int width, int height);
        static void CursorPos(GLFWwindow* window, double xpos, double ypos);
        static void WdClose(GLFWwindow* window);
        static void Scroll(GLFWwindow* window, double xoffset, double yoffset);
        static void MouseButton(GLFWwindow* window, int button, int action, int mods);
        static void Key(GLFWwindow* window, int key, int scancode, int action, int mods);
    private:
        GLFWEvent();
        static GLFWEvent *m_glfwEvent;
        Device *m_device;
    };
}