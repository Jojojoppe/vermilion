#pragma once

#include <string>

namespace Vermilion{
namespace Core{

/**
 * @brief Type of windowing platform
 */
enum WindowPlatform{
	WINDOW_PLATFORM_NONE = 0,
	WINDOW_PLATFORM_GLFW
};
const std::string WindowPlatformString[] = {
	"NONE",
	"GLFW"
};


extern const int windowPlatform[];

class Instance;

class Window{
	public:
		int width, height;

	private:

	public:
		static Window * create(int platform, int renderPlatform, Instance * instance);
		virtual ~Window() = default;

		virtual void createWindow(int width, int height){};
		virtual void startRender(){};
		virtual void endRender(){};

		virtual bool shouldClose(){return true;};

#ifdef VMCORE_OPENGL
		// OpenGL specific functions
		virtual void * getLoadProc(){return nullptr;};
		virtual void activateContext(){};
#endif
#ifdef VMCORE_VULKAN
		// Vulkan specific functions

#endif

	private:
};

}}
