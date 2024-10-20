#include "window.h"

namespace FF::Wrapper {
	static void windowResized(GLFWwindow* window, int width, int height) {
		//reinterpret_cast 是重新解释,static_cast是本来就是
		auto pUserData = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		pUserData->mWindowResized = true;
	}

	Window::Window(const int& width, const int& height) {
		mWidth = width;
		mHeight = height;
		glfwInit();

		//设置环境,关掉OpenGL的API，禁止窗口改变大小
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		mWindow = glfwCreateWindow(mWidth, mHeight, "Vulkan Window", nullptr, nullptr);
		if (!mWindow) {
			std::cerr << "Error: failed to create window" << std::endl;
		}

		glfwSetWindowUserPointer(mWindow, this);
		glfwSetFramebufferSizeCallback(mWindow, windowResized);
	}

	Window::~Window() {
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	bool Window::shouldClose() {
		return glfwWindowShouldClose(mWindow);
	}

	void Window::pollEvents() {
		glfwPollEvents();
	}



}