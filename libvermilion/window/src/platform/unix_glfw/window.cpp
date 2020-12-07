#ifdef VMWINDOW_UNIX_GLFW

#include "window.hpp"
#include <vermilion/window/window.hpp>
#include <vermilion/core/logger.hpp>

#include <GLFW/glfw3.h>

Vermilion::Window::UNIX_GLFW_WindowInstance::UNIX_GLFW_WindowInstance(Window * window){
	this->vmWindowWindow = window;

	this->vmWindowWindow->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Starting GLFW window on UNIX");
}

Vermilion::Window::UNIX_GLFW_WindowInstance::~UNIX_GLFW_WindowInstance(){

}

void Vermilion::Window::UNIX_GLFW_WindowInstance::open(Vermilion::Core::ContextProperties * contextProperties){
	this->vmWindowWindow->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "Initialize GLFW");

	glfwInit();

	// Set API
	switch(contextProperties->API){
		case VMCORE_API_OPENGL:
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			break;
		case VMCORE_API_VULKAN:
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			break;
		default:
			this->vmWindowWindow->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_FATAL, "API %d not supported", contextProperties->API);
	}

	// Create window
	this->window = glfwCreateWindow(this->vmWindowWindow->properties.width, this->vmWindowWindow->properties.height,
			"floatme", nullptr, nullptr);

	// Finalize context initialization
	this->vmWindowWindow->vmCoreInstance->initContext(contextProperties);
}

#endif
