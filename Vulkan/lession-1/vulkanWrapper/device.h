#pragma once
#include "../base.h"
#include "instance.h"
#include "windowSurface.h"

namespace FF::Wrapper {

	const std::vector<const char*> deviceRequiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	class Device {
	public:
		using Ptr = std::shared_ptr<Device>;
		static Ptr create(Instance::Ptr instance, WindowSurface::Ptr surface) { return std::make_shared<Device>(instance,surface); }

		Device(Instance::Ptr instance, WindowSurface::Ptr surface);
		~Device();

		void pickPhysicalDevice();
		int rateDevice(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device);

		void initQueueFamilies(VkPhysicalDevice device);

		void createLogicalDevice();
		bool isQueueFamilyComplete();

		[[nodiscard]] auto getDevice() const { return mDevice; }
		[[nodiscard]] auto getPhysicalDevice() const { return mPhysicalDevice; }
	private:
		VkPhysicalDevice mPhysicalDevice{ VK_NULL_HANDLE };
		Instance::Ptr mInstance;
		WindowSurface::Ptr mSurface{ nullptr };

		//存储当前渲染任务队列族的id
		std::optional<uint32_t> mGraphicQueueFamily;
		//渲染任务队列
		VkQueue mGraphicQueue{ VK_NULL_HANDLE };

		//渲染结束后需要提供显示队列
		std::optional<uint32_t> mPresentQueueFamily;
		VkQueue mPresentQueue{ VK_NULL_HANDLE };

		//逻辑设备
		VkDevice mDevice{ VK_NULL_HANDLE };
	};
}