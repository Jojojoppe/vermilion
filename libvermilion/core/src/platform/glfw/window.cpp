#ifdef VMCORE_GLFW

#include "window.hpp"
#include <vermilion/instance.hpp>

Vermilion::Core::GLFW::Window::Window(Vermilion::Core::Instance * instance){
	this->instance = instance;

	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Creating GLFW window");
}

Vermilion::Core::GLFW::Window::~Window(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Destroying GLFW window");
}

#endif
