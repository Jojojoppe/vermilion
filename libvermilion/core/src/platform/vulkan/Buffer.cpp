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

Vermilion::Core::Vulkan::Buffer::Buffer(Vermilion::Core::Vulkan::API* api, size_t size, Vermilion::Core::BufferType type, Vermilion::Core::BufferUsage usage, Vermilion::Core::BufferDataUsage dataUsage){
	this->api = api;
	this->instance = api->instance;
	this->size = size;
	this->type = type;
	this->usage = usage;
	this->dataUsage = dataUsage;

	VkBufferUsageFlags vkusageflags = 0;
	VmaMemoryUsage vmausage = VMA_MEMORY_USAGE_UNKNOWN;

	switch(type){
		case BUFFER_TYPE_VERTEX:
			vkusageflags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			break;
		case BUFFER_TYPE_INDEX:
			vkusageflags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			break;
		case BUFFER_TYPE_UNIFORM:
			vkusageflags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			break;
		case BUFFER_TYPE_STORAGE:
			vkusageflags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			break;
		default:
			break;
	}

	switch(usage){
		case BUFFER_USAGE_WRITE_ONLY:
			if(dataUsage==BUFFER_DATA_USAGE_STATIC){
				vkusageflags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmausage = VMA_MEMORY_USAGE_GPU_ONLY;
			}else{
				vmausage = VMA_MEMORY_USAGE_CPU_TO_GPU;
			}
			break;
		case BUFFER_USAGE_READ_WRITE:
			if(dataUsage==BUFFER_DATA_USAGE_STATIC){
				vkusageflags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				vmausage = VMA_MEMORY_USAGE_GPU_ONLY;
			}else{
				vmausage = VMA_MEMORY_USAGE_CPU_TO_GPU;
			}
			break;
		case BUFFER_USAGE_READ_ONLY:
			if(dataUsage==BUFFER_DATA_USAGE_STATIC){
				vkusageflags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				vmausage = VMA_MEMORY_USAGE_GPU_ONLY;
			}else{
				vmausage = VMA_MEMORY_USAGE_CPU_TO_GPU;
			}
			break;
		default:
			break;
	}

	Vermilion::Core::Vulkan::createBuffer(api, this->size, vkusageflags, vmausage, vk_buffer, vk_allocation);
}

Vermilion::Core::Vulkan::Buffer::~Buffer(){
	vmaDestroyBuffer(api->vma_allocator, vk_buffer, vk_allocation);
}

void Vermilion::Core::Vulkan::Buffer::setData(void * data, size_t size){
	if(this->usage==BUFFER_USAGE_READ_WRITE || this->usage==BUFFER_USAGE_WRITE_ONLY){
		if(size==0 || size>this->size){
			size = this->size;
		}
		if(this->dataUsage==BUFFER_DATA_USAGE_STATIC){
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
}

void Vermilion::Core::Vulkan::Buffer::getData(void * data, size_t size){
	if(this->usage==BUFFER_USAGE_READ_ONLY || this->usage==BUFFER_USAGE_READ_WRITE){
		if(size>this->size){
			size = this->size;
		}
		if(this->dataUsage==BUFFER_DATA_USAGE_STATIC){
			VkBuffer stagingBuffer;
			VmaAllocation stagingBufferMemory;
			Vermilion::Core::Vulkan::createBuffer(api, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer, stagingBufferMemory);

			api->beginSingleTimeCommands();
				VkBufferCopy copyRegion = {};
				copyRegion.srcOffset = 0; // Optional
				copyRegion.dstOffset = 0; // Optional
				copyRegion.size = size;
				vkCmdCopyBuffer(api->vk_commandBuffer, vk_buffer, stagingBuffer, 1, &copyRegion);
			api->endSingleTimeCommands();

			// Fill buffer
			void * gpudata;
			vmaMapMemory(api->vma_allocator, stagingBufferMemory, &gpudata);
			memcpy(data, gpudata, size);
			vmaUnmapMemory(api->vma_allocator, stagingBufferMemory);

			vmaDestroyBuffer(api->vma_allocator, stagingBuffer, stagingBufferMemory);
		}else{
			// Fill buffer
			void * gpudata;
			vmaMapMemory(api->vma_allocator, this->vk_allocation, &gpudata);
			memcpy(data, gpudata, size);
			vmaUnmapMemory(api->vma_allocator, this->vk_allocation);
		}
	}
}

#endif
