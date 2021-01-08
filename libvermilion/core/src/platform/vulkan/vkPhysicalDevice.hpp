#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <vector>
#include <optional>

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

struct QueueFamilyIndices{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class vkPhysicalDevice{
	public:
		VkPhysicalDevice vk_physicaldevice = VK_NULL_HANDLE;

	private:
		API * api;
		Vermilion::Core::Instance * instance;

	public:
		vkPhysicalDevice(API * api);
		~vkPhysicalDevice();

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	private:
};

}}}
#endif
