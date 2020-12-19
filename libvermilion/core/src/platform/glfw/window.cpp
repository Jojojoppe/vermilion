#ifdef VMCORE_GLFW

#include "window.hpp"
#include <vermilion/instance.hpp>

Vermilion::Core::GLFW::Window::Window(int renderPlatform, Vermilion::Core::Instance * instance){
	this->instance = instance;
	this->renderPlatform = renderPlatform;

	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Initializing GLFW");
	glfwErrorCallbackLogger = &instance->logger;
	glfwSetErrorCallback(Vermilion::Core::GLFW::glfwErrorCallback);
	glfwInit();
}

void Vermilion::Core::GLFW::Window::createWindow(int width, int height){
	this->width = width;
	this->height = height;
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Creating window of %dx%d", this->width, this->height);

	// Set glfw settings accordingly to render API
	switch(this->renderPlatform){
		case Vermilion::Core::RenderPlatform::RENDER_PLATFORM_OPENGL:
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			break;
		case Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN:
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			break;
		default:
			break;
	}

	this->window = glfwCreateWindow(this->width, this->height, "floatme", nullptr, nullptr);

	// Initialize render context
	this->instance->api->init();
}

Vermilion::Core::GLFW::Window::~Window(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Deinitializing GLFW");
	glfwTerminate();
}

void Vermilion::Core::GLFW::glfwErrorCallback(int err, const char * msg){
	glfwErrorCallbackLogger->log(VMCORE_LOGLEVEL_ERROR, "GLFW ERROR: %s", msg);
}

#ifdef VMCORE_OPENGL
// OpenGL specific functions

void * Vermilion::Core::GLFW::Window::getLoadProc(){
	return (void*)glfwGetProcAddress;
}

void Vermilion::Core::GLFW::Window::activateContext(){
	glfwMakeContextCurrent(this->window);
}

#endif

#ifdef VMCORE_VULKAN
// Vulkan specific functions

#endif

#endif
