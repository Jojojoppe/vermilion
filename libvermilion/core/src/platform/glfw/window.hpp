#ifdef VMCORE_GLFW
#pragma once

#include <vermilion/instance.hpp>

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

#ifdef VMCORE_OPENGL
		// OpenGL specific functions
		virtual void * getLoadProc() override;
		virtual void activateContext() override;
#endif
#ifdef VMCORE_VULKAN
		// Vulkan specific functions

#endif

	private:

};

static Vermilion::Core::Logger * glfwErrorCallbackLogger;
static void glfwErrorCallback(int err, const char * msg);

}}}

#endif
