#pragma once

#include "../base.h"
#include "device.h"
#include "shader.h"
#include "renderPass.h"

namespace FF::Wrapper {
	class Pipeline {
	public:
		using Ptr = std::shared_ptr<Pipeline>;
		static Ptr create(const Device::Ptr& device, const RenderPass::Ptr& renderPss) {
			return std::make_shared<Pipeline>(device,renderPss);
		}
		Pipeline(const Device::Ptr &device,const RenderPass::Ptr &renderPss);
		~Pipeline();

		void setShaderGroup(const std::vector<Shader::Ptr> &shaderGroup);

		void setViewports(const std::vector<VkViewport>& viewport) { mViewports = viewport; }
		void setScissors(const std::vector<VkRect2D>& scissors) { mScissors = scissors; }

		void pushBlendAttachment(const VkPipelineColorBlendAttachmentState& blendAttachment) { 
			mBlendAttachmentState.push_back(blendAttachment); 
		}

		void build();

	public:
		VkPipelineVertexInputStateCreateInfo mVertexInputState{};
		VkPipelineInputAssemblyStateCreateInfo mAssemblyState{};
		VkPipelineViewportStateCreateInfo mViewportState{};
		VkPipelineRasterizationStateCreateInfo mRasterState{};
		VkPipelineMultisampleStateCreateInfo mSampleState{};
		std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachmentState{};
		VkPipelineColorBlendStateCreateInfo mBlendState{};
		VkPipelineDepthStencilStateCreateInfo mDepthStencilState{};
		VkPipelineLayoutCreateInfo mLayoutState{};

	public:
		[[nodiscard]] auto getPipeline() const {
			return mPipeline;
		}

	private:
		VkPipeline mPipeline{ VK_NULL_HANDLE };
		VkPipelineLayout mLayout{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		RenderPass::Ptr mRenderPass{ nullptr };

		std::vector<Shader::Ptr> mShaders{};

		std::vector<VkViewport> mViewports{};
		std::vector<VkRect2D> mScissors{};


	};
}