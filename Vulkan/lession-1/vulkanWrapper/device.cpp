#include "device.h"

namespace FF::Wrapper {
	Device::Device(Instance::Ptr instance,WindowSurface::Ptr surface) {
		mInstance = instance;
		mSurface = surface;
		pickPhysicalDevice();
		initQueueFamilies(mPhysicalDevice);
		createLogicalDevice();
	}

	Device::~Device() {
		vkDestroyDevice(mDevice, nullptr);
		mSurface.reset();
		mInstance.reset();
	}

	void Device::pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("Error:failed to enumeratePhysicalDevice");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, devices.data());

		std::multimap<int, VkPhysicalDevice> candidates;
		for (const auto& device : devices) {
			int score = rateDevice(device);
			candidates.insert(std::make_pair(score, device));
		}
		if (candidates.rbegin()->first > 0 && isDeviceSuitable(candidates.rbegin()->second)) {
			mPhysicalDevice = candidates.rbegin()->second;
		}

		if (mPhysicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("Error:failed to get PhysicalDevice");
		}
	}

	int Device::rateDevice(VkPhysicalDevice device) {
		int score = 0;

		//设备名称 类型 支持vulkan的版本
		VkPhysicalDeviceProperties deviceProp;
		vkGetPhysicalDeviceProperties(device, &deviceProp);

		//纹理压缩格式，浮点数运算特性 多视口渲染等
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		//独立显卡分值最高
		if (deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}
		score += deviceProp.limits.maxImageDimension2D;
		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		return score;
	}

	bool Device::isDeviceSuitable(VkPhysicalDevice device) {
		//设备名称 类型 支持vulkan的版本
		VkPhysicalDeviceProperties deviceProp;
		vkGetPhysicalDeviceProperties(device, &deviceProp);

		//纹理压缩格式，浮点数运算特性 多视口渲染等
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


		return deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader && deviceFeatures.samplerAnisotropy;
	}

	//初始化队列族
	void Device::initQueueFamilies(VkPhysicalDevice device) {
		uint32_t queueFamilyCount = 0;

		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		//找到一个可以渲染的队列族
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				mGraphicQueueFamily = i;
			}
			
			//寻找支持显示的队列族
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(device,i,mSurface->getSurface(),&presentSupport);
			if (presentSupport) {
				mPresentQueueFamily = i;
			}

			if (isQueueFamilyComplete()) {
				break;
			}
			
			++i;
		}
	}

	void Device::createLogicalDevice() {
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> queueFamilies = { mGraphicQueueFamily.value(),mPresentQueueFamily.value() };
		float queuePriority = 1.0;

		for (uint32_t queueFamily : queueFamilies) {
			//填写创建信息
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		//填写逻辑设备创建信息
		VkPhysicalDeviceFeatures devicesFeatures = {};
		devicesFeatures.samplerAnisotropy = VK_TRUE;


		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pEnabledFeatures = &devicesFeatures;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceRequiredExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceRequiredExtensions.data();

		//layer层
		if (mInstance->getEnableValidationLayer()) {
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			deviceCreateInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create logical device");
		}

		//之后就可以向mGraphicQueue中传递命令
		vkGetDeviceQueue(mDevice, mGraphicQueueFamily.value(), 0, &mGraphicQueue);
		vkGetDeviceQueue(mDevice, mPresentQueueFamily.value(), 0, &mPresentQueue);
	}

	bool Device::isQueueFamilyComplete() {
		return mGraphicQueueFamily.has_value() && mPresentQueueFamily.has_value();
	}
}