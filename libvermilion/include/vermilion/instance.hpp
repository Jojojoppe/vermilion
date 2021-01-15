#pragma once

#include <memory>

#include <vermilion/api.hpp>
#include <vermilion/window.hpp>
#include <vermilion/logger.hpp>
#include <vermilion/Shader.hpp>
#include <vermilion/Pipeline.hpp>
#include <vermilion/Buffer.hpp>
#include <vermilion/Renderable.hpp>
#include <vermilion/Texture.hpp>

namespace Vermilion{
namespace Core{

class API;
class RenderTarget;

/**
 * @brief Hint types
 */
enum HintType{
	HINT_TYPE_NONE = 0,
	HINT_TYPE_WINDOW_PLATFORM,
	HINT_TYPE_RENDER_PLATFORM,
	HINT_TYPE_WINDOW_HEIGHT,
	HINT_TYPE_WINDOW_WIDTH,
	HINT_TYPE_LOGLEVEL,
};

/**
 * @brief The instance class it the most top level class of Vermilion
 */
class Instance{
	public:
		Logger logger;
		std::shared_ptr<API> api;
		std::unique_ptr<Window> window;

		int platform_render;
		int platform_window;

	private:

	public:

		/**
		 * @brief Create Vermilion instance
		 *
		 * @param hintType List of types of setup hints, terminated with zero
		 * @param hintValue List of values of setup hints, terminated with zero
		 */
		Instance(int * hintType, int * hintValue);

		~Instance();

		void startRender();
		void endRender();

		std::shared_ptr<RenderTarget> getDefaultRenderTarget();

		std::shared_ptr<Shader> createShader(const std::string& source, ShaderType type);
		std::shared_ptr<ShaderProgram> createShaderProgram(std::initializer_list<std::shared_ptr<Shader>> shaders);

		std::shared_ptr<Pipeline> createPipeline(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<ShaderProgram> shaderProgram, 
			std::initializer_list<BufferLayoutElement> vertexLayout, std::initializer_list<std::shared_ptr<UniformBuffer>> uniformBuffers,
			std::initializer_list<std::shared_ptr<Sampler>> samplers);

		std::shared_ptr<VertexBuffer> createVertexBuffer(std::vector<float>& vertices);
		std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<unsigned int>& indices);
		std::shared_ptr<UniformBuffer> createUniformBuffer(size_t size);
		std::shared_ptr<Renderable> createRenderable(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer, 
			unsigned int vertexOffset=0, unsigned int indexOffset=0, unsigned int length=0);

		void streamData(std::shared_ptr<UniformBuffer> uniformBuffer, void * data);

		std::shared_ptr<Texture> createTexture(const std::string& path="", size_t width=0, size_t height=0, unsigned int channels=0);
		std::shared_ptr<Sampler> createSampler(std::shared_ptr<Texture> texture);

	private:
		int parseHintType_RENDER_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_HEIGHT(int * hintType, int * hintValue);
		int parseHintType_WINDOW_WIDTH(int * hintType, int * hintValue);
		int parseHintType_LOGLEVEL(int * hintType, int * hintValue);
};

}}
