#include "glfw_event.h"
#include <iostream>
namespace R3D {
	GLFWEvent::GLFWEvent(RDevice& in_device) :m_device(in_device)
	{
	}

	GLFWEvent::~GLFWEvent()
	{

	}
	void GLFWEvent::Init()
	{
		std::cout << "init eventsys" << std::endl;
	}
	void GLFWEvent::Release()
	{
		std::cout << "release eventsys" << std::endl;
	}
}