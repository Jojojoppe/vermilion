#ifdef VMCORE_OPENGL
#include "Renderable.hpp"

#include "api.hpp"


Vermilion::Core::OpenGL::Renderable::Renderable(Vermilion::Core::OpenGL::API * api, std::shared_ptr<Vermilion::Core::Buffer> vertexBuffer, std::shared_ptr<Vermilion::Core::Buffer> indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length){
    this->api = api;
    this->instance = api->instance;

    this->vertexBuffer = vertexBuffer;
    this->indexBuffer = indexBuffer;
    this->length = length;
    this->vertexOffset = vertexOffset;
    this->indexOffset = indexOffset;
}

Vermilion::Core::OpenGL::Renderable::~Renderable(){
    
}

#endif