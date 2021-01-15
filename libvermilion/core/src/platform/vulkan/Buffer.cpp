#ifdef VMCORE_VULKAN
#include "Buffer.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

Vermilion::Core::Vulkan::VertexBuffer::VertexBuffer(Vermilion::Core::Vulkan::API* api, std::vector<float>& vertices){
	this->api = api;
	this->instance = api->instance;
	this->element_size = sizeof(float);
	this->count = vertices.size();
	this->size = this->element_size*this->count;

	// Create staging buffer
	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferMemory;
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.requiredFlags = VMA_MEMORY_USAGE_CPU_TO_GPU;
	if(vmaCreateBuffer(api->vma_allocator, &bufferInfo, &allocInfo, &stagingBuffer, &stagingBufferMemory, nullptr)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create vertex staging buffer");
		throw std::runtime_error("Vermilion::Core::Vulkan::VertexBuffer::VertexBuffer() - Could not create vertex staging buffer");
	}

	// Fill buffer
	void * gpudata;
	vmaMapMemory(api->vma_allocator, stagingBufferMemory, &gpudata);
	memcpy(gpudata, vertices.data(), size);
	vmaUnmapMemory(api->vma_allocator, stagingBufferMemory);
	
	bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	allocInfo = {};
	allocInfo.requiredFlags = VMA_MEMORY_USAGE_GPU_ONLY;
	if(vmaCreateBuffer(api->vma_allocator, &bufferInfo, &allocInfo, &vk_buffer, &vk_allocation, nullptr)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create vertex buffer");
		throw std::runtime_error("Vermilion::Core::Vulkan::VertexBuffer::VertexBuffer() - Could not create vertex buffer");
	}
	
	{
		// Copy to buffer itself on GPU
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = api->vk_commandPool->vk_commandPool;
		allocInfo.commandBufferCount = 1;
		    
		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(api->vk_device->vk_device, &allocInfo, &commandBuffer);
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, stagingBuffer, vk_buffer, 1, &copyRegion);
		
		vkEndCommandBuffer(commandBuffer);
		
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		
		vkQueueSubmit(api->vk_device->vk_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(api->vk_device->vk_graphicsQueue);
		
		vkFreeCommandBuffers(api->vk_device->vk_device, api->vk_commandPool->vk_commandPool, 1, &commandBuffer);
	}


	vmaDestroyBuffer(api->vma_allocator, stagingBuffer, stagingBufferMemory);
}

Vermilion::Core::Vulkan::VertexBuffer::~VertexBuffer(){
	vmaDestroyBuffer(api->vma_allocator, vk_buffer, vk_allocation);
}

Vermilion::Core::Vulkan::IndexBuffer::IndexBuffer(Vermilion::Core::Vulkan::API* api, std::vector<unsigned int>& indices){
	this->api = api;
	this->instance = api->instance;
	this->element_size = sizeof(float);
	this->count = indices.size();
	this->size = this->element_size*this->count;

	// Create staging buffer
	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferMemory;
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.requiredFlags = VMA_MEMORY_USAGE_CPU_TO_GPU;
	if(vmaCreateBuffer(api->vma_allocator, &bufferInfo, &allocInfo, &stagingBuffer, &stagingBufferMemory, nullptr)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create index staging buffer");
		throw std::runtime_error("Vermilion::Core::Vulkan::IndexBuffer::IndexBuffer() - Could not create index staging buffer");
	}

	// Fill buffer
	void * gpudata;
	vmaMapMemory(api->vma_allocator, stagingBufferMemory, &gpudata);
	memcpy(gpudata, indices.data(), size);
	vmaUnmapMemory(api->vma_allocator, stagingBufferMemory);
	
	bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	allocInfo = {};
	allocInfo.requiredFlags = VMA_MEMORY_USAGE_GPU_ONLY;
	if(vmaCreateBuffer(api->vma_allocator, &bufferInfo, &allocInfo, &vk_buffer, &vk_allocation, nullptr)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create index buffer");
		throw std::runtime_error("Vermilion::Core::Vulkan::Indexuffer::IndexBuffer() - Could not create index buffer");
	}
	
	{
		// Copy to buffer itself on GPU
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = api->vk_commandPool->vk_commandPool;
		allocInfo.commandBufferCount = 1;
		    
		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(api->vk_device->vk_device, &allocInfo, &commandBuffer);
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, stagingBuffer, vk_buffer, 1, &copyRegion);
		
		vkEndCommandBuffer(commandBuffer);
		
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		
		vkQueueSubmit(api->vk_device->vk_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(api->vk_device->vk_graphicsQueue);
		
		vkFreeCommandBuffers(api->vk_device->vk_device, api->vk_commandPool->vk_commandPool, 1, &commandBuffer);
	}


	vmaDestroyBuffer(api->vma_allocator, stagingBuffer, stagingBufferMemory);
}

Vermilion::Core::Vulkan::IndexBuffer::~IndexBuffer(){
	vmaDestroyBuffer(api->vma_allocator, vk_buffer, vk_allocation);
}

Vermilion::Core::Vulkan::UniformBuffer::UniformBuffer(Vermilion::Core::Vulkan::API* api, size_t length){
	this->api = api;
	this->instance = api->instance;
	this->size = length;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.requiredFlags = VMA_MEMORY_USAGE_CPU_TO_GPU;

	vk_buffer.resize(api->vk_swapchain->swapChainImages.size());
	vk_allocation.resize(api->vk_swapchain->swapChainImages.size());

	for(int i=0; i<api->vk_swapchain->swapChainImages.size(); i++){
		if(vmaCreateBuffer(api->vma_allocator, &bufferInfo, &allocInfo, &vk_buffer[i], &vk_allocation[i], nullptr)!=VK_SUCCESS){
			this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create uniform buffer");
			throw std::runtime_error("Vermilion::Core::Vulkan::UniformBuffer::UniformBuffer() - Could not create uniform buffer");
		}
	}
}

Vermilion::Core::Vulkan::UniformBuffer::~UniformBuffer(){
	for(int i=0; i<api->vk_swapchain->swapChainImages.size(); i++){
		vmaDestroyBuffer(api->vma_allocator, vk_buffer[i], vk_allocation[i]);
	}
}

void Vermilion::Core::Vulkan::UniformBuffer::streamData(void * data){
	void* gpudata;
	vmaMapMemory(api->vma_allocator, vk_allocation[api->imageIndex], &gpudata);
	memcpy(gpudata, data, this->size);
	vmaUnmapMemory(api->vma_allocator, vk_allocation[api->imageIndex]);
}

#endif
