#ifdef VMCORE_VULKAN
#include "RenderTarget.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include <vermilion/window.hpp>
#include "vkPhysicalDevice.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

Vermilion::Core::Vulkan::RenderTarget::RenderTarget(Vermilion::Core::Instance * instance, int width, int height){
	this->instance = instance;
	this->api = std::static_pointer_cast<Vermilion::Core::Vulkan::API>(instance->api).get();

	// TODO create Render target things
}

// Create Default render target
Vermilion::Core::Vulkan::RenderTarget::RenderTarget(Vermilion::Core::Vulkan::API * api){
	this->api = api;
	this->instance = api->instance;

	this->renderpass.reset(new vkRenderPass(api, api->vk_swapchain->swapChainImageFormat));
	int swapchainsize = api->vk_swapchain->swapChainImages.size();
	this->framebuffers.resize(swapchainsize);
	for(int i=0; i<swapchainsize; i++){
		this->framebuffers[i].reset(new Vermilion::Core::Vulkan::vkFrameBuffer(api, 
					api->vk_swapchain->swapChainExtent.width, api->vk_swapchain->swapChainExtent.height, 
					api->vk_swapchain->swapChainImageViews[i]->imageView, this->renderpass->vk_renderPass));
	}

	// Create command buffers for render target (one for each swapchain image)
	vk_commandBuffers.resize(swapchainsize);
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = api->vk_commandPool->vk_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) vk_commandBuffers.size();
	if(vkAllocateCommandBuffers(api->vk_device->vk_device, &allocInfo, vk_commandBuffers.data()) != VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to create command buffers for swapchain images");
		throw std::runtime_error("Vermilion::Core::Vulkan::RenderTarget::RenderTarget() - Failed to create command buffers for swapchain images");
	}
}

Vermilion::Core::Vulkan::RenderTarget::~RenderTarget(){	
}

#endif
