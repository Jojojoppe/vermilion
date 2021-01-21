#ifdef VMCORE_VULKAN
#include "vkCommandPool.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include "../../window.hpp"
#include "vkPhysicalDevice.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

Vermilion::Core::Vulkan::vkCommandPool::vkCommandPool(API * api){
	this->api = api;
	this->instance = api->instance;

	QueueFamilyIndices queueFamilyIndices = api->vk_physicaldevice->findQueueFamilies(api->vk_physicaldevice->vk_physicaldevice);
	
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	if(vkCreateCommandPool(api->vk_device->vk_device, &poolInfo, nullptr, &this->vk_commandPool) != VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to create command pool");
		throw std::runtime_error("Vermilion::Core::Vulkan::vkCommandPool::vkCommandPool() - Failed to create command pool");
	}
}

Vermilion::Core::Vulkan::vkCommandPool::~vkCommandPool(){	
	vkDestroyCommandPool(api->vk_device->vk_device, vk_commandPool, nullptr);
}

#endif
