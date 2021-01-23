#include "Pipeline.hpp"

Vermilion::Core::PipelineLayoutUniform::PipelineLayoutUniform(std::string name, size_t size){
    this->name = name;
    this->type = PIPELINE_LAYOUT_UNIFORM_TYPE_NONE;
    this->size = size;
    this->offset = 0;
}