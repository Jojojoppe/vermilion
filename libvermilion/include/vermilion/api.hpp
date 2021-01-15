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

class API{
	public:

	private:

	public:
		static API * create(int platform, Instance * instance);
		virtual ~API() = default;

		virtual void init(){};
		virtual void startRender(){};
		virtual void endRender(){};

		virtual std::shared_ptr<RenderTarget> getDefaultRenderTarget(){return nullptr;};

		virtual std::shared_ptr<Shader> createShader(const std::string& source, ShaderType type){return nullptr;};
		virtual std::shared_ptr<ShaderProgram> createShaderProgram(std::initializer_list<std::shared_ptr<Shader>> shaders){return nullptr;};

		virtual std::shared_ptr<Pipeline> createPipeline(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<ShaderProgram> shaderProgram, 
			std::initializer_list<BufferLayoutElement> vertexLayout, std::initializer_list<std::shared_ptr<UniformBuffer>> uniformBuffers){return nullptr;};

		virtual std::shared_ptr<VertexBuffer> createVertexBuffer(std::vector<float>& vertices){return nullptr;};
		virtual std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<unsigned int>& indices){return nullptr;};
		virtual std::shared_ptr<UniformBuffer> createUniformBuffer(size_t length){return nullptr;};
		virtual std::shared_ptr<Renderable> createRenderable(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer, 
			unsigned int vertexOffset, unsigned int indexOffset, unsigned int length){return nullptr;};

		virtual void streamData(std::shared_ptr<UniformBuffer> uniformBuffer, void * data){};

		virtual std::shared_ptr<Texture> createTexture(const std::string& path, size_t width, size_t height, unsigned int channels){return nullptr;};
		virtual std::shared_ptr<Sampler> createSampler(){return nullptr;};

	private:
};

}}
