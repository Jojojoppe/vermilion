#pragma once

#include <memory>
#include <unordered_map>

#include <vermilion/window.hpp>
#include <vermilion/logger.hpp>
#include <vermilion/vermilion.hpp>

namespace Vermilion{
namespace Core{

class API;

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

class Renderable;
class RenderTarget;
class Texture;
class Shader;
class ShaderProgram;
class PipelineLayout;
class Pipeline;
class Sampler;
class Buffer;
class Binding;

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

		// Vermilion objects: <ID, object>
		unsigned int IDcounter = 1;
		// ID==0: default render target
		std::unordered_map<unsigned int, std::shared_ptr<Renderable>> renderables;
		std::unordered_map<unsigned int, std::shared_ptr<RenderTarget>> rendertargets;
		std::unordered_map<unsigned int, std::shared_ptr<Texture>> textures;
		std::unordered_map<unsigned int, std::shared_ptr<Shader>> shaders;
		std::unordered_map<unsigned int, std::shared_ptr<ShaderProgram>> shaderprograms;
		std::unordered_map<unsigned int, std::shared_ptr<PipelineLayout>> pipelinelayouts;
		std::unordered_map<unsigned int, std::shared_ptr<Pipeline>> pipelines;
		std::unordered_map<unsigned int, std::shared_ptr<Sampler>> samplers;
		std::unordered_map<unsigned int, std::shared_ptr<Buffer>> buffers;
		std::unordered_map<unsigned int, std::shared_ptr<Binding>> bindings;

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
		void endRender(std::initializer_list<VmRenderTarget*> extraRenderTargets = {});

		VmRenderTarget getDefaultRenderTarget();
		VmRenderTarget createRenderTarget(VmTexture& texture);

		VmShader createShader(const std::string& source, ShaderType type);
		VmShaderProgram createShaderProgram(std::initializer_list<VmShader*> shaders);

		VmPipelineLayout createPipelineLayout(std::initializer_list<BufferLayoutElement> vertexLayout, std::initializer_list<PipelineLayoutBinding> bindings);
		VmPipeline createPipeline(VmRenderTarget& renderTarget, VmShaderProgram& shaderProgram, VmPipelineLayout& pipelineLayout, PipelineSettings settings);
		VmBinding createBinding(std::initializer_list<VmBuffer*> buffers, std::initializer_list<VmSampler*> samplers);

		VmBuffer createBuffer(size_t size, BufferType type, BufferUsage usage, BufferDataUsage dataUsage);

		VmRenderable createRenderable(VmBuffer& vertexBuffer, VmBuffer& indexBuffer, 
			unsigned int vertexOffset, unsigned int indexOffset, unsigned int length);

		VmTexture createTexture(size_t width=0, size_t height=0, unsigned int channels=0);
		VmSampler createSampler(VmTexture& texture);

	private:
		int parseHintType_RENDER_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_HEIGHT(int * hintType, int * hintValue);
		int parseHintType_WINDOW_WIDTH(int * hintType, int * hintValue);
		int parseHintType_LOGLEVEL(int * hintType, int * hintValue);
};

}}
