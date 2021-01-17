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

	this->instance->logger.log(VMCORE_LOGLEVEL_INFO, "OpenGL info:");
	this->instance->logger.log(VMCORE_LOGLEVEL_INFO, "  Vendor:   %s", glGetString(GL_VENDOR));
	this->instance->logger.log(VMCORE_LOGLEVEL_INFO, "  Renderer: %s", glGetString(GL_RENDERER));
	this->instance->logger.log(VMCORE_LOGLEVEL_INFO, "  Version:  %s", glGetString(GL_VERSION));

	glClear(GL_COLOR_BUFFER_BIT);
}

void Vermilion::Core::OpenGL::API::startRender(){
}

void Vermilion::Core::OpenGL::API::endRender(std::initializer_list<std::shared_ptr<Vermilion::Core::RenderTarget>> extraRenderTargets){
}

std::shared_ptr<Vermilion::Core::RenderTarget> Vermilion::Core::OpenGL::API::getDefaultRenderTarget(){
	return nullptr;
}

std::shared_ptr<Vermilion::Core::Shader> Vermilion::Core::OpenGL::API::createShader(const std::string& source, Vermilion::Core::ShaderType type){
	return nullptr;
}

std::shared_ptr<Vermilion::Core::ShaderProgram> Vermilion::Core::OpenGL::API::createShaderProgram(std::initializer_list<std::shared_ptr<Vermilion::Core::Shader>> shaders){
	return nullptr;
}

#endif
