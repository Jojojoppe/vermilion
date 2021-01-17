#ifdef VMCORE_VULKAN
#include "RenderTarget.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include <vermilion/window.hpp>
#include "vkPhysicalDevice.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"
#include "vk_mem_alloc.h"

#include <string.h>
#include <stdexcept>
#include <cstdint>
#include <array>


Vermilion::Core::Vulkan::RenderTarget::RenderTarget(Vermilion::Core::Vulkan::API * api, std::shared_ptr<Vermilion::Core::Texture> texture){
	this->api = api;
	this->instance = api->instance;
	this->texture = std::static_pointer_cast<Vermilion::Core::Vulkan::Texture>(texture);

	if(texture==nullptr){
		return;
	}

	// Create depth image
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = this->texture->width;
	imageInfo.extent.height = this->texture->height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;         // TODO use LINEAIR for direct access
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional
	VmaAllocationCreateInfo IallocInfo = {};
	IallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	if(vmaCreateImage(api->vma_allocator, &imageInfo, &IallocInfo, &this->depthImage, &this->depthImageMemory, nullptr)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create depth image");
		throw std::runtime_error("Vermilion::Core::Vulkan::RenderTarger::create() - Could not create depth image");
	}

	// Create image view
	depthImageView.reset(new Vermilion::Core::Vulkan::vkImageView2D(this->api, this->depthImage, imageInfo.format, VK_IMAGE_ASPECT_DEPTH_BIT));
	// Create render pass
	this->renderpass.reset(new Vermilion::Core::Vulkan::vkRenderPass(api, this->texture->format, imageInfo.format));
	// Create framebuffer
	this->framebuffer.reset(new Vermilion::Core::Vulkan::vkFrameBuffer(api, this->texture->width, this->texture->height, this->texture->vk_imageView->imageView, this->depthImageView->imageView, this->renderpass->vk_renderPass));

	// Create command buffers for render target
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = api->vk_commandPool->vk_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	if(vkAllocateCommandBuffers(api->vk_device->vk_device, &allocInfo, &vk_commandBuffer) != VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to create command buffer for render target");
		throw std::runtime_error("Vermilion::Core::Vulkan::RenderTarget::RenderTarget() - Failed to create command buffer for render target");
	}
}

Vermilion::Core::Vulkan::RenderTarget::~RenderTarget(){
	depthImageView.reset();
	vmaDestroyImage(api->vma_allocator, this->depthImage, this->depthImageMemory);
}

