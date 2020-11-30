#pragma once

#include <vermilion/core/instance.hpp>
#include <memory>

#define VMWINDOW_PLATFORM_NONE 1
#define VMWINDOW_PLATFORM_UNIX_X11 2
#define VMWINDOW_PLATFORM_UNIX_GLFW 3
#define VMWINDOW_PLATFORM_WINDOWS_GLFW 4
#define VMWINDOW_PLATFORM_WINDOWS 5

namespace Vermilion{
namespace Window{

class WindowInstance{
	public:
		std::shared_ptr<Vermilion::Core::Instance> vmCoreInstance;
	public:
		~WindowInstance() = default;
};

class Window{
	public:
		std::shared_ptr<Vermilion::Core::Instance> vmCoreInstance;
	private:
		std::unique_ptr<WindowInstance> windowInstance;
	public:
		Window(std::shared_ptr<Vermilion::Core::Instance> instance);
		~Window();
};

}}
