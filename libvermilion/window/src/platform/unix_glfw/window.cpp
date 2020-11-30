#ifdef VMWINDOW_UNIX_GLFW

#include "window.hpp"
#include <vermilion/window/window.hpp>
#include <vermilion/core/logger.hpp>

Vermilion::Window::UNIX_GLFW_WindowInstance::UNIX_GLFW_WindowInstance(std::shared_ptr<Vermilion::Core::Instance> instance){
	this->vmCoreInstance = instance;

	vmCoreInstance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Starting GLFW window on UNIX");
}

Vermilion::Window::UNIX_GLFW_WindowInstance::~UNIX_GLFW_WindowInstance(){

}

#endif
