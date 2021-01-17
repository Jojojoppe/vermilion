#ifdef VMCORE_VULKAN
#include "vk_mem_alloc.h"
#include <stdexcept>

uint32_t findMemoryType(VmaAllocator& allocator, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(allocator.physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

VkResult vmaCreateAllocator(VmaAllocatorCreateInfo * allocatorCreateInfo, VmaAllocator * allocator){
    allocator->device = allocatorCreateInfo->device;
    allocator->physicalDevice = allocatorCreateInfo->physicalDevice;
    allocator->instance = allocatorCreateInfo->instance;

    return VK_SUCCESS;
}

VkResult vmaDestroyAllocator(VmaAllocator& allocator){
    return VK_SUCCESS;
}

VkResult vmaCreateBuffer(VmaAllocator& allocator, VkBufferCreateInfo * bufferCreateInfo, VmaAllocationCreateInfo * allocationCreateInfo, VkBuffer * buffer, VmaAllocation * allocation, void * unused){
    VkResult res;

    res = vkCreateBuffer(allocator.device, bufferCreateInfo, nullptr, buffer);
    if(res!=VK_SUCCESS){
        return res;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(allocator.device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(allocator, memRequirements.memoryTypeBits, _VMA_MEMORY_USAGE_required[allocationCreateInfo->usage]);
    res = vkAllocateMemory(allocator.device, &allocInfo, nullptr, &allocation->memory);
    if(res!=VK_SUCCESS){
        vkDestroyBuffer(allocator.device, *buffer, nullptr);
        return res;
    }

    vkBindBufferMemory(allocator.device, *buffer, allocation->memory, 0);
    allocation->size = bufferCreateInfo->size;
    return VK_SUCCESS;
}

VkResult vmaDestroyBuffer(VmaAllocator& allocator, VkBuffer & buffer, VmaAllocation & allocation){
    
    vkFreeMemory(allocator.device, allocation.memory, nullptr);
    vkDestroyBuffer(allocator.device, buffer, nullptr);

    buffer = VK_NULL_HANDLE;
    allocation = {};
    return VK_SUCCESS;
}

VkResult vmaMapMemory(VmaAllocator& allocator, VmaAllocation & allocation, void ** data){
    vkMapMemory(allocator.device, allocation.memory, 0, allocation.size, 0, data);
    return VK_SUCCESS;
}

VkResult vmaUnmapMemory(VmaAllocator& allocator, VmaAllocation & allocation){
    vkUnmapMemory(allocator.device, allocation.memory);
    return VK_SUCCESS;
}

VkResult vmaCreateImage(VmaAllocator& allocator, VkImageCreateInfo * imageCreateInfo, VmaAllocationCreateInfo * allocationCreateInfo, VkImage * image, VmaAllocation * allocation, void * unused){
    VkResult res;

    res = vkCreateImage(allocator.device, imageCreateInfo, nullptr, image);
    if(res!=VK_SUCCESS){
        return res;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(allocator.device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(allocator, memRequirements.memoryTypeBits, _VMA_MEMORY_USAGE_required[allocationCreateInfo->usage]);
    res = vkAllocateMemory(allocator.device, &allocInfo, nullptr, &allocation->memory);
    if(res!=VK_SUCCESS){
        vkDestroyImage(allocator.device, *image, nullptr);
        return res;
    }

    vkBindImageMemory(allocator.device, *image, allocation->memory, 0);
    allocation->size = 0;
    return VK_SUCCESS;
}

VkResult vmaDestroyImage(VmaAllocator& allocator, VkImage & image, VmaAllocation & allocation){
    vkDestroyImage(allocator.device, image, nullptr);
    vkFreeMemory(allocator.device, allocation.memory, nullptr);

    image = VK_NULL_HANDLE;
    allocation = {};
    return VK_SUCCESS;
}

#endif