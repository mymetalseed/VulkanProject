#pragma once
#include "../base.h"
#include "device.h"
#include "window.h"
#include "windowSurface.h"

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
	private:
		VkSwapchainKHR mSwapChain{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		Window::Ptr mWindow{nullptr};
		WindowSurface::Ptr mSurface{nullptr};

		//实际创建swapchain之后的运行时参数
		VkFormat mSwapChainFormat;
		VkExtent2D mSwapChainExtent;
		//VkImage由SwapChain创建，销毁也要交给SwapChain
		std::vector<VkImage> mSwapChainImages{};
		//对图像的管理器。
		std::vector<VkImageView> mSwapChainViews{};

	};
}