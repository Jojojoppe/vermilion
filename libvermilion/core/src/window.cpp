#include <vermilion/window.hpp>
#include <vermilion/instance.hpp>

#include "platform/glfw/window.hpp"

const int Vermilion::Core::windowPlatform[] = {
#ifdef VMCORE_GLFW
	Vermilion::Core::WindowPlatform::WINDOW_PLATFORM_GLFW,
#endif
	Vermilion::Core::WindowPlatform::WINDOW_PLATFORM_NONE
};

Vermilion::Core::Window * Vermilion::Core::Window::create(int platform, Vermilion::Core::Instance * instance){
	switch(platform){
#ifdef VMCORE_GLFW
		case Vermilion::Core::WindowPlatform::WINDOW_PLATFORM_GLFW:
			return new Vermilion::Core::GLFW::Window(instance);
			break;
#endif
		default:
			instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Render platform not supported");
			break;
	}
	return nullptr;
}
