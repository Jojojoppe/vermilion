#pragma once

#include <string>
#include <memory>
#include <vector>
#include <vermilion/vermilion.hpp>

namespace Vermilion{
namespace Core{

extern const int windowPlatform[];

const std::string WindowPlatformString[] = {
	"NONE",
	"GLFW"
};

class RenderTarget;

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
		virtual void endRender(std::vector<std::shared_ptr<Vermilion::Core::RenderTarget>>& extraRenderTargets){};

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
