#include "CommandBuffer.h"

namespace FF::Wrapper {
	CommandBuffer::CommandBuffer(const Device::Ptr& device, const CommandPool::Ptr& commandPool, bool asSecondary) {
		mDevice = device;
		mCommandPool = commandPool;
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = mCommandPool->getCommandPool();
		allocInfo.level = asSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		if (vkAllocateCommandBuffers(mDevice->getDevice(), &allocInfo, &mCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create commandBuffer");
		}

	}
	CommandBuffer::~CommandBuffer() {
		//会随着CommandPool的析构而释放
		if (mCommandBuffer != VK_NULL_HANDLE) {
			//只会干掉自己
			vkFreeCommandBuffers(mDevice->getDevice(), mCommandPool->getCommandPool(), 1, &mCommandBuffer);
		}
	}


	void CommandBuffer::begin(
		VkCommandBufferUsageFlags flag, 
		const VkCommandBufferInheritanceInfo& inheritance
	) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = flag;
		beginInfo.pInheritanceInfo = &inheritance;

		if (vkBeginCommandBuffer(mCommandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to begin command buffer");
		}
	}

	void CommandBuffer::beginRenderPass(
		const VkRenderPassBeginInfo& renderPassBeginInfo, 
		const VkSubpassContents& subPassContents
	) {
		vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, subPassContents);
	}

	void CommandBuffer::bindGraphicPipeline(const VkPipeline& pipeline) {
		vkCmdBindPipeline(mCommandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,pipeline);
	}

	void CommandBuffer::bindVertexBuffer(const std::vector<VkBuffer>& buffers) {
		std::vector<VkDeviceSize> offsets(buffers.size(),0);

		vkCmdBindVertexBuffers(mCommandBuffer, 0, static_cast<uint32_t>(buffers.size()), buffers.data(),offsets.data());
	}

	void CommandBuffer::bindIndexBuffer(const VkBuffer& buffer) {
		vkCmdBindIndexBuffer(mCommandBuffer, buffer, 0,VK_INDEX_TYPE_UINT32);

	}

	void CommandBuffer::bindDescriptorSet(const VkPipelineLayout layout,const VkDescriptorSet &descriptorSet) {
		vkCmdBindDescriptorSets(
			mCommandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS, layout,
			0,1,&descriptorSet,0,nullptr
			);
	}

	void CommandBuffer::draw(size_t vertexCount) {
		vkCmdDraw(mCommandBuffer, vertexCount, 1, 0, 0);
	}

	void CommandBuffer::drawIndex(size_t indexCount) {
		vkCmdDrawIndexed(mCommandBuffer, indexCount, 1, 0, 0,0);
	}

	void CommandBuffer::endRenderPass() {
		vkCmdEndRenderPass(mCommandBuffer);
	}

	void CommandBuffer::end() {
		if (vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to end command buffer");
		}
	}

	void CommandBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t copyInfoCount, const std::vector<VkBufferCopy>& copyInfos) {
		vkCmdCopyBuffer(mCommandBuffer, srcBuffer, dstBuffer, copyInfoCount, copyInfos.data());
	}

	void CommandBuffer::submitSync(VkQueue queue, VkFence fence) {
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCommandBuffer;

		vkQueueSubmit(queue, 1, &submitInfo, fence);

		vkQueueWaitIdle(queue);
	}

}