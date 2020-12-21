#ifdef VMCORE_VULKAN
#include "vkInstance.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"

#include <string.h>
#include <stdexcept>

// VULKAN SETTINGS
std::vector<const char*> Vermilion::Core::Vulkan::validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

std::vector<const char*> Vermilion::Core::Vulkan::deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_MAINTENANCE1_EXTENSION_NAME,
};

Vermilion::Core::Vulkan::vkInstance::vkInstance(API * api){
	this->api = api;
	this->instance = api->instance;

	// Create vulkan application information struct
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vermilion application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "vermilion";
	appInfo.engineVersion = VK_MAKE_VERSION(VERMILION_VERSION_MAJOR, VERMILION_VERSION_MINOR, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Get window information
	uint32_t windowExtensionCount = 0;
	char ** windowExtensions = (char**) this->instance->window->getRequiredExtensions(&windowExtensionCount);
	std::vector<const char*> requiredExtensions(windowExtensions, windowExtensions+windowExtensionCount);
	// Add debug extension if in DEBUG build
	requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	
	// Get all available extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Vulkan supported and enabled extensions:");
	for(auto& it : requiredExtensions){
		bool found = false;
		for(auto& jt : extensions){
			if(strcmp(jt.extensionName, it)==0){
				found = true;
			}
		}
		if(found)
			this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "  [v] %s", it);
		else
			this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "  [x] %s", it);
	}

	// Create vulkan creation information struct
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.enabledLayerCount = 0;

	// Enable validation layers if in DEBUG build
	if(!checkValidationLayerSupport(validationLayers)){
		this->instance->logger.log(VMCORE_LOGLEVEL_ERROR, "Validation layers requested but not supported");
	}
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	if(vkCreateInstance(&createInfo, nullptr, &vk_instance)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create vk_instance");
		throw std::runtime_error("Vermilion::Core::Vulkan::vkInstance::vkInstance() - Could not create vk_instance");
	}

}

Vermilion::Core::Vulkan::vkInstance::~vkInstance(){	
	vkDestroyInstance(vk_instance, nullptr);
}


bool Vermilion::Core::Vulkan::vkInstance::checkValidationLayerSupport(const std::vector<const char*>& validationLayers){
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	bool allLayersFound = true;
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Check for support of validation layers:");
	for(const char* layerName : validationLayers){
		bool layerFound = false;
		for(const auto& layerProperties : availableLayers){
			if(strcmp(layerName, layerProperties.layerName) == 0){
				layerFound = true;
				this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "  [v] %s", layerName);
				break;
			}
		}
		if(!layerFound){
			allLayersFound = false;
			this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "  [x] %s", layerName);
			return false;
		}
	}
	return true;
}

#endif
