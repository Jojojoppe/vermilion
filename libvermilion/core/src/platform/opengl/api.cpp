#ifdef VMCORE_OPENGL

#include "api.hpp"
#include <vermilion/instance.hpp>

#include <glad/glad.h>

#include "Texture.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "Renderable.hpp"
#include "Pipeline.hpp"

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

	this->defaultRenderTarget = std::make_shared<Vermilion::Core::OpenGL::DefaultRenderTarget>(this);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vermilion::Core::flipLoading();
}

void Vermilion::Core::OpenGL::API::resize(){
	instance->window->resized();
}

void Vermilion::Core::OpenGL::API::startRender(){
}

void Vermilion::Core::OpenGL::API::endRender(std::vector<std::shared_ptr<Vermilion::Core::RenderTarget>>& extraRenderTargets){
}

std::shared_ptr<Vermilion::Core::RenderTarget> Vermilion::Core::OpenGL::API::getDefaultRenderTarget(){
	return std::static_pointer_cast<Vermilion::Core::RenderTarget>(this->defaultRenderTarget);
}

std::shared_ptr<Vermilion::Core::Texture> Vermilion::Core::OpenGL::API::createTexture(size_t width, size_t height, unsigned int channels){
    return std::static_pointer_cast<Vermilion::Core::Texture>(std::make_shared<Vermilion::Core::OpenGL::Texture>(this, width, height, channels));
}

std::shared_ptr<Vermilion::Core::Sampler> Vermilion::Core::OpenGL::API::createSampler(std::shared_ptr<Vermilion::Core::Texture> texture){
    return std::static_pointer_cast<Vermilion::Core::Sampler>(std::make_shared<Vermilion::Core::OpenGL::Sampler>(this, texture));
}

std::shared_ptr<Vermilion::Core::Shader> Vermilion::Core::OpenGL::API::createShader(const std::string& source, Vermilion::Core::ShaderType type){
    return std::static_pointer_cast<Vermilion::Core::Shader>(std::make_shared<Vermilion::Core::OpenGL::Shader>(this, source, type));
}

std::shared_ptr<Vermilion::Core::ShaderProgram> Vermilion::Core::OpenGL::API::createShaderProgram(std::vector<std::shared_ptr<Vermilion::Core::Shader>>& shaders){
    return std::static_pointer_cast<Vermilion::Core::ShaderProgram>(std::make_shared<Vermilion::Core::OpenGL::ShaderProgram>(this, shaders));
}

std::shared_ptr<Vermilion::Core::Buffer> Vermilion::Core::OpenGL::API::createBuffer(size_t size, BufferType type, BufferUsage usage, BufferDataUsage dataUsage){
    return std::static_pointer_cast<Vermilion::Core::Buffer>(std::make_shared<Vermilion::Core::OpenGL::Buffer>(this, size, type, usage, dataUsage));
}

std::shared_ptr<Vermilion::Core::Renderable> Vermilion::Core::OpenGL::API::createRenderable(std::shared_ptr<Vermilion::Core::Buffer> vertexBuffer, std::shared_ptr<Vermilion::Core::Buffer> indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length){
    return std::static_pointer_cast<Vermilion::Core::Renderable>(std::make_shared<Vermilion::Core::OpenGL::Renderable>(this, vertexBuffer, indexBuffer, vertexOffset, indexOffset, length));
}

std::shared_ptr<Vermilion::Core::PipelineLayout> Vermilion::Core::OpenGL::API::createPipelineLayout(std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout, std::initializer_list<Vermilion::Core::PipelineLayoutBinding> bindings){
    return std::static_pointer_cast<Vermilion::Core::PipelineLayout>(std::make_shared<Vermilion::Core::OpenGL::PipelineLayout>(this, vertexLayout, bindings));
}

std::shared_ptr<Vermilion::Core::Pipeline> Vermilion::Core::OpenGL::API::createPipeline(std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, 
		std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, std::shared_ptr<Vermilion::Core::PipelineLayout> pipelineLayout, Vermilion::Core::PipelineSettings settings){
    return std::static_pointer_cast<Vermilion::Core::Pipeline>(std::make_shared<Vermilion::Core::OpenGL::Pipeline>(this, renderTarget, shaderProgram, settings, pipelineLayout));
}

std::shared_ptr<Vermilion::Core::Binding> Vermilion::Core::OpenGL::API::createBinding(std::vector<std::shared_ptr<Vermilion::Core::Buffer>>& buffers, std::vector<std::shared_ptr<Vermilion::Core::Sampler>>& samplers){
    return std::static_pointer_cast<Vermilion::Core::Binding>(std::make_shared<Vermilion::Core::OpenGL::Binding>(this, buffers, samplers));
}


#endif