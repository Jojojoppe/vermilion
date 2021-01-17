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

class vkImageView2D{
	public:
		VkImageView imageView;
	
	private:
		API * api;
		Vermilion::Core::Instance * instance;

	public:
		vkImageView2D(API * api, VkImage image, VkFormat format, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
		~vkImageView2D();

	private:
};

}}}
#endif