void Vermilion::Core::Vulkan::RenderTarget::start(){
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional
	
	if(vkBeginCommandBuffer(vk_commandBuffer, &beginInfo) != VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to start command buffer recording");
		throw std::runtime_error("Vermilion::Core::Vulkan::RenderTarget::start() - Failed to start command buffer recording");
	}
	
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = this->renderpass->vk_renderPass;
	renderPassInfo.framebuffer = this->framebuffer->vk_frameBuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = {(uint32_t)this->texture->width, (uint32_t)this->texture->height};
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {1.0f, 0.0f, 0.0f, 1.0f};
	clearValues[1].depthStencil = {1.0f, 0};
	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(vk_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Vermilion::Core::Vulkan::RenderTarget::end(){
	vkCmdEndRenderPass(vk_commandBuffer);
	if(vkEndCommandBuffer(vk_commandBuffer) != VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to end command buffer recording");
		throw std::runtime_error("Vermilion::Core::Vulkan::RenderTarget::end() - Failed to end command buffer recording");
	}
}

void Vermilion::Core::Vulkan::RenderTarget::draw(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, std::shared_ptr<Vermilion::Core::Binding> binding, std::shared_ptr<Vermilion::Core::Renderable> renderable, int instanceCount, int firstInstance){
	std::shared_ptr<Vermilion::Core::Vulkan::VertexBuffer> vkVertexBuffer = std::static_pointer_cast<Vermilion::Core::Vulkan::VertexBuffer>(renderable->vertexBuffer);	
	std::shared_ptr<Vermilion::Core::Vulkan::IndexBuffer> vkIndexBuffer = std::static_pointer_cast<Vermilion::Core::Vulkan::IndexBuffer>(renderable->indexBuffer);	
	std::shared_ptr<Vermilion::Core::Vulkan::Pipeline> vkPipeline = std::static_pointer_cast<Vermilion::Core::Vulkan::Pipeline>(pipeline);
	std::shared_ptr<Vermilion::Core::Vulkan::Binding> vkBinding = std::static_pointer_cast<Vermilion::Core::Vulkan::Binding>(binding);

	VkBuffer vertexBuffers[] = {vkVertexBuffer->vk_buffer};
	VkDeviceSize offsets[] = {0};

	vkPipeline->bind(binding);

	vkCmdBindPipeline(vk_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->vk_pipeline);
	vkCmdSetViewport(vk_commandBuffer, 0, 1, &vkPipeline->viewport);
	vkCmdSetScissor(vk_commandBuffer, 0, 1, &vkPipeline->scissor);
	vkCmdBindVertexBuffers(vk_commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(vk_commandBuffer, vkIndexBuffer->vk_buffer, 0, VK_INDEX_TYPE_UINT32);
	// TODO fix the array in descriptorSets[vkBinding]
	vkCmdBindDescriptorSets(vk_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->vk_pipelineLayout, 0, 1, &vkPipeline->descriptorSets[vkBinding][0], 0, nullptr);
	vkCmdDrawIndexed(vk_commandBuffer, renderable->length, instanceCount, renderable->indexOffset, renderable->vertexOffset, firstInstance);
}

VkFormat Vermilion::Core::Vulkan::RenderTarget::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(api->vk_physicaldevice->vk_physicaldevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

// DEFAULT RENDER TARGET -----------------------------------------------------------------------------------

Vermilion::Core::Vulkan::DefaultRenderTarget::DefaultRenderTarget(Vermilion::Core::Vulkan::API * api, std::shared_ptr<Vermilion::Core::Texture> texture) :
		Vermilion::Core::Vulkan::RenderTarget(api, texture){
	this->api = api;
	this->instance = api->instance;

	this->create();
}

Vermilion::Core::Vulkan::DefaultRenderTarget::~DefaultRenderTarget(){	
}

void Vermilion::Core::Vulkan::DefaultRenderTarget::start(){
	int i = this->api->imageIndex;
	// for (size_t i = 0; i < vk_commandBuffers.size(); i++) {
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
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues.data();
		vkCmdBeginRenderPass(vk_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	// }
}

void Vermilion::Core::Vulkan::DefaultRenderTarget::end(){
	int i = this->api->imageIndex;
	// for(int i=0; i<vk_commandBuffers.size(); i++){
		vkCmdEndRenderPass(vk_commandBuffers[i]);
		if(vkEndCommandBuffer(vk_commandBuffers[i]) != VK_SUCCESS){
			this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to end command buffer recording");
			throw std::runtime_error("Vermilion::Core::Vulkan::RenderTarget::end() - Failed to end command buffer recording");
		}
	// }
}

void Vermilion::Core::Vulkan::DefaultRenderTarget::draw(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, std::shared_ptr<Vermilion::Core::Binding> binding, std::shared_ptr<Vermilion::Core::Renderable> renderable, int instanceCount, int firstInstance){
	std::shared_ptr<Vermilion::Core::Vulkan::VertexBuffer> vkVertexBuffer = std::static_pointer_cast<Vermilion::Core::Vulkan::VertexBuffer>(renderable->vertexBuffer);	
	std::shared_ptr<Vermilion::Core::Vulkan::IndexBuffer> vkIndexBuffer = std::static_pointer_cast<Vermilion::Core::Vulkan::IndexBuffer>(renderable->indexBuffer);	
	std::shared_ptr<Vermilion::Core::Vulkan::Pipeline> vkPipeline = std::static_pointer_cast<Vermilion::Core::Vulkan::Pipeline>(pipeline);
	std::shared_ptr<Vermilion::Core::Vulkan::Binding> vkBinding = std::static_pointer_cast<Vermilion::Core::Vulkan::Binding>(binding);

	VkBuffer vertexBuffers[] = {vkVertexBuffer->vk_buffer};
	VkDeviceSize offsets[] = {0};

	vkPipeline->bind(binding);

	int i = this->api->imageIndex;
	// for(int i=0; i<vk_commandBuffers.size(); i++){
		vkCmdBindPipeline(vk_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->vk_pipeline);
	vkCmdSetViewport(vk_commandBuffers[i], 0, 1, &vkPipeline->viewport);
	vkCmdSetScissor(vk_commandBuffers[i], 0, 1, &vkPipeline->scissor);
		vkCmdBindVertexBuffers(vk_commandBuffers[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(vk_commandBuffers[i], vkIndexBuffer->vk_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(vk_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->vk_pipelineLayout, 0, 1, &vkPipeline->descriptorSets[vkBinding][i], 0, nullptr);
		vkCmdDrawIndexed(vk_commandBuffers[i], renderable->length, instanceCount, renderable->indexOffset, renderable->vertexOffset, firstInstance);
	// }
}

void Vermilion::Core::Vulkan::DefaultRenderTarget::create(){

	// Create depth image
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = api->vk_swapchain->swapChainExtent.width;
	imageInfo.extent.height = api->vk_swapchain->swapChainExtent.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;         // TODO use LINEAIR for direct access
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional
	VmaAllocationCreateInfo IallocInfo = {};
	IallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	if(vmaCreateImage(api->vma_allocator, &imageInfo, &IallocInfo, &this->depthImage, &this->depthImageMemory, nullptr)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create depth image");
		throw std::runtime_error("Vermilion::Core::Vulkan::RenderTarger::create() - Could not create depth image");
	}

	// Create image view
	depthImageView.reset(new Vermilion::Core::Vulkan::vkImageView2D(this->api, this->depthImage, imageInfo.format, VK_IMAGE_ASPECT_DEPTH_BIT));

	this->renderpass.reset(new vkRenderPass(api, api->vk_swapchain->swapChainImageFormat, imageInfo.format));


	int swapchainsize = api->vk_swapchain->swapChainImages.size();
	this->framebuffers.resize(swapchainsize);
	for(int i=0; i<swapchainsize; i++){
		this->framebuffers[i].reset(new Vermilion::Core::Vulkan::vkFrameBuffer(api, 
					api->vk_swapchain->swapChainExtent.width, api->vk_swapchain->swapChainExtent.height, 
					api->vk_swapchain->swapChainImageViews[i]->imageView, this->depthImageView->imageView, this->renderpass->vk_renderPass));
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

void Vermilion::Core::Vulkan::DefaultRenderTarget::reset(){
	depthImageView.reset();
	vmaDestroyImage(api->vma_allocator, this->depthImage, this->depthImageMemory);
}


#endif