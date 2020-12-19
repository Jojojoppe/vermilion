#ifdef VMCORE_OPENGL

#include "api.hpp"
#include <vermilion/instance.hpp>

Vermilion::Core::OpenGL::API::API(Vermilion::Core::Instance * instance){
	this->instance = instance;

	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Creating OpenGL context");
}

Vermilion::Core::OpenGL::API::~API(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Destroying OpenGL context");
}

#endif
