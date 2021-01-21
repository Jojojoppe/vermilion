#pragma once

#include <memory>
#include <unordered_map>

#include <vermilion/window.hpp>
#include <vermilion/logger.hpp>
#include <vermilion/vermilion.hpp>

namespace Vermilion{
namespace Core{

class API;

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
	friend class VmObject;
	public:
		Logger logger;
		std::shared_ptr<API> api;
		std::unique_ptr<Window> window;

		int platform_render;
		int platform_window;

		// Vermilion objects: <ID, object>
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
		unsigned int IDcounter = 1;

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

		void getDefaultRenderTarget(VmRenderTarget& renderTarget);
		void createRenderTarget(VmRenderTarget& renderTarget, VmTexture& texture);

		void createShader(VmShader& shader, const std::string& source, ShaderType type);
		void createShaderProgram(VmShaderProgram& shaderProgram, std::initializer_list<VmShader*> shaders);

		void createPipelineLayout(VmPipelineLayout& pipelineLayout, std::initializer_list<BufferLayoutElement> vertexLayout, std::initializer_list<PipelineLayoutBinding> bindings);
		void createPipeline(VmPipeline& pipeline, VmRenderTarget& renderTarget, VmShaderProgram& shaderProgram, VmPipelineLayout& pipelineLayout, PipelineSettings settings);
		void createBinding(VmBinding& binding, std::initializer_list<VmBuffer*> buffers, std::initializer_list<VmSampler*> samplers);

		void createBuffer(VmBuffer& buffer, size_t size, BufferType type, BufferUsage usage, BufferDataUsage dataUsage);

		void createRenderable(VmRenderable& renderable, VmBuffer& vertexBuffer, VmBuffer& indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length);

		void createTexture(VmTexture& texture, size_t width=0, size_t height=0, unsigned int channels=0);
		void createSampler(VmSampler& sampler, VmTexture& texture);

	private:
		int parseHintType_RENDER_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_HEIGHT(int * hintType, int * hintValue);
		int parseHintType_WINDOW_WIDTH(int * hintType, int * hintValue);
		int parseHintType_LOGLEVEL(int * hintType, int * hintValue);
};

}}
