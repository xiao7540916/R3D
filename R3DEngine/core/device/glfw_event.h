#pragma once
namespace R3D {
	class RDevice;
	class GLFWEvent
	{
	public:
		GLFWEvent(RDevice& in_device);
		~GLFWEvent();
		void Init();
		void Release();
	private:
		RDevice& m_device;
	};
}