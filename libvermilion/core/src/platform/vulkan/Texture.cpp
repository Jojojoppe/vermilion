#ifdef VMCORE_VULKAN
#include "Texture.hpp"
#include "api.hpp"

#include <stb_image.h>
#include <stdexcept>
#include <cstdint>
#include <cstring>

#include "Buffer.hpp"

static const VkFormat VulkanChannelFormatField[] = {
    VK_FORMAT_UNDEFINED,
    VK_FORMAT_R8_SRGB,
    VK_FORMAT_R8G8_SRGB,
    VK_FORMAT_R8G8B8_SRGB,
    VK_FORMAT_R8G8B8A8_SRGB,
};

Vermilion::Core::Vulkan::Texture::Texture(Vermilion::Core::Vulkan::API * api, const std::string& path, size_t width, size_t height, unsigned int channels){
    this->api = api;
    this->instance = api->instance;

    stbi_uc * pixels = nullptr;
    if(path.compare("")==0){
        // Create empty texture
        this->width = width;
        this->height = height;
        this->channels = channels;

        // TODO not supported yet
    }else{
        // Create texture with pixel data
        pixels = stbi_load(path.c_str(), (int*)&this->width, (int*)&this->height, (int*)&this->channels, STBI_rgb_alpha);
        this->channels = 4;

        // Create staging buffer
        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferMemory;
        size_t size = this->width*this->height*this->channels;
        Vermilion::Core::Vulkan::createBuffer(api, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer, stagingBufferMemory);

        // Fill buffer
        void * gpudata;
        vmaMapMemory(this->api->vma_allocator, stagingBufferMemory, &gpudata);
        memcpy(gpudata, pixels, this->width*this->height*this->channels);
        vmaUnmapMemory(this->api->vma_allocator, stagingBufferMemory);

        this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Create texture of size %d", this->width*this->height*this->channels);

        // Create vulkan image
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(this->width);
        imageInfo.extent.height = static_cast<uint32_t>(this->height);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VulkanChannelFormatField[this->channels];
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;         // TODO use LINEAIR for direct access
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; // TODO other usage for render destination?
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.flags = VMA_MEMORY_USAGE_GPU_ONLY;
        if(vmaCreateImage(api->vma_allocator, &imageInfo, &allocInfo, &this->vk_image, &this->vma_allocation, nullptr)!=VK_SUCCESS){
            this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create image");
            throw std::runtime_error("Vermilion::Core::Vulkan::Texture::Texture() - Could not create image");
        }

        stbi_image_free(pixels);

        {
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
            
            // Transition image to right layout
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = this->vk_image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.srcAccessMask = 0; // TODO
            barrier.dstAccessMask = 0; // TODO
            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;
            if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && barrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            } else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && barrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            } else {
                throw std::invalid_argument("unsupported layout transition!");
            }
            vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );
            
            vkEndCommandBuffer(commandBuffer);
            
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;
            
            vkQueueSubmit(api->vk_device->vk_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(api->vk_device->vk_graphicsQueue);
            
            vkFreeCommandBuffers(api->vk_device->vk_device, api->vk_commandPool->vk_commandPool, 1, &commandBuffer);
        }

        {
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

            // Copy data
            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = {0, 0, 0};
            region.imageExtent = {
                static_cast<uint32_t>(this->width),
                static_cast<uint32_t>(this->height),
                1
            };
            vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer,
                this->vk_image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region
            );

            vkEndCommandBuffer(commandBuffer);
            
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;
            
            vkQueueSubmit(api->vk_device->vk_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(api->vk_device->vk_graphicsQueue);
            
            vkFreeCommandBuffers(api->vk_device->vk_device, api->vk_commandPool->vk_commandPool, 1, &commandBuffer);
        }

        {
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
            
            // Transition image to right layout
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = this->vk_image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.srcAccessMask = 0; // TODO
            barrier.dstAccessMask = 0; // TODO
            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;
            if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && barrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            } else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && barrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            } else {
                throw std::invalid_argument("unsupported layout transition!");
            }
            vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );
            
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

        // Create image view
        vk_imageView.reset(new Vermilion::Core::Vulkan::vkImageView2D(this->api, this->vk_image, VulkanChannelFormatField[this->channels]));
    }
}

Vermilion::Core::Vulkan::Texture::~Texture(){
    vk_imageView.reset();
    vmaDestroyImage(this->api->vma_allocator, this->vk_image, this->vma_allocation);
}

Vermilion::Core::Vulkan::Sampler::Sampler(Vermilion::Core::Vulkan::API * api, std::shared_ptr<Vermilion::Core::Texture> texture){
    this->api = api;
    this->instance = api->instance;
    this->texture = std::static_pointer_cast<Vermilion::Core::Vulkan::Texture>(texture);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(api->vk_physicaldevice->vk_physicaldevice, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    if(vkCreateSampler(api->vk_device->vk_device, &samplerInfo, nullptr, &this->vk_sampler)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create sampler");
		throw std::runtime_error("Vermilion::Core::Vulkan::Sampler::Sampler() - Could not create sampler");
    }
}

Vermilion::Core::Vulkan::Sampler::~Sampler(){
    vkDestroySampler(this->api->vk_device->vk_device, this->vk_sampler, nullptr);
}

#endif