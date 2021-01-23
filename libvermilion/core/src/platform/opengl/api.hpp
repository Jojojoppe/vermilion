#ifdef VMCORE_OPENGL
#pragma once

#include <vermilion/instance.hpp>
#include "../../api.hpp"
#include "RenderTarget.hpp"

namespace Vermilion{
namespace Core{
namespace OpenGL{

class API : public Vermilion::Core::API{
	public:
		Vermilion::Core::Instance * instance;

	private:
		std::shared_ptr<DefaultRenderTarget> defaultRenderTarget;

	public:
		API(Vermilion::Core::Instance * instance);
		virtual ~API() override;

		virtual void init() override;
		virtual void startRender() override;
		virtual void endRender(std::vector<std::shared_ptr<Vermilion::Core::RenderTarget>>& extraRenderTargets) override;

		virtual void resize() override;

        virtual std::shared_ptr<Vermilion::Core::Texture> createTexture(size_t width, size_t height, unsigned int channels) override;
        virtual std::shared_ptr<Vermilion::Core::Sampler> createSampler(std::shared_ptr<Vermilion::Core::Texture> texture);

        virtual std::shared_ptr<Vermilion::Core::Shader> createShader(const std::string& source, Vermilion::Core::ShaderType type) override;
        virtual std::shared_ptr<Vermilion::Core::ShaderProgram> createShaderProgram(std::vector<std::shared_ptr<Vermilion::Core::Shader>>& shaders) override;

		virtual std::shared_ptr<Vermilion::Core::Buffer> createBuffer(size_t size, BufferType type, BufferUsage usage, BufferDataUsage dataUsage) override;
		virtual std::shared_ptr<Vermilion::Core::Renderable> createRenderable(std::shared_ptr<Vermilion::Core::Buffer> vertexBuffer, std::shared_ptr<Vermilion::Core::Buffer> indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length) override;

		virtual std::shared_ptr<Vermilion::Core::PipelineLayout> createPipelineLayout(std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout, std::initializer_list<Vermilion::Core::PipelineLayoutBinding> bindings, std::initializer_list<Vermilion::Core::PipelineLayoutUniform> uniforms) override;
		virtual std::shared_ptr<Vermilion::Core::Pipeline> createPipeline(std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, 
			std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, std::shared_ptr<Vermilion::Core::PipelineLayout> pipelineLayout, Vermilion::Core::PipelineSettings settings) override;
		virtual std::shared_ptr<Vermilion::Core::Binding> createBinding(std::vector<std::shared_ptr<Vermilion::Core::Buffer>>& buffers, std::vector<std::shared_ptr<Vermilion::Core::Sampler>>& samplers)override;

		virtual std::shared_ptr<Vermilion::Core::RenderTarget> getDefaultRenderTarget() override;
		virtual std::shared_ptr<Vermilion::Core::RenderTarget> createRenderTarget(std::shared_ptr<Vermilion::Core::Texture> texture) override;

	private:

};

}}}

#endif