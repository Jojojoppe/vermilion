#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

#include "../../Pipeline.hpp"
#include "../../Buffer.hpp"
#include "../../Texture.hpp"

namespace Vermilion{
namespace Core{

class Instance;
class ShaderProgram;
class RenderTarget;

namespace Vulkan{

class API;
class Buffer;
class Sampler;

class Binding : public Vermilion::Core::Binding{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		std::vector<std::shared_ptr<Vermilion::Core::Vulkan::Buffer>> buffers;
		std::vector<std::shared_ptr<Vermilion::Core::Vulkan::Sampler>> samplers;

		Binding(API * api, std::vector<std::shared_ptr<Vermilion::Core::Buffer>>& buffers, std::vector<std::shared_ptr<Vermilion::Core::Sampler>>& samplers);
		~Binding();
};

class PipelineLayout : public Vermilion::Core::PipelineLayout{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		VkPipelineLayout vk_pipelineLayout;
		VkDescriptorSetLayout vk_descriptorSetLayout;
		unsigned int stride = 0;
		std::vector<Vermilion::Core::BufferLayoutElement> vertexLayout;
		std::vector<Vermilion::Core::PipelineLayoutBinding> bindings;
		std::unordered_map<std::string, std::shared_ptr<Vermilion::Core::PipelineLayoutUniform>> uniforms;

		PipelineLayout(API * api, std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout, std::initializer_list<Vermilion::Core::PipelineLayoutBinding> bindings, std::initializer_list<Vermilion::Core::PipelineLayoutUniform> uniforms);
		~PipelineLayout();
};

class Pipeline : public Vermilion::Core::Pipeline{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

		std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget;
		std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram;

		std::vector<VkDescriptorPool> vk_descriptorPool;
		unsigned int lastPool = 0;
		unsigned int lastDescriptor = 0;

		unsigned int stride = 0;

		Vermilion::Core::PipelineSettings settings;
		
	public:
		VkPipeline vk_pipeline;

		std::shared_ptr<PipelineLayout> pipelineLayout;
		std::unordered_map<std::shared_ptr<Vermilion::Core::Vulkan::Binding>, std::vector<VkDescriptorSet>> descriptorSets;

		// Viewport and sciccors
		VkViewport viewport;
		VkRect2D scissor;

		Pipeline(API * api, std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, Vermilion::Core::PipelineSettings settings,
			std::shared_ptr<Vermilion::Core::PipelineLayout> pipelineLayout);
		~Pipeline();

		void destroy();
		void create();

		virtual void setViewPort(unsigned int width, unsigned height, unsigned int x, unsigned int y) override;
		virtual void setScissor(unsigned int width, unsigned height, unsigned int x, unsigned int y) override;

		void bind(std::shared_ptr<Vermilion::Core::Binding> binding);

		void createDescriptorPool();
};



}}}
#endif
