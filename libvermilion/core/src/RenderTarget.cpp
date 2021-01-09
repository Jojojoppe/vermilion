#include <vermilion/RenderTarget.hpp>
#include <vermilion/instance.hpp>

//#include "platform/opengl/RenderTarget.hpp"
#include "platform/vulkan/RenderTarget.hpp"

#include <stdexcept>

Vermilion::Core::RenderTarget * Vermilion::Core::RenderTarget::create(Vermilion::Core::Instance * instance, int width, int height){
	switch(instance->platform_render){
#ifdef VMCORE_OPENGL
//		case Vermilion::Core::RenderPlatform::RENDER_PLATFORM_OPENGL:
//			return new Vermilion::Core::OpenGL::RenderTarget(instance, width, height);
//			break;
#endif
#ifdef VMCORE_VULKAN
		case Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN:
			return new Vermilion::Core::Vulkan::RenderTarget(instance, width, height);
			break;
#endif
		default:
			instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Render platform not supported");
			throw std::runtime_error("Vermilion::Core::RenderTarget::create() - Render platform not supported");
			break;
	}
	return nullptr;
}
