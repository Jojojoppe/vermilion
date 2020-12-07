#pragma once

#include <vermilion/core/instance.hpp>
#include <vermilion/core/context.hpp>
#include <memory>
#include <string>

#define VMWINDOW_PLATFORM_UNIX_X11 1
#define VMWINDOW_PLATFORM_UNIX_GLFW 2
#define VMWINDOW_PLATFORM_WINDOWS_GLFW 3
#define VMWINDOW_PLATFORM_WINDOWS 4

namespace Vermilion{
namespace Window{

class Window;

class WindowProperties{
	public:
		int width, height;
		std::string title;
		WindowProperties(int width=800, int height=800, const std::string& title="floatme");
};

class WindowInstance{
	public:
		Window * vmWindowWindow;

	public:
		~WindowInstance() = default;

		virtual void open(Vermilion::Core::ContextProperties * contextProperties){};
};

class Window{
	public:
		std::shared_ptr<Vermilion::Core::Instance> vmCoreInstance;
		WindowProperties properties;
	private:
		std::unique_ptr<WindowInstance> windowInstance;

	public:
		Window(std::shared_ptr<Vermilion::Core::Instance> instance, WindowProperties& properties);
		~Window();

		Vermilion::Core::ContextProperties* getContextProperties();
		void open(Vermilion::Core::ContextProperties * contextProperties);
};

}}
