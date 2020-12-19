#include <vermilion/window/window.hpp>
#include <vermilion/core/logger.hpp>
#include <cstdio>
#include <iostream>

#include "platform/unix_glfw/window.hpp"
#include "platform/windows_glfw/window.hpp"

const int VmWindowSupportedPlatforms[] = {
#ifdef VMWINDOW_UNIX_X11
//	VMWINDOW_PLATFORM_UNIX_X11,
#endif
#ifdef VMWINDOW_UNIX_GLFW
	VMWINDOW_PLATFORM_UNIX_GLFW,
#endif
#ifdef VMWINDOW_WIN32_GLFW
	VMWINDOW_PLATFORM_WINDOWS_GLFW,
#endif
#ifdef VMWINDOW_WIN32
//	VMWINDOW_PLATFORM_WINDOWS,
#endif
	0
};

const char * VmWindowSupportedPlatformName[] = {
	"None",
	"Unix with X11",
	"Unix with GLFW",
	"Windows with GLFW",
	"Native Windows",
};


Vermilion::Window::Window::Window(std::shared_ptr<Vermilion::Core::Instance> instance, WindowProperties& properties){
	this->vmCoreInstance = instance;

	this->properties.width = properties.width;
	this->properties.height = properties.height;
	this->properties.title = properties.title;

	const int * supported = VmWindowSupportedPlatforms;
	while(*supported){
		vmCoreInstance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Compiled with support for '%s'", VmWindowSupportedPlatformName[*supported]);
		supported++;
	}

	//vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "Trying to create window with title \"%s\"", this->properties.title);
	//vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "                             width %d", properties.width);
	//vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "                             height %d", properties.height);

	switch(VmWindowSupportedPlatforms[0]){

#ifdef VMWINDOW_UNIX_GLFW
		case VMWINDOW_PLATFORM_UNIX_GLFW:
			windowInstance.reset(new Vermilion::Window::UNIX_GLFW_WindowInstance(this));
			break;
#endif
#ifdef VMWINDOW_WIN32_GLFW
		case VMWINDOW_PLATFORM_WINDOWS_GLFW:
			windowInstance.reset(new Vermilion::Window::WINDOWS_GLFW_WindowInstance(this));
			break;
#endif

		default:
			vmCoreInstance->logger.log(VMCORE_LOGLEVEL_FATAL, "Platform not supported to run Vermilion");
			break;
	}
}

Vermilion::Window::Window::~Window(){

}

Vermilion::Core::ContextProperties* Vermilion::Window::Window::getContextProperties(){
	Vermilion::Core::ContextProperties* contextProperties = new Vermilion::Core::ContextProperties();;
	contextProperties->width = 400;
	contextProperties->height = 400;
	
	return contextProperties;
}

void Vermilion::Window::Window::open(Vermilion::Core::ContextProperties * contextProperties){
	this->windowInstance->open(contextProperties);	
}

Vermilion::Window::WindowProperties::WindowProperties(int width, int height, const std::string& title){
	this->title = title;
	this->width = width;
	this->height = height;
}

bool Vermilion::Window::Window::shouldClose(){
	return this->windowInstance->shouldClose();
}

void Vermilion::Window::Window::present(){
	this->windowInstance->present();
}
