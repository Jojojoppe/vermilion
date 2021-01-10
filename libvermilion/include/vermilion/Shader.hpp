#pragma once

#include <vermilion/logger.hpp>

#include <memory>
#include <vector>

namespace Vermilion{
namespace Core{

enum ShaderType{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_GEOMETRY,
	SHADER_TYPE_FRAGMENT,
	SHADER_TYPE_COMPUTE
};

class Shader{
	public:
		int type;

		~Shader() = default;
};

class ShaderProgram{
	public:
		std::vector<std::shared_ptr<Shader>> shaders;

		~ShaderProgram() = default;
};

}}
