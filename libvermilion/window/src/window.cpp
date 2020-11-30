#include <vermilion/window/window.hpp>
#include <vermilion/core/logger.hpp>
#include <cstdio>
#include <iostream>

#include "platform/unix_glfw/window.hpp"
#include "platform/unix_X11/window.hpp"

const int VmWindowSupportedPlatforms[] = {
	VMWINDOW_PLATFORM_NONE,
#ifdef VMWINDOW_UNIX_X11
	VMWINDOW_PLATFORM_UNIX_X11,
#endif
#ifdef VMWINDOW_UNIX_GLFW
	VMWINDOW_PLATFORM_UNIX_GLFW,
#endif
	0
};

const char * VmWindowSupportedPlatformName[] = {
	"None",
	"None",
	"Unix with X11",
	"Unix with GLFW",
	"Windows with GLFW",
	"Native Windows",
};


Vermilion::Window::Window::Window(std::shared_ptr<Vermilion::Core::Instance> instance){
	this->vmCoreInstance = instance;

	const int * supported = VmWindowSupportedPlatforms;
	while(*supported){
		vmCoreInstance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Compiled with support for '%s'", VmWindowSupportedPlatformName[*supported]);
		supported++;
	}

	// TODO select a supported WindowInstance
//	windowInstance.reset(new Vermilion::Window::UNIX_X11_WindowInstance(this->vmCoreInstance));
}

Vermilion::Window::Window::~Window(){

}
