#include "window.h"
#include "../application.h"
#include "../Camera.h"

namespace FF::Wrapper {
	static void windowResized(GLFWwindow* window, int width, int height) {
		//reinterpret_cast 是重新解释,static_cast是本来就是
		auto pUserData = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		pUserData->mWindowResized = true;
	}

	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
		auto pUserData = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		auto app = pUserData->mApp;
		if (!app.expired()) {
			auto appReal = app.lock();
			appReal->onMouseMove(xpos, ypos);
		}
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
		glfwSetCursorPosCallback(mWindow, cursorPositionCallback);
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

	void Window::processEvent() {
		auto pUserData = reinterpret_cast<Window*>(glfwGetWindowUserPointer(mWindow));
		auto app = pUserData->mApp;
		if (app.expired()) {
			return;
		}
		auto appReal = app.lock();

		if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS) {
			appReal->onKeyDown(CAMERA_MOVE::MOVE_FRONT);
		}
		if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS) {
			appReal->onKeyDown(CAMERA_MOVE::MOVE_BACK);
		}
		if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS) {
			appReal->onKeyDown(CAMERA_MOVE::MOVE_LEFT);
		}
		if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS) {
			appReal->onKeyDown(CAMERA_MOVE::MOVE_RIGHT);
		}
	}

}