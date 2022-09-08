#include<iostream>
#include<device/r_device.h>
using namespace std;
using namespace R3D;
int main() {
	RDevice& device = RDevice::CreateDevice("windowtest", 800, 600);

	while (device.Run()) {
		glClearColor(1.0f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(device.GetWindow());
		glfwPollEvents();
	}
	glfwTerminate();
	device.ReleaseDevice();
	return 0;
}
