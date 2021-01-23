#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>

#include "Buffer.hpp"
#include "Renderable.hpp"
#include "RenderTarget.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

VmRenderTarget::~VmRenderTarget(){
    if(ID){
        instance->rendertargets[ID].reset();
        instance->rendertargets.erase(ID);
    }
}
void VmRenderTarget::start(float r, float g, float b, float a){
    instance->rendertargets[ID]->start(r, g, b, a);
}
void VmRenderTarget::end(){
    instance->rendertargets[ID]->end();
}
void VmRenderTarget::draw(VmPipeline& pipeline, VmBinding& binding, VmRenderable& renderable, unsigned int instances, unsigned int firstinstance){
    auto pl = instance->pipelines[pipeline.ID];
    auto bd = instance->bindings[binding.ID];
    auto rdb = instance->renderables[renderable.ID];
    instance->rendertargets[ID]->draw(pl, bd, rdb, instances, firstinstance);
}
void VmRenderTarget::setUniform(VmPipeline& pipeline, const std::string& name, void * data){
    auto pl = instance->pipelines[pipeline.ID];
    instance->rendertargets[ID]->setUniform(pl, name, data);
}

VmShader::~VmShader(){
    instance->shaders[ID].reset();
    instance->shaders.erase(ID);
}
Vermilion::Core::ShaderType VmShader::type(){
    auto shd = instance->shaders[ID];
    return (Vermilion::Core::ShaderType)shd->type;
}

VmShaderProgram::~VmShaderProgram(){
    instance->shaderprograms[ID].reset();
    instance->shaderprograms.erase(ID);
}

VmPipelineLayout::~VmPipelineLayout(){
    instance->pipelinelayouts[ID].reset();
    instance->pipelinelayouts.erase(ID);
}

VmPipeline::~VmPipeline(){
    instance->pipelines[ID].reset();
    instance->pipelines.erase(ID);
}
void VmPipeline::setViewport(int width, int height, int x, int y){
    instance->pipelines[ID]->setViewPort(width, height, x, y);
}
void VmPipeline::setScissor(int width, int height, int x, int y){
    instance->pipelines[ID]->setScissor(width, height, x, y);
}

VmBinding::~VmBinding(){
    instance->bindings[ID].reset();
    instance->bindings.erase(ID);
}

VmBuffer::~VmBuffer(){
    instance->buffers[ID].reset();
    instance->buffers.erase(ID);
}
Vermilion::Core::BufferType VmBuffer::type(){
    auto buf = instance->buffers[ID];
    return buf->type;
}
size_t VmBuffer::size(){
    auto buf = instance->buffers[ID];
    return buf->size;
}
void VmBuffer::setData(void * data, size_t size){
    auto buf = instance->buffers[ID];
    buf->setData(data, size);
}
void VmBuffer::getData(void * data, size_t size){
    auto buf = instance->buffers[ID];
    buf->getData(data, size);
}

VmRenderable::~VmRenderable(){
    instance->renderables[ID].reset();
    instance->renderables.erase(ID);
}
unsigned int VmRenderable::vertexOffset(){
    auto ren = instance->renderables[ID];
    return ren->vertexOffset;
}
unsigned int VmRenderable::indexOffset(){
    auto ren = instance->renderables[ID];
    return ren->indexOffset;
}
unsigned int VmRenderable::length(){
    auto ren = instance->renderables[ID];
    return ren->length;
}
void VmRenderable::vertexOffset(unsigned int vertexOffset){
    auto ren = instance->renderables[ID];
    ren->vertexOffset = vertexOffset;
}
void VmRenderable::indexOffset(unsigned int indexOffset){
    auto ren = instance->renderables[ID];
    ren->indexOffset = indexOffset;
}
void VmRenderable::length(unsigned int length){
    auto ren = instance->renderables[ID];
    ren->length = length;
}

VmTexture::~VmTexture(){
    instance->textures[ID].reset();
    instance->textures.erase(ID);
}
size_t VmTexture::width(){
    auto tex = instance->textures[ID];
    return tex->width;
}
size_t VmTexture::height(){
    auto tex = instance->textures[ID];
    return tex->height;
}
size_t VmTexture::channels(){
    auto tex = instance->textures[ID];
    return tex->channels;
}
void VmTexture::setData(void * data, size_t size){
    auto tex = instance->textures[ID];
    tex->setData(data, size);
}

VmSampler::~VmSampler(){
    instance->samplers[ID].reset();
    instance->samplers.erase(ID);
}