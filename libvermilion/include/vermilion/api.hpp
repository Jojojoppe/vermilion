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

		virtual std::shared_ptr<PipelineLayout> createPipelineLayout(std::initializer_list<BufferLayoutElement> vertexLayout, std::initializer_list<PipelineLayoutBinding> bindings){return nullptr;};
		virtual std::shared_ptr<Pipeline> createPipeline(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<PipelineLayout> pipelineLayout, PipelineSettings settings){return nullptr;};
		virtual std::shared_ptr<Binding> createBinding(std::initializer_list<std::shared_ptr<Buffer>> buffers, std::initializer_list<std::shared_ptr<Sampler>> samplers){return nullptr;};

		virtual std::shared_ptr<Buffer> createBuffer(size_t size, BufferType type, BufferUsage usage, BufferDataUsage dataUsage){return nullptr;};

		virtual std::shared_ptr<Renderable> createRenderable(std::shared_ptr<Buffer> vertexBuffer, std::shared_ptr<Buffer> indexBuffer, 
			unsigned int vertexOffset, unsigned int indexOffset, unsigned int length){return nullptr;};

		virtual std::shared_ptr<Texture> createTexture(size_t width, size_t height, unsigned int channels){return nullptr;};
		virtual std::shared_ptr<Sampler> createSampler(std::shared_ptr<Texture> texture){return nullptr;};

	private:
};

}}
