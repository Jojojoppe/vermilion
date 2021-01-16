#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

#include <vermilion/Pipeline.hpp>
#include <vermilion/Buffer.hpp>
#include <vermilion/Texture.hpp>

namespace Vermilion{
namespace Core{

class Instance;
class ShaderProgram;
class RenderTarget;

namespace Vulkan{

class API;
class UniformBuffer;
class Sampler;

class Binding : public Vermilion::Core::Binding{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		std::vector<std::shared_ptr<Vermilion::Core::Vulkan::UniformBuffer>> uniformBuffers;
		std::vector<std::shared_ptr<Vermilion::Core::Vulkan::Sampler>> samplers;

		Binding(API * api, std::initializer_list<std::shared_ptr<Vermilion::Core::UniformBuffer>> uniformBuffers, std::initializer_list<std::shared_ptr<Vermilion::Core::Sampler>> samplers);
		~Binding();
};

class Pipeline : public Vermilion::Core::Pipeline{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

		std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget;
		std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram;
		std::vector<Vermilion::Core::BufferLayoutElement> vertexLayout;
		std::initializer_list<Vermilion::Core::PipelineLayoutBinding> layoutBindings;

		std::vector<VkDescriptorPool> vk_descriptorPool;
		VkDescriptorSetLayout vk_descriptorSetLayout;

		unsigned int stride = 0;
		
	public:
		VkPipeline vk_pipeline;
		VkPipelineLayout vk_pipelineLayout;

		std::unordered_map<std::shared_ptr<Vermilion::Core::Vulkan::Binding>, std::vector<VkDescriptorSet>> descriptorSets;

		Pipeline(API * api, std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, 
			std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout, std::initializer_list<Vermilion::Core::PipelineLayoutBinding> layoutBindings);
		~Pipeline();

		void destroy();
		void create();

		void bind(std::shared_ptr<Vermilion::Core::Binding> binding);

		void createDescriptorPool();
};



}}}
#endif
