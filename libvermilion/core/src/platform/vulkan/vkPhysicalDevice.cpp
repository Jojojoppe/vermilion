#ifdef VMCORE_VULKAN
#include "vkPhysicalDevice.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"

#include <string.h>
#include <stdexcept>
#include <set>

Vermilion::Core::Vulkan::vkPhysicalDevice::vkPhysicalDevice(API * api){
	this->api = api;
	this->instance = api->instance;


	// Get list of devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(api->vk_instance->vk_instance, &deviceCount, nullptr);
	if(deviceCount==0){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "No GPU with Vulkan support found");
		throw std::runtime_error("Vermilion::Core::Vulkan::vkPhysicalDevice::vkPhysicalDevice() - No GPU found with Vulkan support");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(api->vk_instance->vk_instance, &deviceCount, devices.data());

	// Pick a suitable device
	vk_physicaldevice = VK_NULL_HANDLE;
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "GPU's with Vulkan support:");
	for(const auto& dev : devices){
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(dev, &deviceProperties);

		this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "  - %s:", deviceProperties.deviceName);

		// Check for queues
		Vermilion::Core::Vulkan::QueueFamilyIndices indices = findQueueFamilies(dev);
		if(!indices.isComplete()){
			this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "    Not all required queues are supported");
			continue;
		}

		// Check for extensions
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(dev, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(dev, nullptr, &extensionCount, availableExtensions.data());
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "    Required device extensions:");
		for (const auto& extension : availableExtensions) {
			if(requiredExtensions.count(extension.extensionName)){
				this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "      [v] %s", extension.extensionName);
			}
			requiredExtensions.erase(extension.extensionName);
		}
		if(!requiredExtensions.empty()){
			for (const auto& extension : requiredExtensions) {
				this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "      [x] %s", extension.c_str());
			}
			continue;
		}

		vk_physicaldevice = dev;
	}
	if(vk_physicaldevice==VK_NULL_HANDLE){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to find suitable GPU");
		throw std::runtime_error("Vermilion::Core::Vulkan::vkPhysicalDevice::vkPhysicalDevice() - Failed to find suitable GPU");
	}
}

Vermilion::Core::Vulkan::vkPhysicalDevice::~vkPhysicalDevice(){	
}

Vermilion::Core::Vulkan::QueueFamilyIndices Vermilion::Core::Vulkan::vkPhysicalDevice::findQueueFamilies(VkPhysicalDevice device){
	Vermilion::Core::Vulkan::QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for(const auto& queueFamily : queueFamilies){
		if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->api->vk_surface, &presentSupport);
		if(presentSupport){
			indices.presentFamily = i;
		}

		i++;
	}
	return indices;
}

#endif
