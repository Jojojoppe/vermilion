#ifdef VMCORE_GLFW

#include "window.hpp"
#include <vermilion/instance.hpp>

#include <stdint.h>
#include <stdexcept>

Vermilion::Core::GLFW::Window::Window(int renderPlatform, Vermilion::Core::WindowCallbackFunctions windowCallbackFunctions, Vermilion::Core::Instance * instance){
	this->instance = instance;
	this->renderPlatform = renderPlatform;
	this->windowCallbackFunctions = windowCallbackFunctions;

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
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	
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

	glfwSetWindowUserPointer(this->window, this);
	glfwSetFramebufferSizeCallback(this->window, [](GLFWwindow * window, int width, int height){
		((Vermilion::Core::GLFW::Window*)(glfwGetWindowUserPointer(window)))->instance->api->resize();
	});
	glfwSetMouseButtonCallback(this->window, [](GLFWwindow * window, int button, int action, int mods){
		((Vermilion::Core::GLFW::Window*)(glfwGetWindowUserPointer(window)))->mouseButton(button, action, mods);
	});
	glfwSetCursorPosCallback(this->window, [](GLFWwindow * window, double x, double y){
		((Vermilion::Core::GLFW::Window*)(glfwGetWindowUserPointer(window)))->mousePos(x, y);
	});

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

void Vermilion::Core::GLFW::Window::startRender(){
	glfwPollEvents();
	this->instance->api->startRender();
}

void Vermilion::Core::GLFW::Window::endRender(std::initializer_list<std::shared_ptr<Vermilion::Core::RenderTarget>> extraRenderTargets){
	this->instance->api->endRender(extraRenderTargets);
	if(this->renderPlatform==Vermilion::Core::RenderPlatform::RENDER_PLATFORM_OPENGL){
		glfwSwapBuffers(this->window);
	}
}

bool Vermilion::Core::GLFW::Window::shouldClose(){
	return !glfwWindowShouldClose(this->window);
}

void Vermilion::Core::GLFW::Window::setUserPointer(void * userPointer){
	this->userPointer = userPointer;
}

void Vermilion::Core::GLFW::Window::resized(){
	if(this->windowCallbackFunctions.resizeCallback){
		int width, height;
		glfwGetFramebufferSize(this->window, &width, &height);
		this->width = width;
		this->height = height;
		this->windowCallbackFunctions.resizeCallback(this->instance, userPointer, width, height);
	}
}

void Vermilion::Core::GLFW::Window::mouseButton(int button, int action, int mods){
	if(this->windowCallbackFunctions.mouseButtonCallback){
		Vermilion::Core::WindowMouseAction wact = WINDOW_MOUSE_ACTION_PRESS;
		if(action==GLFW_RELEASE){
			wact = WINDOW_MOUSE_ACTION_RELEASE;
		}
		Vermilion::Core::WindowMouseButton wbtn= WINDOW_MOUSE_BUTTON_LEFT;
		if(button==GLFW_MOUSE_BUTTON_MIDDLE){
			wbtn = WINDOW_MOUSE_BUTTON_MIDDLE;
		}
		if(button==GLFW_MOUSE_BUTTON_RIGHT){
			wbtn = WINDOW_MOUSE_BUTTON_RIGHT;
		}
		this->windowCallbackFunctions.mouseButtonCallback(this->instance, userPointer, wbtn, wact);
	}
}

void Vermilion::Core::GLFW::Window::mousePos(double x, double y){
	if(this->windowCallbackFunctions.mousePosCallback){
		this->windowCallbackFunctions.mousePosCallback(this->instance, userPointer, x, y);
	}
}

void Vermilion::Core::GLFW::Window::getFrameBufferSize(int * width, int * height){
	glfwGetFramebufferSize(this->window, width, height);
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

void * Vermilion::Core::GLFW::Window::getRequiredExtensions(unsigned int * count){
	return (void*)glfwGetRequiredInstanceExtensions((uint32_t*)count);
}

void * Vermilion::Core::GLFW::Window::getSurface(void * instance){
	VkInstance * vkinstance = (VkInstance*)instance;
	VkSurfaceKHR surface;
	if(glfwCreateWindowSurface(*vkinstance, this->window, nullptr, &surface)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create Vulkan window surface");
		throw std::runtime_error("Vermilion::Core::GLFW::Window::getSurface() - Could not create Vulkan window surface");
	}
	return (void*) surface;
}

#endif

#endif
