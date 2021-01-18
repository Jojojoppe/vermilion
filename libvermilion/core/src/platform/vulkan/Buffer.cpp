#ifdef VMCORE_VULKAN
#include "Buffer.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

void Vermilion::Core::Vulkan::createBuffer(API * api, size_t size, VkBufferUsageFlags usage, VmaMemoryUsage properties, VkBuffer& buffer, VmaAllocation& memory){
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = properties;
	VkResult res = vmaCreateBuffer(api->vma_allocator, &bufferInfo, &allocInfo, &buffer, &memory, nullptr);
	if(res!=VK_SUCCESS){
		api->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create buffer or allocate memory for buffer: %d", res);
		throw std::runtime_error("Vermilion::Core::Vulkan::createBuffer() - Could not create buffer or allocate memory for buffer");
	}
	return;
}

Vermilion::Core::Vulkan::VertexBuffer::VertexBuffer(Vermilion::Core::Vulkan::API* api, size_t size, Vermilion::Core::BufferType type){
	this->api = api;
	this->instance = api->instance;
	this->size = size;
	this->type = type;

	if(type==Vermilion::Core::BufferType::BUFFER_TYPE_STATIC){
		// Create vertex buffer	
		Vermilion::Core::Vulkan::createBuffer(api, this->size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, vk_buffer, vk_allocation);
	}else{
		Vermilion::Core::Vulkan::createBuffer(api, this->size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, vk_buffer, vk_allocation);
	}
}

Vermilion::Core::Vulkan::VertexBuffer::~VertexBuffer(){
	vmaDestroyBuffer(api->vma_allocator, vk_buffer, vk_allocation);
}

void Vermilion::Core::Vulkan::VertexBuffer::setData(void * data, size_t size){
	if(size==0 || size>this->size){
		size = this->size;
	}
	if(this->type==Vermilion::Core::BufferType::BUFFER_TYPE_STATIC){
		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferMemory;
		Vermilion::Core::Vulkan::createBuffer(api, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer, stagingBufferMemory);

		// Fill buffer
		void * gpudata;
		vmaMapMemory(api->vma_allocator, stagingBufferMemory, &gpudata);
		memcpy(gpudata, data, size);
		vmaUnmapMemory(api->vma_allocator, stagingBufferMemory);

		api->beginSingleTimeCommands();
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(api->vk_commandBuffer, stagingBuffer, vk_buffer, 1, &copyRegion);
		api->endSingleTimeCommands();

		vmaDestroyBuffer(api->vma_allocator, stagingBuffer, stagingBufferMemory);
	}else{
		// Fill buffer
		void * gpudata;
		vmaMapMemory(api->vma_allocator, this->vk_allocation, &gpudata);
		memcpy(gpudata, data, size);
		vmaUnmapMemory(api->vma_allocator, this->vk_allocation);
	}
}

Vermilion::Core::Vulkan::IndexBuffer::IndexBuffer(Vermilion::Core::Vulkan::API* api, size_t size, Vermilion::Core::BufferType type){
	this->api = api;
	this->instance = api->instance;
	this->size = size;
	this->type = type;

	if(type==Vermilion::Core::BufferType::BUFFER_TYPE_STATIC){
		Vermilion::Core::Vulkan::createBuffer(api, this->size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, vk_buffer, vk_allocation);
	}else{
		Vermilion::Core::Vulkan::createBuffer(api, this->size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, vk_buffer, vk_allocation);
	}
}

Vermilion::Core::Vulkan::IndexBuffer::~IndexBuffer(){
	vmaDestroyBuffer(api->vma_allocator, vk_buffer, vk_allocation);
}

void Vermilion::Core::Vulkan::IndexBuffer::setData(void * data, size_t size){
	if(size==0 || size>this->size){
		size = this->size;
	}
	if(this->type==Vermilion::Core::BufferType::BUFFER_TYPE_STATIC){
		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferMemory;
		Vermilion::Core::Vulkan::createBuffer(api, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer, stagingBufferMemory);

		// Fill buffer
		void * gpudata;
		vmaMapMemory(api->vma_allocator, stagingBufferMemory, &gpudata);
		memcpy(gpudata, data, size);
		vmaUnmapMemory(api->vma_allocator, stagingBufferMemory);

		api->beginSingleTimeCommands();
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(api->vk_commandBuffer, stagingBuffer, vk_buffer, 1, &copyRegion);
		api->endSingleTimeCommands();

		vmaDestroyBuffer(api->vma_allocator, stagingBuffer, stagingBufferMemory);
	}else{
		// Fill buffer
		void * gpudata;
		vmaMapMemory(api->vma_allocator, this->vk_allocation, &gpudata);
		memcpy(gpudata, data, size);
		vmaUnmapMemory(api->vma_allocator, this->vk_allocation);
	}
}

