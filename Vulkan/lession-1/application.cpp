#include "application.h"

namespace FF {

	void Application::run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanUp();
	}

	void Application::initWindow() {
		glfwInit();

		//设置环境,关掉OpenGL的API，禁止窗口改变大小
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		mWindow = glfwCreateWindow(WIDTH, HEIGHT,"Vulkan Window",nullptr,nullptr);
		if (!mWindow) {
			std::cerr << "Error: failed to create window" << std::endl;
		}


	}

	void Application::initVulkan() {

	}

	void Application::mainLoop() {
		while (!glfwWindowShouldClose(mWindow)) {
			glfwPollEvents();
		}
	}

	void Application::cleanUp() {
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}
}