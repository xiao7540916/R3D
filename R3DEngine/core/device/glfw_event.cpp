#include "glfw_event.h"
#include <iostream>
namespace R3D {
	GLFWEvent::GLFWEvent()
	{
	}

	GLFWEvent::~GLFWEvent()
	{

	}
	GLFWEvent* GLFWEvent::m_glfwEvent = nullptr;
	GLFWEvent* GLFWEvent::GetInstance()
	{
		if (m_glfwEvent == nullptr) {
			m_glfwEvent = new GLFWEvent();
		}
		return m_glfwEvent;
	}
	void GLFWEvent::Init(RDevice* in_device)
	{
		m_device = in_device;
		std::cout << "init eventsys" << std::endl;
	}
	void GLFWEvent::Release()
	{
		std::cout << "release eventsys" << std::endl;
	}
}