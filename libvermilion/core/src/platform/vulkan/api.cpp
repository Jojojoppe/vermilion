#ifdef VMCORE_VULKAN

#include "api.hpp"
#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>

#include <string.h>

Vermilion::Core::Instance * Vermilion::Core::Vulkan::API::static_instance;

Vermilion::Core::Vulkan::API::API(Vermilion::Core::Instance * instance){
	this->instance = instance;
	static_instance = instance;

	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Creating Vulkan context");
}

Vermilion::Core::Vulkan::API::~API(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Destroying Vulkan context");

	vk_swapchain.reset();
	vk_device.reset();
	vk_physicaldevice.reset();
	DestroyDebugUtilsMessengerEXT(vk_instance->vk_instance, vk_debugMessenger, nullptr);
	vkDestroySurfaceKHR(vk_instance->vk_instance, vk_surface, nullptr);
	vk_instance.reset();
}

void Vermilion::Core::Vulkan::API::init(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Initializing Vulkan context");

	this->vk_instance.reset(new Vermilion::Core::Vulkan::vkInstance(this));

	// Get surface from window
	this->vk_surface = (VkSurfaceKHR) this->instance->window->getSurface(&vk_instance->vk_instance);

	// Setup debug messenger
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
	if(CreateDebugUtilsMessengerEXT(vk_instance->vk_instance, &createInfo, nullptr, &vk_debugMessenger)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_ERROR, "Failed to setup Vulkan debug messenger");
	}

	this->vk_physicaldevice.reset(new Vermilion::Core::Vulkan::vkPhysicalDevice(this));
	this->vk_device.reset(new Vermilion::Core::Vulkan::vkDevice(this));
	this->vk_swapchain.reset(new Vermilion::Core::Vulkan::vkSwapChain(this));
}

void Vermilion::Core::Vulkan::API::startRender(){
}

void Vermilion::Core::Vulkan::API::endRender(){
}

// DEBUG STUFF
// -----------
VKAPI_ATTR VkBool32 VKAPI_CALL Vermilion::Core::Vulkan::API::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData){

	static_instance->logger.log(VMCORE_LOGLEVEL_TRACE, "Vulkan validation layer: %s", pCallbackData->pMessage);
	return VK_FALSE;
}
VkResult Vermilion::Core::Vulkan::API::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
void Vermilion::Core::Vulkan::API::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

// -----------

#endif
