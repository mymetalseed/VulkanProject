#pragma once
#include "../base.h"
#include "device.h"
#include "window.h"
#include "windowSurface.h"
#include "renderPass.h"
#include "image.h"
#include "commandPool.h"

namespace FF::Wrapper {
	struct SwapChainSupportInfo {
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;

	};


	class SwapChain {
	public:
		using Ptr = std::shared_ptr<SwapChain>;
		static Ptr create(
			const Device::Ptr& device, 
			const Window::Ptr& window, 
			const WindowSurface::Ptr& surface,
			const CommandPool::Ptr& commandPool
		) {
			return std::make_shared<SwapChain>(device, window, surface, commandPool);
		}
		SwapChain(
			const Device::Ptr &device,
			const Window::Ptr &window,
			const WindowSurface::Ptr &surface,
			const CommandPool::Ptr& commandPool
		);
		~SwapChain();

		SwapChainSupportInfo querySwapChainSupportInfo();
		VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormat);
	
		VkPresentModeKHR chooseSurfacePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void createFrameBuffers(const RenderPass::Ptr& renderPass);

		[[nodiscard]] auto getImageCount() const {
			return mImageCount;
		}

		[[nodiscard]] auto getSwapChain() const {
			return mSwapChain;
		}

		[[nodiscard]] auto getFormat() const {
			return mSwapChainFormat;
		}

		[[nodiscard]] auto getFrameBuffer(const int index) const {
			return mSwapChainFrameBuffers[index];
		}

		[[nodiscard]] auto getExtent() const {
			return mSwapChainExtent;
		}
	private:
		VkSwapchainKHR mSwapChain{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		Window::Ptr mWindow{nullptr};
		WindowSurface::Ptr mSurface{nullptr};

		//实际创建swapchain之后的运行时参数
		VkFormat mSwapChainFormat;
		VkExtent2D mSwapChainExtent;

		uint32_t mImageCount{ 0 };

		//VkImage由SwapChain创建，销毁也要交给SwapChain
		std::vector<VkImage> mSwapChainImages{};
		//对图像的管理器。
		std::vector<VkImageView> mSwapChainImageViews{};

		std::vector<VkFramebuffer> mSwapChainFrameBuffers{};

		//深度图片
		std::vector<Image::Ptr> mDepthImages{};

		std::vector<Image::Ptr> mMultiSampleImages{};

	private:
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,uint32_t mipLevels=1);
	};
}