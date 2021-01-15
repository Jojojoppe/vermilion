#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

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

class Pipeline : public Vermilion::Core::Pipeline{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

		std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget;
		std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram;
		std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout;
		std::initializer_list<std::shared_ptr<Vermilion::Core::UniformBuffer>> uniformBuffers;
		std::initializer_list<std::shared_ptr<Vermilion::Core::Sampler>> samplers;

	public:
		VkPipeline vk_pipeline;
		VkPipelineLayout vk_pipelineLayout;
		VkDescriptorSetLayout vk_descriptorSetLayout;
		std::vector<VkDescriptorSet> vk_descriptorSets;

		Pipeline(API * api, std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, 
			std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout, std::initializer_list<std::shared_ptr<Vermilion::Core::UniformBuffer>> uniformBuffers,
			std::initializer_list<std::shared_ptr<Vermilion::Core::Sampler>> samplers);
		~Pipeline();

		void destroy();
		void create();
};

}}}
#endif
