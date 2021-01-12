#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

#include <vermilion/Pipeline.hpp>

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

	public:
		VkPipeline vk_pipeline;
		VkPipelineLayout vk_pipelineLayout;

		Pipeline(API * api, std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram);
		~Pipeline();

		void destroy();
		void create();
};

}}}
#endif
