#pragma once
#include "../base.h"
#include "instance.h"
#include "windowSurface.h"

namespace FF::Wrapper {
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