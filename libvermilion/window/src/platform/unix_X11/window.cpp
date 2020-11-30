#ifdef VMWINDOW_UNIX_X11

#include "window.hpp"
#include <vermilion/window/window.hpp>
#include <vermilion/core/logger.hpp>

Vermilion::Window::UNIX_X11_WindowInstance::UNIX_X11_WindowInstance(std::shared_ptr<Vermilion::Core::Instance> instance){
	this->vmCoreInstance = instance;

	vmCoreInstance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Starting X11 window on UNIX");
}

Vermilion::Window::UNIX_X11_WindowInstance::~UNIX_X11_WindowInstance(){

}

#endif
