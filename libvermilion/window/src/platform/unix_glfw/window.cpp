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
	glfwDestroyWindow(this->window);
	glfwTerminate();
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
	this->api = contextProperties->API;
	this->vmWindowWindow->vmCoreInstance->initContext(contextProperties);
}

bool Vermilion::Window::UNIX_GLFW_WindowInstance::shouldClose(){
	glfwPollEvents();
	return !glfwWindowShouldClose(this->window);
}

void Vermilion::Window::UNIX_GLFW_WindowInstance::present(){
	switch(this->api){
#ifdef VMCORE_OPENGL
		case VMCORE_API_OPENGL:
			glfwSwapBuffers(this->window);
			break;
#endif
#ifdef VMCORE_VULKAN
		case VMCORE_API_VULKAN:
			break;
#endif
		default:
			this->vmWindowWindow->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_FATAL, "API %d not supported", this->api);
	}
}

#endif
