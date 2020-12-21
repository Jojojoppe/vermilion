#ifdef VMCORE_VULKAN
#include "vkPhysicalDevice.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include "vkPhysicalDevice.hpp"

#include <string.h>
#include <stdexcept>
#include <set>

Vermilion::Core::Vulkan::vkDevice::vkDevice(API * api){
	this->api = api;
	this->instance = api->instance;

	Vermilion::Core::Vulkan::QueueFamilyIndices indices = api->vk_physicaldevice->findQueueFamilies(api->vk_physicaldevice->vk_physicaldevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	createInfo.enabledLayerCount = 0;
	// IF DEBUG
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	if(vkCreateDevice(api->vk_physicaldevice->vk_physicaldevice, &createInfo, nullptr, &vk_device)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create logic device");
		throw std::runtime_error("Vermilion::Core::Vulkan::vkDevice::vkDevice() - Could not create logic device");
	}

	vkGetDeviceQueue(vk_device, indices.graphicsFamily.value(), 0, &vk_graphicsQueue);
	vkGetDeviceQueue(vk_device, indices.presentFamily.value(), 0, &vk_presentQueue);

}

Vermilion::Core::Vulkan::vkDevice::~vkDevice(){	
	vkDestroyDevice(vk_device, nullptr);
}

#endif
