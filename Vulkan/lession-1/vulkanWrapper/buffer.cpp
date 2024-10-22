#include "buffer.h"
#include "CommandBuffer.h"
#include "commandPool.h"

namespace FF::Wrapper {

	Buffer::Ptr Buffer::createVertexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
		auto buffer = Buffer::create(device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			//GPU本地可读
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		buffer->updateBufferByStage(pData, size);
		return buffer;
	}

	Buffer::Ptr Buffer::createIndexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
		auto buffer = Buffer::create(device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			//GPU本地可读
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		buffer->updateBufferByStage(pData, size);
		return buffer;
	}

	Buffer::Ptr Buffer::createUniformBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
		auto buffer = Buffer::create(device, size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			//GPU本地可读
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		if (pData != nullptr) {
			buffer->updateBufferByStage(pData, size);
		}
		
		return buffer;
	}


	Buffer::Buffer(const Device::Ptr& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		mDevice = device;

		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		//只是创建了一个CPU端的描述对象，还没有在GPU端分配内存
		if (vkCreateBuffer(mDevice->getDevice(), &createInfo, nullptr, &mBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create buffer");
		}

		//创建显存空间
		VkMemoryRequirements memReq{};
		vkGetBufferMemoryRequirements(mDevice->getDevice(), mBuffer, &memReq);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memReq.size;

		//查找符合上述BUFFER需求的内存类型的全部ID
		allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits,properties);

		//分配GPU Buffer
		if (vkAllocateMemory(mDevice->getDevice(), &allocInfo, nullptr, &mBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to allocate memory");
		}

		//绑定GPUBuffer
		vkBindBufferMemory(mDevice->getDevice(), mBuffer, mBufferMemory, 0);

		mBufferInfo.buffer = mBuffer;
		mBufferInfo.offset = 0;
		mBufferInfo.range = size;
	}

	Buffer::~Buffer() {
		if (mBuffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(mDevice->getDevice(), mBuffer, nullptr);
		}

		if (mBufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(mDevice->getDevice(), mBufferMemory, nullptr);
		}
	}

	uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(mDevice->getPhysicalDevice(), &memProps);

		for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
			if ((typeFilter & (1 << i)) && ((memProps.memoryTypes[i].propertyFlags & properties) == properties)) {
				return i;
			}
		}

		throw std::runtime_error("Error: cannot find the property memory type");
	}

	void Buffer::updateBufferByMap(void *data,size_t size) {
		void* memPtr = nullptr;

		//memPtr指向mBufferMemory的起始地址
		//gpu的显存map到cpu的内存里,拷贝完需要unmap
		vkMapMemory(mDevice->getDevice(), mBufferMemory, 0, size, 0, &memPtr);
		memcpy(memPtr, data, size);
		vkUnmapMemory(mDevice->getDevice(), mBufferMemory);
	}

	void Buffer::updateBufferByStage(void* data,size_t size) {
		auto stageBuffer = Buffer::create(
			mDevice, 
			size, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			//memcpy的时候直接刷到gpu中
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		stageBuffer->updateBufferByMap(data, size);

		copyBuffer(stageBuffer->getBuffer(), mBuffer, static_cast<VkDeviceSize>(size));
	}

	void Buffer::copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, VkDeviceSize size) {
		//绘制队列族也可以传输
		auto commandPool = CommandPool::create(mDevice);
		auto commandBuffer = CommandBuffer::create(mDevice, commandPool);

		//提交一次就结束
		commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VkBufferCopy copyInfo{};
		copyInfo.size = size;
		
		commandBuffer->copyBuffer(srcBuffer, dstBuffer, 1, { copyInfo });

		commandBuffer->end();

		commandBuffer->submitSync(mDevice->getGraphicQueue(), VK_NULL_HANDLE);

	}


}