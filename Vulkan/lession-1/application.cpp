#include "application.h"

namespace FF {

	void Application::run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanUp();
	}

	void Application::initWindow() {
		mWindow = Wrapper::Window::create(WIDTH, HEIGHT);
	}

	void Application::initVulkan() {
		mInstance = Wrapper::Instance::create(true);
		mSurface = Wrapper::WindowSurface::create(mInstance,mWindow);

		mDevice = Wrapper::Device::create(mInstance,mSurface);
		mSwapChain = Wrapper::SwapChain::create(mDevice,mWindow,mSurface);

	
		mPipeline = Wrapper::Pipeline::create(mDevice);
		createPipeline();
	}

	void Application::createPipeline() {
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)WIDTH;
		viewport.height = (float)HEIGHT;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = {0,0};
		scissor.extent = { WIDTH,HEIGHT };

		std::vector<Wrapper::Shader::Ptr> shaderGroup{};
		auto shaderVertex = Wrapper::Shader::create(mDevice, "shaders/vs.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
		shaderGroup.push_back(shaderVertex);
		auto shaderFragment = Wrapper::Shader::create(mDevice, "shaders/fs.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
		shaderGroup.push_back(shaderFragment);

		mPipeline->setShaderGroup(shaderGroup);
		//顶点的排布模式
		mPipeline->mVertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		mPipeline->mVertexInputState.vertexBindingDescriptionCount = 0;
		mPipeline->mVertexInputState.pVertexBindingDescriptions = nullptr;
		mPipeline->mVertexInputState.vertexAttributeDescriptionCount = 0;
		mPipeline->mVertexInputState.pVertexAttributeDescriptions = nullptr;

		//图元装配
		mPipeline->mAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		mPipeline->mAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mPipeline->mAssemblyState.primitiveRestartEnable = VK_FALSE;


	}

	void Application::mainLoop() {
		while (!mWindow->shouldClose()) {
			mWindow->pollEvents();
		}
	}

	void Application::cleanUp() {
		//使mInstance的智能指针置为0，保证可以析构掉
		mPipeline.reset();
		mSwapChain.reset();
		mDevice.reset();
		mSurface.reset();
		mInstance.reset();
		mWindow.reset();
	}
}