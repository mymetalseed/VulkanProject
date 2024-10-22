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
			mDatas = {
				{{0.0f,-0.5f,0.0f},  {1.0f,0.0f,0.0f}},
				{{0.5f,0.5f,0.0f},  {0.0f,1.0f,0.0f}},
				{{-0.5f,0.5f,0.0f},  {0.0f,0.0f,1.0f}},
			};

			mIndexDatas = {
				0,1,2
			};

			mVertexBuffer = Wrapper::Buffer::createVertexBuffer(device,mDatas.size()*sizeof(Vertex),mDatas.data());
			mIndexBuffer = Wrapper::Buffer::createIndexBuffer(device, mIndexDatas.size() * sizeof(unsigned int), mIndexDatas.data());
		}

		~Model() {}

		std::vector<VkVertexInputBindingDescription> getVertexInputBindingDescription() {
			std::vector<VkVertexInputBindingDescription> bindingDes{};
			bindingDes.resize(1);
			bindingDes[0].binding = 0;
			bindingDes[0].stride = sizeof(Vertex);
			bindingDes[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDes;
		}

		//Attribute相关信息，与VertexShader里面的location相关
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDes{};
			attributeDes.resize(2);

			attributeDes[0].binding = 0;
			attributeDes[0].location = 0;
			attributeDes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDes[0].offset = offsetof(Vertex, mPosition);

			attributeDes[1].binding = 0;
			attributeDes[1].location = 1;
			attributeDes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDes[1].offset = offsetof(Vertex, mColor);

			return attributeDes;
		}

		[[nodiscard]] auto getVertexBuffer() const { return mVertexBuffer; }
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
		std::vector<Vertex> mDatas{};
		std::vector<unsigned int> mIndexDatas{};

		Wrapper::Buffer::Ptr mVertexBuffer{ nullptr };
		Wrapper::Buffer::Ptr mIndexBuffer{ nullptr };

		ObjectUniform mUniform;

		float mAngle{ 0.0f };
	};
}