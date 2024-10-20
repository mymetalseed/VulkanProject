#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	class Buffer {
	public:
		using Ptr = std::shared_ptr<Buffer>;
		static Ptr create(const Device::Ptr& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
			return std::make_shared<Buffer>(device, size, usage, properties);
		}
	public:
		static Ptr createVertexBuffer(const Device::Ptr& device, VkDeviceSize size,void* pData);
		static Ptr createIndexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData);

	public:


		Buffer(const Device::Ptr& device,VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags properties);
		~Buffer();

		/*
		1 通过内存Mapping的形式，直接对内存进行更改，适用于 HostVisible 类型的内存
		2 如果本内存是LocalOptimal,那么就必须创建中间的StageBuffer，先复制到StageBuffer,再拷贝入目标Buffer
		*/
		void updateBufferByMap(void *data,size_t size);
		void updateBufferByStage(void *data,size_t size);

		void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, VkDeviceSize size);

		[[nodiscard]] auto getBuffer() const { return mBuffer; }

	private:
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		VkBuffer mBuffer{ VK_NULL_HANDLE };
		//实际的内存空间
		VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
	};
}