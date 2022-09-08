#pragma once
namespace R3D {
	class RDevice;
	class GLFWEvent
	{
	public:
		~GLFWEvent();
		static GLFWEvent* GetInstance();
		void Init(RDevice* in_device);
		void Release();
	private:
		GLFWEvent();
		static GLFWEvent* m_glfwEvent;
		RDevice* m_device;
	};
}