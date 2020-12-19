#ifdef VMWINDOW_UNIX_GLFW
#pragma once

#include <vermilion/window/window.hpp>

#include <GLFW/glfw3.h>

namespace Vermilion{
namespace Window{

class UNIX_GLFW_WindowInstance : public WindowInstance{
	private:
		GLFWwindow * window;

	public:
		UNIX_GLFW_WindowInstance(Window * window);
		~UNIX_GLFW_WindowInstance();

		virtual void open(Vermilion::Core::ContextProperties * contextProperties) override;
		virtual void present() override;

		virtual bool shouldClose() override;
};

}}
#endif
