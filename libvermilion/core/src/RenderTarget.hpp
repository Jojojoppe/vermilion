#pragma once

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include <vermilion/window.hpp>
#include <vermilion/logger.hpp>

#include "Pipeline.hpp"
#include "Buffer.hpp"
#include "Renderable.hpp"

#include <memory>

namespace Vermilion{
namespace Core{

class RenderTarget{
	public:
		~RenderTarget() = default;

		virtual void start(float r=0.0, float g=0.0, float b=0.0, float a=1.0){};
		virtual void end(){};

		virtual void draw(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Binding> binding, std::shared_ptr<Renderable> renderable, int instanceCount=1, int firstInstance=0){};
};

}}
