#include <vermilion/vermilion.hpp>

#include <vermilion/instance.hpp>

int main(int argc, char ** argv){

	// Create Vermilion instance
	int hintType[] = {
		Vermilion::Core::HintType::HINT_TYPE_WINDOW_PLATFORM, 
		Vermilion::Core::HintType::HINT_TYPE_RENDER_PLATFORM, 
		Vermilion::Core::HintType::HINT_TYPE_WINDOW_WIDTH,
		Vermilion::Core::HintType::HINT_TYPE_WINDOW_HEIGHT,
	0};
	int hintValue[] = {
		Vermilion::Core::WindowPlatform::WINDOW_PLATFORM_GLFW, 
		Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN,
		400,
		400,
	0};
	Vermilion::Core::Instance vmInstance(hintType, hintValue);

	for(;;);

	return 0;
}
