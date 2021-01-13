#ifdef VMCORE_VULKAN
#include "Renderable.hpp"

#include "api.hpp"


Vermilion::Core::Vulkan::Renderable::Renderable(Vermilion::Core::Vulkan::API * api, std::shared_ptr<Vermilion::Core::VertexBuffer> vertexBuffer, std::shared_ptr<Vermilion::Core::IndexBuffer> indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length){
    this->api = api;
    this->instance = api->instance;

    this->vertexBuffer = vertexBuffer;
    this->indexBuffer = indexBuffer;
    if(length){
        this->length = length;
    }else{
        this->length = indexBuffer->count;
    }
    this->vertexOffset = vertexOffset;
    this->indexOffset = indexOffset;
}

Vermilion::Core::Vulkan::Renderable::~Renderable(){
    
}

#endif