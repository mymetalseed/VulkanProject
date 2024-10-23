#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {

	/*
	���������Ҫ��һ�ű��������������ͼƬ����ô��������
	��Ҫ��undefinedLayout�任��ΪTransferDst,Ȼ�������ݿ���
	���֮����ת����ShaderReadOnly
	*/

	class Image {
	public:
		using Ptr = std::shared_ptr<Image>;
		static Ptr create(
			const Device::Ptr& device,
			const int& width,
			const int& height,
			const VkFormat& format,
			const VkImageType& imageType,
			const VkImageTiling& tiling,
			const VkImageUsageFlags& usage,
			const VkSampleCountFlagBits& sample,
			const VkMemoryPropertyFlags& properties,
			const VkImageAspectFlags &aspectFlags//view
		) {
			return std::make_shared<Image>(device, width,height, format, imageType, tiling, usage, sample, properties, aspectFlags);
		}

		Image(
			const Device::Ptr& device,
			const int& width,
			const int& height,
			const VkFormat& format,
			const VkImageType& imageType,
			const VkImageTiling& imageTiling,
			const VkImageUsageFlags& usage,
			const VkSampleCountFlagBits& sample,
			const VkMemoryPropertyFlags &properties,
			const VkImageAspectFlags& aspectFlags//view
		);
		~Image();

		void setImageLayout(
			VkImageLayout newLayout,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			VkImageSubresourceRange subresourceRange
		);

	private:
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	private:
		Device::Ptr mDevice{ nullptr };
		VkImage mImage{ VK_NULL_HANDLE };
		VkDeviceMemory mImageMemory{ VK_NULL_HANDLE };
		VkImageView mImageView{ VK_NULL_HANDLE };

		VkImageLayout mLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
	};
}