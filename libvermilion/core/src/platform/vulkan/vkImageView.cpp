#ifdef VMCORE_VULKAN
#include "vkImageView.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include <vermilion/window.hpp>
#include "vkPhysicalDevice.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

Vermilion::Core::Vulkan::vkImageView2D::vkImageView2D(API * api, VkImage image, VkFormat format){
	this->api = api;
	this->instance = api->instance;

	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	if(vkCreateImageView(this->api->vk_device->vk_device, &createInfo, nullptr, &imageView) != VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to create image view");
		throw std::runtime_error("Vermilion::Core::Vulkan::vkImageView2D::vkImageView2D() - Failed to create image view");
	}
}

Vermilion::Core::Vulkan::vkImageView2D::~vkImageView2D(){	
	vkDestroyImageView(this->api->vk_device->vk_device, imageView, nullptr);
}

#endif
