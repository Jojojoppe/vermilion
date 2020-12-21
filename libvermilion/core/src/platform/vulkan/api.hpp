#ifdef VMCORE_VULKAN
#pragma once

#include <vermilion/instance.hpp>

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>

#include "vkInstance.hpp"

namespace Vermilion{
namespace Core{
namespace Vulkan{

class API : public Vermilion::Core::API{
	public:
		Vermilion::Core::Instance * instance;
		static Vermilion::Core::Instance * static_instance;

	private:
		// Vulkan variables
		std::unique_ptr<vkInstance> vk_instance;
		VkDebugUtilsMessengerEXT vk_debugMessenger;

	public:
		API(Vermilion::Core::Instance * instance);
		virtual ~API() override;

		virtual void init() override;
		virtual void startRender() override;
		virtual void endRender() override;

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
