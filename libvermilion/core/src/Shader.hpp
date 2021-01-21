#pragma once

#include <vermilion/vermilion.hpp>
#include <vermilion/logger.hpp>

#include <memory>
#include <vector>

namespace Vermilion{
namespace Core{

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
