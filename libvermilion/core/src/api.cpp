#include "api.hpp"
#include <vermilion/instance.hpp>

#include "platform/opengl/api.hpp"
#include "platform/vulkan/api.hpp"

#include <stdexcept>

const int Vermilion::Core::renderPlatform[] = {
#ifdef VMCORE_OPENGL
	Vermilion::Core::RenderPlatform::RENDER_PLATFORM_OPENGL,
#endif
#ifdef VMCORE_VULKAN
	Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN,
#endif
	Vermilion::Core::RenderPlatform::RENDER_PLATFORM_NONE
};

Vermilion::Core::API * Vermilion::Core::API::create(int platform, Vermilion::Core::Instance * instance){
	switch(platform){
#ifdef VMCORE_OPENGL
		case Vermilion::Core::RenderPlatform::RENDER_PLATFORM_OPENGL:
			return new Vermilion::Core::OpenGL::API(instance);
			break;
#endif
#ifdef VMCORE_VULKAN
		case Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN:
			return new Vermilion::Core::Vulkan::API(instance);
			break;
#endif
		default:
			instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Render platform not supported");
			throw std::runtime_error("Vermilion::Core::API::create() - Render platform not supported");
			break;
	}
	return nullptr;
}
