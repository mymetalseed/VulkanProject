#include "application.h"

namespace FF {

	void Application::run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanUp();
	}

	void Application::initWindow() {
		mWindow = Wrapper::Window::create(WIDTH, HEIGHT);
	}

	void Application::initVulkan() {
		mInstance = Wrapper::Instance::create(true);
		mSurface = Wrapper::WindowSurface::create(mInstance,mWindow);

		mDevice = Wrapper::Device::create(mInstance,mSurface);
		mSwapChain = Wrapper::SwapChain::create(mDevice,mWindow,mSurface);
	}

	void Application::mainLoop() {
		while (!mWindow->shouldClose()) {
			mWindow->pollEvents();
		}
	}

	void Application::cleanUp() {
		//ʹmInstance������ָ����Ϊ0����֤����������
		mSwapChain.reset();
		mDevice.reset();
		mSurface.reset();
		mInstance.reset();
		mWindow.reset();
	}
}