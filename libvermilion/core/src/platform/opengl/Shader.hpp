#ifdef VMCORE_OPENGL
#pragma once

#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

#include "../../Shader.hpp"
#include <glad/glad.h>

namespace Vermilion{
namespace Core{

class Instance;

namespace OpenGL{

class API;

class Shader : public Vermilion::Core::Shader{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		std::string source;

		Shader(API * api, const std::string& source, Vermilion::Core::ShaderType type);
		~Shader();
};

class ShaderProgram : public Vermilion::Core::ShaderProgram{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:	
		unsigned int shaderProgram;

		ShaderProgram(API * api, std::vector<std::shared_ptr<Vermilion::Core::Shader>>& shaders);
		~ShaderProgram();
};

}}}
#endif
