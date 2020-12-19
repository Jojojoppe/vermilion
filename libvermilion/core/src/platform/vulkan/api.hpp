#ifdef VMCORE_VULKAN
#pragma once

#include <vermilion/instance.hpp>

#include <vulkan/vulkan.h>
#include <stdint.h>

namespace Vermilion{
namespace Core{
namespace Vulkan{

class API : public Vermilion::Core::API{
	public:

	private:
		Vermilion::Core::Instance * instance;

		// Vulkan variables
		VkInstance vk_instance;
		VkPhysicalDevice vk_physicalDevice = VK_NULL_HANDLE;

	public:
		API(Vermilion::Core::Instance * instance);
		virtual ~API() override;

		virtual void init() override;
		virtual void startRender() override;
		virtual void endRender() override;

	private:

		void createInstance();
		void pickPhysicalDevice();

};

}}}

#endif
