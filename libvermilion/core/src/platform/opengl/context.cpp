#ifdef VMCORE_OPENGL

#include "context.hpp"
#include <vermilion/core/context.hpp>
#include <vermilion/core/logger.hpp>
#include <vermilion/core/instance.hpp>

Vermilion::Core::OPENGL_Context::OPENGL_Context(Vermilion::Core::Instance * instance){
	this->vmCoreInstance = instance;
}

Vermilion::Core::OPENGL_Context::~OPENGL_Context(){
}

void Vermilion::Core::OPENGL_Context::init(Vermilion::Core::ContextProperties * properties){
	this->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "Initializing OpenGL context");
}

#endif
