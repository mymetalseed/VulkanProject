#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	//思路
	/*
	1 attachment  
		VkAttachmentDescription : 描述一个颜色或者深度模板attach的结构，并不是一个真正的attach，只是描述
		颜色与深度模板

		VkAttachmentReference 说明本个Subpass 需要的attachment其中的一个，这一个的数组索引id是多少，这一个附着的图片期望格式是什么
		VkSubPass 用来填写一个子Pass的描述结构
		VkSubpassDependency: 描述不同的子流程之间的依赖关系
	*/

	class SubPass {
	public:
		SubPass();
		~SubPass();

		void addColorAttachmentReference(const VkAttachmentReference& ref);
		void addInputAttachmentReference(const VkAttachmentReference& ref);
		void setDepthStencilAttachmentReference(const VkAttachmentReference& ref);

		void buildSubPassDescription();

		[[nodiscard]] auto getSubPassDescription() const { return mSubPassDescription; }

	private:
		VkSubpassDescription mSubPassDescription{};
		std::vector<VkAttachmentReference> mColorAttachmentReferences{};
		std::vector<VkAttachmentReference> mInputAttachmentReferences{};
		VkAttachmentReference mDepthStencilAttachmentReference{};
	};

	class RenderPass {
	public:
		using Ptr = std::shared_ptr<RenderPass>;
		static Ptr create(const Device::Ptr& device) { return std::make_shared<RenderPass>(device); }

		RenderPass(const Device::Ptr& device);
		~RenderPass();

		void addSubPass(const SubPass& subpass);
		void addDependency(const VkSubpassDependency& dependency);
		void addAttachment(const VkAttachmentDescription &attachmentDes);

		void buildRenderPass();

		[[nodiscard]] auto getRenderPass() const { return mRenderPass; }
	private:
		VkRenderPass mRenderPass{ VK_NULL_HANDLE };
		std::vector<SubPass> mSubPasses{};
		std::vector<VkSubpassDependency> mDependencies;
		std::vector<VkAttachmentDescription> mAttachmentDescriptions{};

		Device::Ptr mDevice{ nullptr };
	};
}