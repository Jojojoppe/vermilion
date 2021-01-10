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
}

Vermilion::Core::Vulkan::RenderTarget::~RenderTarget(){	
}

#endif
