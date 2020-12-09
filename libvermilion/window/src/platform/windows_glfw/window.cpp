#ifdef VMWINDOW_WIN32_GLFW

#include "window.hpp"
#include <vermilion/window/window.hpp>
#include <vermilion/core/logger.hpp>

Vermilion::Window::WINDOWS_GLFW_WindowInstance::WINDOWS_GLFW_WindowInstance(Vermilion::Window::Window * window){
	this->vmWindowWindow = window;

	vmWindowWindow->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Starting GLFW window on WINDOWS");
}

Vermilion::Window::WINDOWS_GLFW_WindowInstance::~WINDOWS_GLFW_WindowInstance(){

}

void Vermilion::Window::WINDOWS_GLFW_WindowInstance::open(Vermilion::Core::ContextProperties * contextProperties){
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

	switch(contextProperties->API){
		case VMCORE_API_OPENGL:
			glfwMakeContextCurrent(this->window);
#ifdef VMCORE_OPENGL
			contextProperties->opengl_procAddress = (void*)glfwGetProcAddress;
#endif
			break;
		case VMCORE_API_VULKAN:
			break;
		default:
			this->vmWindowWindow->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_FATAL, "API %d not supported", contextProperties->API);
	}
	// Finalize context initialization
	this->vmWindowWindow->vmCoreInstance->initContext(contextProperties);
}

#endif
