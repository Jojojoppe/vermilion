#include <vermilion/api.hpp>

const int Vermilion::Core::renderPlatform[] = {
#ifdef VMCORE_OPENGL
	Vermilion::Core::RenderPlatform::RENDER_PLATFORM_OPENGL,
#endif
#ifdef VMCORE_VULKAN
	Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN,
#endif
	Vermilion::Core::RenderPlatform::RENDER_PLATFORM_NONE
};
