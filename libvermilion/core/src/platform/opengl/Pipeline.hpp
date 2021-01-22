#ifdef VMCORE_OPENGL
#pragma once

#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include "../../Pipeline.hpp"
#include "../../Buffer.hpp"
#include "../../Texture.hpp"

namespace Vermilion{
namespace Core{

class Instance;
class ShaderProgram;
class RenderTarget;

namespace OpenGL{

class API;
class Buffer;
class Sampler;

class Binding : public Vermilion::Core::Binding{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		std::vector<std::shared_ptr<Buffer>> buffers;
		std::vector<std::shared_ptr<Sampler>> samplers;

		Binding(API * api, std::vector<std::shared_ptr<Vermilion::Core::Buffer>>& buffers, std::vector<std::shared_ptr<Vermilion::Core::Sampler>>& samplers);
		~Binding();
};

class PipelineLayout : public Vermilion::Core::PipelineLayout{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		unsigned int stride = 0;
		unsigned int vao;
		std::vector<Vermilion::Core::BufferLayoutElement> vertexLayout;
		std::vector<Vermilion::Core::PipelineLayoutBinding> bindings;

		PipelineLayout(API * api, std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout, std::initializer_list<Vermilion::Core::PipelineLayoutBinding> bindings);
		~PipelineLayout();
};

class Pipeline : public Vermilion::Core::Pipeline{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

		std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget;

		unsigned int stride = 0;

		
	public:
		std::shared_ptr<PipelineLayout> pipelineLayout;
		std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram;

		unsigned int vpwidth, vpheight, vpx, vpy, swidth, sheight, sx, sy;
		unsigned int drawmode;
		bool drawlined = false;
		Vermilion::Core::PipelineSettings settings;

		Pipeline(API * api, std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, Vermilion::Core::PipelineSettings settings, std::shared_ptr<Vermilion::Core::PipelineLayout> pipelineLayout);
		~Pipeline();

		virtual void setViewPort(unsigned int width, unsigned height, unsigned int x, unsigned int y) override;
		virtual void setScissor(unsigned int width, unsigned height, unsigned int x, unsigned int y) override;
};

}}}
#endif
