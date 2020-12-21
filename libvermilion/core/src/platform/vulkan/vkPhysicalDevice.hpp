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

class vkPhysicalDevice{
	public:
		VkPhysicalDevice vk_physicaldevice = VK_NULL_HANDLE;

	private:
		API * api;
		Vermilion::Core::Instance * instance;

	public:
		vkPhysicalDevice(API * api);
		~vkPhysicalDevice();

	private:
};

}}}
#endif
