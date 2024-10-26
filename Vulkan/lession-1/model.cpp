#include "model.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace FF {
	void Model::loadModel(const std::string& path, const Wrapper::Device::Ptr& device) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
			throw std::runtime_error("Error: failed to load model");
		}

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				//先取出顶点位置相关数据
				mPositions.push_back(attrib.vertices[3 * index.vertex_index + 0]);
				mPositions.push_back(attrib.vertices[3 * index.vertex_index + 1]);
				mPositions.push_back(attrib.vertices[3 * index.vertex_index + 2]);

				//取出uv值
				mUVs.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
				mUVs.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);

				//索引
				mIndexDatas.push_back(mIndexDatas.size());
			}
		}
		mPositionBuffer = Wrapper::Buffer::createVertexBuffer(device, mPositions.size() * sizeof(float), mPositions.data());
		//mVertexBuffer = Wrapper::Buffer::createVertexBuffer(device,mDatas.size()*sizeof(Vertex),mDatas.data());
		mIndexBuffer = Wrapper::Buffer::createIndexBuffer(device, mIndexDatas.size() * sizeof(unsigned int), mIndexDatas.data());
		mUVBuffer = Wrapper::Buffer::createVertexBuffer(device, mUVs.size() * sizeof(float), mUVs.data());
	}
}