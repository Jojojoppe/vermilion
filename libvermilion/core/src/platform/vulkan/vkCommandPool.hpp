#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <vector>
#include <optional>
#include <memory>

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class vkCommandPool{
	public:
		VkCommandPool vk_commandPool;

	private:
		API * api;
		Vermilion::Core::Instance * instance;

	public:
		vkCommandPool(API * api);
		~vkCommandPool();

	private:
};

}}}
#endif
