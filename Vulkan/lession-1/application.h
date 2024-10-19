#pragma once

#include "base.h"
#include "vulkanWrapper/instance.h"
#include "vulkanWrapper/device.h"
#include "vulkanWrapper/window.h"
#include "vulkanWrapper/windowSurface.h"
#include "vulkanWrapper/swapChain.h"
#include "vulkanWrapper/shader.h"
#include "vulkanWrapper/pipeline.h"
#include "vulkanWrapper/renderPass.h"
#include "vulkanWrapper/commandPool.h"
#include "vulkanWrapper/CommandBuffer.h"
#include "vulkanWrapper/semaphor.h"
#include "vulkanWrapper/fence.h"

namespace FF {
	const int WIDTH = 800;
	const int HEIGHT = 600;

	class Application {
	public:
		Application() = default;
		~Application() = default;

		void run();
	private:
		void initWindow();
		void initVulkan();
		void mainLoop();
		void Render();
		void cleanUp();

	private:
		void createPipeline();
		void createRenderPass();
	private:
		int mCurrentFrame{ 0 };
		Wrapper::Instance::Ptr mInstance{ nullptr };
		Wrapper::Device::Ptr mDevice{ nullptr };
		Wrapper::Window::Ptr mWindow{ nullptr };
		Wrapper::WindowSurface::Ptr mSurface{ nullptr };
		Wrapper::SwapChain::Ptr mSwapChain{ nullptr };
		Wrapper::Pipeline::Ptr mPipeline{ nullptr };
		Wrapper::RenderPass::Ptr mRenderPass{ nullptr };
		Wrapper::CommandPool::Ptr mCommandPool{ nullptr };
		std::vector<Wrapper::CommandBuffer::Ptr> mCommandBuffers{ nullptr };
		std::vector<Wrapper::Semaphore::Ptr> mImageAvailableSemaphores{};
		std::vector<Wrapper::Semaphore::Ptr> mRenderFinishedSemaphores{};
		std::vector<Wrapper::Fence::Ptr> mFences{};
	};
}