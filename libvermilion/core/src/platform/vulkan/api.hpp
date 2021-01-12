#ifdef VMCORE_VULKAN
#pragma once

#include <vermilion/instance.hpp>

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>
#include <vector>

#include "vkInstance.hpp"
#include "vkPhysicalDevice.hpp"
#include "vkDevice.hpp"
#include "vkSwapChain.hpp"
#include "vkCommandPool.hpp"
#include "RenderTarget.hpp"
#include "vk_mem_alloc.h"

namespace Vermilion{
namespace Core{
namespace Vulkan{

extern std::vector<const char*> validationLayers;
extern std::vector<const char*> deviceExtensions;

class Pipeline;

class API : public Vermilion::Core::API{
	public:
		Vermilion::Core::Instance * instance;
		static Vermilion::Core::Instance * static_instance;

		// Vulkan variables
		std::unique_ptr<vkInstance> vk_instance;
		VkSurfaceKHR vk_surface;
		VkDebugUtilsMessengerEXT vk_debugMessenger;
		std::unique_ptr<vkPhysicalDevice> vk_physicaldevice;
		std::unique_ptr<vkDevice> vk_device;
		std::unique_ptr<vkSwapChain> vk_swapchain;
		std::unique_ptr<vkCommandPool> vk_commandPool;
		VmaAllocator vma_allocator;

		std::shared_ptr<RenderTarget> default_renderTarget;

		std::vector<VkSemaphore> imageAvailableSemaphore;
		std::vector<VkSemaphore> renderFinishedSemaphore;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;

		int maxFramesInFlight;
		int currentFrame = 0;

		std::vector<std::shared_ptr<Pipeline>> pipelines;

	private:

	public:
		API(Vermilion::Core::Instance * instance);
		virtual ~API() override;

		virtual void init() override;
		virtual void startRender() override;
		virtual void endRender() override;

		virtual std::shared_ptr<Vermilion::Core::RenderTarget> getDefaultRenderTarget() override;

		virtual std::shared_ptr<Vermilion::Core::Shader> createShader(std::string source, Vermilion::Core::ShaderType type) override;
		virtual std::shared_ptr<Vermilion::Core::ShaderProgram> createShaderProgram(std::initializer_list<std::shared_ptr<Vermilion::Core::Shader>> shaders) override;

		virtual std::shared_ptr<Vermilion::Core::Pipeline> createPipeline(std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, std::initializer_list<Vermilion::Core::VertexBufferLayoutElement> vertexLayout) override;

		virtual std::shared_ptr<Vermilion::Core::VertexBuffer> createVertexBuffer(void * data, size_t length) override;
		virtual std::shared_ptr<Vermilion::Core::IndexBuffer> createIndexBuffer(void * data, size_t length) override;

	private:

		void resize();

		// DEBUG STUFF
		// -----------
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
					VkDebugUtilsMessageTypeFlagsEXT messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                    void* pUserData);
        static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
					const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
					const VkAllocationCallbacks* pAllocator, 
					VkDebugUtilsMessengerEXT* pDebugMessenger);
        static void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
					VkDebugUtilsMessengerEXT debugMessenger, 
					const VkAllocationCallbacks* pAllocator);	
		// -----------
};

}}}

#endif
