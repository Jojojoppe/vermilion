#pragma once

#include <string>
#include <memory>

#include <vermilion/RenderTarget.hpp>
#include <vermilion/Shader.hpp>
#include <vermilion/Pipeline.hpp>
#include <vermilion/Buffer.hpp>

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

class Instance;
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

		virtual std::shared_ptr<Shader> createShader(std::string source, ShaderType type){return nullptr;};
		virtual std::shared_ptr<ShaderProgram> createShaderProgram(std::initializer_list<std::shared_ptr<Shader>> shaders){return nullptr;};

		virtual std::shared_ptr<Pipeline> createPipeline(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<ShaderProgram> shaderProgram, std::initializer_list<VertexBufferLayoutElement> vertexLayout){return nullptr;};

		virtual std::shared_ptr<VertexBuffer> createVertexBuffer(void * data, size_t length){return nullptr;};
		virtual std::shared_ptr<IndexBuffer> createIndexBuffer(void * data, size_t length){return nullptr;};

	private:
};

}}
