#pragma once

#include "base.h"
#include "vulkanWrapper/buffer.h"
#include "vulkanWrapper/descriptorSetLayout.h"
#include "vulkanWrapper/descriptorPool.h"
#include "vulkanWrapper/description.h"
#include "vulkanWrapper/descriptorSet.h"

using namespace FF;

class UniformManager {
public:
	using Ptr = std::shared_ptr<UniformManager>;
	static Ptr create() { 
		return std::make_shared<UniformManager>(); 
	}
	UniformManager();
	~UniformManager();

	void init(const Wrapper::Device::Ptr &device,int frameCount);

	void update(const VPMatrices &vpMatrices,const ObjectUniform &objectUniform,const int &frameCount);

	[[nodiscard]] auto getDescriptorLayout() const { return mDescriptorSetLayout; }
	[[nodiscard]] auto getDescriptorSet(int frameCount) const { return mDescriptorSet->getDescriptorSet(frameCount); }

private:
	std::vector<Wrapper::UniformParameter::Ptr> mUniformParams;

	Wrapper::DescriptorSetLayout::Ptr mDescriptorSetLayout{ nullptr };
	Wrapper::DescriptorPool::Ptr mDescriptorPool{ nullptr };
	Wrapper::DescriptorSet::Ptr mDescriptorSet{ nullptr };
};