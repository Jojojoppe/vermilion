#ifdef VMCORE_VULKAN
#include "RenderTarget.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include <vermilion/window.hpp>
#include "vkPhysicalDevice.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"

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

	this->create();
}

Vermilion::Core::Vulkan::RenderTarget::~RenderTarget(){	
}

void Vermilion::Core::Vulkan::RenderTarget::start(){
	for (size_t i = 0; i < vk_commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if(vkBeginCommandBuffer(vk_commandBuffers[i], &beginInfo) != VK_SUCCESS){
			this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to start command buffer recording");
			throw std::runtime_error("Vermilion::Core::Vulkan::RenderTarget::start() - Failed to start command buffer recording");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = this->renderpass->vk_renderPass;
		renderPassInfo.framebuffer = this->framebuffers[i]->vk_frameBuffer;
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = this->extent;
		VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(vk_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
}

void Vermilion::Core::Vulkan::RenderTarget::end(){
	for(int i=0; i<vk_commandBuffers.size(); i++){
		vkCmdEndRenderPass(vk_commandBuffers[i]);
		if(vkEndCommandBuffer(vk_commandBuffers[i]) != VK_SUCCESS){
			this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to end command buffer recording");
			throw std::runtime_error("Vermilion::Core::Vulkan::RenderTarget::end() - Failed to end command buffer recording");
		}
	}
}

void Vermilion::Core::Vulkan::RenderTarget::draw(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, std::shared_ptr<Vermilion::Core::Renderable> renderable, int instanceCount, int firstInstance){
	std::shared_ptr<Vermilion::Core::Vulkan::VertexBuffer> vkVertexBuffer = std::static_pointer_cast<Vermilion::Core::Vulkan::VertexBuffer>(renderable->vertexBuffer);	
	std::shared_ptr<Vermilion::Core::Vulkan::IndexBuffer> vkIndexBuffer = std::static_pointer_cast<Vermilion::Core::Vulkan::IndexBuffer>(renderable->indexBuffer);	

	VkBuffer vertexBuffers[] = {vkVertexBuffer->vk_buffer};
	VkDeviceSize offsets[] = {0};

	for(int i=0; i<vk_commandBuffers.size(); i++){
		vkCmdBindPipeline(vk_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, std::static_pointer_cast<Vermilion::Core::Vulkan::Pipeline>(pipeline)->vk_pipeline);
		vkCmdBindVertexBuffers(vk_commandBuffers[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(vk_commandBuffers[i], vkIndexBuffer->vk_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(vk_commandBuffers[i], renderable->length, instanceCount, renderable->indexOffset, renderable->vertexOffset, firstInstance);
	}
}

void Vermilion::Core::Vulkan::RenderTarget::create(){
	this->renderpass.reset(new vkRenderPass(api, api->vk_swapchain->swapChainImageFormat));
	int swapchainsize = api->vk_swapchain->swapChainImages.size();
	this->framebuffers.resize(swapchainsize);
	for(int i=0; i<swapchainsize; i++){
		this->framebuffers[i].reset(new Vermilion::Core::Vulkan::vkFrameBuffer(api, 
					api->vk_swapchain->swapChainExtent.width, api->vk_swapchain->swapChainExtent.height, 
					api->vk_swapchain->swapChainImageViews[i]->imageView, this->renderpass->vk_renderPass));
	}
	this->extent = api->vk_swapchain->swapChainExtent;

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

void Vermilion::Core::Vulkan::RenderTarget::reset(){

}

#endif