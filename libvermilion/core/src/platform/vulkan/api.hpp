#ifdef VMCORE_VULKAN
#pragma once

#include <vermilion/instance.hpp>
#include "../../api.hpp"

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
extern std::vector<const char*> instanceExtensions;

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
		VkCommandBuffer vk_commandBuffer;

		std::shared_ptr<DefaultRenderTarget> default_renderTarget;

		std::vector<VkSemaphore> imageAvailableSemaphore;
		std::vector<VkSemaphore> renderFinishedSemaphore;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;

		int maxFramesInFlight;
		int currentFrame = 0;
		uint32_t imageIndex = 0;

		std::vector<std::shared_ptr<Pipeline>> pipelines;
		std::vector<std::shared_ptr<RenderTarget>> renderTargets;

	private:

	public:
		API(Vermilion::Core::Instance * instance);
		virtual ~API() override;

		virtual void init() override;
		virtual void startRender() override;
		virtual void endRender(std::vector<std::shared_ptr<Vermilion::Core::RenderTarget>>& extraRenderTargets) override;
		virtual void resize() override;

		virtual std::shared_ptr<Vermilion::Core::RenderTarget> getDefaultRenderTarget() override;
		virtual std::shared_ptr<Vermilion::Core::RenderTarget> createRenderTarget(std::shared_ptr<Vermilion::Core::Texture> texture) override;

		virtual std::shared_ptr<Vermilion::Core::Shader> createShader(const std::string& source, Vermilion::Core::ShaderType type) override;
		virtual std::shared_ptr<Vermilion::Core::ShaderProgram> createShaderProgram(std::vector<std::shared_ptr<Vermilion::Core::Shader>>& shaders) override;

		virtual std::shared_ptr<Vermilion::Core::PipelineLayout> createPipelineLayout(std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout, std::initializer_list<Vermilion::Core::PipelineLayoutBinding> bindings, std::initializer_list<Vermilion::Core::PipelineLayoutUniform> uniforms) override;
		virtual std::shared_ptr<Vermilion::Core::Pipeline> createPipeline(std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, 
			std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, std::shared_ptr<Vermilion::Core::PipelineLayout> pipelineLayout, Vermilion::Core::PipelineSettings settings) override;
		virtual std::shared_ptr<Vermilion::Core::Binding> createBinding(std::vector<std::shared_ptr<Vermilion::Core::Buffer>>& buffers, std::vector<std::shared_ptr<Vermilion::Core::Sampler>>& samplers)override;

		virtual std::shared_ptr<Vermilion::Core::Buffer> createBuffer(size_t size, Vermilion::Core::BufferType type, Vermilion::Core::BufferUsage usage, Vermilion::Core::BufferDataUsage dataUsage) override;

		virtual std::shared_ptr<Vermilion::Core::Renderable> createRenderable(std::shared_ptr<Vermilion::Core::Buffer> vertexBuffer, 
			std::shared_ptr<Vermilion::Core::Buffer> indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length) override;

		virtual std::shared_ptr<Vermilion::Core::Texture> createTexture(size_t width, size_t height, unsigned int channels) override;
		virtual std::shared_ptr<Vermilion::Core::Sampler> createSampler(std::shared_ptr<Vermilion::Core::Texture> texture) override;

		void beginSingleTimeCommands();
		void endSingleTimeCommands();

	private:


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
