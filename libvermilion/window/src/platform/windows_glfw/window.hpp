#ifdef VMWINDOW_WIN32_GLFW
#pragma once

#include <vermilion/window/window.hpp>

#include <GLFW/glfw3.h>

namespace Vermilion{
namespace Window{

class WINDOWS_GLFW_WindowInstance : public WindowInstance{
	private:
		GLFWwindow * window;

	public:
		WINDOWS_GLFW_WindowInstance(Window * window);
		~WINDOWS_GLFW_WindowInstance();

		virtual void open(Vermilion::Core::ContextProperties * contextProperties) override;
};

}}
#endif
