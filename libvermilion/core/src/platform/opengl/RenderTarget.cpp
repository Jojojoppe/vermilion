#ifdef VMCORE_OPENGL
#include "RenderTarget.hpp"

#include "api.hpp"
#include "Pipeline.hpp"
#include "Renderable.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include <stdexcept>

int OpenGLBufferLayoutElementTypeToOpenGLBaseType[] = {
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
};

Vermilion::Core::OpenGL::RenderTarget::RenderTarget(Vermilion::Core::OpenGL::API * api, std::shared_ptr<Vermilion::Core::Texture> texture){
    this->api = api;
    this->instance = api->instance;
    this->texture = std::static_pointer_cast<Vermilion::Core::OpenGL::Texture>(texture);

    if(texture!=nullptr){
        glGenFramebuffers(1, &this->framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

        glBindTexture(GL_TEXTURE_2D, this->texture->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture->texture, 0);

        glGenRenderbuffers(1, &this->depthbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, this->depthbuffer); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, texture->width, texture->height);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthbuffer);

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

Vermilion::Core::OpenGL::RenderTarget::~RenderTarget(){
    glDeleteRenderbuffers(1, &this->depthbuffer);
    glDeleteFramebuffers(1, &this->framebuffer);
}

void Vermilion::Core::OpenGL::RenderTarget::start(float r, float g, float b, float a){
    glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Vermilion::Core::OpenGL::RenderTarget::end(){
    glBindTexture(GL_TEXTURE_2D, this->texture->texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Vermilion::Core::OpenGL::RenderTarget::draw(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, std::shared_ptr<Vermilion::Core::Binding> binding, std::shared_ptr<Vermilion::Core::Renderable> renderable, int instanceCount, int firstInstance){
    auto glpipeline = std::static_pointer_cast<Vermilion::Core::OpenGL::Pipeline>(pipeline);
    auto glbinding = std::static_pointer_cast<Vermilion::Core::OpenGL::Binding>(binding);
    auto glrenderable = std::static_pointer_cast<Vermilion::Core::OpenGL::Renderable>(renderable);
    auto glvertex = std::static_pointer_cast<Vermilion::Core::OpenGL::Buffer>(glrenderable->vertexBuffer);
    auto glindex = std::static_pointer_cast<Vermilion::Core::OpenGL::Buffer>(glrenderable->indexBuffer);
    auto glshaderprogram = std::static_pointer_cast<Vermilion::Core::OpenGL::ShaderProgram>(glpipeline->shaderProgram);

    glUseProgram(glshaderprogram->shaderProgram);
    glViewport(glpipeline->vpx, glpipeline->vpy, glpipeline->vpwidth, glpipeline->vpheight);
    glScissor(glpipeline->sx, glpipeline->sy, glpipeline->swidth, glpipeline->sheight);
    if(glpipeline->settings.depthtest==Vermilion::Core::PipelineSettingsDepthTest::PIPELINE_SETTINGS_DEPTH_TEST_ENABLED){
        glEnable(GL_DEPTH_TEST);
    }else{
        glDisable(GL_DEPTH_TEST);
    }

    int i = 0;
    int bufi = 0;
    int sami = 0;
    for(auto& b : glpipeline->pipelineLayout->bindings){
        switch(b){
            case Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER:
            case Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_STORAGE_BUFFER:
                glBindBufferBase(glbinding->buffers[bufi]->destination, i, glbinding->buffers[bufi]->buffer);
                bufi++;
                break;
            case Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_SAMPLER:
                glActiveTexture(GL_TEXTURE0+i);
                glBindTexture(GL_TEXTURE_2D, glbinding->samplers[sami]->texture->texture);
                glBindSampler(i, glbinding->samplers[sami]->sampler);
                sami++;
                break;
        }
        i++;
    }

    glBindVertexArray(glpipeline->pipelineLayout->vao);
    glBindBuffer(glvertex->destination, glvertex->buffer);
    switch(glpipeline->settings.cullmode){
        case Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_NONE:
            glDisable(GL_CULL_FACE);
            break;
        case Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_BACK_C:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CW);
            break;
        case Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_BACK_CC:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            break;
    }
    char * offset = 0;
    for(int i=0; i<glpipeline->pipelineLayout->vertexLayout.size(); i++){
        auto& v = glpipeline->pipelineLayout->vertexLayout[i];
        glVertexAttribPointer(i, v.count, OpenGLBufferLayoutElementTypeToOpenGLBaseType[v.type], v.normalized, glpipeline->pipelineLayout->stride, (void*)offset);
        offset += v.size;
        glEnableVertexAttribArray(i);
    }
    glBindBuffer(glindex->destination, glindex->buffer);

    glDrawElementsInstancedBaseVertexBaseInstance(glpipeline->drawmode, glrenderable->length, GL_UNSIGNED_INT, (void*)(glrenderable->indexOffset*sizeof(unsigned int)), instanceCount, glrenderable->vertexOffset, firstInstance);
}

void Vermilion::Core::OpenGL::RenderTarget::setUniform(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, const std::string& name, void * data){
    auto glpipeline = std::static_pointer_cast<Vermilion::Core::OpenGL::Pipeline>(pipeline);
    auto glprog = std::static_pointer_cast<Vermilion::Core::OpenGL::ShaderProgram>(glpipeline->shaderProgram);
    glUseProgram(glprog->shaderProgram);
    int loc = glGetUniformLocation(glprog->shaderProgram, name.c_str());
    if(loc<0){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Uniform with name %s not found in pipeline", name.c_str());
		throw std::runtime_error("Vermilion::Core::OpenGL::RenderTarget::setUniform() - Uniform not found in pipeline");
    }
    switch(glpipeline->pipelineLayout->uniforms[name]->type){
        case Vermilion::Core::PipelineLayoutUniformType::PIPELINE_LAYOUT_UNIFORM_TYPE_FLOAT1:
            glUniform1f(loc, *((float*)data));
            break;
        default:
           break;
    }
}

Vermilion::Core::OpenGL::DefaultRenderTarget::DefaultRenderTarget(Vermilion::Core::OpenGL::API * api, std::shared_ptr<Vermilion::Core::Texture> texture) :
        Vermilion::Core::OpenGL::RenderTarget(api, texture){
}

Vermilion::Core::OpenGL::DefaultRenderTarget::~DefaultRenderTarget(){
}

void Vermilion::Core::OpenGL::DefaultRenderTarget::start(float r, float g, float b, float a){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Vermilion::Core::OpenGL::DefaultRenderTarget::end(){
}

#endif