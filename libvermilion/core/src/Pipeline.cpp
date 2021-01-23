#include "Pipeline.hpp"

Vermilion::Core::PipelineLayoutUniform::PipelineLayoutUniform(std::string name, size_t size){
    this->name = name;
    this->type = PIPELINE_LAYOUT_UNIFORM_TYPE_NONE;
    this->size = size;
    this->offset = 0;
}

Vermilion::Core::PipelineLayoutBinding::PipelineLayoutBinding(Vermilion::Core::PipelineLayoutBindingType type, unsigned int offset, size_t size){
    this->type = type;
    this->offset = offset;
    this->size = size;
}