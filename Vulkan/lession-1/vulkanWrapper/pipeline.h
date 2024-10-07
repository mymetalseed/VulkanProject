#pragma once

#include "../base.h"
#include "device.h"
#include "shader.h"

namespace FF::Wrapper {
	class Pipeline {
	public:
		using Ptr = std::shared_ptr<Pipeline>;
		static Ptr create(const Device::Ptr& device) {
			return std::make_shared<Pipeline>(device);
		}
		Pipeline(const Device::Ptr &device);
		~Pipeline();

		void setShaderGroup(const std::vector<Shader::Ptr> &shaderGroup);
		void build();

	public:
		VkPipelineVertexInputStateCreateInfo mVertexInputState{};
		VkPipelineInputAssemblyStateCreateInfo mAssemblyState{};
		VkPipelineViewportStateCreateInfo mViewportState{};
		VkPipelineRasterizationStateCreateInfo mRasterState{};
		VkPipelineMultisampleStateCreateInfo mSampleState{};
		VkPipelineColorBlendAttachmentState mBlendAttachmentState{};
		VkPipelineColorBlendStateCreateInfo mBlendState{};
		VkPipelineDepthStencilStateCreateInfo mDepthStencilState{};
		VkPipelineLayoutCreateInfo mLayoutState{};

	private:
		VkPipeline mPipeline{ VK_NULL_HANDLE };
		VkPipelineLayout mLayout{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		std::vector<Shader::Ptr> mShaders{};

	};
}