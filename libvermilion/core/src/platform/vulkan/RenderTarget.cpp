#ifdef VMCORE_VULKAN
#include "RenderTarget.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include <vermilion/window.hpp>
#include "vkPhysicalDevice.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

Vermilion::Core::Vulkan::RenderTarget::RenderTarget(Vermilion::Core::Instance * instance, int width, int height){
	this->instance = instance;

	// TODO Create frame buffer and render pass (or use systemwide render pass)
}

Vermilion::Core::Vulkan::RenderTarget::~RenderTarget(){	
}

#endif
