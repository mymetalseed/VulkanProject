#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	/*
	fence是控制一次队列提交的标志，与Semaphore区别，semaphore控制单一命令提交信息内的
	不通知行阶段之间的依赖关系，semaphore无法手动用api去激发
	fence 控制一个队列(GraphicQueue)里面一次性提交的所有指令执行完毕
	fence 分为激发态/飞激发态，并且可以进行api级别的控制(就是类似于锁)
	*/
	class Fence {
	public:
		using Ptr = std::shared_ptr<Fence>;
		static Ptr create(const Device::Ptr& device,bool signaled = true) {
			return std::make_shared<Fence>(device, signaled);
		}

		Fence(const Device::Ptr& device,bool signaled = true);
		~Fence();

		//置为非激发态
		void resetFence();

		//调用此函数，如果fence没有被激发，那么就阻塞在这里，等待激发
		void block(uint64_t timeout = UINT64_MAX);

		[[nodiscard]] auto getFence()const { return mFence; }

	private:
		VkFence mFence{VK_NULL_HANDLE};
		Device::Ptr mDevice{ nullptr };
	};
}