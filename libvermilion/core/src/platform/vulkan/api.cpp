#ifdef VMCORE_VULKAN

#include "api.hpp"
#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "Shader.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"
#include "Renderable.hpp"
#include "Texture.hpp"

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

	for(int i=0; i<uniformBuffers.size(); i++){
		uniformBuffers[i].reset();
	}

	for(int i=0; i<pipelines.size(); i++){
		pipelines[i].reset();
	}

	vkFreeCommandBuffers(this->vk_device->vk_device, this->vk_commandPool->vk_commandPool, 1, &vk_commandBuffer);

	for(int i=0; i<maxFramesInFlight; i++){
		vkDestroySemaphore(vk_device->vk_device, imageAvailableSemaphore[i], nullptr);
		vkDestroySemaphore(vk_device->vk_device, renderFinishedSemaphore[i], nullptr);
		vkDestroyFence(vk_device->vk_device, inFlightFences[i], nullptr);
	}
	vk_commandPool.reset();
	default_renderTarget.reset();
	vk_swapchain.reset();
	vmaDestroyAllocator(vma_allocator);
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

	// Create memory allocator
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
	allocatorInfo.physicalDevice = vk_physicaldevice->vk_physicaldevice;
	allocatorInfo.device = vk_device->vk_device;
	allocatorInfo.instance = vk_instance->vk_instance;
	vmaCreateAllocator(&allocatorInfo, &vma_allocator);

	this->vk_swapchain.reset(new Vermilion::Core::Vulkan::vkSwapChain(this));
	this->vk_commandPool.reset(new Vermilion::Core::Vulkan::vkCommandPool(this));

	// Create render target
	this->default_renderTarget.reset(new Vermilion::Core::Vulkan::DefaultRenderTarget(this));

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

	// Allocate one time use command buffer
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = this->vk_commandPool->vk_commandPool;
	allocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(this->vk_device->vk_device, &allocInfo, &vk_commandBuffer);

}

void Vermilion::Core::Vulkan::API::resize(){
	// Recreate swapchain etc for resize
	vkDeviceWaitIdle(vk_device->vk_device);

	// Cleanup swapchain
	for(const auto& pipeline : pipelines){
		pipeline->destroy();
	}
	this->vk_commandPool.reset();
	default_renderTarget->reset();
	vk_swapchain.reset();

	// Rebuild all
	this->vk_swapchain.reset(new Vermilion::Core::Vulkan::vkSwapChain(this));
	this->vk_commandPool.reset(new Vermilion::Core::Vulkan::vkCommandPool(this));
	this->default_renderTarget->create();
	for(const auto& pipeline : pipelines){
		pipeline->create();
	}
	vkDeviceWaitIdle(vk_device->vk_device);
}

