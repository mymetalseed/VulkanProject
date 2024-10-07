#include "pipeline.h"

namespace FF::Wrapper {
	Pipeline::Pipeline(const Device::Ptr& device) {
		mDevice = device;
	}

	Pipeline::~Pipeline() {
		if (mLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(mDevice->getDevice(), mLayout, nullptr);
		}

		if (mPipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(mDevice->getDevice(), mPipeline, nullptr);
		}
	}

	void Pipeline::setShaderGroup(const std::vector<Shader::Ptr>& shaderGroup) {
		mShaders = shaderGroup;
	}

	void Pipeline::build() {
		std::vector<VkPipelineShaderStageCreateInfo> shaderCreateInfos{};
		for (const auto& shader : mShaders) {
			VkPipelineShaderStageCreateInfo shaderCreateInfo{};
			shaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderCreateInfo.stage = shader->getShaderStage();
			shaderCreateInfo.pName = shader->getShaderEntryPoint().c_str();
			shaderCreateInfo.module = shader->getShaderModule();

			shaderCreateInfos.push_back(shaderCreateInfo);
		}
	}
}