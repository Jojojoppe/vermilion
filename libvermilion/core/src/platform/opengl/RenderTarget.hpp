#ifdef VMCORE_OPENGL
#pragma once

#include <stdint.h>
#include <memory>

#include "../../RenderTarget.hpp"
#include "../../Buffer.hpp"
#include "../../Renderable.hpp"
#include "Texture.hpp"
#include <glad/glad.h>

namespace Vermilion{
namespace Core{

class Instance;

namespace OpenGL{

class API;

class RenderTarget : public Vermilion::Core::RenderTarget{
	public:
		std::shared_ptr<Texture> texture;

	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		unsigned int framebuffer;
		unsigned int depthbuffer;
		RenderTarget(API * api, std::shared_ptr<Vermilion::Core::Texture> texture);
		~RenderTarget();

		virtual void start(float r, float g, float b, float a) override;
		virtual void end() override;

		virtual void draw(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, std::shared_ptr<Vermilion::Core::Binding> binding, std::shared_ptr<Vermilion::Core::Renderable> renderable, int instanceCount, int firstInstance) override;

		virtual void setUniform(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, const std::string& name, void * data) override;
};

class DefaultRenderTarget : public RenderTarget{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		DefaultRenderTarget(API * api, std::shared_ptr<Vermilion::Core::Texture> texture = nullptr);
		~DefaultRenderTarget();

		virtual void start(float r, float g, float b, float a) override;
		virtual void end() override;
};

}}}
#endif
