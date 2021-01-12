#pragma once

#include <vermilion/instance.hpp>
#include <vermilion/window.hpp>
#include <vermilion/logger.hpp>

#include <vermilion/Pipeline.hpp>
#include <vermilion/Buffer.hpp>

#include <memory>

namespace Vermilion{
namespace Core{

class RenderTarget{
	public:
		~RenderTarget() = default;

		virtual void start(){};
		virtual void end(){};

		virtual void draw(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<VertexBuffer> vertexBuffer, int vertexCount, int instanceCount, int firstVertex, int firstInstance){};
		virtual void draw(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer, int indexCount, int instanceCount, int firstInstance){};
};

}}
