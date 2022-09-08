#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<device/glfw_event.h>
namespace R3D {
	class RDevice
	{
	public:
		~RDevice();
		static RDevice* GetDevice();
		RDevice* Init(const char* in_appname, int in_width, int in_height);
		void Release();
		void InitControlSystem();
		bool Run();
		GLFWwindow* GetWindow();
		void SetAppName(const char* in_appname);
	private:
		static RDevice* m_device;
		GLFWwindow* m_window = nullptr;
		GLFWEvent* m_eventControl = nullptr;
		bool runable = false;
	private:
		RDevice();
		RDevice(const RDevice&) = delete;
		RDevice& operator=(const RDevice&) = delete;
	};
}

