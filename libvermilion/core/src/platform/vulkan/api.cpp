#ifdef VMCORE_VULKAN

#include "api.hpp"
#include <vermilion/instance.hpp>

Vermilion::Core::Vulkan::API::API(Vermilion::Core::Instance * instance){
	this->instance = instance;

	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Creating Vulkan context");
}

Vermilion::Core::Vulkan::API::~API(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Destroying Vulkan context");
}

void Vermilion::Core::Vulkan::API::init(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Initializing Vulkan context");
}

#endif
