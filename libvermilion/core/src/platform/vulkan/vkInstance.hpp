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

class vkInstance{
	public:
		VkInstance vk_instance;

	private:
		API * api;
		Vermilion::Core::Instance * instance;

	public:
		vkInstance(API * api);
		~vkInstance();

	private:
		bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
};

}}}
#endif
