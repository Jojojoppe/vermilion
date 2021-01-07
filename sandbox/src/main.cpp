#include <vermilion/vermilion.hpp>

#include <vermilion/instance.hpp>

int main(int argc, char ** argv){

	// Create Vermilion instance
	int hintType[] = {
		Vermilion::Core::HintType::HINT_TYPE_WINDOW_PLATFORM, 
		Vermilion::Core::HintType::HINT_TYPE_RENDER_PLATFORM, 
		Vermilion::Core::HintType::HINT_TYPE_WINDOW_WIDTH,
		Vermilion::Core::HintType::HINT_TYPE_WINDOW_HEIGHT,
		Vermilion::Core::HintType::HINT_TYPE_LOGLEVEL,
	0};
	int hintValue[] = {
		Vermilion::Core::WindowPlatform::WINDOW_PLATFORM_GLFW, 
		Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN,
//		Vermilion::Core::RenderPlatform::RENDER_PLATFORM_OPENGL,
		400,
		400,
		VMCORE_LOGLEVEL_DEBUG,
	0};
	Vermilion::Core::Instance vmInstance(hintType, hintValue);
//	Vermilion::Core::Instance vmInstance(nullptr, nullptr);

	while(vmInstance.window->shouldClose()){
		vmInstance.startRender();
		vmInstance.endRender();
	}

	return 0;
}
