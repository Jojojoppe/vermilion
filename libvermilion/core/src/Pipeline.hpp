#pragma once

#include <vermilion/vermilion.hpp>
#include <vermilion/logger.hpp>

#include <memory>
#include <vector>

namespace Vermilion{
namespace Core{

class PipelineLayout{
	public:
		~PipelineLayout() = default;
};

class Pipeline{
	public:
		~Pipeline() = default;
		virtual void setViewPort(unsigned int width, unsigned int height, unsigned int x=0, unsigned int y=0){};
		virtual void setScissor(unsigned int width, unsigned int height, unsigned int x=0, unsigned int y=0){};
};

class Binding{
	public:
		~Binding() = default;
};

}}