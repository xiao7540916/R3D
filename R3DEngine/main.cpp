#include<iostream>
#include<device/r_device.h>
//#include<spdlog/spdlog.h>
using namespace std;
using namespace R3D;
int main() {
	RDevice* device = RDevice::GetDevice();
	device->Init("windowtest", 800, 600);

    //spdlog::info("Welcome to spdlog!");
    //spdlog::error("Some error message with arg: {}", 1);

    //spdlog::warn("Easy padding in numbers like {:08d}", 12);
    //spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    //spdlog::info("Support for floats {:03.2f}", 1.23456);
    //spdlog::info("Positional args are {1} {0}..", "too", "supported");
    //spdlog::info("{:<30}", "left aligned");

    //spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    //spdlog::debug("This message should be displayed..");

    //// change log pattern
    //spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    //// Compile time log levels
    //// define SPDLOG_ACTIVE_LEVEL to desired level
    //SPDLOG_TRACE("Some trace message with param {}", 42);
    //SPDLOG_DEBUG("Some debug message");

	while (device->Run()) {
		glClearColor(1.0f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(device->GetWindow());
		glfwPollEvents();
	}
	glfwTerminate();
	device->Release();
	delete device;
	return 0;
}
