#ifdef VMCORE_VULKAN
#pragma once
#include <vulkan/vulkan.h>
#include <stdint.h>

struct VmaAllocator{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
};

struct VmaAllocatorCreateInfo{
    uint32_t vulkanApiVersion;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
};

VkResult vmaCreateAllocator(VmaAllocatorCreateInfo * allocatorCreateInfo, VmaAllocator * allocator);
VkResult vmaDestroyAllocator(VmaAllocator& allocator);

typedef VkFlags VmaMemoryUsage;

#define VMA_MEMORY_USAGE_UNDEFINED 0
#define VMA_MEMORY_USAGE_CPU_ONLY 1
#define VMA_MEMORY_USAGE_GPU_ONLY 2
#define VMA_MEMORY_USAGE_CPU_TO_GPU 3
#define VMA_MEMORY_USAGE_GPU_TO_CPU 4

static VkFlags _VMA_MEMORY_USAGE_required[]{
    0,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    0,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
};
static VkFlags _VMA_MEMORY_USAGE_preferred[]{
    0,
    0,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    0,
    VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
};

struct VmaAllocation{
    VkDeviceMemory memory;
    size_t size;
};

struct VmaAllocationCreateInfo{
    VmaMemoryUsage usage;
};

VkResult vmaCreateBuffer(VmaAllocator& allocator, VkBufferCreateInfo * bufferCreateInfo, VmaAllocationCreateInfo * allocationCreateInfo, VkBuffer * buffer, VmaAllocation * allocation, void * unused);

VkResult vmaDestroyBuffer(VmaAllocator& allocator, VkBuffer & buffer, VmaAllocation & allocation);

VkResult vmaMapMemory(VmaAllocator& allocator, VmaAllocation & allocation, void ** data);

VkResult vmaUnmapMemory(VmaAllocator& allocator, VmaAllocation & allocation);

VkResult vmaCreateImage(VmaAllocator& allocator, VkImageCreateInfo * imageCreateInfo, VmaAllocationCreateInfo * allocationCreateInfo, VkImage * image, VmaAllocation * allocation, void * unused);

VkResult vmaDestroyImage(VmaAllocator& allocator, VkImage & image, VmaAllocation & allocation);

#endif