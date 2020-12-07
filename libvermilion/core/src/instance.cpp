#include <vermilion/core/instance.hpp>
#include <vermilion/vermilion.hpp>

#include "platform/opengl/context.hpp"

Vermilion::Core::Instance::Instance(){
	this->logger.log(VMCORE_LOGLEVEL_INFO, "Vermilion core version %d.%d", VERMILION_VERSION_MAJOR, VERMILION_VERSION_MINOR);
	this->logger.log(VMCORE_LOGLEVEL_INFO, "%s - %s [%s]", VERMILION_AUTHOR_NAME, VERMILION_AUTHOR_EMAIL, VERMILION_LICENCE);
	this->logger.log(VMCORE_LOGLEVEL_INFO, "----");
}

Vermilion::Core::Instance::~Instance(){

}

void Vermilion::Core::Instance::createContext(Vermilion::Core::ContextProperties * properties){
	// TODO final API choosing
	properties->API = VMCORE_API_OPENGL;

	switch(properties->API){

#ifdef VMCORE_OPENGL
		case VMCORE_API_OPENGL:
			this->context.reset(new Vermilion::Core::OPENGL_Context(this));
			break;
#endif

		default:
			this->logger.log(VMCORE_LOGLEVEL_FATAL, "API not supported");
			break;
	}
}

void Vermilion::Core::Instance::initContext(Vermilion::Core::ContextProperties * properties){
	this->context->init(properties);
}
