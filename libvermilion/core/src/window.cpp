#include <vermilion/window.hpp>

const int Vermilion::Core::windowPlatform[] = {
#ifdef VMCORE_GLFW
	Vermilion::Core::WindowPlatform::WINDOW_PLATFORM_GLFW,
#endif
	Vermilion::Core::WindowPlatform::WINDOW_PLATFORM_NONE
};
