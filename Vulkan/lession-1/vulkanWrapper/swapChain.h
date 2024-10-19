#pragma once
#include "../base.h"
#include "device.h"
#include "window.h"
#include "windowSurface.h"
#include "renderPass.h"

namespace FF::Wrapper {
	struct SwapChainSupportInfo {
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;

	};


	class SwapChain {
	public:
		using Ptr = std::shared_ptr<SwapChain>;
		static Ptr create(const Device::Ptr& device, const Window::Ptr& window, const WindowSurface::Ptr& surface) {
			return std::make_shared<SwapChain>(device, window, surface);
		}
		SwapChain(const Device::Ptr &device,const Window::Ptr &window,const WindowSurface::Ptr &surface);
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

		//ʵ�ʴ���swapchain֮�������ʱ����
		VkFormat mSwapChainFormat;
		VkExtent2D mSwapChainExtent;

		uint32_t mImageCount{ 0 };

		//VkImage��SwapChain����������ҲҪ����SwapChain
		std::vector<VkImage> mSwapChainImages{};
		//��ͼ��Ĺ�������
		std::vector<VkImageView> mSwapChainImageViews{};

		std::vector<VkFramebuffer> mSwapChainFrameBuffers{};

	private:
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,uint32_t mipLevels=1);
	};
}