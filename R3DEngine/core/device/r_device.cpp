#include "r_device.h"
#include<iostream>

namespace R3D {
	RDevice::RDevice()
	{
	}

	RDevice::~RDevice()
	{
	}

	RDevice& RDevice::GetDevice()
	{
		static RDevice g_device;
		return g_device;
	}
	RDevice& RDevice::CreateDevice(const char* in_appname, int in_width, int in_height)
	{
		static bool onceInit = true;
		RDevice& device = GetDevice();
		if (onceInit) {
			device.runable = true;
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			device.m_window = glfwCreateWindow(in_width, in_height, in_appname, nullptr, nullptr);
			if (device.m_window == nullptr) {
				std::cout << "Failed to create GLFW window" << std::endl;
				glfwTerminate();
			}
			glfwMakeContextCurrent(device.m_window);
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				std::cout << "Failed to initialize GLAD" << std::endl;
			}
			glViewport(0, 0, in_width, in_height);
			device.InitControlSystem();
			onceInit = false;
		}
		return device;
	}

	void RDevice::InitControlSystem()
	{
		RDevice& device = GetDevice();
		device.m_eventControl = new GLFWEvent(device);
		device.m_eventControl->Init();
	}
	bool RDevice::Run()
	{
		RDevice& device = GetDevice();
		device.runable = !glfwWindowShouldClose(device.GetWindow());
		return GetDevice().runable;
	}
	void RDevice::ReleaseDevice()
	{
		RDevice& device = GetDevice();
		device.m_eventControl->Release();
		delete device.m_eventControl;
	}
	GLFWwindow* RDevice::GetWindow()
	{
		return m_window;
	}

	void RDevice::SetAppName(const char* in_appname)
	{
		glfwSetWindowTitle(m_window, in_appname);
	}
}
