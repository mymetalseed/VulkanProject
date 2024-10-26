#pragma once

#include "base.h"
#include "vulkanWrapper/buffer.h"
#include "vulkanWrapper/device.h"

namespace FF {
	struct Vertex {
		glm::vec3 mPosition;
		glm::vec3 mColor;
	};

	class Model {
	public:
		using Ptr = std::shared_ptr<Model>;
		static Ptr create(const Wrapper::Device::Ptr& device) {
			return std::make_shared<Model>(device);
		}
		Model(const Wrapper::Device::Ptr &device) {
			/*
			mDatas = {
				{{0.0f,-0.5f,0.0f},  {1.0f,0.0f,0.0f}},
				{{0.5f,0.5f,0.0f},  {0.0f,1.0f,0.0f}},
				{{-0.5f,0.5f,0.0f},  {0.0f,0.0f,1.0f}},
			};
			*/

			mPositions = {
				0.0f,0.5f,0.0f,
				0.5f,0.0f,0.0f,
				-0.5f,0.0f,0.0f,
				0.0f,-0.5f,0.0f,

				0.3f,0.5f,0.2f,
				1.8f,0.0f,0.2f,
				-0.8f,0.0f,0.2f,
				0.3f,-0.5f,0.2f,
			};

			mColors = {
				1.0f,0.0f,0.0f,
				0.0f,1.0f,0.0f,
				0.0f,0.0f,1.0f,
				1.0f,0.0f,0.0f,

				1.0f,0.0f,0.0f,
				0.0f,1.0f,0.0f,
				0.0f,0.0f,1.0f,
				1.0f,0.0f,0.0f,
			};

			mUVs = {
				0.0f,1.0f,
				0.0f,0.0f,
				1.0f,1.0f,
				1.0f,0.0f,

				0.0f,1.0f,
				0.0f,0.0f,
				1.0f,1.0f,
				1.0f,0.0f,
			};

			mIndexDatas = {
				0,2,1,1,2,3,
				4,6,5,5,6,7
			};

			mPositionBuffer = Wrapper::Buffer::createVertexBuffer(device, mPositions.size() * sizeof(float), mPositions.data());
			mColorBuffer = Wrapper::Buffer::createVertexBuffer(device, mColors.size() * sizeof(float), mColors.data());
			//mVertexBuffer = Wrapper::Buffer::createVertexBuffer(device,mDatas.size()*sizeof(Vertex),mDatas.data());
			mIndexBuffer = Wrapper::Buffer::createIndexBuffer(device, mIndexDatas.size() * sizeof(unsigned int), mIndexDatas.data());
			mUVBuffer = Wrapper::Buffer::createVertexBuffer(device, mUVs.size() * sizeof(float), mUVs.data());
		}

		~Model() {}

		std::vector<VkVertexInputBindingDescription> getVertexInputBindingDescription() {
			std::vector<VkVertexInputBindingDescription> bindingDes{};
			bindingDes.resize(3);
			bindingDes[0].binding = 0;
			bindingDes[0].stride = sizeof(float) * 3;
			bindingDes[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			bindingDes[1].binding = 1;
			bindingDes[1].stride = sizeof(float) * 3;
			bindingDes[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			bindingDes[2].binding = 2;
			bindingDes[2].stride = sizeof(float) * 2;
			bindingDes[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			 
			return bindingDes;
		}

		//Attribute相关信息，与VertexShader里面的location相关
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDes{};
			attributeDes.resize(3);

			attributeDes[0].binding = 0;
			attributeDes[0].location = 0;
			attributeDes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			//attributeDes[0].offset = offsetof(Vertex, mPosition);
			attributeDes[0].offset = 0;

			attributeDes[1].binding = 1;
			attributeDes[1].location = 1;
			attributeDes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			//attributeDes[1].offset = offsetof(Vertex, mColor);
			attributeDes[1].offset = 0;

			attributeDes[2].binding = 2;
			attributeDes[2].location = 2;
			attributeDes[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDes[2].offset = 0;

			return attributeDes;
		}

		[[nodiscard]] auto getVertexBuffers() const { 
			std::vector<VkBuffer> buffers{
				mPositionBuffer->getBuffer(),
				mColorBuffer->getBuffer(),
				mUVBuffer->getBuffer()
			};
			return buffers;
		}
		[[nodiscard]] auto getIndexBuffer() const { return mIndexBuffer; }
		[[nodiscard]] auto getIndexCount() const { return mIndexDatas.size(); }
		[[nodiscard]] auto getUniform() const { return mUniform; }

		void setModelMatrix(glm::mat4 matrix) {
			mUniform.mModelMatrix = matrix;
		}

		void update() {
			glm::mat4 rotateMatrix = glm::mat4(1.0f);
			rotateMatrix = glm::rotate(rotateMatrix, glm::radians(mAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			mUniform.mModelMatrix = rotateMatrix;

			mAngle += 0.01f;
		}

	private:
		std::vector<float> mPositions{};
		std::vector<float> mColors{};
		std::vector<unsigned int> mIndexDatas{};
		std::vector<float> mUVs{};

		Wrapper::Buffer::Ptr mPositionBuffer{ nullptr };
		//Wrapper::Buffer::Ptr mVertexBuffer{ nullptr };
		Wrapper::Buffer::Ptr mColorBuffer{ nullptr };
		Wrapper::Buffer::Ptr mIndexBuffer{ nullptr };
		Wrapper::Buffer::Ptr mUVBuffer{ nullptr };

		ObjectUniform mUniform;

		float mAngle{ 0.0f };
	};
}