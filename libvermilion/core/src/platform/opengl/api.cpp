#ifdef VMCORE_OPENGL

#include "api.hpp"
#include <vermilion/instance.hpp>

#include <glad/glad.h>

Vermilion::Core::OpenGL::API::API(Vermilion::Core::Instance * instance){
	this->instance = instance;

	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Creating OpenGL context");
}

Vermilion::Core::OpenGL::API::~API(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Destroying OpenGL context");
}

void Vermilion::Core::OpenGL::API::init(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Initializing OpenGL context");

	this->instance->window->activateContext();
	void * procAddress = this->instance->window->getLoadProc();
	gladLoadGLLoader((GLADloadproc)procAddress);
}

#endif
