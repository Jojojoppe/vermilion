#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <vector>
#include <optional>
#include <memory>

#include "vkImageView.hpp"

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class vkSwapChain{
	public:
		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		std::vector<std::unique_ptr<vkImageView2D>> swapChainImageViews;
	
	private:
		API * api;
		Vermilion::Core::Instance * instance;

	public:
		vkSwapChain(API * api);
		~vkSwapChain();

	private:
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};

}}}
#endif
