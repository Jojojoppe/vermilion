#ifdef VMCORE_OPENGL

#include "context.hpp"
#include <vermilion/core/context.hpp>
#include <vermilion/core/logger.hpp>
#include <vermilion/core/instance.hpp>

#include <glad/glad.h>

Vermilion::Core::OPENGL_Context::OPENGL_Context(Vermilion::Core::Instance * instance){
	this->vmCoreInstance = instance;
}

Vermilion::Core::OPENGL_Context::~OPENGL_Context(){
}

void Vermilion::Core::OPENGL_Context::init(Vermilion::Core::ContextProperties * properties){
	this->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "Initializing OpenGL context");

	gladLoadGLLoader((GLADloadproc)properties->opengl_procAddress);

	this->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "OpenGL info:");
	this->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "  vendor: %s", glGetString(GL_VENDOR));
	this->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "  Renderer: %s", glGetString(GL_RENDERER));
	this->vmCoreInstance->logger.log(VMCORE_LOGLEVEL_INFO, "  Version: %s", glGetString(GL_VERSION));
}

#endif
