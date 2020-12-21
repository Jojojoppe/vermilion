#ifdef VMCORE_VULKAN
#include "vkPhysicalDevice.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"

#include <string.h>
#include <stdexcept>

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

		// TODO add checks

		this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "  [v] %s", deviceProperties.deviceName);
		vk_physicaldevice = dev;
	}
	if(vk_physicaldevice==VK_NULL_HANDLE){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Failed to find suitable GPU");
		throw std::runtime_error("Vermilion::Core::Vulkan::vkPhysicalDevice::vkPhysicalDevice() - Failed to find suitable GPU");
	}
}

Vermilion::Core::Vulkan::vkPhysicalDevice::~vkPhysicalDevice(){	
}

#endif
