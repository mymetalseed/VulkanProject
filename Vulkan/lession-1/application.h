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
#include "vulkanWrapper/buffer.h"
#include "vulkanWrapper/descriptorSetLayout.h"
#include "vulkanWrapper/descriptorPool.h"
#include "vulkanWrapper/description.h"
#include "vulkanWrapper/descriptorSet.h"
#include "uniformManager.h"
#include "vulkanWrapper/image.h"
#include "vulkanWrapper/sampler.h"
#include "Texture/texture.h"

#include "model.h"

namespace FF {

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
		void createCommandBuffers();
		void createSyncObject();

		//重建交换链: 当窗口大小发生变化时，交换链也要发生变化,Frame View Pipeline RenderPass Sync(frameCount变化的话)都会发生变化
		void recreateSwapChain();
		void cleanupSwapChain();

	private:
		unsigned int mWidth = 800;
		unsigned int mHeight = 600;

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

		UniformManager::Ptr mUniformManager{ nullptr };

		Model::Ptr mModel{ nullptr };
		VPMatrices mVPMatrices;

	};
}