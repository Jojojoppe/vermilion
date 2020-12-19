#ifdef VMCORE_VULKAN

#include "context.hpp"
#include <vermilion/core/context.hpp>
#include <vermilion/core/logger.hpp>
#include <vermilion/core/instance.hpp>

#include <stdint.h>

Vermilion::Core::VULKAN_Context::VULKAN_Context(Vermilion::Core::Instance * instance){
	this->vmCoreInstance = instance;
	this->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "Creating Vulkan context");

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	this->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_DEBUG, "%d extensions supported", extensionCount);

}

Vermilion::Core::VULKAN_Context::~VULKAN_Context(){
}

void Vermilion::Core::VULKAN_Context::init(Vermilion::Core::ContextProperties * properties){
	this->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "Initializing Vulkan context");
}

#endif