void Vermilion::Core::Vulkan::API::startRender(){
	vkWaitForFences(vk_device->vk_device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    VkResult res = vkAcquireNextImageKHR(vk_device->vk_device, vk_swapchain->swapChain, UINT64_MAX, imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if(res==VK_ERROR_OUT_OF_DATE_KHR){
		this->resize();
		return;
	}else if(res!=VK_SUCCESS && res !=VK_SUBOPTIMAL_KHR){
		throw std::runtime_error("Vermilion::Core::Vulkan::API::endRender() - Failed to acquire swap chain image");
	}

	if(imagesInFlight[imageIndex] != VK_NULL_HANDLE){
		vkWaitForFences(vk_device->vk_device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];
}

void Vermilion::Core::Vulkan::API::endRender(std::initializer_list<std::shared_ptr<Vermilion::Core::RenderTarget>> extraRenderTargets){
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	std::vector<VkCommandBuffer> commandBuffers;
	for(const auto& t : extraRenderTargets){
		commandBuffers.push_back(std::static_pointer_cast<Vermilion::Core::Vulkan::RenderTarget>(t)->vk_commandBuffer);
	}
	commandBuffers.push_back(default_renderTarget->vk_commandBuffers[imageIndex]);

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphore[currentFrame]};
	VkSemaphore signalSemaphores[] = {renderFinishedSemaphore[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = commandBuffers.size();
	submitInfo.pCommandBuffers = commandBuffers.data();
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
	VkResult res = vkQueuePresentKHR(vk_device->vk_presentQueue, &presentInfo);
	if(res==VK_ERROR_OUT_OF_DATE_KHR || res==VK_SUBOPTIMAL_KHR){
		this->resize();
		return;
	}else if(res!=VK_SUCCESS){
		throw std::runtime_error("Vermilion::Core::Vulkan::API::endRender() - Failed to present swap chain image");
	}

	vkDeviceWaitIdle(vk_device->vk_device);

	currentFrame = (currentFrame+1)%maxFramesInFlight;
}

std::shared_ptr<Vermilion::Core::RenderTarget> Vermilion::Core::Vulkan::API::getDefaultRenderTarget(){
	return std::static_pointer_cast<Vermilion::Core::RenderTarget>(default_renderTarget);
};

std::shared_ptr<Vermilion::Core::RenderTarget> Vermilion::Core::Vulkan::API::createRenderTarget(std::shared_ptr<Vermilion::Core::Texture> texture){
	return std::static_pointer_cast<Vermilion::Core::RenderTarget>(std::make_shared<Vermilion::Core::Vulkan::RenderTarget>(this, texture));
}

std::shared_ptr<Vermilion::Core::Shader> Vermilion::Core::Vulkan::API::createShader(const std::string& source, Vermilion::Core::ShaderType type){
	return std::static_pointer_cast<Vermilion::Core::Shader>(std::make_shared<Vermilion::Core::Vulkan::Shader>(this, source, type));
}

std::shared_ptr<Vermilion::Core::ShaderProgram> Vermilion::Core::Vulkan::API::createShaderProgram(std::initializer_list<std::shared_ptr<Vermilion::Core::Shader>> shaders){
	return std::static_pointer_cast<Vermilion::Core::ShaderProgram>(std::make_shared<Vermilion::Core::Vulkan::ShaderProgram>(this, shaders));
}

std::shared_ptr<Vermilion::Core::Pipeline> Vermilion::Core::Vulkan::API::createPipeline(std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, 
		std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout,
		std::initializer_list<Vermilion::Core::PipelineLayoutBinding> layoutBindings){
	std::shared_ptr<Vermilion::Core::Vulkan::Pipeline> newpipeline = std::make_shared<Vermilion::Core::Vulkan::Pipeline>(this, renderTarget, shaderProgram, vertexLayout, layoutBindings);
	pipelines.push_back(newpipeline);
	return std::static_pointer_cast<Vermilion::Core::Pipeline>(newpipeline);
}

std::shared_ptr<Vermilion::Core::Binding> Vermilion::Core::Vulkan::API::createBinding(std::initializer_list<std::shared_ptr<Vermilion::Core::UniformBuffer>> uniformBuffers, std::initializer_list<std::shared_ptr<Vermilion::Core::Sampler>> samplers){
	return std::static_pointer_cast<Vermilion::Core::Binding>(std::make_shared<Vermilion::Core::Vulkan::Binding>(this, uniformBuffers, samplers));
}

std::shared_ptr<Vermilion::Core::VertexBuffer> Vermilion::Core::Vulkan::API::createVertexBuffer(std::vector<float>& vertices){
	return std::static_pointer_cast<Vermilion::Core::VertexBuffer>(std::make_shared<Vermilion::Core::Vulkan::VertexBuffer>(this, vertices));
}

std::shared_ptr<Vermilion::Core::IndexBuffer> Vermilion::Core::Vulkan::API::createIndexBuffer(std::vector<unsigned int>& indices){
	return std::static_pointer_cast<Vermilion::Core::IndexBuffer>(std::make_shared<Vermilion::Core::Vulkan::IndexBuffer>(this, indices));
}

std::shared_ptr<Vermilion::Core::UniformBuffer> Vermilion::Core::Vulkan::API::createUniformBuffer(size_t length){
	auto n = std::make_shared<Vermilion::Core::Vulkan::UniformBuffer>(this, length);
	uniformBuffers.push_back(n);
	return std::static_pointer_cast<Vermilion::Core::UniformBuffer>(n);
}

std::shared_ptr<Vermilion::Core::Renderable> Vermilion::Core::Vulkan::API::createRenderable(std::shared_ptr<Vermilion::Core::VertexBuffer> vertexBuffer, std::shared_ptr<Vermilion::Core::IndexBuffer> indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length){
	return std::static_pointer_cast<Vermilion::Core::Renderable>(std::make_shared<Vermilion::Core::Vulkan::Renderable>(this, vertexBuffer, indexBuffer, vertexOffset, indexOffset, length));
}

void Vermilion::Core::Vulkan::API::streamData(std::shared_ptr<Vermilion::Core::UniformBuffer> uniformBuffer, void * data){
	std::static_pointer_cast<Vermilion::Core::Vulkan::UniformBuffer>(uniformBuffer)->streamData(data);
};

std::shared_ptr<Vermilion::Core::Texture> Vermilion::Core::Vulkan::API::createTexture(const std::string& path, size_t width, size_t height, unsigned int channels){
	return std::static_pointer_cast<Vermilion::Core::Texture>(std::make_shared<Vermilion::Core::Vulkan::Texture>(this, path, width, height, channels));
}

std::shared_ptr<Vermilion::Core::Sampler> Vermilion::Core::Vulkan::API::createSampler(std::shared_ptr<Vermilion::Core::Texture> texture){
	return std::static_pointer_cast<Vermilion::Core::Sampler>(std::make_shared<Vermilion::Core::Vulkan::Sampler>(this, texture));
}





void Vermilion::Core::Vulkan::API::beginSingleTimeCommands(){
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(vk_commandBuffer, &beginInfo);
}

void Vermilion::Core::Vulkan::API::endSingleTimeCommands(){
	vkEndCommandBuffer(vk_commandBuffer);
	
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vk_commandBuffer;
	if(vkQueueSubmit(this->vk_device->vk_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not submit single time command buffer");
		throw std::runtime_error("Vermilion::Core::Vulkan::API::endSingleTimeCommands() - Could not submit single time command buffer");
	}
	vkQueueWaitIdle(this->vk_device->vk_graphicsQueue);
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
