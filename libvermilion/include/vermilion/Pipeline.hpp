#pragma once

#include <vermilion/logger.hpp>

#include <memory>
#include <vector>

namespace Vermilion{
namespace Core{

enum PipelineLayoutBinding{
	PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER,
	PIPELINE_LAYOUT_BINDING_SAMPLER
};

class Pipeline{
	public:
		~Pipeline() = default;
};

class Binding{
	public:
		~Binding() = default;
};

}}