Vermilion::Core::Vulkan::UniformBuffer::UniformBuffer(Vermilion::Core::Vulkan::API* api, size_t size, Vermilion::Core::BufferType type){
	this->api = api;
	this->instance = api->instance;
	this->size = size;
	this->type = type;

	if(type==Vermilion::Core::BufferType::BUFFER_TYPE_STATIC){
		Vermilion::Core::Vulkan::createBuffer(api, this->size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, vk_buffer, vk_allocation);
	}else{
		Vermilion::Core::Vulkan::createBuffer(api, this->size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, vk_buffer, vk_allocation);
	}
}

Vermilion::Core::Vulkan::UniformBuffer::~UniformBuffer(){
	vmaDestroyBuffer(api->vma_allocator, vk_buffer, vk_allocation);
}

void Vermilion::Core::Vulkan::UniformBuffer::setData(void * data, size_t size){
	if(size==0 || size>this->size){
		size = this->size;
	}
	if(this->type==Vermilion::Core::BufferType::BUFFER_TYPE_STATIC){
		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferMemory;
		Vermilion::Core::Vulkan::createBuffer(api, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer, stagingBufferMemory);

		// Fill buffer
		void * gpudata;
		vmaMapMemory(api->vma_allocator, stagingBufferMemory, &gpudata);
		memcpy(gpudata, data, size);
		vmaUnmapMemory(api->vma_allocator, stagingBufferMemory);

		api->beginSingleTimeCommands();
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(api->vk_commandBuffer, stagingBuffer, vk_buffer, 1, &copyRegion);
		api->endSingleTimeCommands();

		vmaDestroyBuffer(api->vma_allocator, stagingBuffer, stagingBufferMemory);
	}else{
		// Fill buffer
		void * gpudata;
		vmaMapMemory(api->vma_allocator, this->vk_allocation, &gpudata);
		memcpy(gpudata, data, size);
		vmaUnmapMemory(api->vma_allocator, this->vk_allocation);
	}
}

Vermilion::Core::Vulkan::StorageBuffer::StorageBuffer(Vermilion::Core::Vulkan::API* api, size_t size, Vermilion::Core::BufferType type){
	this->api = api;
	this->instance = api->instance;
	this->size = size;
	this->type = type;

	if(type==Vermilion::Core::BufferType::BUFFER_TYPE_STATIC){
		Vermilion::Core::Vulkan::createBuffer(api, this->size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, vk_buffer, vk_allocation);
	}else{
		Vermilion::Core::Vulkan::createBuffer(api, this->size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, vk_buffer, vk_allocation);
	}
}

Vermilion::Core::Vulkan::StorageBuffer::~StorageBuffer(){
	vmaDestroyBuffer(api->vma_allocator, vk_buffer, vk_allocation);
}

void Vermilion::Core::Vulkan::StorageBuffer::setData(void * data, size_t size){
	if(size==0 || size>this->size){
		size = this->size;
	}
	if(this->type==Vermilion::Core::BufferType::BUFFER_TYPE_STATIC){
		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferMemory;
		Vermilion::Core::Vulkan::createBuffer(api, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer, stagingBufferMemory);

		// Fill buffer
		void * gpudata;
		vmaMapMemory(api->vma_allocator, stagingBufferMemory, &gpudata);
		memcpy(gpudata, data, size);
		vmaUnmapMemory(api->vma_allocator, stagingBufferMemory);

		api->beginSingleTimeCommands();
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(api->vk_commandBuffer, stagingBuffer, vk_buffer, 1, &copyRegion);
		api->endSingleTimeCommands();

		vmaDestroyBuffer(api->vma_allocator, stagingBuffer, stagingBufferMemory);
	}else{
		// Fill buffer
		void * gpudata;
		vmaMapMemory(api->vma_allocator, this->vk_allocation, &gpudata);
		memcpy(gpudata, data, size);
		vmaUnmapMemory(api->vma_allocator, this->vk_allocation);
	}
}



#endif
