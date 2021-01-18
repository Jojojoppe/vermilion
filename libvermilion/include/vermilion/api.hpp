#pragma once

#include <string>
#include <memory>

#include <vermilion/RenderTarget.hpp>
#include <vermilion/Shader.hpp>
#include <vermilion/Pipeline.hpp>
#include <vermilion/Buffer.hpp>
#include <vermilion/Renderable.hpp>
#include <vermilion/Texture.hpp>

namespace Vermilion{
namespace Core{

/**
 * @brief Type of render API
 */
enum RenderPlatform{
	RENDER_PLATFORM_NONE = 0,
	RENDER_PLATFORM_OPENGL,
	RENDER_PLATFORM_VULKAN
};
const std::string RenderPlatformString[] = {
	"NONE",
	"OpenGL",
	"Vulkan"
};

extern const int renderPlatform[];
class RenderTarget;
class Instance;
class Window;

class API{
	public:

	private:

	public:
		static API * create(int platform, Instance * instance);
		virtual ~API() = default;

		virtual void init(){};
		virtual void startRender(){};
		virtual void endRender(std::initializer_list<std::shared_ptr<RenderTarget>> extraRenderTargets){};
		virtual void resize(){};

		virtual std::shared_ptr<RenderTarget> getDefaultRenderTarget(){return nullptr;};
		virtual std::shared_ptr<RenderTarget> createRenderTarget(std::shared_ptr<Texture> texture){return nullptr;};

		virtual std::shared_ptr<Shader> createShader(const std::string& source, ShaderType type){return nullptr;};
		virtual std::shared_ptr<ShaderProgram> createShaderProgram(std::initializer_list<std::shared_ptr<Shader>> shaders){return nullptr;};

		virtual std::shared_ptr<Pipeline> createPipeline(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<ShaderProgram> shaderProgram, PipelineSettings settings,
			std::initializer_list<BufferLayoutElement> vertexLayout, std::initializer_list<PipelineLayoutBinding> layoutBindings){return nullptr;};
		virtual std::shared_ptr<Binding> createBinding(std::initializer_list<std::shared_ptr<UniformBuffer>> uniformBuffers, std::initializer_list<std::shared_ptr<Sampler>> samplers){return nullptr;};

		virtual std::shared_ptr<VertexBuffer> createVertexBuffer(std::vector<float>& vertices, BufferType type){return nullptr;};
		virtual std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<unsigned int>& indices, BufferType type){return nullptr;};
		virtual std::shared_ptr<UniformBuffer> createUniformBuffer(size_t length, BufferType type){return nullptr;};
		virtual std::shared_ptr<Renderable> createRenderable(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer, 
			unsigned int vertexOffset, unsigned int indexOffset, unsigned int length){return nullptr;};

		virtual std::shared_ptr<Texture> createTexture(const std::string& path, size_t width, size_t height, unsigned int channels){return nullptr;};
		virtual std::shared_ptr<Texture> createTexture(void * data, size_t width, size_t height, unsigned int channels){return nullptr;};
		virtual std::shared_ptr<Sampler> createSampler(std::shared_ptr<Texture> texture){return nullptr;};

	private:
};

}}
