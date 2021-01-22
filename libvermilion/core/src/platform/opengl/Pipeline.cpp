#ifdef VMCORE_OPENGL
#include "Pipeline.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include "Buffer.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

Vermilion::Core::OpenGL::PipelineLayout::PipelineLayout(Vermilion::Core::OpenGL::API * api, std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout, std::initializer_list<Vermilion::Core::PipelineLayoutBinding> layoutBindings){
    this->api = api;
    this->instance = api->instance;
    for(auto& b : layoutBindings) this->bindings.push_back(b);

    unsigned int offset = 0;
    for(auto& element : vertexLayout){
    	auto e = element;
    	e.offset = offset;
    	offset += e.size;
    	stride += e.size;
    	this->vertexLayout.push_back(e);
    }

    glGenVertexArrays(1, &this->vao);
}

Vermilion::Core::OpenGL::PipelineLayout::~PipelineLayout(){
    glDeleteVertexArrays(1, &this->vao);
}


Vermilion::Core::OpenGL::Pipeline::Pipeline(Vermilion::Core::OpenGL::API * api, std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, Vermilion::Core::PipelineSettings settings, std::shared_ptr<Vermilion::Core::PipelineLayout> pipelineLayout){
    this->api = api;
    this->instance = api->instance;
    this->renderTarget = renderTarget;
    this->shaderProgram = shaderProgram;
    this->settings = settings;
    this->pipelineLayout = std::static_pointer_cast<Vermilion::Core::OpenGL::PipelineLayout>(pipelineLayout);
    switch(settings.poygonmode){
        case Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE:
        case Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE_LINE:
            this->drawmode = GL_TRIANGLES;
            break;
        case Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_LINE:
            this->drawmode = GL_LINE;
            break;
        case Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_POINT:
            this->drawmode = GL_POINT;
            break;
    }
    if(settings.poygonmode==Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE_LINE){
        this->drawlined = true;
    }

    vpwidth = api->instance->window->width;
    swidth = api->instance->window->width;
    vpheight = api->instance->window->height;
    sheight = api->instance->window->height;
    vpx = sx = vpy = sy = 0;
}

Vermilion::Core::OpenGL::Pipeline::~Pipeline(){
}

void Vermilion::Core::OpenGL::Pipeline::setViewPort(unsigned int width, unsigned int height, unsigned int x, unsigned int y){
    vpwidth = width;
    vpheight = height;
    vpx = x;
    vpy = y;
}

void Vermilion::Core::OpenGL::Pipeline::setScissor(unsigned int width, unsigned int height, unsigned int x, unsigned int y){
    swidth = width;
    sheight = height;
    sx = x;
    sy = y;
}

Vermilion::Core::OpenGL::Binding::Binding(Vermilion::Core::OpenGL::API* api, std::vector<std::shared_ptr<Vermilion::Core::Buffer>>& buffers, std::vector<std::shared_ptr<Vermilion::Core::Sampler>>& samplers){
    this->api = api;
    this->instance = api->instance;
    for(auto& b : buffers) this->buffers.push_back(std::static_pointer_cast<Vermilion::Core::OpenGL::Buffer>(b));
    for(auto& s : samplers) this->samplers.push_back(std::static_pointer_cast<Vermilion::Core::OpenGL::Sampler>(s));
}

Vermilion::Core::OpenGL::Binding::~Binding(){
}

#endif