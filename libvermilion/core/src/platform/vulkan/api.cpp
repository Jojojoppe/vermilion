#ifdef VMCORE_VULKAN

#include "api.hpp"
#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "Shader.hpp"
#include "Pipeline.hpp"

#include <string.h>
#include <stdexcept>

Vermilion::Core::Instance * Vermilion::Core::Vulkan::API::static_instance;

Vermilion::Core::Vulkan::API::API(Vermilion::Core::Instance * instance){
	this->instance = instance;
	static_instance = instance;

	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Creating Vulkan context");
}

Vermilion::Core::Vulkan::API::~API(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Destroying Vulkan context");

	vkDeviceWaitIdle(vk_device->vk_device);

	for(int i=0; i<maxFramesInFlight; i++){
		vkDestroySemaphore(vk_device->vk_device, imageAvailableSemaphore[i], nullptr);
		vkDestroySemaphore(vk_device->vk_device, renderFinishedSemaphore[i], nullptr);
		vkDestroyFence(vk_device->vk_device, inFlightFences[i], nullptr);
	}
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
	this->vk_commandPool.reset(new Vermilion::Core::Vulkan::vkCommandPool(this));

	// Create render target
	this->default_renderTarget.reset(new Vermilion::Core::Vulkan::RenderTarget(this));

	maxFramesInFlight = vk_swapchain->swapChainImages.size()-1;
	imageAvailableSemaphore.resize(maxFramesInFlight);
	renderFinishedSemaphore.resize(maxFramesInFlight);
	inFlightFences.resize(maxFramesInFlight);
	imagesInFlight.resize(vk_swapchain->swapChainImages.size(), VK_NULL_HANDLE);

	// Create render semaphores and fences
	VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for(int i=0; i<maxFramesInFlight; i++){
		if(vkCreateSemaphore(vk_device->vk_device, &semaphoreInfo, nullptr, &imageAvailableSemaphore[i])!=VK_SUCCESS){
			this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create semaphore");
			throw std::runtime_error("Vermilion::Core::Vulkan::API::API() - Could not create semaphore");
		}
		if(vkCreateSemaphore(vk_device->vk_device, &semaphoreInfo, nullptr, &renderFinishedSemaphore[i])!=VK_SUCCESS){
			this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create semaphore");
			throw std::runtime_error("Vermilion::Core::Vulkan::API::API() - Could not create semaphore");
		}
		if(vkCreateFence(vk_device->vk_device, &fenceInfo, nullptr, &inFlightFences[i])!=VK_SUCCESS){
			this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create fence");
			throw std::runtime_error("Vermilion::Core::Vulkan::API::API() - Could not create fence");
		}
	}
}

void Vermilion::Core::Vulkan::API::startRender(){

}

void Vermilion::Core::Vulkan::API::endRender(){
	vkWaitForFences(vk_device->vk_device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
    vkAcquireNextImageKHR(vk_device->vk_device, vk_swapchain->swapChain, UINT64_MAX, imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if(imagesInFlight[imageIndex] != VK_NULL_HANDLE){
		vkWaitForFences(vk_device->vk_device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = {imageAvailableSemaphore[currentFrame]};
	VkSemaphore signalSemaphores[] = {renderFinishedSemaphore[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &default_renderTarget->vk_commandBuffers[imageIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(vk_device->vk_device, 1, &inFlightFences[currentFrame]);
	if(vkQueueSubmit(vk_device->vk_graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not submit to graphics queue");
		throw std::runtime_error("Vermilion::Core::Vulkan::API::endRender() - Could not submit to graphics queue");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = {vk_swapchain->swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	vkQueuePresentKHR(vk_device->vk_presentQueue, &presentInfo);

	vkDeviceWaitIdle(vk_device->vk_device);

	currentFrame = (currentFrame+1)%maxFramesInFlight;
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
