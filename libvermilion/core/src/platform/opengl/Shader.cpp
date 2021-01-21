#ifdef VMCORE_OPENGL
#include "Shader.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

Vermilion::Core::OpenGL::Shader::Shader(Vermilion::Core::OpenGL::API * api, const std::string& source, Vermilion::Core::ShaderType type){
	this->api = api;
	this->instance = api->instance;
	this->type = type;

	this->source = source;
}

Vermilion::Core::OpenGL::Shader::~Shader(){
}

Vermilion::Core::OpenGL::ShaderProgram::ShaderProgram(Vermilion::Core::OpenGL::API * api, std::vector<std::shared_ptr<Vermilion::Core::Shader>>& shaders){
	this->api = api;
	this->instance = api->instance;

	this->shaderProgram = glCreateProgram();

	std::vector<unsigned int> shdrs;
	for(auto& s : shaders){
		int shaderType = 0;
		switch(s->type){
			case Vermilion::Core::ShaderType::SHADER_TYPE_VERTEX:
				shaderType = GL_VERTEX_SHADER;
				break;
			case Vermilion::Core::ShaderType::SHADER_TYPE_FRAGMENT:
				shaderType = GL_FRAGMENT_SHADER;
				break;
			default:
				instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Unknown shader type");
				throw std::runtime_error("Vermilion::Core::OpenGL::Shader::Shader() - Unknown shader type");
				break;
		}

		int shader = glCreateShader(shaderType);
		const GLchar* sr = std::static_pointer_cast<Vermilion::Core::OpenGL::Shader>(s)->source.c_str();
		glShaderSource(shader, 1, &sr, NULL);
		glCompileShader(shader);
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success){
			char err[1024];
			glGetShaderInfoLog(shader, 1024, NULL, err);
			instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not compile shader: %s", err);
			throw std::runtime_error("Vermilion::Core::OpenGL::Shader::Shader() - Could not compile shader");
		}

		glAttachShader(this->shaderProgram, shader);
		shdrs.push_back(shader);
	}
	glLinkProgram(this->shaderProgram);
	int success;
	glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
	if(!success){
		char err[1024];
	    glGetProgramInfoLog(this->shaderProgram, 1024, NULL, err);
		instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not link shaders: %s", err);
		throw std::runtime_error("Vermilion::Core::OpenGL::ShaderProgram::ShaderProgram() - Could not link shaders");
	}

	for(auto& s : shdrs){
		glDeleteShader(s);
	}
}

Vermilion::Core::OpenGL::ShaderProgram::~ShaderProgram(){
	glDeleteProgram(this->shaderProgram);
}

#endif
