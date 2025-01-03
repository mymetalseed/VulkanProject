#include "swapChain.h"

namespace FF::Wrapper {
	SwapChain::SwapChain(
		const Device::Ptr& device, 
		const Window::Ptr& window, 
		const WindowSurface::Ptr& surface,
		const CommandPool::Ptr &commandPool
	) {
		mDevice = device;
		mWindow = window;
		mSurface = surface;

		auto swapChainSupportInfo = querySwapChainSupportInfo();

		//选择Format
		VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupportInfo.mFormats);

		//选择PresentMode
		VkPresentModeKHR presentMode = chooseSurfacePresentMode(swapChainSupportInfo.mPresentModes);

		//选择交换链范围
		VkExtent2D extent = chooseExtent(swapChainSupportInfo.mCapabilities);

		//设置图像缓冲数量
		mImageCount = swapChainSupportInfo.mCapabilities.minImageCount + 1;

		//如果maxImageCount = 0，说明只要内存不爆炸，我们就可以设置任意数量的images
		if (swapChainSupportInfo.mCapabilities.maxImageCount > 0 && mImageCount > swapChainSupportInfo.mCapabilities.maxImageCount) {
			mImageCount = swapChainSupportInfo.mCapabilities.maxImageCount;
		}

		//填写创建信息,初始化必须置空，因为会有忘记设置的变量，值为随机
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface->getSurface();
		createInfo.minImageCount = mImageCount;//我现在设置的数量，是适合当前情况,但是可能得到更多
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		//图像包含的层次，VR一般有两个
		createInfo.imageArrayLayers = 1;
		//交换链生成的头像，到底用于何处
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		std::vector<uint32_t> queueFamilies = { mDevice->getGraphicQueueFamily().value() ,mDevice->getPresentQueueFamily().value() };
		//因为交换链的图像，会被用来渲染或者显示，
		if (mDevice->getGraphicQueueFamily().value() == mDevice->getPresentQueueFamily().value()) {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;//两个渲染队列一致
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount;
			createInfo.pQueueFamilyIndices = queueFamilies.data();
		}

		//交换链的图像初始化变化，比如是否需要反转

		createInfo.preTransform = swapChainSupportInfo.mCapabilities.currentTransform;

		//不与原本窗口中的图片混合
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;

		//当前窗体被挡住的部分，不用绘制，但是会回读
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(mDevice->getDevice(), &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
			throw std::runtime_error("Error:failed to create swapChain");
		}

		mSwapChainFormat = surfaceFormat.format;
		mSwapChainExtent = extent;

		//系统可能创建更多的Image,当前的imageCount是最小数量
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, nullptr);
		mSwapChainImages.resize(mImageCount);
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, mSwapChainImages.data());

		//创建imageView
		mSwapChainImageViews.resize(mImageCount);
		for (int i = 0; i < mImageCount; ++i) {
			mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
		
		//创建depthImage
		mDepthImages.resize(mImageCount);
		VkImageSubresourceRange region{};
		region.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		region.baseMipLevel = 0;
		region.levelCount = 1;
		region.baseArrayLayer = 0;
		region.layerCount = 1;

		for (int i = 0; i < mImageCount; ++i) {
			mDepthImages[i] = Image::createDepthImage(
				mDevice, 
				mSwapChainExtent.width, 
				mSwapChainExtent.height,
				mDevice->getMaxUsableSampleCount()
			);
			mDepthImages[i]->setImageLayout(
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				region, commandPool
			);
		}

		VkImageSubresourceRange regionMultiSample{};
		regionMultiSample.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		regionMultiSample.baseMipLevel = 0;
		regionMultiSample.levelCount = 1;
		regionMultiSample.baseArrayLayer = 0;
		regionMultiSample.layerCount = 1;
		
		//创建MultiSampleImage
		mMultiSampleImages.resize(mImageCount);
		for (int i = 0; i < mImageCount; ++i) {
			mMultiSampleImages[i] = Image::createRenderTargteImage(
				mDevice, 
				mSwapChainExtent.width, 
				mSwapChainExtent.height,
				mSwapChainFormat
				);
			mMultiSampleImages[i]->setImageLayout(
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				regionMultiSample, commandPool
			);
		}
	}

	void SwapChain::createFrameBuffers(const RenderPass::Ptr& renderPass){
		//创建FrameBuffer
		mSwapChainFrameBuffers.resize(mImageCount);
		for (int i = 0; i < mImageCount; ++i) {
			//FrameBuffer里面为一帧的数据，比如有n个ColorAttachment, 1个DepthStencilAttachment
			//这些东西的集合为一个FrameBuffer，送入管线，就会形成一个GPU的结合，由输送的attachments构成

			//注意数组中的顺序,必须和renderPass要匹配
			std::array<VkImageView, 3> attachments = { 
				mSwapChainImageViews[i],
				mMultiSampleImages[i]->getImageView(),
				mDepthImages[i]->getImageView(),
			};

			VkFramebufferCreateInfo frameBufferCreateInfo{};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.renderPass = renderPass->getRenderPass();
			frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			frameBufferCreateInfo.pAttachments = attachments.data();
			frameBufferCreateInfo.width = mSwapChainExtent.width;
			frameBufferCreateInfo.height = mSwapChainExtent.height;
			frameBufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(mDevice->getDevice(), &frameBufferCreateInfo, nullptr, &mSwapChainFrameBuffers[i])!=VK_SUCCESS) {
				throw std::runtime_error("Error: failed to create frameBuffer");
			}
		}

	}


	SwapChain::~SwapChain() {
		for (auto& imageView:mSwapChainImageViews) {
			vkDestroyImageView(mDevice->getDevice(), imageView, nullptr);
		}
		for (auto& frameBuffer : mSwapChainFrameBuffers) {
			vkDestroyFramebuffer(mDevice->getDevice(), frameBuffer, nullptr);
		}
		if (mSwapChain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(mDevice->getDevice(), mSwapChain, nullptr);
		}
	
		mWindow.reset();
		mSurface.reset();
		mDevice.reset();
	}

	SwapChainSupportInfo SwapChain::querySwapChainSupportInfo() {
		SwapChainSupportInfo info;
		//获取基础特性
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &info.mCapabilities);
	
		//获取表面支持格式
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, nullptr);

		if (formatCount != 0) {
			info.mFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, info.mFormats.data());
		}

		//获取呈现模式(垂直同步..)
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			info.mPresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, info.mPresentModes.data());
		}

		return info;
	}

	VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		//如果只返回了一个未定义的格式，那么就没有首选格式，我们自己做一个
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
		//选择队列的作用方式
		//是否immediate
		//设备上 只有FIFO是绝对支持的,如果在移动设备上，为了节省电源，优先选择FIFO
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return bestMode;
	}

	//交换链上每张图片的带下
	VkExtent2D SwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		//如果出现以下情况，说明系统不允许我们自己设定extent
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}

		//由于高清屏幕情况下，比如苹果，窗体的坐标大小，并不等于像素的长宽
		//统一交给glfw处理
		int width = 0, height = 0;
		glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		//规定在max与min之间
		actualExtent.width = std::max(
			capabilities.maxImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));


	}

	VkImageView SwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;

		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView{ VK_NULL_HANDLE };
		if (vkCreateImageView(mDevice->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create image view in swapchain");
		}

		return imageView;
	}

}