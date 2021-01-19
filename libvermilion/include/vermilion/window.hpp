#pragma once

#include <string>
#include <memory>

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
class RenderTarget;

struct WindowCallbackFunctions{
	void (*resizeCallback)(Vermilion::Core::Instance * instance, void * userPointer) = nullptr;
};

class Window{
	public:
		int width, height;
		WindowCallbackFunctions windowCallbackFunctions;

	private:

	public:
		static Window * create(int platform, int renderPlatform, WindowCallbackFunctions windowCallbackFunctions, Instance * instance);
		virtual ~Window() = default;

		virtual void setUserPointer(void * p){};

		virtual void createWindow(int width, int height){};
		virtual void startRender(){};
		virtual void endRender(std::initializer_list<std::shared_ptr<Vermilion::Core::RenderTarget>> extraRenderTargets){};

		virtual bool shouldClose(){return true;};

		virtual void resized(){};

		virtual void getFrameBufferSize(int * width, int * height){};

#ifdef VMCORE_OPENGL
		// OpenGL specific functions
		virtual void * getLoadProc(){return nullptr;};
		virtual void activateContext(){};
#endif
#ifdef VMCORE_VULKAN
		// Vulkan specific functions
		virtual void * getRequiredExtensions(unsigned int * count){return nullptr;};
		virtual void * getSurface(void * instance){return nullptr;};

#endif

	private:
};

}}
