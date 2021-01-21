#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

#include "../../Shader.hpp"

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class Shader : public Vermilion::Core::Shader{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:

		VkShaderModuleCreateInfo vk_createInfo;
		VkPipelineShaderStageCreateInfo vk_shaderStageInfo;
		std::vector<uint32_t> bytecode;

		Shader(API * api, const std::string& source, Vermilion::Core::ShaderType type);
		~Shader();
};

class ShaderProgram : public Vermilion::Core::ShaderProgram{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:	

		std::vector<VkShaderModule> vk_shadermodules;

		ShaderProgram(API * api, std::vector<std::shared_ptr<Vermilion::Core::Shader>>& shaders);
		~ShaderProgram();

		void createModules();
		void destroyModules();
};

}}}
#endif
