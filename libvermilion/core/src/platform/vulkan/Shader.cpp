#ifdef VMCORE_VULKAN
#include "Shader.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>

Vermilion::Core::Vulkan::Shader::Shader(Vermilion::Core::Vulkan::API * api, std::string source, Vermilion::Core::ShaderType type){
	this->api = api;
	this->instance = api->instance;
	this->type = type;

	shaderc::Compiler comp;
	shaderc::CompileOptions compoptions;

	vk_createInfo = {};
	vk_shaderStageInfo = {};

	switch(type){

		case Vermilion::Core::ShaderType::SHADER_TYPE_VERTEX:{
			this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Compile vertex shader");
			shaderc::CompilationResult res = comp.CompileGlslToSpv(source, shaderc_vertex_shader, "vertex_shader", compoptions);
			if(res.GetCompilationStatus()!=shaderc_compilation_status_success){
				this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Could not compile vertex shader: %s", res.GetErrorMessage());
				throw std::runtime_error("Vermilion::Core::Vulkan::Shader::Shader() - Could not compile vertex shader");
			}
			this->bytecode = std::vector<uint32_t>(res.cbegin(), res.cend());
			this->vk_shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;			
		}break;

		case Vermilion::Core::ShaderType::SHADER_TYPE_FRAGMENT:{
			this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Compile fragment shader");
			shaderc::CompilationResult res = comp.CompileGlslToSpv(source, shaderc_fragment_shader, "vertex_shader", compoptions);
			if(res.GetCompilationStatus()!=shaderc_compilation_status_success){
				this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Could not compile fragment shader: %s", res.GetErrorMessage());
				throw std::runtime_error("Vermilion::Core::Vulkan::Shader::Shader() - Could not compile fragment shader");
			}
			this->bytecode = std::vector<uint32_t>(res.cbegin(), res.cend());
			this->vk_shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;												   
	    }break;

		default:
			this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Unknown shader type %d", type);
			throw std::runtime_error("Vermilion::Core::Vulkan::Shader::Shader() - Unknown shader type");
	}

	vk_createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vk_createInfo.codeSize = bytecode.size()*sizeof(uint32_t);
	vk_createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());
	vk_shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vk_shaderStageInfo.pName = "main";

}

Vermilion::Core::Vulkan::Shader::~Shader(){	
}

Vermilion::Core::Vulkan::ShaderProgram::ShaderProgram(Vermilion::Core::Vulkan::API * api, std::initializer_list<std::shared_ptr<Vermilion::Core::Shader>> shaders){
	this->api = api;
	this->instance = api->instance;
}

Vermilion::Core::Vulkan::ShaderProgram::~ShaderProgram(){	
}

#endif
