#include "swapChain.h"

namespace FF::Wrapper {
	SwapChain::SwapChain(const Device::Ptr& device, const Window::Ptr& window, const WindowSurface::Ptr& surface) {
		mDevice = device;
		mWindow = window;
		mSurface = surface;

		auto swapChainSupportInfo = querySwapChainSupportInfo();

		//ѡ��Format
		VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupportInfo.mFormats);

		//ѡ��PresentMode
		VkPresentModeKHR presentMode = chooseSurfacePresentMode(swapChainSupportInfo.mPresentModes);

		//ѡ�񽻻�����Χ
		VkExtent2D extent = chooseExtent(swapChainSupportInfo.mCapabilities);

		//����ͼ�񻺳�����
		uint32_t imageCount = swapChainSupportInfo.mCapabilities.minImageCount + 1;

		//���maxImageCount = 0��˵��ֻҪ�ڴ治��ը�����ǾͿ�����������������images
		if (swapChainSupportInfo.mCapabilities.maxImageCount > 0 && imageCount > swapChainSupportInfo.mCapabilities.maxImageCount) {
			imageCount = swapChainSupportInfo.mCapabilities.maxImageCount;
		}

		//��д������Ϣ,��ʼ�������ÿգ���Ϊ�����������õı�����ֵΪ���
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface->getSurface();
		createInfo.minImageCount = imageCount;//���������õ����������ʺϵ�ǰ���,���ǿ��ܵõ�����
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		//ͼ������Ĳ�Σ�VRһ��������
		createInfo.imageArrayLayers = 1;
		//���������ɵ�ͷ�񣬵������ںδ�
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		std::vector<uint32_t> queueFamilies = { mDevice->getGraphicQueueFamily().value() ,mDevice->getPresentQueueFamily().value() };
		//��Ϊ��������ͼ�񣬻ᱻ������Ⱦ������ʾ��
		if (mDevice->getGraphicQueueFamily().value() == mDevice->getPresentQueueFamily().value()) {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;//������Ⱦ����һ��
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount;
			createInfo.pQueueFamilyIndices = queueFamilies.data();
		}

		//��������ͼ���ʼ���仯�������Ƿ���Ҫ��ת

		createInfo.preTransform = swapChainSupportInfo.mCapabilities.currentTransform;

		//����ԭ�������е�ͼƬ���
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;

		//��ǰ���屻��ס�Ĳ��֣����û��ƣ����ǻ�ض�
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(mDevice->getDevice(), &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
			throw std::runtime_error("Error:failed to create swapChain");
		}

		mSwapChainFormat = surfaceFormat.format;
		mSwapChainExtent = extent;
	}

	SwapChain::~SwapChain() {
		if (mSwapChain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(mDevice->getDevice(), mSwapChain, nullptr);
		}
	
		mWindow.reset();
		mSurface.reset();
		mDevice.reset();
	}

	SwapChainSupportInfo SwapChain::querySwapChainSupportInfo() {
		SwapChainSupportInfo info;
		//��ȡ��������
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &info.mCapabilities);
	
		//��ȡ����֧�ָ�ʽ
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, nullptr);

		if (formatCount != 0) {
			info.mFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, info.mFormats.data());
		}

		//��ȡ����ģʽ(��ֱͬ��..)
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			info.mPresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, info.mPresentModes.data());
		}

		return info;
	}

	VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		//���ֻ������һ��δ����ĸ�ʽ����ô��û����ѡ��ʽ�������Լ���һ��
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
			return { VK_FORMAT_B8G8R8A8_SRGB,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::chooseSurfacePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		//ѡ����е����÷�ʽ
		//�Ƿ�immediate
		//�豸�� ֻ��FIFO�Ǿ���֧�ֵ�,������ƶ��豸�ϣ�Ϊ�˽�ʡ��Դ������ѡ��FIFO
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return bestMode;
	}

	//��������ÿ��ͼƬ�Ĵ���
	VkExtent2D SwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		//����������������˵��ϵͳ�����������Լ��趨extent
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}

		//���ڸ�����Ļ����£�����ƻ��������������С�������������صĳ���
		//ͳһ����glfw����
		int width = 0, height = 0;
		glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		//�涨��max��min֮��
		actualExtent.width = std::max(
			capabilities.maxImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));


	}

}