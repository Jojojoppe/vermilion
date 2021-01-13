#pragma once

#include <vermilion/instance.hpp>
#include <vermilion/window.hpp>
#include <vermilion/logger.hpp>

#include <vermilion/Pipeline.hpp>
#include <vermilion/Buffer.hpp>
#include <vermilion/Renderable.hpp>

#include <memory>

namespace Vermilion{
namespace Core{

class RenderTarget{
	public:
		~RenderTarget() = default;

		virtual void start(){};
		virtual void end(){};

		virtual void draw(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Renderable> renderable, int instanceCount=1, int firstInstance=0){};
};

}}
