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

		void * userPointer;

	public:
		Window(int renderPlatform, Vermilion::Core::WindowCallbackFunctions windowCallbackFunctions, Vermilion::Core::Instance * instance);
		virtual ~Window() override;
		virtual void setUserPointer(void * p) override;

		virtual void createWindow(int width, int height) override;
		virtual void startRender() override;
		virtual void endRender(std::vector<std::shared_ptr<Vermilion::Core::RenderTarget>>& extraRenderTargets) override;

		virtual bool shouldClose() override;

		virtual void resized() override;
		void mouseButton(int button, int action, int mods);
		void mousePos(double x, double y);
		void mouseEnter(int direction);
		void scroll(double x, double y);

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
