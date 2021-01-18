#pragma once

#include <vermilion/logger.hpp>

#include <memory>
#include <vector>

namespace Vermilion{
namespace Core{

enum PipelineLayoutBinding{
	PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER = 1,
	PIPELINE_LAYOUT_BINDING_SAMPLER
};

class Pipeline{
	public:
		~Pipeline() = default;
		virtual void setViewPort(unsigned int width, unsigned int height, unsigned int x=0, unsigned int y=0){};
};

class Binding{
	public:
		~Binding() = default;
};

}}