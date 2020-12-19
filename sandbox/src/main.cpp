#include <vermilion/vermilion.hpp>

#include <vermilion/instance.hpp>

int main(int argc, char ** argv){

	// Create Vermilion instance
	int hintType[] = {Vermilion::Core::HintType::HINT_TYPE_WINDOW_PLATFORM, Vermilion::Core::HintType::HINT_TYPE_RENDER_PLATFORM, 0};
	int hintValue[] = {Vermilion::Core::WindowPlatform::WINDOW_PLATFORM_GLFW, Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN, 0};
	Vermilion::Core::Instance vmInstance(hintType, hintValue);

	return 0;
}
