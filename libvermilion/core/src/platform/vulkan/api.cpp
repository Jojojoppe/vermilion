#ifdef VMCORE_VULKAN

#include "api.hpp"
#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "Shader.hpp"
#include "Pipeline.hpp"

#include <string.h>

Vermilion::Core::Instance * Vermilion::Core::Vulkan::API::static_instance;

Vermilion::Core::Vulkan::API::API(Vermilion::Core::Instance * instance){
	this->instance = instance;
	static_instance = instance;

	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Creating Vulkan context");
}

Vermilion::Core::Vulkan::API::~API(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Destroying Vulkan context");

	vk_commandPool.reset();
	default_renderTarget.reset();
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

	// Create render target
	this->default_renderTarget.reset(new Vermilion::Core::Vulkan::RenderTarget(this));

	this->vk_commandPool.reset(new Vermilion::Core::Vulkan::vkCommandPool(this));
}

void Vermilion::Core::Vulkan::API::startRender(){
}

void Vermilion::Core::Vulkan::API::endRender(){
}

std::shared_ptr<Vermilion::Core::RenderTarget> Vermilion::Core::Vulkan::API::getDefaultRenderTarget(){
	return std::static_pointer_cast<Vermilion::Core::RenderTarget>(default_renderTarget);
};

std::shared_ptr<Vermilion::Core::Shader> Vermilion::Core::Vulkan::API::createShader(std::string source, Vermilion::Core::ShaderType type){
	return std::static_pointer_cast<Vermilion::Core::Shader>(std::make_shared<Vermilion::Core::Vulkan::Shader>(this, source, type));
}

std::shared_ptr<Vermilion::Core::ShaderProgram> Vermilion::Core::Vulkan::API::createShaderProgram(std::initializer_list<std::shared_ptr<Vermilion::Core::Shader>> shaders){
	return std::static_pointer_cast<Vermilion::Core::ShaderProgram>(std::make_shared<Vermilion::Core::Vulkan::ShaderProgram>(this, shaders));
}

std::shared_ptr<Vermilion::Core::Pipeline> Vermilion::Core::Vulkan::API::createPipeline(std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram){
	return std::static_pointer_cast<Vermilion::Core::Pipeline>(std::make_shared<Vermilion::Core::Vulkan::Pipeline>(this, renderTarget, shaderProgram));
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
