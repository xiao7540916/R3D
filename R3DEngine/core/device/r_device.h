#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<device/glfw_event.h>
namespace R3D {
	class RDevice
	{
	public:
		static RDevice& GetDevice();
		static RDevice& CreateDevice(const char* in_appname, int in_width, int in_height);
		static void ReleaseDevice();
		static void InitControlSystem();
		static bool Run();
		GLFWwindow* GetWindow();
		void SetAppName(const char* in_appname);
	private:
		GLFWwindow* m_window = nullptr;
		GLFWEvent* m_eventControl = nullptr;
		bool runable;
	private:
		RDevice();
		~RDevice();
		RDevice(const RDevice&) = delete;
		RDevice& operator=(const RDevice&) = delete;
	};
}

