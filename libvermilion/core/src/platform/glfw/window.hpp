#ifdef VMCORE_GLFW
#pragma once

#include <vermilion/instance.hpp>

#ifdef VMCORE_VULKAN
	#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

namespace Vermilion{
namespace Core{
namespace GLFW{

class Window : public Vermilion::Core::Window{
	public:

	private:
		Vermilion::Core::Instance * instance;
		int renderPlatform;
		
		GLFWwindow * window;

	public:
		Window(int renderPlatform, Vermilion::Core::Instance * instance);
		virtual ~Window() override;

		virtual void createWindow(int width, int height) override;
		virtual void startRender() override;
		virtual void endRender() override;

		virtual bool shouldClose() override;

		virtual void getFrameBufferSize(int * width, int * height) override;

#ifdef VMCORE_OPENGL
		// OpenGL specific functions
		virtual void * getLoadProc() override;
		virtual void activateContext() override;
#endif
#ifdef VMCORE_VULKAN
		// Vulkan specific functions
		virtual void * getRequiredExtensions(unsigned int * count) override;
		virtual void * getSurface(void * instance) override;
#endif

	private:

};

static Vermilion::Core::Logger * glfwErrorCallbackLogger;
static void glfwErrorCallback(int err, const char * msg);

}}}

#endif
