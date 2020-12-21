#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <vector>

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class vkDevice{
	public:
		VkDevice vk_device = VK_NULL_HANDLE;
		VkQueue vk_graphicsQueue;
		VkQueue vk_presentQueue;

	private:
		API * api;
		Vermilion::Core::Instance * instance;

	public:
		vkDevice(API * api);
		~vkDevice();

	private:
};

}}}
#endif
