#include <vermilion/Buffer.hpp>

Vermilion::Core::VertexBufferLayoutElement::VertexBufferLayoutElement(std::string name, unsigned int count, unsigned int size, bool normalized){
    this->name = name;
    this->count = count;
    this->normalized = normalized;
    this->size = size*count;
    this->offset = 0;
    this->type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_NONE;
}