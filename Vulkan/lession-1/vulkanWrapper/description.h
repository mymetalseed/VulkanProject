#pragma once
#include "buffer.h"
#include "../Texture/texture.h"

namespace FF::Wrapper {
	//����֪�� ���ֵ��У���������Щuniform��ÿһ��������binding��ÿһ����ʲô����
	struct UniformParameter {
		using Ptr = std::shared_ptr<UniformParameter>;
		static Ptr create() {
			return std::make_shared<UniformParameter>();
		}

		size_t mSize{ 0 };
		uint32_t mBinding{ 0 };
		//����ÿһ��binding�㣬���п��ܴ��벻ֹһ��uniform,������һ��Matrix[],count��������Ĵ�С
		//��Ҫʹ��indexedDescriptor����
		uint32_t mCount{ 0 };
		VkDescriptorType mDescriptorType;
		VkShaderStageFlagBits mStage;

		std::vector<Buffer::Ptr> mBuffers{};
		Texture::Ptr mTexture;
	};
}

