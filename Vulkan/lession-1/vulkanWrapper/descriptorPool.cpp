#include "descriptorPool.h"

namespace FF::Wrapper{
	DescriptorPool::DescriptorPool(const Device::Ptr &device) {
		mDevice = device;


	}

	DescriptorPool::~DescriptorPool() {
		if (mPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(mDevice->getDevice(), mPool, nullptr);
		}
	}

	void DescriptorPool::build(std::vector<UniformParameter::Ptr>& params,const int& frameCount) {
		//descriptor
		//descriptorSet(descriptorA(Buffer),descriptorB(Buffer),descriptorC(Buffer)) 描述符集
		//descriptorSet 可能需要N个 因为描述符集合当中，可以绑定了buffer
		//当前一帧提交的时候，其他帧正在绘制中
		//即uniformBuffer正在被读取，此时cpu端的下一个循环，却对其进行了数据的修改

		int uniformBufferCount = 0;
		//纹理这个种类的uniform有多少个...
		int textureCount = 0;

		for (const auto& param : params) {
			if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
				uniformBufferCount++;
			}
	
			if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
				textureCount++;
			}
		}

		//描述每一种uniform都有多少个
		std::vector<VkDescriptorPoolSize> poolSizes{};
		VkDescriptorPoolSize uniformBufferSize{};
		uniformBufferSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBufferSize.descriptorCount = uniformBufferCount * frameCount;
		poolSizes.push_back(uniformBufferSize);

		VkDescriptorPoolSize textureSize{};
		textureSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//这个size是指有多少个descriptor
		textureSize.descriptorCount = textureCount * frameCount;
		poolSizes.push_back(textureSize);

		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		createInfo.pPoolSizes = poolSizes.data();
		createInfo.maxSets = static_cast<uint32_t>(frameCount);

		if (vkCreateDescriptorPool(mDevice->getDevice(), &createInfo, nullptr, &mPool) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create descriptor pool");
		}
	}
}