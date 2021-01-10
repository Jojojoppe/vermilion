#ifdef VMCORE_VULKAN
#include "vkFrameBuffer.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include <vermilion/window.hpp>
#include "vkPhysicalDevice.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

Vermilion::Core::Vulkan::vkFrameBuffer::vkFrameBuffer(API * api, int width, int height, VkImageView imageview, VkRenderPass renderpass){
	this->api = api;
	this->instance = api->instance;

	VkImageView attachments[] = {
		imageview
	};

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderpass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = 1;

	if(vkCreateFramebuffer(api->vk_device->vk_device, &framebufferInfo, nullptr, &vk_frameBuffer) != VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to create frame buffer");
		throw std::runtime_error("Vermilion::Core::Vulkan::vkFrameBuffer::vkFrameBuffer() - Failed to create frame buffer");
	}
}

Vermilion::Core::Vulkan::vkFrameBuffer::~vkFrameBuffer(){	
	vkDestroyFramebuffer(this->api->vk_device->vk_device, vk_frameBuffer, nullptr);
}

#endif
