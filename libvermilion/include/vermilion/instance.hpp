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
		Instance(int * hintType, int * hintValue, WindowCallbackFunctions windowCallbackFunctions);

		~Instance();

		void startRender();
		void endRender(std::initializer_list<std::shared_ptr<RenderTarget>> extraRenderTargets = {});

		std::shared_ptr<RenderTarget> getDefaultRenderTarget();
		std::shared_ptr<RenderTarget> createRenderTarget(std::shared_ptr<Texture> texture);

		std::shared_ptr<Shader> createShader(const std::string& source, ShaderType type);
		std::shared_ptr<ShaderProgram> createShaderProgram(std::initializer_list<std::shared_ptr<Shader>> shaders);

		std::shared_ptr<PipelineLayout> createPipelineLayout(std::initializer_list<BufferLayoutElement> vertexLayout, std::initializer_list<PipelineLayoutBinding> bindings);
		std::shared_ptr<Pipeline> createPipeline(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<PipelineLayout> pipelineLayout, PipelineSettings settings);
		std::shared_ptr<Binding> createBinding(std::initializer_list<std::shared_ptr<Buffer>> buffers, std::initializer_list<std::shared_ptr<Sampler>> samplers);

		std::shared_ptr<Buffer> createBuffer(size_t size, BufferType type, BufferUsage usage, BufferDataUsage dataUsage);

		std::shared_ptr<Renderable> createRenderable(std::shared_ptr<Buffer> vertexBuffer, std::shared_ptr<Buffer> indexBuffer, 
			unsigned int vertexOffset, unsigned int indexOffset, unsigned int length);

		std::shared_ptr<Texture> createTexture(size_t width=0, size_t height=0, unsigned int channels=0);
		std::shared_ptr<Sampler> createSampler(std::shared_ptr<Texture> texture);

	private:
		int parseHintType_RENDER_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_HEIGHT(int * hintType, int * hintValue);
		int parseHintType_WINDOW_WIDTH(int * hintType, int * hintValue);
		int parseHintType_LOGLEVEL(int * hintType, int * hintValue);
};

}}
